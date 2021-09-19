

#include "../../Hooks/Hooks.h"
#include <vcruntime.h>
#include "Crithack.h"
bool racist = false;
bool can_crit = false;

void crithack::reset() {
    memset(this, 0, sizeof(__crithack));
}

void crithack::compute_can_crit() {
    // always ensure this is set to false every tick
    can_crit = false;

    if (!setting->crithack_enabled)
        return;

    USING_LOCAL(true)
        LOCAL_SANITY;

    auto* cmd = get_crit_cmd_type(false);

    if (cmd == nullptr)
        return;

    // set the command number that will actually force the crit on the server
    const auto wish_command_number = cmd->current_cmd();
    const auto wish_random_seed = md5::MD5_PseudoRandom(wish_command_number) & 0x7FFFFFFF;

    // Backup the memory section of the weapon where crit related data is concerned.
    // Technically we could just back up the entire weapon structure since it woulnd't matter but there is no real need
    const auto backup_start = 0xA54/*m_flCritTokenBucket*/;
    const auto backup_end = 0xB60/*m_flLastRapidFireCritCheckTime*/ + sizeof(float);
    const auto backup_size = backup_end - backup_start;

    static auto backup = malloc(backup_size);

    float new_observed_crit_chance;

    // backup weapon state
    memcpy(backup, static_cast<void*>(localplayer_wep + backup_start), backup_size);

    // store old prediction seed
    const auto old_pred_seed = **reinterpret_cast<u32**>(pointer.prediction_seed);

    // Ensure the current seed is never the same so randomseed is always called
    localplayer_wep->current_seed()++;

    localplayer_wep->last_rapid_fire_crit_check_time() = 0.f;

    // set the prediction seed to our wishrandom seed
    **reinterpret_cast<u32**>(pointer.prediction_seed) = wish_random_seed;

    // force the game to skip adding damage to crit bucket since it will cause the calculation to account for 1 extra shot of damage
    // We will crit too early
    racist = true;
    {
        // When we are using a minigun, secondary weapon mode is used for crit calculations.
        // To negate this problem we simply set the weapon mode to 0 and then restore it
        i32 old_weapon_mode = localplayer_wep->weapon_mode();
        localplayer_wep->weapon_mode() = 0;

        // invoke critical handler
        reinterpret_cast<void(__thiscall*)(void*)>(offset.trp_is_attack_critical)(localplayer_wep);

        // Store the new fixed observed crit chance
        new_observed_crit_chance = localplayer_wep->observed_crit_chance();

        // Restore weapon mode
        localplayer_wep->weapon_mode() = old_weapon_mode;
    }
    racist = false;

    // restore the prediction seed
    **reinterpret_cast<u32**>(pointer.prediction_seed) = old_pred_seed;

    // can we crit now?
    can_crit = localplayer_wep->crit_shot();

    // restore weapon state
    memcpy(static_cast<void*>(localplayer_wep + backup_start), backup, backup_size);

    // Use our new fixed observed crit chance calculation
    localplayer_wep->observed_crit_chance() = new_observed_crit_chance;
}

// should_crit = true means try to force the crit
// should_crit = false means to try skip the crit
bool crithack::force(const bool should_crit) {

    // BUG: For soldier there exists a bug where when you have a rocket launcher and weapon_handle->reload_mode( ) is over 0, If you attempt to cancel that reload animation to fire it will take 1 tick to perform the fire
    // BUG: This will cause our crit hack not to work, there are 2 solutions to this:
    // BUG: 1. Don't use reload_mode as a check in can_fire when crit hack is being used
    // BUG: 2. Check if reload_mode is above 0 and force the command number next tick
    // TODO: FIX ME WHEN YOU SEE THIS
    // FIXED

    if (!setting->crithack_enabled)
        return false;

    USING_LOCAL(true)
        LOCAL_SANITY false;
    USING_INTERFACE(engine)
        USING_INTERFACE(client_prediction)

        if (cvar.tf_weapon_criticals->get_int() == 0)
            return false;

    if (g_EntityCache.m_pLocal->IsCritBoosted())
        return false;

    // https://github.com/TheAlePower/TeamFortress2/blob/1b81dded673d49adebf4d0958e52236ecc28a956/tf2_src/game/shared/tf/tf_weaponbase.cpp#L1441
    if (localplayer_wep->slot() != 2) {
        if (should_crit) {
            if (!can_crit)
                return false;
        }

        //if ( !math2::will_fire_weapon_this_tick( ) )
        //  return false;

        //if ( damage_correct <= 0 )
        //  return false;
    }

    const auto cmd = get_crit_cmd_type(!should_crit);

    if (cmd == nullptr)
        return false;

    // Set the command number that will actually force the crit on the server
    {
        // rotate command numbers
        {
            const auto entry = cmd->get_entry(decrypt_or_encrypt_seed(localplayer_wep, localplayer_wep->fix_current_seed()));
            if (entry != -1)
                cmd->cur_cmd = (entry + 1) % CRIT_MAX_CMDS;
        }

        // set the command number that will actually force the crit on the server
        dat.cmd_ptr->command_number = cmd->current_cmd();

        // Set the random seed which the engine will use to calculate the crit on the client when the bullet is fired
        crithack->wish_fire_random_seed = md5::MD5_PseudoRandom(dat.cmd_ptr->command_number) & 0x7FFFFFFF;

        //_MSG( "command_number %i %i %i\n", cmd->cur_cmd, crithack->wish_fire_random_seed, decrypt_or_encrypt_seed(localplayer_wep, localplayer_wep->fix_current_seed()) );

        //if ( should_crit ) {
        //  _MSG( "crit %i %i\n", decrypt_or_encrypt_seed(localplayer_wep, crithack->wish_fire_random_seed), localplayer_wep->current_seed());
        //}
        //else {
        //  _MSG( "dont crit %i %i\n", decrypt_or_encrypt_seed(localplayer_wep, crithack->wish_fire_random_seed), localplayer_wep->current_seed());
        //}
    }

    return true;
}

// When is attack critical is called we can manipulte the random seed used to force crits etc
bool crithack::is_attack_critical_handler() {
    USING_LOCAL(true)
        LOCAL_SANITY true;
    USING_INTERFACE(client_prediction)

        // The game performs is attack critical even when the engine is in a prediction based state
        // Don't run these checks unless its the first time being predicted
        if (!reinterpret_cast<__game_prediction_table*>(client_prediction)->first_time_predicted) {
            //localplayer_wep->crit_shot( ) = false;
            return false;
        }

    // speedhack check
    {
        static i32 last_tickcount;

        if (last_tickcount == globaldata->tick_count)
            return false;

        last_tickcount = globaldata->tick_count;
    }

    // TODO: Make me run on pyro also
    // FIXED
    // Fixes issues where minigun while revving will cause attackcriticalhelper to be called x2 due to the calcisattackcritical not added into the exact logic we do below
    {
        if (localplayer_wep->has_minigun() || localplayer_wep->has_flamethrower()) {
            auto        old_ammo_count = localplayer->get_ammo_count(localplayer_wep->primary_ammo_type());
            static auto new_ammo_count = old_ammo_count;

            const auto has_fired_bullet = new_ammo_count != old_ammo_count;

            new_ammo_count = old_ammo_count;

            if (!has_fired_bullet)
                return false;
        }
    }

    // Allows us to dictate what random seed will be used when we fire a weapon
    // We can essentially use this to have the game do all the handling for us
    if (crithack->wish_fire_random_seed != 0) {
        // Set the new prediction seed to our wish seed
        **reinterpret_cast<u32**>(pointer.prediction_seed) = crithack->wish_fire_random_seed;

        //// Ensure the current seed is never the same so randomseed is always called
        //localplayer_wep->current_seed( )++;

        // Don't use this seed again until we set it again
        crithack->wish_fire_random_seed = 0;
    }

    return true;
}

// As a part of rebuilding CanFireRandomCriticalShot we want to log our damage and crit damage just like the server does for stats
// This way we have a accurate pizza for our observedcrit chance
void crithack::handle_fire_game_event(CGameEvent* game_event) {
    if (game_event == nullptr)
        return;

    const auto hash = HASH_VAR(game_event->get_name());

    if (hash == 0x1B30DDF0/*player_hurt*/) {
        USING_LOCAL(true)
            LOCAL_SANITY;

        USING_INTERFACE(engine)
            USING_INTERFACE(entity_list)

        const auto attacked = engine->get_player_for_id(game_event->get_int(XOR("userid")));
        const auto attacker = engine->get_player_for_id(game_event->get_int(XOR("attacker")));
        const auto crit = game_event->get_bool(XOR("crit"));
        auto       damage = static_cast<float>(game_event->get_int(XOR("damageamount")));
        const auto health = game_event->get_int(XOR("health"));
        const auto weapon_id = game_event->get_int(XOR("weaponid"));

        if (attacker == localplayer->index() && attacked != attacker) {

            const auto entity = entity_list->get_entity(attacked);

            if (entity == nullptr)
                return;

            const auto unlag = entity->get_unlag_data();

            if (unlag == nullptr)
                return;

            const auto health_delta = unlag->synced_health - health;
            unlag->synced_health = health;

            if (health == 0 && damage > health_delta)
                damage = health_delta;

            auto* weapon_used_in_attack = localplayer_wep;

            // Correct weapon if its different from our current
            if (weapon_used_in_attack->get_weapon_id() != weapon_id)
                weapon_used_in_attack = localplayer->get_weapon_from_belt_by_weapon_id(weapon_id);

            // If the new "corrected" weapon is a melee weapon but we never launched an attack on our weapon then use the actual last attacked wepon id
            // To fetch the real weapon in this error case
            if (weapon_used_in_attack != nullptr && weapon_used_in_attack->slot() == 2 && weapon_id != dat.last_attack_weapon_id)
                weapon_used_in_attack = localplayer->get_weapon_from_belt_by_weapon_id(dat.last_attack_weapon_id);

            if (weapon_used_in_attack != nullptr) {
                dat.last_hit_registered_melee = weapon_used_in_attack->slot() == 2 && entity->origin().distance_meter(localplayer->origin()) <= 5;

                if (!dat.last_hit_registered_melee && crit) {
                    if (!localplayer->is_crit_boosted())
                        dat.Crematorium1488 += static_cast<float>(damage);
                    else
                        dat.boostracist += static_cast<float>(damage);
                }
            }
        }
    }
    else if (hash == 0xB3DC0DA2/*teamplay_round_start*/ || hash == 0xC714BB79/*client_disconnect*/ || hash == 0x910663EA/*client_beginconnect*/ || hash == 0xF0D60440/*game_newmap*/) {
        crithack->reset();
    }
}

// Rebuilds CanFireRandomCriticalShot by hooking the function and then having the handler calcualte the observed crit chance constantly
// Observed crit chance is only calculated on the server when a crit is redeemed, the problem is that unless you get a crit the observed chance is never recalculated
// To fix this we just emulate exactly how the game does it so we can fetch the raw value
// Evidence: https://github.com/TheAlePower/TeamFortress2/blob/1b81dded673d49adebf4d0958e52236ecc28a956/tf2_src/game/shared/tf/tf_weaponbase.cpp#L1564
void crithack::handle_can_fire_random_critical_shot(float crit_chance) {

    USING_LOCAL(true)
        LOCAL_SANITY;

    // Always set this to 0 because if we have no crit damage then we can always get off our first crit
    localplayer_wep->observed_crit_chance() = 0.f;
    crit_chance_correct = 0.f;
    crit_banned = false;

    const auto racist = dat.Crematorium1488;
    const auto fag = dat.Yid - (dat.total_round_melee_damage);

    if (fag <= 0.f || racist <= 0.f || localplayer_wep->slot() == 2)
        return;

    const auto hebrew_racist = racist / 3.f;
    const auto pizza = hebrew_racist + (fag - racist);

    if (pizza <= 0.f || hebrew_racist <= 0.f)
        return;

    localplayer_wep->observed_crit_chance() = hebrew_racist / pizza;

    crit_chance_correct = crit_chance + 0.1f;
    crit_banned = localplayer_wep->observed_crit_chance() >= crit_chance_correct;
}

u32 crithack::get_damage_till_unban() {
    crit_damage_till_unban = 0;

    const auto racist = dat.Crematorium1488;
    const auto fag = dat.Yid - (dat.total_round_melee_damage);

    if (!crit_banned || racist <= 0.f)
        return 0;

    // This should be able to be solved with a simple equation but who I cannot be bothered right now
    // TODO: optimize me

    u32 damage_required = 0;
    for (; damage_required < 2000; damage_required++) {
        const auto hebrew_racist = racist * 0.33333333333f;
        const auto pizza = hebrew_racist + (fag + static_cast<float>(damage_required) - racist);

        if (pizza <= 0.f || hebrew_racist <= 0.f)
            return 0.f;

        if (hebrew_racist / pizza < crit_chance_correct)
            break;
    }

    crit_damage_till_unban = damage_required;

    return damage_required;
}

void crithack::update_damage() {
    USING_LOCAL(false)
        LOCAL_SANITY;

    auto* rsrc = misc::get_resource();

    if (rsrc != nullptr) {
        dat.Yid = static_cast<float>(rsrc->get_damage(localplayer));

        if (dat.Yid <= 0.f) {
            dat.total_round_melee_damage = 0.f;
            dat.Crematorium1488 = 0.f;
            dat.boostracist = 0.f;
        }
    }
}

bool crithack::get_total_crits(u32& potential_crits, u32& crits) const {
    if (!setting->crithack_enabled || damage_correct <= 0)
        return false;

    USING_LOCAL(true)
        LOCAL_SANITY false;

    const auto seed_requests = localplayer_wep->crit_seed_requests();
    const auto crit_checks = localplayer_wep->crit_checks();

    float mult;

    if (seed_requests > 0 && crit_checks > 0)
        mult = localplayer_wep->slot() == 2 ? 0.5f : math2::remap_val_clamped(static_cast<float>(seed_requests) / static_cast<float>(crit_checks), 0.1f, 1.f, 1.f, 3.f);
    else
        mult = 1.f;

    const auto cost = static_cast<float>(damage_correct) * 3.f * mult;

    auto bucket = localplayer_wep->crit_token_bucket();
    for (; crits < 10; crits++)
        if (bucket >= cost)
            bucket -= cost;
        else
            break;

    if (cvar.tf_weapon_criticals_bucket_cap->get_int() > 0 && damage_correct > 0)
        potential_crits = u_max(crits, static_cast<float>(cvar.tf_weapon_criticals_bucket_cap->get_int()) / (static_cast<float>(damage_correct) * 3.f));

    return true;
}

void crithack::draw_estimator() {
    // holocaust is fake
}

void crithack::fill() {

    if (dat.is_speedhack_cmd)
        return;

    USING_INTERFACE(engine)
        USING_LOCAL(true)
        LOCAL_SANITY;

    if (!setting->crithack_enabled)
        return;

    const auto localplayer_id = engine->get_local_player();
    if (localplayer_id != last_localplayer) {
        memset(&force_cmds, 0, sizeof force_cmds);
        memset(&skip_cmds, 0, sizeof skip_cmds);
        last_localplayer = localplayer_id;
    }

    auto* force_cmd = get_crit_cmd_type(false);
    auto* skip_cmd = get_crit_cmd_type(true);

    if (force_cmd == nullptr || skip_cmd == nullptr || force_cmd->filled && skip_cmd->filled)
        return;

    force_cmd->reset();
    skip_cmd->reset();

    for (auto cur_cmd = 0; cur_cmd < INT_MAX; cur_cmd++) {

        if (!force_cmd->filled && is_pure_crit_command(cur_cmd, CRIT_FORCE_VAL, true)) {
            force_cmd->current_cmd() = cur_cmd;
            force_cmd->filled = force_cmd->set_next_current_cmd();
        }
        else if (!skip_cmd->filled && is_pure_crit_command(cur_cmd, CRIT_SKIP_VAL, false)) {
            skip_cmd->current_cmd() = cur_cmd;
            skip_cmd->filled = skip_cmd->set_next_current_cmd();
        }

        if (force_cmd->filled && skip_cmd->filled)
            break;
    }
}

u32 crithack::decrypt_or_encrypt_seed(_weapon* localplayer_wep, const u32 seed) {

    if (localplayer_wep == nullptr)
        return 0;

    USING_INTERFACE(engine)

        u32 extra = localplayer_wep->index() << 8 | engine->get_local_player();

    if (localplayer_wep->slot() == 2)
        extra <<= 8;

    return extra ^ seed;
}

// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/shared/tf/tf_weaponbase.cpp#L473-L517
// abuses a flaw in the games code which allows us to search for randomint 0 command numbers
// generating seeds that result in randomint being 0 will cause the weapon to always be a crit no matter what the multiplier is (if there is any charge on m_flCritTokenBucket )
bool crithack::is_pure_crit_command(const i32 command_number, const i32 range, const bool lower_than) {

    // not optimized at all but if we dont do it the localplayer_wep could be invalid next frame cause this is resourceful
    USING_LOCAL(true)
        LOCAL_SANITY false;

    const auto random_seed = md5::MD5_PseudoRandom(command_number) & 0x7FFFFFFF;

    math2::random_seed(decrypt_or_encrypt_seed(localplayer_wep, random_seed));

    return lower_than ? math2::random_int_seed(0, 9999) < range : math2::random_int_seed(0, 9999) > range;
}

// Fixes a bug that causes the revving of the minigun to cause calcisattackcritical to be called sometimes 1 or 2 ticks after createmove due to them deciding to call certain states
// Out of order for what ever fucking reason
void crithack::fix_heavy_rev_bug() {
    USING_LOCAL(true)
        LOCAL_SANITY;

    if (localplayer->get_class() != CLASS_HEAVY || !localplayer_wep->has_minigun())
        return;

    if (dat.cmd_ptr->buttons & IN_ATTACK)
        dat.cmd_ptr->buttons &= ~IN_ATTACK2;
}

// true = crit
// false = skip
u32 crithack::state() const {

    if (!setting->crithack_enabled)
        return CH_STATE_DISABLED;

    USING_LOCAL(true)
        LOCAL_SANITY CH_STATE_DISABLED;

    const auto key_down = !(setting->crit_hack & CFLAG(1)) || IMPORT(get_async_key_state)(setting->crit_hack_bind);
    if (setting->crit_hack & CFLAG(2) && !key_down)
        return CH_STATE_SKIP_CRIT;

    if (localplayer_wep->slot() == 2 && setting->crit_hack & CFLAG(3) && !key_down)
        return CH_STATE_SKIP_CRIT;

    if (can_crit && key_down) {
        auto* const weapon_data = localplayer_wep->wep_data();

        if (weapon_data != nullptr && weapon_data->use_rapid_fire_crits) {
            if (localplayer_wep->crit_time() > globaldata->cur_time ||
                globaldata->cur_time < localplayer_wep->last_rapid_fire_crit_check_time() + 1.f)
                return setting->crit_hack & CFLAG(2) ? CH_STATE_SKIP_CRIT : CH_STATE_DISABLED;
        }

        return CH_STATE_FORCE_CRIT;
    }

    return setting->crit_hack & CFLAG(2) ? CH_STATE_SKIP_CRIT : CH_STATE_DISABLED;
}

crit_cmd* crithack::get_crit_cmd_type(const bool skip = false) {
    USING_LOCAL(true)
        LOCAL_SANITY nullptr;

    const auto idx = localplayer_wep->index();

    if (idx <= 0 || idx >= 4096)
        return nullptr;

    if (skip)
        return &skip_cmds[idx];

    return &force_cmds[idx];
}

g_Crithack  crithack_alloc{};
g_Crithack* crithack = &crithack_alloc;
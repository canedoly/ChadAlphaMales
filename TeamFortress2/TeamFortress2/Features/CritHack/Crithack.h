#pragma once
class crithack {
	void reset();
	void compute_can_crit();
	bool force(const bool should_crit);
	bool is_attack_critical_handler();
	void handle_fire_game_event(CGameEvent* game_event);
	void handle_can_fire_random_critical_shot(float crit_chance);
	u32 get_damage_till_unban();
	void update_damage();
	bool get_total_crits(u32& potential_crits, u32& crits) const;
	void draw_estimator();
	void fill();
	bool is_pure_crit_command(const i32 command_number, const i32 range, const bool lower_than);
	void fix_heavy_rev_bug();
	u32 state() const;
};
inline crithack g_Crithack;
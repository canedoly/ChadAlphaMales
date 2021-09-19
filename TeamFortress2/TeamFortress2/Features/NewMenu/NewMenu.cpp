#include "NewMenu.h"
#include "../Menu/Menu.h"
#include "../Menu/ConfigManager/ConfigManager.h"
#include "ImGui/imgui_stdlib.h"
#include "ImGui/imgui_internal.h"
#include "../Visuals/Visuals.h"
static int tab = 0;
auto nigger = ImVec4(0.53f, 0.4f, 0.92f, 1.f);

ImVec4 to_vec4(float r, float g, float b, float a)
{
    return ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

ImVec4 mColor(Color_t color) {
    return ImVec4(Color::TOFLOAT(color.r), Color::TOFLOAT(color.g), Color::TOFLOAT(color.b), Color::TOFLOAT(color.a));
}

Color_t vColor(ImVec4 color) {
    return {
        (byte)(color.x * 256.0f > 255 ? 255 : color.x * 256.0f),
        (byte)(color.y * 256.0f > 255 ? 255 : color.y * 256.0f),
        (byte)(color.z * 256.0f > 255 ? 255 : color.z * 256.0f),
        (byte)(color.w * 256.0f > 255 ? 255 : color.w * 256.0f)
    };
}

void ColorPicker(const char* label, Color_t& color) {
    ImVec4 FUCKOFF = mColor(color);
    ImGui::PushItemWidth(150);
    if (ImGui::ColorEdit4(label, &FUCKOFF.x)) {
        color = vColor(FUCKOFF);
    }
    ImGui::PopItemWidth();
}

Color_t* vpColor(ImVec4 color) {
    Color_t col = { (byte)(color.x * 255.0f),
        (byte)(color.y * 255.0f),
        (byte)(color.z * 255.0f),
        (byte)(color.w * 255.0f) };
    return &col;
}

float* cColor(ImVec4 color, Color_t& out) {
    out = *vpColor(color);

    return &color.x;
}

void plsfix(int lul) {
    ImGui::SameLine(ImGui::GetContentRegionMax().x - lul);
    ImGui::SetNextItemWidth(lul);
}

void CustomStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameBorderSize = 1.0f;

    style.Colors[ImGuiCol_Border] = ImVec4(0.24f, 0.24f, 0.24f, 0.33f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.08f, 0.09f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = nigger;

    style.Colors[ImGuiCol_Button] = ImVec4(0.07f, 0.08f, 0.09f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_MenuBarBg];

    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05, 0.05, 0.05, 1.00);
    style.Colors[ImGuiCol_ScrollbarGrab] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = style.Colors[ImGuiCol_MenuBarBg];
    style.Colors[ImGuiCol_ScrollbarGrabActive] = style.Colors[ImGuiCol_MenuBarBg];
}

bool InputKeybind(const char* label, CVar<int>& output, bool bAllowNone = true)
{
    bool active = false;

    auto VK2STR = [&](const short key) -> const char* {
        switch (key) {
        case VK_LBUTTON: return "M1";
        case VK_RBUTTON: return "M2";
        case VK_MBUTTON: return "M3";
        case VK_XBUTTON1: return "M4";
        case VK_XBUTTON2: return "M5";
        case VK_SPACE: return "SPC";
        case 0x0: return "None";
        case VK_A: return "A";
        case VK_B: return "B";
        case VK_C: return "C";
        case VK_D: return "D";
        case VK_E: return "E";
        case VK_F: return "F";
        case VK_G: return "G";
        case VK_H: return "H";
        case VK_I: return "J";
        case VK_J: return "I";
        case VK_K: return "K";
        case VK_L: return "L";
        case VK_M: return "M";
        case VK_N: return "N";
        case VK_O: return "O";
        case VK_P: return "P";
        case VK_Q: return "Q";
        case VK_R: return "R";
        case VK_S: return "S";
        case VK_T: return "T";
        case VK_U: return "U";
        case VK_V: return "V";
        case VK_W: return "W";
        case VK_X: return "X";
        case VK_Y: return "Y";
        case VK_Z: return "Z";
        case VK_0: return "0";
        case VK_1: return "1";
        case VK_2: return "2";
        case VK_3: return "3";
        case VK_4: return "4";
        case VK_5: return "5";
        case VK_6: return "6";
        case VK_7: return "7";
        case VK_8: return "8";
        case VK_9: return "9";
        case VK_ESCAPE: return "ESC";
        case VK_SHIFT: return "SHFT";
        case VK_LSHIFT: return "SHFT";
        case VK_RSHIFT: return "SHFT";
        case VK_CONTROL: return "CTRL";
        case VK_MENU: return "ALT";
        default: break;
        }

        WCHAR output[16] = { L"\0" };
        if (const int result = GetKeyNameTextW(MapVirtualKeyW(key, MAPVK_VK_TO_VSC) << 16, output, 16)) {
            char outputt[128];
            sprintf(outputt, "%ws", output);
            return outputt;
        }

        return "VK2STR_FAILED";
    };

    auto labell = VK2STR(output.m_Var);

    const auto id = ImGui::GetID(label);
    ImGui::PushID(label);

    //ImGui::TextUnformatted(label);
    //ImGui::SameLine();
    if (ImGui::GetActiveID() == id) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
        ImGui::Button("...", ImVec2(45, 17));
        ImGui::PopStyleColor();

static float time = g_Interfaces.Engine->Time();
float elapsed = g_Interfaces.Engine->Time() - time;
static CVar<int>* curr = nullptr, * prevv = curr;
if (curr != prevv) {
    time = g_Interfaces.Engine->Time();
    prevv = curr;
}

if (curr == nullptr && elapsed > 0.1f) {
    for (short n = 0; n < 256; n++) {
        if ((n > 0x0 && n < 0x7) || (n > L'A' - 1 && n < L'Z' + 1) || (n > L'0' - 1 && n < L'9' + 1) || n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT || n == VK_ESCAPE || n == VK_HOME || n == VK_CONTROL || n == VK_MENU) {
            if ((!ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[0])) {
                ImGui::ClearActiveID();
                break;
            }
            if (GetAsyncKeyState(n) & 0x8000)
            {
                if (n == VK_HOME || n == VK_INSERT) {
                    break;
                }

                if (n == VK_ESCAPE && bAllowNone) {
                    ImGui::ClearActiveID();
                    output.m_Var = 0x0;
                    break;
                }

                output.m_Var = n;
                ImGui::ClearActiveID();
                break;
            }
        } //loop
    }
}

if (curr != prevv) {
    time = g_Interfaces.Engine->Time();
    prevv = curr;
}

ImGui::GetCurrentContext()->ActiveIdAllowOverlap = true;
if ((!ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[0]))
ImGui::ClearActiveID();
    }
    else if (ImGui::Button(VK2STR(output.m_Var), ImVec2(45, 17))) {
    ImGui::SetActiveID(id, ImGui::GetCurrentWindow());
    }

    ImGui::PopID();

    return true;
}

void combo(const char* label, int* current_item, const char* const* items, int items_count, int popup_max_height_in_items = -1) {
    ImGui::PushItemWidth(100);
    ImGui::Combo(label, current_item, items, items_count, popup_max_height_in_items);
    ImGui::PopItemWidth();
}

ImVec4 clear_color = ImVec4(0.51f, 0.55f, 0.57f, 1.00f);
static bool one = true;
static bool two = false;
static int sl = 0;

void AimbotTab() {
    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);
        ImGui::MenuChild(_("Global"), ImVec2(250, 240), false);
        {
            ImGui::Checkbox(_("Enabled"), &Vars::Aimbot::Global::Active.m_Var);
            //ImGui::Checkbox("Autoshoot", &Vars::Aimbot::Global::AutoShoot.m_Var);
            plsfix(50);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(nigger.x / 1.5, nigger.y / 1.5, nigger.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(nigger.x, nigger.y, nigger.z, 255));
            InputKeybind(_("Aim key"), Vars::Aimbot::Global::AimKey);
            ImGui::PopStyleColor(3);
            ImGui::SliderFloat("Aimbot FOV", &Vars::Aimbot::Hitscan::AimFOV.m_Var, 1.0f, 180.f, "%.0f", 1.0f);
            ImGui::Checkbox(_("Aim at players"), &Vars::Aimbot::Global::AimPlayers.m_Var);
            ImGui::Checkbox(_("Aim at buildings"), &Vars::Aimbot::Global::AimBuildings.m_Var);
            ImGui::Checkbox(_("Ignore invulnerable"), &Vars::Aimbot::Global::IgnoreInvlunerable.m_Var);
            ImGui::Checkbox(_("Ignore cloaked"), &Vars::Aimbot::Global::IgnoreCloaked.m_Var);
            ImGui::Checkbox(_("Ignore friends"), &Vars::Aimbot::Global::IgnoreFriends.m_Var);
            ImGui::Checkbox(_("Ignore taunting"), &Vars::Aimbot::Global::IgnoreTaunting.m_Var);

        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }


    {//left bottom
        ImGui::SetCursorPosY(290);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);
        ImGui::MenuChild(_("Crithack"), ImVec2(250, 185), false);
        {
            static bool nigger = false;
            ImGui::Checkbox("Enabled", &nigger);
            if (nigger) {
                ImGui::Text("Why did you enable this you fucking tard");
                ImGui::Text("Have patience");
            }
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    
    {//right
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(270);
        ImGui::MenuChild(_("Hitscan"), ImVec2(250, 425), false);
        {
            ImGui::Checkbox(_("Active###Hit scan"), &Vars::Aimbot::Hitscan::Active.m_Var);
            static const char* hitscanSortMethod[]{ "FOV", "Distance" };
            ImGui::Combo(_("Sort method###hitscanSortMethod"), &Vars::Aimbot::Hitscan::SortMethod.m_Var, hitscanSortMethod, IM_ARRAYSIZE(hitscanSortMethod));

            static const char* hitscanAimMethod[]{ "Plain", "Smooth", "Silent" };
            ImGui::Combo(_("Aim method###hitscanAimMethod"), &Vars::Aimbot::Hitscan::AimMethod.m_Var, hitscanAimMethod, IM_ARRAYSIZE(hitscanAimMethod));

            static const char* hitscanAimHitbox[]{ "Head", "Body", "Auto" };
            ImGui::Combo(_("Hitbox###hitscanAimHitbox"), &Vars::Aimbot::Hitscan::AimHitbox.m_Var, hitscanAimHitbox, IM_ARRAYSIZE(hitscanAimHitbox));

            ImGui::SliderFloat(_("Smooth factor"), &Vars::Aimbot::Hitscan::SmoothingAmount.m_Var, 1.0f, 10.f, "%.0f", 1.0f);

            static const char* hitscanTapFire[]{ "Off", "Distance", "Always" };
            ImGui::Combo(_("Tap-fire"), &Vars::Aimbot::Hitscan::TapFire.m_Var, hitscanTapFire, IM_ARRAYSIZE(hitscanTapFire));
            ImGui::Checkbox(_("Minigun rev up"), &Vars::Aimbot::Hitscan::AutoRev.m_Var);
            ImGui::Checkbox(_("Multipoint"), &Vars::Aimbot::Hitscan::ScanHitboxes.m_Var);
            //ImGui::Checkbox("Head multipoint", &Vars::Aimbot::Hitscan::ScanHead.m_Var);
            ImGui::Checkbox(_("Wait for headshot"), &Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var);
            ImGui::Checkbox(_("Wait for charge"), &Vars::Aimbot::Hitscan::WaitForCharge.m_Var);
            ImGui::Checkbox(_("Scoped only"), &Vars::Aimbot::Hitscan::ScopedOnly.m_Var);
            ImGui::Checkbox(_("Scope automatically"), &Vars::Aimbot::Hitscan::AutoScope.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//right
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(525);
        ImGui::MenuChild(_("Projectile"), ImVec2(260, 166), false);
        {
            ImGui::Checkbox(_("Active###Projectile"), &Vars::Aimbot::Projectile::Active.m_Var);
            //ImGui::Checkbox(_("Performance mode"), &Vars::Aimbot::Projectile::PerformanceMode.m_Var);
            //static const char* projectileSortMethod[]{ "FOV", "Distance" };
            //ImGui::Combo(_("Sort method###projectileSortMethod"), &Vars::Aimbot::Projectile::SortMethod.m_Var, projectileSortMethod, IM_ARRAYSIZE(projectileSortMethod));

            static const char* projectileAimMethod[]{ "Plain", "Silent" };
            ImGui::Combo(_("Aim method###projectileAimMethod"), &Vars::Aimbot::Projectile::AimMethod.m_Var, projectileAimMethod, IM_ARRAYSIZE(projectileAimMethod));

            static const char* projectileAimHitbox[]{ "Body", "Feet", "Auto" };
            ImGui::Combo(_("Aim position###projectileAimPosition"), &Vars::Aimbot::Projectile::AimPosition.m_Var, projectileAimHitbox, IM_ARRAYSIZE(projectileAimHitbox));

            //ImGui::Checkbox(_("Aim at feet if on ground (Demoman)"), &Vars::Aimbot::Projectile::FeetAimIfOnGround.m_Var);
            //ImGui::SliderFloat(_("Aim FOV###projectileFOV"), &Vars::Aimbot::Projectile::AimFOV.m_Var, 1.0f, 180.f, "%.0f", 1.0f);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//right
        ImGui::SetCursorPosY(215);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(525);
        ImGui::MenuChild(_("Melee"), ImVec2(260, 260), false);
        {
            ImGui::Checkbox(_("Active###Melee"), &Vars::Aimbot::Melee::Active.m_Var);
            static const char* meleeSortMethod[]{ "FoV", "Distance" }; 
            ImGui::Combo(_("Sort method###meleeSortMethod"), &Vars::Aimbot::Melee::SortMethod.m_Var, meleeSortMethod, IM_ARRAYSIZE(meleeSortMethod));
            static const char* meleeAimMethod[]{ "Plain", "Smooth", "Silent" }; 
            ImGui::Combo(_("Aim method###meleeAimMethod"), &Vars::Aimbot::Melee::AimMethod.m_Var, meleeAimMethod, IM_ARRAYSIZE(meleeAimMethod));
            //ImGui::SliderFloat(_("Aim FoV###meleeFoV"), &Vars::Aimbot::Melee::AimFOV.m_Var, 1.0f, 180.f, "%.0f", 1.0f);
            ImGui::SliderFloat(_("Smooth factor###meleeSmooth"), &Vars::Aimbot::Melee::SmoothingAmount.m_Var, 1.0f, 10.f, "%.0f", 1.0f);
            ImGui::Checkbox(_("Range check"), &Vars::Aimbot::Melee::RangeCheck.m_Var);
            ImGui::Checkbox(_("Melee prediction"), &Vars::Aimbot::Melee::PredictSwing.m_Var);
            ImGui::Checkbox(_("Whip teammates"), &Vars::Aimbot::Melee::WhipTeam.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    /*
    {//right
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(320);
        ImGui::MenuChild(_("Settings"), ImVec2(285, 415), false);
        {
            ImGui::Checkbox(_("Checkbox enabled"), &one);
            plsfix(23);
            ImGui::ColorEdit3(_("lol"), (float*)&nigger, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview);
            //ImGui::ColorEdit3(_("lol"), (float*)&ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview);
            ImGui::Checkbox(_("Checkbox disabled"), &two);
            ImGui::SliderInt(_("Slider"), &sl, -20, 20, "%d", ImGuiSliderFlags_AlwaysClamp);
            ImGui::Checkbox(_("Enable function"), &one);

            if (ImGui::BeginCombo(_("Other"), _("None"), 0, true))
            {
                ImGui::Checkbox(_("Enable function"), &one);
                ImGui::SliderInt(_("Slider"), &sl, -20, 20);
                ImGui::SliderInt(_("Slider"), &sl, -20, 20);
                ImGui::Checkbox(_("Enable function"), &one);
                ImGui::EndCombo();
            }
            ImGui::Selectable(_("Selectable enabled"), &one);
            ImGui::Selectable(_("Selectable disabled"), &two);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    */
}

void TriggerbotTab() {
    {//Left Upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);
        ImGui::MenuChild(_("Global"), ImVec2(220, 200), false);
        {
            ImGui::Checkbox(_("Enabled"), &Vars::Triggerbot::Global::Active.m_Var);
            plsfix(50);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(nigger.x / 1.5, nigger.y / 1.5, nigger.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(nigger.x, nigger.y, nigger.z, 255));
            InputKeybind(_("Trigger key"), Vars::Triggerbot::Global::TriggerKey);
            ImGui::PopStyleColor(3);
            ImGui::Checkbox(_("Shoot players##gASsp"), &Vars::Triggerbot::Shoot::TriggerPlayers.m_Var);
            ImGui::Checkbox(_("Ignore Invulnerable"), &Vars::Triggerbot::Global::IgnoreInvlunerable.m_Var);
            ImGui::Checkbox(_("Ignore Cloaked"), &Vars::Triggerbot::Global::IgnoreCloaked.m_Var);
            ImGui::Checkbox(_("Ignore Friends"), &Vars::Triggerbot::Global::IgnoreFriends.m_Var);
            ImGui::Checkbox(_("Wait for charge##gASwfc"), &Vars::Triggerbot::Shoot::WaitForCharge.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    
    { // Left Middle
        ImGui::SetCursorPosY(250);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);

        ImGui::MenuChild(_("Auto Airblast"), ImVec2(220, 110), false);
        ImGui::Checkbox(_("Active##gAAB"), &Vars::Triggerbot::Blast::Active.m_Var);
        ImGui::Checkbox(_("Rage mode##gAABr"), &Vars::Triggerbot::Blast::Rage.m_Var);
        ImGui::Checkbox(_("Silent##gAABs"), &Vars::Triggerbot::Blast::Silent.m_Var);

        ImGui::EndChild();
        ImGui::EndGroup();
    }
    { // Left Bottom
        ImGui::SetCursorPosY(360);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);

        ImGui::MenuChild(_("Auto Detonator"), ImVec2(220, 110), false);
        ImGui::Checkbox("Active###gAD", &Vars::Triggerbot::Detonate::Active.m_Var);
        ImGui::Checkbox("Stickybombs###gADs", &Vars::Triggerbot::Detonate::Stickies.m_Var);
        ImGui::Checkbox("Detonator flares###gADd", &Vars::Triggerbot::Detonate::Flares.m_Var);
        //ImGui::SliderFloat("Detonate radius###gADr", &Vars::Triggerbot::Detonate::RadiusScale.m_Var, 0.5f, 1.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    // Left middle row
    /*
    { // Left Middle Upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(220);

        ImGui::MenuChild(_("Auto Shoot"), ImVec2(200, 200), false);
        //ImGui::Checkbox(_("Active##gAS"), &Vars::Triggerbot::Shoot::Active.m_Var);
        ImGui::Checkbox(_("Shoot players##gASsp"), &Vars::Triggerbot::Shoot::TriggerPlayers.m_Var);
        //ImGui::Checkbox(_("Shoot buildings##gASsb"), &Vars::Triggerbot::Shoot::TriggerBuildings.m_Var);
        ImGui::Checkbox(_("Head only##gASho"), &Vars::Triggerbot::Shoot::HeadOnly.m_Var);
        //ImGui::SliderFloat(_("Head scale##gAShs"), &Vars::Triggerbot::Shoot::HeadScale.m_Var, 0.5f, 1.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
        ImGui::Checkbox(_("Wait for charge##gASwfc"), &Vars::Triggerbot::Shoot::WaitForCharge.m_Var);


        ImGui::EndChild();
        ImGui::EndGroup();
    }
    */
    { // Left Middle Upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(240);

        ImGui::MenuChild(_("Auto Stab"), ImVec2(220, 200), false);
        ImGui::Checkbox(_("Active###gABS"), &Vars::Triggerbot::Stab::Active.m_Var);
        ImGui::Checkbox(_("Rage mode###gABSr"), &Vars::Triggerbot::Stab::RageMode.m_Var);
        ImGui::Checkbox(_("Silent###gABSs"), &Vars::Triggerbot::Stab::Silent.m_Var);
        ImGui::Checkbox(_("Disguise after stab###gABSd"), &Vars::Triggerbot::Stab::Disguise.m_Var);
        ImGui::Checkbox(_("Ignore razorback###gABSig"), &Vars::Triggerbot::Stab::IgnRazor.m_Var);
        ImGui::SliderFloat(_("Stab range"), &Vars::Triggerbot::Stab::Range.m_Var, 0.5f, 1.0f, "%.1f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_AlwaysClamp);


        ImGui::EndChild();
        ImGui::EndGroup();
    }
    { // Left Middle Middle
        ImGui::SetCursorPosY(250);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(240);

        ImGui::MenuChild(_("Auto Uber"), ImVec2(220, 160), false);
        ImGui::Checkbox(_("Active###gAU"), &Vars::Triggerbot::Uber::Active.m_Var);
        ImGui::Checkbox(_("Only on friends###gAUf"), &Vars::Triggerbot::Uber::OnlyFriends.m_Var);
        ImGui::Checkbox(_("Uber self###gAUs"), &Vars::Triggerbot::Detonate::Flares.m_Var);
        ImGui::SliderFloat(_("Health left###gAUhp"), &Vars::Triggerbot::Uber::HealthLeft.m_Var, 1.f, 99.f, "%.0f%%", ImGuiSliderFlags_AlwaysClamp);

        ImGui::EndChild();
        ImGui::EndGroup();
    }
}
int whatthefuck = 1;

void ESPTab() {
    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);
        ImGui::MenuChild(_("Player"), ImVec2(253, 425), false);
        {
            ImGui::Checkbox(_("Enabled"), &Vars::ESP::Players::Active.m_Var);
            ImGui::Checkbox(_("Local ESP"), &Vars::ESP::Players::ShowLocal.m_Var);
            //ImGui::Checkbox(_("Player health"), &Vars::ESP::Players::Health.m_Var);
            ImGui::Checkbox(_("Player health bar"), &Vars::ESP::Players::HealthBar.m_Var);
            ImGui::Checkbox(_("Player conditions"), &Vars::ESP::Players::Cond.m_Var);
            ImGui::Checkbox(_("Player name"), &Vars::ESP::Players::Name.m_Var);
            ImGui::Checkbox(_("Player Dlights"), &Vars::ESP::Players::Dlights.m_Var);

            static const char* ignoreTeammatesEsp[]{ "Off", "All", "Keep friends" };
            ImGui::Combo(_("Ignore teammates###ESPteam"), &Vars::ESP::Players::IgnoreTeammates.m_Var, ignoreTeammatesEsp, IM_ARRAYSIZE(ignoreTeammatesEsp));

            static const char* ignoreCloakedEsp[]{ "Off", "All", "Enemies only" };
            ImGui::Combo(_("Ignore cloaked###ESPcloak"), &Vars::ESP::Players::IgnoreCloaked.m_Var, ignoreCloakedEsp, IM_ARRAYSIZE(ignoreCloakedEsp));

            static const char* classEsp[]{ "Off", "Icon", "Text" };
            ImGui::Combo(_("Player class"), &Vars::ESP::Players::Class.m_Var, classEsp, IM_ARRAYSIZE(classEsp));

            static const char* uberESP[]{ "Off", "Text", "Bar" }; 
            ImGui::Combo(_("Player ubercharge"), &Vars::ESP::Players::Uber.m_Var, uberESP, IM_ARRAYSIZE(uberESP));

            static const char* boxESP[]{ "Off", "Simple", "Cornered", "3D" }; 
            ImGui::Combo(_("Player box"), &Vars::ESP::Players::Box.m_Var, boxESP, IM_ARRAYSIZE(boxESP));

            ImGui::SliderFloat(_("Dlight radius"), &Vars::ESP::Players::DlightRadius.m_Var, 5.0f, 400.f, "%.0f", ImGuiSliderFlags_Logarithmic);

        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(273);
        ImGui::MenuChild(_("Buildings"), ImVec2(253, 425), false);
        {
            ImGui::Checkbox("Enabled", &Vars::ESP::Buildings::Active.m_Var);
            ImGui::Checkbox("Ignore team buildings###ESPbuildingsteam", &Vars::ESP::Buildings::IgnoreTeammates.m_Var);
            ImGui::Checkbox("Building name", &Vars::ESP::Buildings::Name.m_Var);
            ImGui::Checkbox("Building health", &Vars::ESP::Buildings::Health.m_Var);
            ImGui::Checkbox("Building health bar", &Vars::ESP::Buildings::HealthBar.m_Var);
            ImGui::Checkbox("Building conditions", &Vars::ESP::Buildings::Cond.m_Var);
            ImGui::Checkbox("Building level", &Vars::ESP::Buildings::Level.m_Var);
            ImGui::Checkbox("Building owner", &Vars::ESP::Buildings::Owner.m_Var);

            static const char* boxESPb[]{ "Off", "Simple", "Cornered", "3D" };
            ImGui::Combo("Building box", &Vars::ESP::Buildings::Box.m_Var, boxESPb, IM_ARRAYSIZE(boxESPb));

            ImGui::Checkbox("Building Dlights", &Vars::ESP::Buildings::Dlights.m_Var);
            ImGui::PushItemWidth(100); ImGui::SliderFloat("Dlight radius###buildingsdlights", &Vars::ESP::Buildings::DlightRadius.m_Var, 5.0f, 400.f, "%.0f", ImGuiSliderFlags_Logarithmic);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(532);
        ImGui::MenuChild(_("World"), ImVec2(253, 425), false);
        {
            ImGui::Checkbox("Pickups ESP", &Vars::ESP::World::Active.m_Var);
            ImGui::Checkbox("Healthpack ESP", &Vars::ESP::World::HealthText.m_Var);
            ImGui::Checkbox("Ammopack ESP", &Vars::ESP::World::AmmoText.m_Var);
            ImGui::Checkbox("World modulation", &Vars::Visuals::WorldModulation.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    //plsfix(10);
    ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - 43);
    if (ImGui::Button(">>", ImVec2(35,0)))
        whatthefuck = 2;
}

void ESPTab2() { // Chams

    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);
        ImGui::MenuChild(_("Player Chams"), ImVec2(253, 425), false);
        {
            ImGui::Checkbox("Player chams", &Vars::Chams::Players::Active.m_Var);
            ImGui::Checkbox("Local chams", &Vars::Chams::Players::ShowLocal.m_Var);
            ImGui::Checkbox("Chams on cosmetics", &Vars::Chams::Players::Wearables.m_Var);
            ImGui::Checkbox("Chams on weapons", &Vars::Chams::Players::Weapons.m_Var);
            ImGui::Checkbox("IgnoreZ", &Vars::Chams::Players::IgnoreZ.m_Var);
            static const char* pchamsMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Fresnel" };
            ImGui::Combo("Player material", &Vars::Chams::Players::Material.m_Var, pchamsMaterials, IM_ARRAYSIZE(pchamsMaterials));
            static const char* ignoreTeammatesChams[]{ "Off", "All", "Keep friends" };
            ImGui::Combo("Ignore teammates###chamsteam", &Vars::Chams::Players::IgnoreTeammates.m_Var, ignoreTeammatesChams, IM_ARRAYSIZE(ignoreTeammatesChams)); 
            ImGui::SliderFloat("Player chams opacity", &Vars::Chams::Players::Alpha.m_Var, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(273);
        ImGui::MenuChild(_("Building Chams"), ImVec2(253, 425), false);
        {
            ImGui::Checkbox("Building chams", &Vars::Chams::Buildings::Active.m_Var);
            ImGui::Checkbox("Ignore team buildings###Chamsbuildingsteam", &Vars::Chams::Buildings::IgnoreTeammates.m_Var);
            ImGui::Checkbox("IgnoreZ###buildingsignorez", &Vars::Chams::Buildings::IgnoreZ.m_Var);
            static const char* pchamsbMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Fresnel" };
            ImGui::Combo("Building material", &Vars::Chams::Buildings::Material.m_Var, pchamsbMaterials, IM_ARRAYSIZE(pchamsbMaterials)); 
            ImGui::SliderFloat("Building chams opacity", &Vars::Chams::Buildings::Alpha.m_Var, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(532);
        ImGui::MenuChild(_("World Chams"), ImVec2(253, 425), false);
        {
            ImGui::Checkbox("Pickup chams", &Vars::Chams::World::Active.m_Var);
            ImGui::Checkbox("Healthpack chams", &Vars::Chams::World::Health.m_Var);
            ImGui::Checkbox("Ammopack chams", &Vars::Chams::World::Ammo.m_Var);
            ImGui::Checkbox("IgnoreZ###pickupsignorez", &Vars::Chams::World::IgnoreZ.m_Var);
            static const char* projectilesTeam[]{ "Off", "All", "Enemy Only" };
            ImGui::Combo("Projectile chams", &Vars::Chams::World::Projectiles.m_Var, projectilesTeam, IM_ARRAYSIZE(projectilesTeam));
            static const char* pchamspMaterials[]{ "None", "Shaded", "Shiny", "Flat", "Fresnel" };
            ImGui::Combo("Pickup chams material", &Vars::Chams::World::Material.m_Var, pchamspMaterials, IM_ARRAYSIZE(pchamspMaterials));
            //ImGui::Checkbox("", &Vars::Chams::World::Material.m_Var);
            ImGui::SliderFloat("Pickup chams opacity", &Vars::Chams::World::Alpha.m_Var, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::SetCursorPosX(15);
    if (ImGui::Button("<<", ImVec2(35, 0)))
        whatthefuck = 1;

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - 43);
    if (ImGui::Button(">>", ImVec2(35, 0)))
        whatthefuck = 3;
}

void ESPTab3() {
    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);
        ImGui::MenuChild(_("Player Glow"), ImVec2(253, 425), false);
        {
            //ImGui::SliderInt("Global glow scale", &Vars::Glow::Main::Scale.m_Var, 1, 10, "%d", ImGuiSliderFlags_Logarithmic);
            ImGui::Checkbox("Player glow", &Vars::Glow::Players::Active.m_Var);
            static const char* ignoreTeammatesGlow[]{ "Off", "All", "Keep friends" };
            ImGui::Combo("Ignore teammates###glowteam", &Vars::Glow::Players::IgnoreTeammates.m_Var, ignoreTeammatesGlow, IM_ARRAYSIZE(ignoreTeammatesGlow));
            ImGui::Checkbox("Glow on cosmetics", &Vars::Glow::Players::Wearables.m_Var);
            ImGui::Checkbox("Glow on weapons", &Vars::Glow::Players::Weapons.m_Var);
            ImGui::SliderFloat("Player glow opacity", &Vars::Glow::Players::Alpha.m_Var, 0.1f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            static const char* colorGlow[]{ "Team", "Health" };
            ImGui::Combo("Player glow color", &Vars::Glow::Players::Color.m_Var, colorGlow, IM_ARRAYSIZE(colorGlow));
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(273);
        ImGui::MenuChild(_("Building Glow"), ImVec2(253, 425), false);
        {
            ImGui::Checkbox("Building glow", &Vars::Glow::Buildings::Active.m_Var);
            ImGui::Checkbox("Ignore team buildings###glowbuildingsteam", &Vars::Glow::Buildings::IgnoreTeammates.m_Var);
            //static const char* ignoreTeammatesGlow[]{ "Off", "All", "Keep friends" }; ImGui::PushItemWidth(100); ImGui::Combo("Ignore teammates###glowteam", &Vars::Glow::Buildings::IgnoreTeammates.m_Var, ignoreTeammatesGlow, IM_ARRAYSIZE(ignoreTeammatesGlow)); ImGui::PopItemWidth(); HelpMarker("Which teammates the glow will ignore drawing on");
            ImGui::SliderFloat("Building glow opacity", &Vars::Glow::Buildings::Alpha.m_Var, 0.1f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
            static const char* colorGlow[]{ "Team", "Health" }; 
            ImGui::Combo("Building glow color", &Vars::Glow::Buildings::Color.m_Var, colorGlow, IM_ARRAYSIZE(colorGlow));
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(532);
        ImGui::MenuChild(_("World Glow"), ImVec2(253, 425), false);
        {
            ImGui::Checkbox("Pickup glow", &Vars::Glow::World::Active.m_Var);
            ImGui::Checkbox("Healthpack glow", &Vars::Glow::World::Health.m_Var);
            ImGui::Checkbox("Ammopack glow", &Vars::Glow::World::Ammo.m_Var);
            static const char* projectilesgTeam[]{ "Off", "All", "Enemy Only" };
            ImGui::Combo("Projectile glow", &Vars::Glow::World::Projectiles.m_Var, projectilesgTeam, IM_ARRAYSIZE(projectilesgTeam));
            ImGui::SliderFloat("Pickup glow opacity", &Vars::Glow::World::Alpha.m_Var, 0.1f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::SetCursorPosX(15);
    if (ImGui::Button("<<", ImVec2(35, 0)))
        whatthefuck = 2;
}

void VisualsTab() {

    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);
        ImGui::MenuChild(_("Local"), ImVec2(253, 425), false);
        {
            ImGui::SliderInt("Field of view", &Vars::Visuals::FieldOfView.m_Var, 70, 150, "%d");
            ImGui::SliderFloat("Aspect Ratio", &Vars::Visuals::AspectRatioValue.m_Var, 0.f, 200.f, "%.f");
            ImGui::Checkbox("Remove scope", &Vars::Visuals::RemoveScope.m_Var);
            ImGui::Checkbox("Remove zoom", &Vars::Visuals::RemoveZoom.m_Var);
            ImGui::Checkbox("Remove recoil", &Vars::Visuals::RemovePunch.m_Var);
            ImGui::Checkbox("Aimbot crosshair", &Vars::Visuals::CrosshairAimPos.m_Var);
            ImGui::Checkbox("Chat info", &Vars::Visuals::ChatInfo.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(273);
        ImGui::MenuChild(_("Spywarning"), ImVec2(253, 175), false);
        {
            ImGui::Checkbox("Active###spywarn", &Vars::Visuals::SpyWarning.m_Var);
            ImGui::Checkbox("Voice command###spywarn1", &Vars::Visuals::SpyWarningAnnounce.m_Var);
            ImGui::Checkbox("Visible only###spywarn2", &Vars::Visuals::SpyWarningVisibleOnly.m_Var);
            ImGui::Checkbox("Ignore friends###spywarn3", &Vars::Visuals::SpyWarningIgnoreFriends.m_Var);
            const char* spyWmodes[]{ "Arrow", "Flash" };
            ImGui::Combo("Warning style", &Vars::Visuals::SpyWarningStyle.m_Var, spyWmodes, IM_ARRAYSIZE(spyWmodes));
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//left upper
        ImGui::SetCursorPosY(225);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(273);
        ImGui::MenuChild(_("Thirdperson"), ImVec2(253, 115), false);
        {
            ImGui::Checkbox("Thirdperson", &Vars::Visuals::ThirdPerson.m_Var);
            plsfix(50);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(nigger.x / 1.5, nigger.y / 1.5, nigger.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(nigger.x, nigger.y, nigger.z, 255));
            InputKeybind("Thirdperson key", Vars::Visuals::ThirdPersonKey);
            ImGui::PopStyleColor(3);
            ImGui::Checkbox("Show silent angles", &Vars::Visuals::ThirdPersonSilentAngles.m_Var);
            ImGui::Checkbox("Instant yaw", &Vars::Visuals::ThirdPersonInstantYaw.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }

    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(532);
        ImGui::MenuChild(_("Radar"), ImVec2(253, 425), false);
        {
            ImGui::Checkbox("Active###radar", &Vars::Radar::Main::Active.m_Var);
            ImGui::Checkbox("Players###radarp", &Vars::Radar::Players::Active.m_Var);
            ImGui::Checkbox("Buildings###radarbuildingsa", &Vars::Radar::Buildings::Active.m_Var);
            //ImGui::Checkbox("World###radarworldd", &Vars::Radar::World::Active.m_Var);
            //ImGui::Checkbox("Health###radarworldda", &Vars::Radar::World::Health.m_Var);
            //ImGui::Checkbox("Ammo###radarworlddb", &Vars::Radar::World::Active.m_Var);
            ImGui::SliderInt("Radar size", &Vars::Radar::Main::Size.m_Var, 20, 200);
            ImGui::SliderInt("Radar range", &Vars::Radar::Main::Range.m_Var, 50, 3000, "%d");
            ImGui::SliderInt("Radar background alpha", &Vars::Radar::Main::BackAlpha.m_Var, 0, 255, "%d");

            ImGui::Checkbox("Player Health bar###radarhealt", &Vars::Radar::Players::Health.m_Var);
            ImGui::Checkbox("Buildings Health bar###radarbuildingsc", &Vars::Radar::Buildings::Health.m_Var);
            const char* iconPlayersRadar[]{ "Scoreboard", "Portraits", "Avatar" };
            ImGui::Combo("Icon###radari", &Vars::Radar::Players::IconType.m_Var, iconPlayersRadar, IM_ARRAYSIZE(iconPlayersRadar));
            static const char* ignoreTeammatespRadar[]{ "Off", "All", "Keep friends" };
            ImGui::Combo("Ignore teammates###radarplayersteam", &Vars::Radar::Players::IgnoreTeam.m_Var, ignoreTeammatespRadar, IM_ARRAYSIZE(ignoreTeammatespRadar));
            ImGui::Checkbox("Ignore team building###radarbuildingsb", &Vars::Radar::Buildings::IgnoreTeam.m_Var); 

        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
}

void MiscTab() {
    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);
        ImGui::MenuChild(_("General"), ImVec2(300, 210), false);
        {
            ImGui::Checkbox(_("Taunt Slide"), &Vars::Misc::TauntSlide.m_Var);
            ImGui::Checkbox(_("Taunt Control"), &Vars::Misc::TauntControl.m_Var);
            ImGui::Checkbox(_("Bypass sv_pure"), &Vars::Misc::BypassPure.m_Var);
            ImGui::Checkbox(_("Medal flip"), &Vars::Misc::MedalFlip.m_Var);
            ImGui::Checkbox(_("Noisemaker spam"), &Vars::Misc::NoisemakerSpam.m_Var);
            ImGui::Checkbox(_("Chat Spam"), &Vars::Misc::ChatSpam.m_Var);
            ImGui::Checkbox(_("No Push"), &Vars::Misc::NoPush.m_Var);
            ImGui::Checkbox(_("No Interp"), &Vars::Misc::DisableInterpolation.m_Var);
            //ImGui::Checkbox(_("Anti AFK"), &Vars::Misc::NoPush.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//left bottom
        ImGui::SetCursorPosY(260);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);
        ImGui::MenuChild(_("Movement"), ImVec2(300, 215), false);
        {
            ImGui::Checkbox(_("Bhop"), &Vars::Misc::AutoJump.m_Var);
            ImGui::Checkbox(_("AutoStrafer"), &Vars::Misc::AutoStrafe.m_Var);
            ImGui::Checkbox(_("Auto RocketJump"), &Vars::Misc::AutoRocketJump.m_Var);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//right
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(320);
        ImGui::MenuChild(_("HvH (autism)"), ImVec2(300, 280), false);
        {
            ImGui::Checkbox(_("AntiAim"), &Vars::AntiHack::AntiAim::Active.m_Var);
            const char* pitch[]{ _("None"), _("Up"), _("Down"), _("Fake up"), _("Fake down") }; ImGui::Combo(_("Pitch"), &Vars::AntiHack::AntiAim::Pitch.m_Var, pitch, IM_ARRAYSIZE(pitch));
            const char* realYaw[]{ _("None"), _("Left"), _("Right"), _("Backwards") }; ImGui::Combo(_("Real yaw"), &Vars::AntiHack::AntiAim::YawReal.m_Var, realYaw, IM_ARRAYSIZE(realYaw));
            const char* fakeYaw[]{ _("None"), _("Left"), _("Right"), _("Backwards") }; ImGui::Combo(_("Fake yaw"), &Vars::AntiHack::AntiAim::YawFake.m_Var, fakeYaw, IM_ARRAYSIZE(fakeYaw));

            ImGui::Checkbox(_("Fakelag"), &Vars::Misc::CL_Move::Fakelag.m_Var);
            plsfix(50);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(nigger.x / 1.5, nigger.y / 1.5, nigger.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(nigger.x, nigger.y, nigger.z, 255));
            InputKeybind(_("Fakelag key"), Vars::Misc::CL_Move::FakelagKey);
            ImGui::PopStyleColor(3);
            ImGui::Checkbox(_("Fakelag on key"), &Vars::Misc::CL_Move::FakelagOnKey.m_Var);
            ImGui::SliderInt(_("Fakelag value"), &Vars::Misc::CL_Move::FakelagValue.m_Var, 1, 14, _("%d"), ImGuiSliderFlags_AlwaysClamp);
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    {//right
        ImGui::SetCursorPosY(330);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(320);
        ImGui::MenuChild(_("Exploits"), ImVec2(300, 144), false);
        {
            ImGui::Checkbox(_("DoubleTap"), &Vars::Misc::CL_Move::Doubletap.m_Var);
            plsfix(50);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(nigger.x / 1.5, nigger.y / 1.5, nigger.z / 1.5, 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(nigger.x, nigger.y, nigger.z, 255));
            InputKeybind(_("DoubleTap Key"), Vars::Misc::CL_Move::DoubletapKey);
            ImGui::SetCursorPosX(8);
            ImGui::Text(_("Recharge Key"));
            plsfix(50);
            InputKeybind(_("Recharge Key"), Vars::Misc::CL_Move::RechargeKey);
            ImGui::PopStyleColor(3);

        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
}

char buffer[256];

void ConfigsTab() {
    {//left upper
        ImGui::SetCursorPosY(50);
        ImGui::BeginGroup();
        ImGui::SetCursorPosX(15);
        ImGui::MenuChild(_("Configs"), ImVec2(300, 425), false);
        {
            ImGui::SetCursorPosX(5);
            static std::wstring selected = {};
            int nConfig = 0;

            for (const auto& entry : std::filesystem::directory_iterator(g_CFG.m_sConfigPath)) {
                if (std::string(std::filesystem::path(entry).filename().string()).find(_(".CAM")) == std::string_view::npos)
                {
                    continue;
                }
                nConfig++;
            }

            for (const auto& entry : std::filesystem::directory_iterator(g_CFG.m_sConfigPath)) {
                if (std::string(std::filesystem::path(entry).filename().string()).find(_(".CAM")) == std::string_view::npos) {
                    continue;
                }

                std::wstring s = entry.path().filename().wstring();
                s.erase(s.end() - 4, s.end());

                std::string configName(s.begin(), s.end());

                ImGui::ListBoxHeader(_("##cfglist"), ImVec2(-5, -110));

                if (ImGui::Selectable(configName.c_str(), s == selected))
                    selected = s;

                ImGui::ListBoxFooter();
                //if (ImGui::Button(configName.c_str()))
                    //selected = s;
            }

            if (nConfig < 100) {
                std::string output = {};

                //ImGui::PushItemWidth(140 - 15);
                ImGui::SetCursorPosY(320);
                ImGui::SetCursorPosX(5);
                ImGui::PushItemWidth(ImGui::GetContentRegionMax().x - 10);
                if (ImGui::InputText(_("###configname"), &output, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    std::wstring outstring(output.begin(), output.end());
                    if (!std::filesystem::exists(g_CFG.m_sConfigPath + L"\\" + outstring)) {

                        g_CFG.Save(outstring.c_str());
                    }
                }
                ImGui::PopItemWidth();
            }

            ImGui::SetCursorPosX(5);

            if (ImGui::Button(_("Save"), { ImGui::GetContentRegionMax().x - 10, 0})) {
                g_CFG.Save(selected.c_str());
                selected.clear();
                sprintf_s(buffer, _("Config %s saved!"), selected.data());
                g_Visuals.vecEventVector.push_back(EventLogging_t{ buffer });
            }
            ImGui::SetCursorPosX(5);
            if (ImGui::Button(_("Load"), { ImGui::GetContentRegionMax().x - 10, 0 })) {
                g_CFG.Load(selected.c_str());
                selected.clear();
                sprintf_s(buffer, _("Config %s loaded!"), selected.data());
                g_Visuals.vecEventVector.push_back(EventLogging_t{ buffer });
            }
            ImGui::SetCursorPosX(5);
            if (ImGui::Button(_("Delete"), { ImGui::GetContentRegionMax().x - 10, 0 })) {
                g_CFG.Remove(selected.c_str());
                selected.clear();
                sprintf_s(buffer, _("Config %s deleted!"), selected.data());
                g_Visuals.vecEventVector.push_back(EventLogging_t{ buffer });
            }
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::SetCursorPos({ ImGui::GetContentRegionMax().x - 90, ImGui::GetContentRegionMax().y - 25});
    if (ImGui::Button(_("Unload Cheat"))) {
        sprintf_s(buffer, _("Unloading..."));
        g_Visuals.vecEventVector.push_back(EventLogging_t{ buffer });
        g_GlobalInfo.Unload = true;
    }
}

IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
std::vector<float> frames;

void CNMenu::Render(IDirect3DDevice9* pDevice) {
	static bool bInitImGui = false;
	static bool bColumnsWidthened = false;
	bool modified_custom_style = false;

    //backup render states
    DWORD colorwrite, srgbwrite;
    pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
    pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);
    pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state);
    pDevice->GetVertexDeclaration(&vertDec);
    pDevice->GetVertexShader(&vertShader);

    //fix drawing without calling engine functons/cl_showpos
    pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
    //removes the source engine color correction
    pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

    static LPDIRECT3DTEXTURE9 lg = nullptr;
	if (!bInitImGui) {
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(FindWindowA(0, _("Team Fortress 2")));
		ImGui_ImplDX9_Init(pDevice);

        if (lg == nullptr) {
            D3DXCreateTextureFromFileInMemory(pDevice, &logo, sizeof(logo), &lg);
        }
        auto& io = ImGui::GetIO();

        ImFontConfig font_config;
        font_config.OversampleH = 1; //or 2 is the same
        font_config.OversampleV = 1;
        font_config.PixelSnapH = 1;

        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x044F, // Cyrillic
            0,
        };
        name = io.Fonts->AddFontFromMemoryTTF((void*)MuseoFont, sizeof(MuseoFont), 19.0f, &font_config, ranges);
        icons = io.Fonts->AddFontFromMemoryTTF((void*)icon, sizeof(icon), 44, &font_config, ranges);
        font = io.Fonts->AddFontFromMemoryTTF((void*)MuseoFont, sizeof(MuseoFont), 13.0f, &font_config, ranges);
        bInitImGui = true;
	}

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		g_Interfaces.Surface->SetCursorAlwaysVisible(menuOpen = !menuOpen);
		//menuOpen = !menuOpen;
		g_Menu.flTimeOnChange = g_Interfaces.Engine->Time();

	}
	g_Menu.m_flFadeElapsed = g_Interfaces.Engine->Time() - g_Menu.flTimeOnChange;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

    ImGui::GetIO().MouseDrawCursor = menuOpen;
	if (menuOpen)
	{
        CustomStyle(); // I know it's probably a bad idea to do this in a loop but oh well you got to sacrifice some stuff sometimes just to get the perfect result.

        auto s = ImVec2{}, p = ImVec2{}, gs = ImVec2{ 800, 510 };
        ImGui::SetNextWindowSize(ImVec2(gs));
        /*
        ImGui::PushFont(font);

        ImGui::Begin(_("lol"), NULL);
        int fps = ImGui::GetIO().Framerate;

        //Get frames
        if (frames.size() > 100) //Max seconds to show
        {
            for (size_t i = 1; i < frames.size(); i++)
            {
                frames[i - 1] = frames[i];
            }
            frames[frames.size() - 1] = fps;
        }
        else
        {
            frames.push_back(fps);
        }
        char text[20];
        sprintf_s(text, 20, _("Frames: %d"), fps);
        ImGui::Text(text);
        ImGui::Text(_("FPS Average: %.f"), 1000.0f / ImGui::GetIO().Framerate);
        ImGui::PlotHistogram(_("Histogram"), &frames[0], frames.size(), 0, NULL, 0.0f, 1000.0f, ImVec2(0, 80.0f));
        ImGui::End();
        ImGui::PopFont();
        ImGui::ShowDemoWindow();
        */
        ImGui::Begin(_("##GUI"), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        {
            {
                nigger = ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg];
                s = ImVec2(ImGui::GetWindowSize().x - ImGui::GetStyle().WindowPadding.x * 2, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2); p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y); auto draw = ImGui::GetWindowDrawList();
                draw->AddRectFilled(p, ImVec2(p.x + s.x, p.y + s.y), ImColor(15, 17, 19, 240), 5);
                draw->AddRectFilled(ImVec2(p.x, p.y + 40), ImVec2(p.x + s.x, p.y + s.y), ImColor(18, 20, 21, 200), 5, ImDrawCornerFlags_Bot);

                ImGui::PushFont(name);
                draw->AddText(ImVec2(p.x + 36 / 2, p.y + 29 / 2), ImColor(255,255,255,255), _("ChadAlphaMales"));
                draw->AddText(ImVec2(p.x + 36 / 2 + ImGui::CalcTextSize(_("ChadAlphaMales")).x, p.y + 29 / 2), ImColor(nigger.x, nigger.y, nigger.z, 255.f), _(".club"));
                ImGui::PopFont();
            }
            {
                ImGui::PushFont(font);
                //ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - 75 * 2);
                ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 75 * 6.7); // 4 = how much tabs, .5 = how much tabs + 0.1
                ImGui::BeginGroup();
                if (ImGui::tab(_("Aimbot"), tab == 0))tab = 0; ImGui::SameLine();
                if (ImGui::tab(_("Triggerbot"), tab == 1))tab = 1; ImGui::SameLine();
                if (ImGui::tab(_("ESP"), tab == 2))tab = 2; ImGui::SameLine();
                if (ImGui::tab(_("Visuals"), tab == 3))tab = 3; ImGui::SameLine();
                if (ImGui::tab(_("Misc"), tab == 4))tab = 4; ImGui::SameLine();
                if (ImGui::tab(_("Config"), tab == 5))tab = 5;
                ImGui::EndGroup();
                ImGui::PopFont();
            }
            ImGui::PushFont(font);
            {
                switch (tab) {
                    case 0: {
                        AimbotTab();
                        break;
                    }
                    case 1: {
                        TriggerbotTab();
                        break;
                    }
                    case 2: {
                        // Probably inefficient but oh well lol.
                        if (whatthefuck == 1)
                            ESPTab();
                        if (whatthefuck == 2)
                            ESPTab2();
                        if (whatthefuck == 3)
                            ESPTab3();

                        break;
                    }


                    case 3: {
                        VisualsTab();
                        break;
                    }
                    case 4: {
                        MiscTab();
                        break;
                    }
                    case 5: {
                        ConfigsTab();
                        break;
                    }
                    default: {
                        ImGui::SetCursorPosX(15);
                        ImGui::Text(_("How the fuck did you manage to get here?\nInvalid tab value : 5 / %d"), tab);
                    }
                }
                ImGui::PopFont();
            }
            ImGui::End();
        }
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    //draw here
    pixel_state->Apply();
    pixel_state->Release();
    pDevice->SetVertexDeclaration(vertDec);
    pDevice->SetVertexShader(vertShader);
    pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
    pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
}
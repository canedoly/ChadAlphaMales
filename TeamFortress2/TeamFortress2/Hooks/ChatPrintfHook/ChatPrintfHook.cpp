#include "ChatPrintfHook.h"

void SanitizeString(std::string& stringValue) {
    for (auto i = stringValue.begin();;) {
        auto const pos = std::find_if(
            i, stringValue.end(),
            [](char const c) { return '\\' == c || '\'' == c || '"' == c; }
        );
        if (pos == stringValue.end()) {
            break;
        }
        i = std::next(stringValue.insert(pos, '\\'), 2);
    }

    stringValue.erase(
        std::remove_if(
            stringValue.begin(), stringValue.end(), [](char const c) {
                return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c;
            }
        ),
        stringValue.end()
                );
};

void __fastcall ChatPrintfHook::Hook(void* ecx, void* edx, int nIndex, int nFilter, const char* fmt, ...) {
    static auto oChatPrintf = Func.Original<fn>();

    if (strlen(fmt) != 0) {
        return oChatPrintf(ecx, edx, nIndex, nFilter, fmt);
    }

    std::string new_ret = fmt;

    SanitizeString(new_ret);

    oChatPrintf(ecx, edx, nIndex, nFilter, new_ret.c_str());
}

void ChatPrintfHook::Init() {
    fn FN = reinterpret_cast<fn>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC B8 ? ? ? ? E8 ? ? ? ? 53 56 57 8D 45 18")));
    Func.Hook(FN, Hook);
}
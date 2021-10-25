#include "ChatPrintfHook.h"

void ReplaceString(std::string& input, const std::string& what, const std::string& with_what)
{
    size_t index;
    index = input.find(what);
    while (index != std::string::npos)
    {
        input.replace(index, what.size(), with_what);
        index = input.find(what, index + with_what.size());
    }
}

void __fastcall ChatPrintfHook::Hook(void* ecx, void* edx, int nIndex, int nFilter, const char* fmt, ...) {
    static auto oChatPrintf = Func.Original<fn>();

    auto buf = std::make_unique<char[]>(1024);
    va_list list;
    va_start(list, fmt);
    vsprintf(buf.get(), fmt, list);
    va_end(list);

    std::string result = buf.get();
    ReplaceString(result, "\n", "");
    ReplaceString(result, "\r", "");
    return oChatPrintf(ecx, edx, nIndex, nFilter, result.c_str());
}

void ChatPrintfHook::Init() {
    fn FN = reinterpret_cast<fn>(g_Pattern.Find(_(L"client.dll"), _(L"55 8B EC B8 ? ? ? ? E8 ? ? ? ? 53 56 57 8D 45 18")));
    Func.Hook(FN, Hook);
}
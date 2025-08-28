#include "ConsoleManager.h"

void ConsoleManager::Init(Prism* pPrism) {

    mPrism = pPrism;

    AllocConsole();

    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

}
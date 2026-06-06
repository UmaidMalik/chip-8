#pragma once

class Emulator;

#ifdef __EMSCRIPTEN__
void SetActiveEmulator(Emulator* emulator);
#endif
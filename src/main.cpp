#include "emulator.hpp"
#include "logger.hpp"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        logger::Error("Usage: Chip8 <rom-file>");
        return 1;
    }

    Emulator emulator;

    if (!emulator.Setup(argv[1]))
    {
        logger::Error("Failed to initialize emulator");
        return 1;
    }

    emulator.Run();
    return 0;
}
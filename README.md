# CHIP-8 Emulator

A cross-platform CHIP-8 emulator written in modern C++ using SDL3.

The project builds on Linux and Windows through CMake and includes automated GitHub Actions workflows for continuous integration and tagged releases.

## Features

* CHIP-8 instruction execution
* 64 × 32 monochrome display
* SDL3 window, rendering, and keyboard input
* Delay and sound timer handling
* Configurable CPU and display timing
* Command-line ROM loading
* Linux and Windows builds
* Included CHIP-8 diagnostic ROMs
* Automated CI and GitHub Releases

## Download

Prebuilt Linux and Windows packages are available from the repository's GitHub Releases page.

Each release contains:

### Linux

```text
chip8-vX.Y.Z-linux-x86_64/
├── Chip8
├── README.md
├── LICENSE
├── THIRD_PARTY_LICENSES.md
└── rom/
    ├── 1-chip8-logo.ch8
    ├── 2-ibm-logo.ch8
    ├── 3-corax+.ch8
    ├── 4-flags.ch8
    ├── 5-quirks.ch8
    ├── 6-keypad.ch8
    ├── 7-beep.ch8
    ├── 8-scrolling.ch8
    └── test_opcode.ch8
```

### Windows

```text
chip8-vX.Y.Z-windows-x86_64/
├── Chip8.exe
├── README.md
├── LICENSE
├── THIRD_PARTY_LICENSES.md
└── rom/
    ├── 1-chip8-logo.ch8
    ├── 2-ibm-logo.ch8
    ├── 3-corax+.ch8
    ├── 4-flags.ch8
    ├── 5-quirks.ch8
    ├── 6-keypad.ch8
    ├── 7-beep.ch8
    ├── 8-scrolling.ch8
    └── test_opcode.ch8
```

## Running the Emulator

The emulator expects the ROM path as its command-line argument.

### Linux

```bash
chmod +x Chip8
./Chip8 rom/1-chip8-logo.ch8
```

For example:

```bash
./Chip8 rom/3-corax+.ch8
./Chip8 rom/5-quirks.ch8
./Chip8 rom/test_opcode.ch8
```

### Windows PowerShell

```powershell
.\Chip8.exe .\rom\1-chip8-logo.ch8
```

For example:

```powershell
.\Chip8.exe .\rom\3-corax+.ch8
.\Chip8.exe .\rom\5-quirks.ch8
.\Chip8.exe .\rom\test_opcode.ch8
```

Paths containing spaces should be placed in quotation marks:

```powershell
.\Chip8.exe "C:\Users\Name\My ROMs\game.ch8"
```

## Included Diagnostic ROMs

The included ROMs are intended for testing emulator correctness:

| ROM                | Purpose                              |
| ------------------ | ------------------------------------ |
| `1-chip8-logo.ch8` | Basic CHIP-8 splash-screen test      |
| `2-ibm-logo.ch8`   | IBM logo rendering test              |
| `3-corax+.ch8`     | Opcode behavior test                 |
| `4-flags.ch8`      | Arithmetic and flag behavior test    |
| `5-quirks.ch8`     | CHIP-8 compatibility-quirk test      |
| `6-keypad.ch8`     | Key input test                       |
| `7-beep.ch8`       | Sound timer and beep test            |
| `8-scrolling.ch8`  | Scrolling and extended behavior test |
| `test_opcode.ch8`  | Additional opcode test ROM           |

The diagnostic ROMs do not guarantee that every CHIP-8 program will behave correctly. Different CHIP-8 interpreters use different compatibility quirks.

## Keyboard Layout

The original CHIP-8 keypad:

```text
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F
```

Typical computer keyboard mapping:

```text
1 2 3 4
Q W E R
A S D F
Z X C V
```

Update this section if the emulator currently uses a different mapping.

## Building from Source

### Requirements

* C++23-compatible compiler
* CMake
* SDL3
* fmt
* Git

SDL3 and fmt are included as Git submodules.

Clone the repository recursively:

```bash
git clone --recursive <repository-url>
cd chip-8
```

For an existing clone:

```bash
git submodule update --init --recursive
```

### Linux

```bash
cmake -S . -B build-linux -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-linux --parallel
```

Run:

```bash
./build-linux/bin/Release/Chip8 rom/1-chip8-logo.ch8
```

The executable location may vary depending on the CMake generator and configuration.

### Windows

From PowerShell or a Visual Studio developer terminal:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
```

Run:

```powershell
.\build\bin\Release\Chip8.exe .\rom\1-chip8-logo.ch8
```

## Continuous Integration

GitHub Actions builds the emulator on:

* Ubuntu
* Windows

Each successful CI run produces downloadable platform-specific build artifacts containing:

* The emulator executable
* This README
* Third-party licensing information
* The included test ROMs

Version tags matching `v*`, such as `v0.1.0`, create a GitHub Release with Linux and Windows packages.

## Third-Party Test ROMs

The test ROMs in `rom/` are redistributed for emulator development and verification.

ROMs `1-chip8-logo.ch8` through `8-scrolling.ch8` originate from the Timendus CHIP-8 test suite and are distributed under GPLv3.

`test_opcode.ch8` originates from corax89's CHIP-8 test ROM project and is distributed under the MIT License.

See `THIRD_PARTY_LICENSES.md` and the upstream projects for complete attribution and license information.

## License

See the repository's `LICENSE` file.

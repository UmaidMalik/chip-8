# CHIP-8 Emulator

A cross-platform CHIP-8 emulator written in modern C++ using SDL3.

The emulator runs natively on Linux and Windows and can also run directly in the browser through WebAssembly and Emscripten.

## Try It Online

**[Launch the CHIP-8 Emulator in your browser](https://umaidmalik.github.io/chip-8/)**

The browser version includes several diagnostic ROMs and also allows you to upload a local CHIP-8 ROM from your computer.

## Features

* CHIP-8 instruction execution
* 64 × 32 monochrome display
* SDL3 window, rendering, and keyboard input
* Command-line ROM loading on Linux and Windows
* Local ROM uploads in the browser
* Bundled diagnostic ROM selector
* Linux x86-64 builds
* Windows x86-64 builds
* WebAssembly browser build
* Automated continuous integration
* Automated GitHub Releases
* Automated GitHub Pages deployment

## Platforms

| Platform | Format                   |
| -------- | ------------------------ |
| Browser  | WebAssembly              |
| Linux    | Native x86-64 executable |
| Windows  | Native x86-64 executable |

## Browser Version

The web version is compiled from the same C++ source code using Emscripten.

It supports:

* Running bundled diagnostic ROMs
* Uploading local ROM files
* ROM files with extensions such as `.ch8`
* Extensionless ROM files such as `PONG`
* Keyboard input through the browser
* SDL3 rendering through an HTML canvas

To upload a ROM:

1. Open the browser emulator.
2. Select **Choose File**.
3. Choose a CHIP-8 ROM from your computer.
4. Click the emulator canvas to give it keyboard focus.

The uploaded file stays in the browser session and is not sent to a server.

## Download

Prebuilt Linux and Windows packages are available from the repository’s GitHub Releases page.

Each release contains the emulator executable, documentation, licenses, and bundled diagnostic ROMs.

### Linux Package

```text
chip8-vX.Y.Z-linux-x86_64/
├── Chip8
├── README.md
├── LICENSE
├── THIRD_PARTY_LICENSES.md
├── licenses/
└── roms/
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

### Windows Package

```text
chip8-vX.Y.Z-windows-x86_64/
├── Chip8.exe
├── README.md
├── LICENSE
├── THIRD_PARTY_LICENSES.md
├── licenses/
└── roms/
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

## Running the Native Emulator

The Linux and Windows executables expect the ROM path as a command-line argument.

ROM filenames do not need to have a `.ch8` extension.

### Linux

```bash
chmod +x Chip8
./Chip8 roms/1-chip8-logo.ch8
```

Other examples:

```bash
./Chip8 roms/3-corax+.ch8
./Chip8 roms/5-quirks.ch8
./Chip8 roms/test_opcode.ch8
./Chip8 /path/to/PONG
```

Paths containing spaces should be placed in quotation marks:

```bash
./Chip8 "/home/user/My ROMs/PONG"
```

### Windows PowerShell

```powershell
.\Chip8.exe .\roms\1-chip8-logo.ch8
```

Other examples:

```powershell
.\Chip8.exe .\roms\3-corax+.ch8
.\Chip8.exe .\roms\5-quirks.ch8
.\Chip8.exe .\roms\test_opcode.ch8
.\Chip8.exe C:\path\to\PONG
```

Paths containing spaces should be placed in quotation marks:

```powershell
.\Chip8.exe "C:\Users\Name\My ROMs\PONG"
```

## Included Diagnostic ROMs

The bundled ROMs are intended for testing emulator behavior and compatibility.

| ROM                | Purpose                              |
| ------------------ | ------------------------------------ |
| `1-chip8-logo.ch8` | Basic CHIP-8 splash-screen test      |
| `2-ibm-logo.ch8`   | IBM logo rendering test              |
| `3-corax+.ch8`     | Opcode behavior test                 |
| `4-flags.ch8`      | Arithmetic and flag behavior test    |
| `5-quirks.ch8`     | CHIP-8 compatibility-quirk test      |
| `6-keypad.ch8`     | Keyboard input test                  |
| `7-beep.ch8`       | Sound timer and beep test            |
| `8-scrolling.ch8`  | Scrolling and extended behavior test |
| `test_opcode.ch8`  | Additional opcode test ROM           |

Passing these tests does not guarantee that every CHIP-8 program will behave identically. Different CHIP-8 interpreters support different compatibility quirks.

## Keyboard Layout

The original CHIP-8 keypad:

```text
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F
```

Computer keyboard mapping:

```text
1 2 3 4
Q W E R
A S D F
Z X C V
```

Click the emulator window or browser canvas before using the keyboard controls.

## Building from Source

### Requirements

For native builds:

* C++23-compatible compiler
* CMake
* Ninja or Visual Studio
* Git

For browser builds:

* Emscripten SDK
* CMake
* Ninja
* Python 3 for local hosting

SDL3 and fmt are included as Git submodules.

Clone the repository recursively:

```bash
git clone --recursive https://github.com/umaid54321/chip-8.git
cd chip-8
```

For an existing clone:

```bash
git submodule update --init --recursive
```

## Linux Build

Configure:

```bash
cmake \
    -S . \
    -B build-linux \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release
```

Build:

```bash
cmake --build build-linux --parallel
```

Run:

```bash
./build-linux/bin/Release/Chip8 roms/1-chip8-logo.ch8
```

The exact executable location may vary depending on the selected CMake generator and output configuration.

## Windows Build

From PowerShell or a Visual Studio developer terminal:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
```

Run:

```powershell
.\build\bin\Release\Chip8.exe .\roms\1-chip8-logo.ch8
```

## Browser and WebAssembly Build

Install and activate the Emscripten SDK before configuring the project.

Configure:

```bash
emcmake cmake \
    -S . \
    -B build-web \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release
```

Build:

```bash
cmake --build build-web --parallel
```

Locate the generated page:

```bash
find build-web -name index.html
```

The output includes files similar to:

```text
index.html
index.js
index.wasm
index.data
style.css
app.js
```

The `.data` file contains the bundled ROM resources.

### Run the Web Build Locally

Serve the output through HTTP instead of opening `index.html` directly.

For example:

```bash
python3 -m http.server 8000 --directory build-web/bin/Release
```

Then open:

```text
http://localhost:8000/
```

Adjust the served directory if CMake generated the files in a different location.

## Project Structure

```text
chip-8/
├── include/
├── src/
├── roms/
├── web/
│   ├── shell.html
│   ├── app.js
│   └── style.css
├── licenses/
├── thirdparty/
│   ├── SDL/
│   └── fmt/
├── .github/
│   └── workflows/
├── CMakeLists.txt
├── README.md
├── LICENSE
└── THIRD_PARTY_LICENSES.md
```

## Third-Party Test ROMs

The test ROMs in `roms/` are redistributed for emulator development and verification.

ROMs `1-chip8-logo.ch8` through `8-scrolling.ch8` originate from the Timendus CHIP-8 test suite and are distributed under GPLv3.

`test_opcode.ch8` originates from corax89’s CHIP-8 test ROM project and is distributed under the MIT License.

See `THIRD_PARTY_LICENSES.md`, the files under `licenses/`, and the upstream projects for complete attribution and licensing information.

## License

The CHIP-8 emulator source code is available under the license in the repository’s `LICENSE` file.

Third-party ROMs and dependencies remain subject to their respective licenses.

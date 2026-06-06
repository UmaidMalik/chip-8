"use strict";

const bundledRomSelect =
    document.getElementById("bundled-rom");

const loadBundledRomButton =
    document.getElementById("load-bundled-rom");

const romUpload =
    document.getElementById("rom-upload");

const canvas =
    document.getElementById("canvas");

const statusElement =
    document.getElementById("status");

let runtimeReady = false;

window.addEventListener("chip8-runtime-ready", () => {
    runtimeReady = true;
    canvas.focus();
});

function setStatus(message) {
    statusElement.textContent = message;
}

function loadRomFromVirtualPath(path) {
    if (!runtimeReady) {
        setStatus("The emulator is still starting.");
        return false;
    }

    if (typeof Module.ccall !== "function") {
        setStatus(
            "ROM switching is not exported from C++ yet."
        );
        return false;
    }

    const result = Module.ccall(
        "load_rom_from_path",
        "number",
        ["string"],
        [path]
    );

    if (result === 1) {
        setStatus(`Loaded ${path}`);
        canvas.focus();
        return true;
    }

    setStatus(`Failed to load ${path}`);
    return false;
}

loadBundledRomButton.addEventListener("click", () => {
    loadRomFromVirtualPath(bundledRomSelect.value);
});

romUpload.addEventListener("change", async () => {
    const file = romUpload.files?.[0];

    if (!file) {
        return;
    }

    if (!runtimeReady) {
        setStatus("The emulator is still starting.");
        return;
    }

    try {
        const bytes =
            new Uint8Array(await file.arrayBuffer());

        const safeName =
            file.name.replace(/[^a-zA-Z0-9._-]/g, "_");

        const virtualPath =
            `/uploads/${Date.now()}-${safeName}`;

        try {
            FS.mkdir("/uploads");
        } catch (error) {
            // The directory already exists.
        }

        FS.writeFile(virtualPath, bytes);

        loadRomFromVirtualPath(virtualPath);
    } catch (error) {
        console.error(error);
        setStatus(`Could not read ${file.name}`);
    }
});

canvas.addEventListener("click", () => {
    canvas.focus();
});
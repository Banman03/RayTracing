# RayTracing
A ray tracing and Monte Carlo path tracing engine implemented in C++.

Includes Lambert diffuse, Blinn-Phong, soft shadows, Whitted recursive ray tracing, and a full path tracer with Next Event Estimation.

---

## Dependencies

You need two things: a **C++17 compiler** and **SDL2** (for the display window).

---

## Linux

**Check if you have them:**
```bash
g++ --version        # need g++ 7 or later
sdl2-config --version
```

**Install if missing (Debian / Ubuntu / Mint):**
```bash
sudo apt update
sudo apt install g++ libsdl2-dev
```

**Install if missing (Fedora / RHEL / CentOS):**
```bash
sudo dnf install gcc-c++ SDL2-devel
```

**Install if missing (Arch / Manjaro):**
```bash
sudo pacman -S gcc sdl2
```

---

## macOS

**Check if you have them:**
```bash
g++ --version        # or clang++ --version — either works
sdl2-config --version
```

**Install the compiler (if missing):**
```bash
xcode-select --install
```
This installs Apple's Clang, which the Makefile will use automatically.

**Install SDL2 via Homebrew (if missing):**
```bash
# Install Homebrew first if you don't have it:
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

brew install sdl2
```

**Apple Silicon (M1/M2/M3) note:** if `sdl2-config` is not found after installing, run:
```bash
echo 'export PATH="/opt/homebrew/bin:$PATH"' >> ~/.zshrc && source ~/.zshrc
```

---

## Windows

The easiest route on Windows is **WSL2** (Windows Subsystem for Linux), which gives you a full Linux environment. Then follow the Linux instructions above.

**Enable WSL2 and install Ubuntu (run in PowerShell as Administrator):**
```powershell
wsl --install
```
Restart when prompted, then open the Ubuntu app and follow the Linux instructions above.

**Alternative — MSYS2 (native Windows build):**
```bash
# Inside the MSYS2 MinGW64 shell:
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 make
```
Then build from the MSYS2 MinGW64 shell using the same `make` command below.

---

## Building

Once dependencies are installed, build both programs from the repository root:

```bash
make
```

This produces two binaries: `raytrace` and `pathtrace`.

To build only one:
```bash
make raytrace
make pathtrace
```

To clean build artifacts:
```bash
make clean
```

---

## Running

**Ray tracer** (Lambert, Blinn-Phong, soft shadows, Whitted):
```bash
./raytrace <scene> <algorithm>
```

```bash
./raytrace ring lambert
./raytrace ring phong
./raytrace ring soft
./raytrace ring whitted
```

**Path tracer** (full global illumination with NEE):
```bash
./pathtrace <scene>
```

```bash
./pathtrace ring
./pathtrace single
```

Available scenes: `ring`, `single`, `random_field`, `solar_system`

**Navigation:** use the arrow keys to move the light position. The path tracer accumulates samples progressively — let it run to watch convergence from noisy to clean.

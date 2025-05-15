# 🎮 Mosaic Game Engine

**Mosaic** is a modern, cross-platform game engine written in **C++20**, built from the ground up as both a learning journey and a foundation for something greater.

Like all of my projects, Mosaic is first and foremost an exploration: a deep dive into systems programming, computer graphics, and game architecture. But it's not a toy. The engine is designed with the long-term goal of becoming a stable, professional-grade platform for interactive experiences — games, simulations, creative tools, and beyond.

Unlike many hobby engines that wrap existing libraries, Mosaic takes a bottom-up approach. I'm building its **core systems from scratch** — from a cache-friendly **Entity-Component-System (ECS)** and **layered input system**, to a **Vulkan/WebGPU-based renderer**, an **async-aware I/O layer**, and eventually a full **editor and asset pipeline**. It's about understanding, not just using.

Mosaic is still under heavy development — but the foundation is solid, and contributions or discussions are always welcome.

---

## ✨ Goals and Highlights

- 🧱 **Build it to understand it** — nearly every core system is handcrafted, from ECS to rendering.
- 🌍 **Cross-platform** — supports Windows, Linux, and the Web (via WebGPU).
- 🎨 **Modern graphics** — Vulkan backend, with a future-facing WebGPU renderer for browser targets.
- 🧠 **Layered architecture** — clean separation of engine, editor, runtime, and testbed.
- 🖱 **Input like Flutter/Unity** — rich input system with context-aware, stateful bindings.
- 🔄 **Sync + Async APIs** — inspired by Dart’s ergonomic approach to I/O.
- 🧪 **Stress-tested** — with unit tests and integration tests covering critical systems.
- 🛠 **CMake + vcpkg** — modern and maintainable build setup, with submodules where needed.

---

## 🛠 Getting Started

Mosaic uses CMake as its build system. Most dependencies are managed via vcpkg, while those unavailable through vcpkg — such as WebGPU, utility libraries, or components requiring special build profiles — are included as submodules under the vendor/ directory.

On Windows, we recommend using Visual Studio 2022 over VS Code for better CMake integration. The default compiler on Windows is MSVC, but Clang is also supported — and is the preferred compiler on Linux and Web targets (via Emscripten).

### 🔧 Build Instructions

- Clone the repository and initialize submodules. `git clone --recurse-submodules https://github.com/WilliamKarolDiCioccio/mosaic`.

- Select a CMake configure preset, then a build preset (the vcpkg toolchain manages installation automatically).

- Build the project — and you're good to go!

> [!WARNING]
> When building for the Web, you might encounter issues related to threading support.
>
> Mosaic uses the -pthread flag in Emscripten to enable Web Workers, which helps retain compatibility with native threading code. Occasionally, you may need to manually patch the build system to support this.
>
> If the build fails with missing atomic features, add the following flag manually in the emdawnwebgpu.port.py file located at:
>
> out/build/emscripten-{PROFILE}/\_deps/emdawnwebgpu-src/
>
> Update the flags section as follows:
>
> ```py
> flags = ['-g', '-std=c++17', '-fno-exceptions']
> flags += ['-matomics', '-mbulk-memory'] # <-- Add this line
> flags += \_compute_library_compile_flags(settings)
>
> This ensures compatibility with multi-threaded WebAssembly targets.
> ```

---

## 📄 License

Mosaic comes under the permissive MIT License to encourage contributions. See the [`LICENSE.md`](./LICENSE.md) file for more information.

---

Thanks for stopping by! 🌟

_— Di Cioccio William Karol_

# 🎮 Mosaic Game Engine

**Mosaic** is a modern, cross-platform game engine written in **C++23**, built both as a learning journey and as the foundation for something greater.

Like all of my projects, Mosaic is first and foremost an exploration — a deep dive into systems programming, computer graphics, and game architecture. But it’s not just a toy. The engine is designed with the long-term goal of becoming a stable, professional-grade platform for interactive experiences: games, simulations, creative tools, and more.

Unlike many hobby engines that simply wrap existing libraries, Mosaic takes a bottom-up approach. Its **core systems are built from scratch** — from a cache-friendly **Entity-Component-System (ECS)** and **layered input system**, to a **Vulkan/WebGPU-based renderer**, an **async-aware I/O layer**, and eventually a full **editor and asset pipeline**. It’s about understanding, not just using.

Mosaic is still under heavy development, but the foundation is solid — and contributions or discussions are always welcome.

For documentation, guides, and API references, visit the [Mosaic Docs](https://williamkaroldicioccio.github.io/mosaic/).

---

## ✨ Goals & Highlights

- 🧱 **From the ground up** — handcrafted core systems, from ECS to rendering.
- 🌍 **Cross-platform** — runs on Windows, Linux, Android, and the Web (via WebGPU).
- 🎨 **Modern graphics** — Vulkan backend with a WebGPU renderer for browser targets.
- 🧠 **Modern architecture** — clean separation of engine, editor, runtime, and games; multi-threaded, ECS-centric design leveraging modern CPUs.
- 🖱 **Intuitive input** — context-aware, stateful bindings inspired by Flutter and Unity.
- 🔄 **Sync & async APIs** — ergonomic I/O modeled after Dart’s async system.
- 🧪 **Well-tested** — unit tests, integration tests, and benchmarks ensure stability under stress.
- 🛠 **CMake + vcpkg** — maintainable build setup with submodules where needed.

---

Thanks for stopping by! 🌟

_— Di Cioccio William Karol_

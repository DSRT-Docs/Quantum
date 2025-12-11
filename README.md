# DSRT Engine

**DSRT Engine** is a modern game engine built from scratch without external dependencies, 
designed for high performance and full control over every aspect of rendering and simulation.

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/dsrt-engine/dsrt)
[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE.md)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20Android%20%7C%20macOS-lightgrey)](Platform.md)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue)](https://en.cppreference.com/w/cpp/17)
[![Code Quality](https://img.shields.io/badge/code%20quality-professional-green)](https://github.com/dsrt-engine/dsrt)

## Features
- **Zero Dependencies**: No external libraries, full control over every component
- **Multi-Platform**: Windows, Linux, Android, and macOS support
- **Modern Rendering**: Custom renderer with render graph system
- **ECS Architecture**: Entity Component System for game logic
- **Built-in Scripting**: Custom VM for game scripting
- **Full Toolchain**: Asset compiler, shader compiler, and editor

## Quick Start

### Prerequisites
- C++17 compatible compiler
- CMake 3.15 or higher

### Building
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

Running Sample

```bash
./bin/DSRT_Editor
```

Project Structure

```
DSRT/
├── Engine/          # Core engine modules
├── Editor/          # Editor application
├── Runtime/         # Runtime launcher
├── Docs/            # Documentation
├── Samples/         # Example projects
└── Tests/           # Test suite
```

Contributing

See CONTRIBUTING.md for contribution guidelines.

License

This project is licensed under the MIT License - see LICENSE.md for details.

## `CHANGELOG.md`
```markdown
# Changelog

All notable changes to DSRT Engine will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2024-01-01
### Added
- Initial release of DSRT Engine
- Core engine architecture
- Multi-platform support
- Custom renderer with render graph
- ECS system implementation
- Built-in scripting VM
- Full-featured editor
- Asset pipeline tools

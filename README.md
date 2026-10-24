# GuitarTools2

A guitar-focused mixing processor built with C++ and JUCE. It combines tone-range filtering, targeted clean-up, tone shaping and level matching in one plugin, so a guitar track can be framed and tidied in a single pass.

## Sections

- **Tone Range**: *Low Cut* and *High Cut* filters with 12, 24, 36 or 48 dB/oct slopes.
- **Clean-Up**: *Kill Reso* cuts a narrow harsh resonance, *Clear Mud* reduces low-mid buildup, and *Tame Lows* compresses only the low band to even out the low end. Each has a *MAX* switch for a stronger setting.
- **Color**: *Depth* (low shelf) and *Presence* (high shelf), each with four modes that set where the shelf works.
- **Levels**: input and output gain with peak meters, plus *Match Loudness*, which keeps the output level equal to the input for honest A/B comparisons.

Also included: a preset browser with factory and user presets, undo/redo, a *Low Tuning* mode that makes the plugin work in lower frequency areas, a resizable interface, an optional tooltip strip and a global bypass.

## Formats

- macOS: AU and VST3
- Windows: VST3

The Windows installer is not code-signed. If Windows shows "Windows protected your PC", click *More info*, then *Run anyway*.

## Building

Requires CMake 3.22 or newer and a C++17 compiler. JUCE is included as a Git submodule.

```bash
git clone --recurse-submodules https://github.com/aaronpetrini96/GuitarToolsV2.git
cd GuitarToolsV2
```

macOS:

```bash
cmake --preset mac_ninja_multi
cmake --build --preset mac_ninja_multi_release
```

Windows (from a Visual Studio Developer Command Prompt):

```bash
cmake --preset win_ninja_multi
cmake --build --preset win_ninja_multi_release
```

The built plugins are copied to the system plugin folders automatically.

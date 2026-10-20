# Master Studio Ultimate — prototype 0.1

A cross-platform JUCE plug-in scaffold for Windows and macOS. It builds as VST3 and AU (AU on macOS), plus Standalone for quick testing.

## Included in this prototype
- Real-time audio processing: high-pass filter, compressor, soft saturation, output level.
- Host-automatable parameters and host state save/restore.
- Simple, responsive starter UI.
- Clean project structure intended for separate future modules.

## Not yet included
- **Pitch correction / Auto-Tune is not implemented in this starter.** It needs a dedicated pitch-detection and pitch-shifting engine, plus latency and artifact testing. The piano/scale UI and offline note editor should be added alongside that engine rather than pretending that a selector alone corrects audio.
- No claim of production readiness until tested in each target DAW and on supported hardware.

## Build
1. Install a C++ toolchain:
   - Windows: Visual Studio 2022 with Desktop development with C++ and CMake.
   - macOS: Xcode command-line tools and CMake.
2. Download JUCE from the official JUCE website and note its folder path.
3. Configure:
   `cmake -S . -B build -DJUCE_DIR=/absolute/path/to/JUCE`
4. Build:
   `cmake --build build --config Release`
5. Find the generated VST3/AU artifacts in the build tree. Test first in a DAW's plug-in sandbox or a plug-in host.

AU is only available on macOS. VST3 availability depends on the host DAW and system configuration.

## Audio-thread notes
The audio callback avoids UI work and dynamic allocation. Any new pitch engine must preserve those real-time constraints.

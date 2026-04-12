# Desktop Audio Mixer (DesktopAM)

A desktop application for managing audio channels with a graphical interface. Built with SFML for rendering and PortAudio for audio I/O.

---

## Features

- Multi-scene UI: Main Menu, Mixer, and Settings
- Per-channel volume and mute controls
- Icon-based navigation buttons
- Custom font rendering with SFML

---

## Dependencies

- [SFML](https://www.sfml-dev.org/) — window creation, rendering, and input
- [PortAudio](https://www.portaudio.com/) — audio stream management

---

## Project Structure

```
DesktopAM/
├── main.cpp              # Entry point
├── uimanager.h/.cpp      # Handles scenes, rendering, and input
├── audiomixer.h/.cpp     # Manages PortAudio initialization and channels
├── global.hpp            # Window constants and color definitions
├── DesktopAM.slnx        # Visual Studio solution file
└── res/
    ├── fonts/
    │   ├── Sekuya.ttf
    │   └── Nunito.ttf
    └── icons/
        ├── mixer_icon.png
        ├── menu_icon.png
        └── settings_icon.png
```

---

## Building
### Windows
- Open the Visual Studio project
- Ensure all the libraries are linked correctly
- Enable proper post-build commands (copying dlls and resource files)
- Build

### Linux
- Clone repo
- Ensure all the libraries are installed and in a dir called "lib" in the project root directory
- Build
- Copy required so and resource files

---

## Scenes

| Scene | Description |
|---|---|
| Main Menu | Landing screen with buttons to open Mixer or Settings |
| Mixer | Audio channel controls |
| Settings | Application settings |

Use the menu button (top-right) to return to the Main Menu from any scene.

---

## Audio Channels

Each channel tracks a name (e.g. "game", "chat", "media"), a volume value from 0.0 to 1.0, a mute state, and a PortAudio stream.

---

## Configuration

Window size and scene colors are set in `global.hpp`.

| Constant | Value |
|---|---|
| WINDOW_WIDTH | 800 |
| WINDOW_HEIGHT | 600 |
| Menu color | Gray (150, 150, 150) |
| Mixer color | Red (255, 0, 0) |
| Settings color | Blue (0, 0, 255) |

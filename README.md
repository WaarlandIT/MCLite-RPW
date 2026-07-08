# MCLite-RPW

Fork of [MCLite](https://github.com/laserir/MCLite) for the LilyGo T-Deck Plus, with changes by PA3RPW.

Lightweight off-grid communicator firmware built on [MeshCore](https://github.com/ripplebiz/MeshCore).

## Changes from upstream

- Hamburger menu button (☰) in status bar → opens admin hub
- Screen-off on keyboard lock — only a key press wakes it
- WiFi/BLE companion toggles persist across reboots
- Radio region/scope setting persists across reboots

## Install

### Dependencies

- [Python 3](https://python.org) + PlatformIO: `pip install platformio`
- Or [VS Code](https://code.visualstudio.com/) with PlatformIO IDE extension

### Build & Flash

```
git clone https://gitlab.waarland.it/Workers_bot/MCLite.git
cd MCLite/firmware
pio run -e tdeck_plus
pio run -e tdeck_plus -t upload
```

### Monitor

```
pio device monitor
```

### Config

Insert FAT32 SD card (≤32 GB) and boot — config auto-generates. Default channels are ready to use.

Edit config.json directly on SD, or use the upstream config tool: https://laserir.github.io/MCLite/tools/config-tool/mclite_config_tool.html

### Firmware Updates

- **SD card**: copy .bin to SD root → boot → install prompt
- **Over WiFi**: Admin → WiFi → connect → Check for updates

## License

MIT. See LICENSE.

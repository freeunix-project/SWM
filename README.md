# SWM (Sigma Window Manager) 🔱

SWM is a minimalist, dynamic tiling window manager for X11, written in C. It is designed for developers who value performance, screen real estate, and the "less is more" philosophy.



## 🛠 Features
- **Dynamic Tiling Engine**: Automatically organizes windows into a Master-Stack layout.
- **Modern Gaps**: Built-in support for inner and outer gaps for a clean, spaced-out aesthetic.
- **Modular Design**: Separated into `swm.c`, `config.h`, and `swm.h` for easy maintenance.
- **Lightweight**: Zero dependencies other than `libX11`.
- **Keyboard-Centric**: Fully controllable via user-defined keybindings.

## 📥 Installation

### Prerequisites
Ensure you have the Xlib header files installed:
- **Arch Linux:** `sudo pacman -S libx11`
- **Ubuntu/Debian:** `sudo apt install libx11-dev`
- **Fedora:** `sudo dnf install libX11-devel`

### Build from Source
```bash
git clone [https://github.com/yourusername/swm.git](https://github.com/yourusername/swm.git)
cd swm
make
sudo make install

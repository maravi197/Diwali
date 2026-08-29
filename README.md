# Diwali CLI 🎆

A cool, lightweight terminal-based Diwali firecracker animation written in C++ using the `ncurses` library. 

## 🚀 Quick Setup 

### 1. Install Dependencies
Make sure you have a C++ compiler and `ncurses` installed based on your OS:
* **Arch Linux:** `sudo pacman -S gcc ncurses`
* **Debian / Ubuntu:** `sudo apt update && sudo apt install g++ libncurses-dev`
* **macOS (Homebrew):** `brew install ncurses`

### 2. Install & Run

```bash
git clone [https://github.com/maravi197/Diwali.git](https://github.com/maravi197/Diwali.git)
cd Diwali
g++ -O2 -Wall diwali.cpp -o diwali -lncurses
sudo cp diwali /usr/local/bin/
Run diwali

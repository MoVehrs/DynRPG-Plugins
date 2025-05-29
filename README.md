# DynRPG-Plugins

Welcome to **DynRPG-Plugins** - a collection of plugins I'm creating for use in my own RPG Maker 2003 project. These plugins are built using **[DynRPG](https://rpg-maker.cherrytree.at/dynrpg/index.html)**, an unofficial patch and plugin system for RPG Maker 2003 developed by Cherry and PepsiOtaku aka rewrking.

## 🚀 Purpose

This repository is meant to:

- Share useful DynRPG plugins with the community.
- Serve as a public archive of my development work.
- Possibly help others enhance their own RPG Maker 2003 games.

## 📦 Included Plugins

> ⚠️ Work in progress! This list will grow over time as I add more.

- **[bare_handed](https://github.com/MoVehrs/DynRPG-Plugins/tree/main/bare_handed)**
- **[dynamic_quickpatch](https://github.com/MoVehrs/DynRPG-Plugins/tree/main/dynamic_quickpatch)**
- **[limit_break](https://github.com/MoVehrs/DynRPG-Plugins/tree/main/limit_break)**

## 🛠 Requirements

- **RPG Maker 2003 v1.08**
- **DynRPG 0.32** installed on your game project **[Link](https://github.com/rewrking/DynRPG)**
- A basic understanding of how to install and use DynRPG plugins

## 📁 Installation

1. Download the `.dll` file from the plugins directory or compile the source yourself.
2. Copy it into your RPG Maker 2003 game's DynPlugins folder.
3. Make sure your game is DynRPG-patched (game root should include `DynRPG.ini` and `dynloader.dll`).
4. Adjust the `DynRPG.ini` file to load the desired plugins. 

## 🧪 Building From Source

If you’d like to build the plugins yourself instead of using the precompiled `.dll` files, here's how to set up your environment correctly.

> ⚠️ DynRPG operates at a **very low level**, meaning that using the wrong compiler can cause crashes or undefined behavior. It's important to follow the guidelines carefully.

### 🧰 Required Tools

- **C++ Compiler:** DynRPG is built with **GCC**, and only works with **GCC version 4.7.1 or higher**.  
  While Cherry originally recommends GCC directly, I'm using **[TDM-GCC 32-bit](https://jmeubank.github.io/tdm-gcc/download/)** as a compatible alternative.

- **IDE (Optional):** I recommend using **[Code::Blocks](https://www.codeblocks.org/)**, a lightweight and easy-to-use C++ IDE that integrates well with TDM-GCC.

### 🧵 Steps to Compile

1. Install **[TDM-GCC 32-bit](https://jmeubank.github.io/tdm-gcc/download/)**.
2. Install **[Code::Blocks](https://www.codeblocks.org/)** and configure it to use TDM-GCC as the default compiler.
3. Clone this repository and open the `.cbp` project file in Code::Blocks, or create a new project using the source files.
4. Build the project. The resulting `.dll` file will be your DynRPG plugin.

### 📚 More Information

For a complete setup guide and tips on using the DynRPG SDK, visit the official documentation by Cherry:

👉 **[Getting Started with DynRPG](https://rpg-maker.cherrytree.at/dynrpg/getting_started.html)**

## 🧩 Other DynRPG Plugin Repositories

Explore more DynRPG plugins created by other developers in the community:

- **[CortiPluginCollection by CortiWins](https://github.com/CortiWins/CortiPluginCollection/tree/main)**
  Collection of DynRPG plugins. Some maintained, some legacy. Documentation in English and German.
  
- **[DynRPG-plugin-archive by rewrking](https://github.com/rewrking/DynRPG-plugin-archive)**
  Archive of older DynRPG plugins and C++ projects from 2012 to 2015.

- **[system_opengl by rewrking](https://github.com/rewrking/DynRPG-system-opengl)**
  A DynRPG plugin to enable OpenGL rendering in older RPG Maker 2003 games.

- **[DynDataAccess by AubreyTheBard](https://github.com/AubreyTheBard/DynDataAccess)**
  A DynRPG plugin to allow access (get and set) to RPG Maker 2003 data which is normally hidden from the developer.

- **[DynGauge by AubreyTheBard](https://github.com/AubreyTheBard/DynGauge)**
  A DynRPG plugin to display health, mana, and ATB gauges, plus status condition icons and counters, for Battlers.
  
- **[DynRPG_Pathfeeder by DNKpp](https://github.com/DNKpp/DynRPG_Pathfeeder)**
  Pathfinding plugin for DynRPG 0.32. Provides path data, movement handled by the developer.

- **[animated-monsters by adketuri](https://github.com/adketuri/animated-monsters)**
  A DynRPG plugin to animate your monsters with the DBS.
  
- **[RPGSS by kyuu](https://github.com/kyuu/dynrpg-rpgss)**
  Lua-based scripting system for RPG Maker 2003. Similar to RGSS for XP.
  

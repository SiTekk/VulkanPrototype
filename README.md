# Vulkan Prototype

## Setup
Clone the repostiory with the recursive parameter.

### Windows Prerequisites
Install [Vulkan](https://vulkan.lunarg.com/), [Visual Studio 2022](https://visualstudio.microsoft.com/de/downloads/) and get the [Premake5](https://premake.github.io/download) binary.  
Run `premake5 vs2022` in the root directory and open the Visual Studio Solution file.

### Linux Prerequisites
Ubuntu / Debian:  
> sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools libglfw3-dev

Fedora:
> sudo dnf install vulkan-tools vulkan-loader-devel mesa-vulkan-devel vulkan-validation-layers-devel glfw-devel

Arch:
> sudo pacman -S vulkan-devel glfw-wayland # glfw-x11 for X11 users

To validate that Vulkan is correctly installed run `vkcube`.  
To validate that the Shader Compiler is correctly install run `glslc --version`  

Download [premake5](https://premake.github.io/download) or check if your distribution provides a package.  
Run `premake5 gmake2` to generate the makefiles.

If you use Visual Studio Code it is recommended to use the [Makefile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools) extension.
# Vulkan Prototype
## Resources
- [How to Learn Vulkan](https://www.jeremyong.com/c++/vulkan/graphics/rendering/2018/03/26/how-to-learn-vulkan/)
- [Vulkan-Tutorial](https://vulkan-tutorial.com/Introduction)  
- [Intels Vulkan Tutorial](https://www.intel.com/content/www/us/en/developer/articles/training/api-without-secrets-introduction-to-vulkan-part-1.html)
- [Uniform Buffers](https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/)  
- [Modern 3d Graphics](https://paroj.github.io/gltut/)  
- [Ray Tracing in One Weekend](https://github.com/RayTracing/raytracing.github.io)  
- [Physically Based Rendering](https://www.pbr-book.org/)  
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
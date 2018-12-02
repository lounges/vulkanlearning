A CLion based take on the Vulkan tutorial located here:
https://vulkan-tutorial.com/

Requires:
- CLion
- MSYS2: https://www.msys2.org/ (use the x64)
- LunarG SDK as mentioned on the site

Setup:
- Download and install MSYS2
- Launch the MSYS2 console and issue the following commands, taking the default each time
    - pacman -S mingw-w64-x86_64-toolchain
    - pacman -S mingw-w64-x86_64-glfw
    - pacman -S mingw-w64-x86_64-glm
- Install CLion and point a MinGW toolchain wherever you installed to (you want the mingw64 in the mys64 folder)
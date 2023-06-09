%VULKAN_SDK%\Bin\glslangValidator.exe -Od -g -V shader.vert
%VULKAN_SDK%\Bin\glslangValidator.exe -g -V shader.frag

XCOPY *.spv ..\..\out\bin\Debug\VulkanPrototype\shader\ /C /S /D /Y /I
XCOPY *.spv ..\..\out\bin\Release\VulkanPrototype\shader\ /C /S /D /Y /I
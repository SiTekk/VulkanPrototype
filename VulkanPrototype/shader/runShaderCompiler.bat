%VULKAN_SDK%\Bin\glslangValidator.exe -Od -g -V shader.vert || EXIT /B
%VULKAN_SDK%\Bin\glslangValidator.exe -g -V shader.frag || EXIT /B

XCOPY *.spv ..\..\out\bin\Debug\VulkanPrototype\shader\ /C /S /D /Y /I
XCOPY *.spv ..\..\out\bin\Release\VulkanPrototype\shader\ /C /S /D /Y /I
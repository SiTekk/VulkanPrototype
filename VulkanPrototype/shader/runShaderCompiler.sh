mkdir -p ../../out/bin/Debug/VulkanPrototype/shader
mkdir -p ../../out/bin/Release/VulkanPrototype/shader

glslc -c shader.frag -o ../../out/bin/Debug/VulkanPrototype/shader/frag.spv
glslc -c shader.frag -o ../../out/bin/Release/VulkanPrototype/shader/frag.spv

glslc -c shader.vert -o ../../out/bin/Debug/VulkanPrototype/shader/vert.spv
glslc -c shader.vert -o ../../out/bin/Release/VulkanPrototype/shader/vert.spv
# CMakeList.txt : CMake project for learning_vulkan_2, include source and define
# project specific logic here.
#

include_directories(
  "${PROJECT_SOURCE_DIR}/third-party/glfw-3.4.bin.WIN64/include"
  "${PROJECT_SOURCE_DIR}/third-party/vulkan/Include"
)


# Add source to this project's executable.
add_executable (learning_vulkan_2 "engine.cpp" "engine.h" "main.cpp" "instance.h" "config.h" "logging.h")

target_link_libraries(learning_vulkan_2 
  "${PROJECT_SOURCE_DIR}/third-party/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib"
  "${PROJECT_SOURCE_DIR}/third-party/vulkan/Lib/vulkan-1.lib"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET learning_vulkan_2 PROPERTY CXX_STANDARD 20)
endif()

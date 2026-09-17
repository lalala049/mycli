cmake_minimum_required(VERSION 3.21)

project(myCLI VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# vcpkg：依赖清单在 vcpkg.json（manifest 模式），由 CMakePresets.json 里的
# CMAKE_TOOLCHAIN_FILE 指向 vcpkg 工具链，configure 时自动安装依赖。
# 新增依赖：编辑 vcpkg.json 的 dependencies 后重新 configure 即可。

add_executable(myCLI
  src/main.c
)

target_compile_definitions(myCLI PRIVATE
  MYCLI_VERSION="${PROJECT_VERSION}"
)

if(MSVC)
  target_compile_options(myCLI PRIVATE /W4)
else()
  target_compile_options(myCLI PRIVATE -Wall -Wextra -Wpedantic)
endif()

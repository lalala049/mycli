cmake_minimum_required(VERSION 3.21)

project(myCLI VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 导出编译数据库（compile_commands.json），供 clangd 等语言服务器使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# 把 compile_commands.json 复制一份到项目根：clangd 从 cwd 向上找，
# 只放在 build/ 里它看不到（VSCode C/C++ 等插件同样受益）。
add_custom_target(ccd ALL
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
  COMMENT "Copying compile_commands.json to project root for clangd"
  VERBATIM
)

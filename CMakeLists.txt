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

# 框架本身编译成静态库（原来把 src/cli.c 直接编进可执行文件，
# 想单独测试框架或复用到别的程序都得复制源码，不干净）。
add_library(mycli STATIC
  src/cli.c
  src/util/map.c
)
# target_include_directories 只影响 mycli（PUBLIC 会传递给链接它的目标），
# 替代原来的全局 include_directories —— 全局命令会影响目录下所有目标。
target_include_directories(mycli PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(mycli PUBLIC MYCLI_VERSION="${PROJECT_VERSION}")

# 可执行程序：只负责注册命令 + 调用框架
add_executable(myCLI tests/main.c)
target_link_libraries(myCLI PRIVATE mycli)

if(MSVC)
  target_compile_options(mycli PRIVATE /W4)
  target_compile_options(myCLI PRIVATE /W4)
else()
  target_compile_options(mycli PRIVATE -Wall -Wextra -Wpedantic)
  target_compile_options(myCLI PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 冒烟测试：真实跑一遍可执行文件，靠退出码判断成败。
# smoke_unknown 期望命令失败（WILL_FAIL），用于锁定"未知命令返回非 0"的行为。
enable_testing()
add_test(NAME smoke_help    COMMAND myCLI todo --help)
add_test(NAME smoke_echo    COMMAND myCLI echo hello world)
add_test(NAME smoke_unknown COMMAND myCLI tod)
set_tests_properties(smoke_unknown PROPERTIES WILL_FAIL TRUE)

# 把 compile_commands.json 复制一份到项目根：clangd 从 cwd 向上找，
# 只放在 build/ 里它看不到（VSCode C/C++ 等插件同样受益）。
add_custom_target(ccd ALL
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
  COMMENT "Copying compile_commands.json to project root for clangd"
  VERBATIM
)

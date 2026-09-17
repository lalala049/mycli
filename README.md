# myCLI

一个 C11 命令行项目脚手架，已配好 **CMake + vcpkg（manifest 模式）+ overseer.nvim 任务**。

## 环境

- CMake ≥ 3.21（当前机器 4.4.3）
- 编译器：MinGW GCC（scoop 安装，已加入 PATH）
- vcpkg：scoop 安装，`VCPKG_ROOT` 已设置

## 目录结构

```
myCLI/
├── CMakeLists.txt       # 构建脚本
├── CMakePresets.json    # default 预设：MinGW + Ninja + vcpkg 工具链
├── vcpkg.json           # 依赖清单（manifest 模式，builtin-baseline 锁定版本）
├── src/
│   └── main.c           # 入口
└── .vscode/
    └── tasks.json       # overseer.nvim 读取的任务定义
```

## 构建与运行

```powershell
# 配置（会按 vcpkg.json 自动安装依赖）
cmake --preset default

# 编译
cmake --build --preset default

# 运行
.\build\default\myCLI.exe

# 清理
cmake --build --preset default --target clean
```

## clangd / 编辑器（消除 stdio.h not found）

- CMake 每次构建会把 `compile_commands.json` 复制到项目根（`ccd` 目标），clangd 从 cwd 向上找得到。
- `.clangd` 把编译驱动指定为 `gcc` 并指向 `build/default` 的编译数据库。
- nvim 侧已配 `--query-driver=.../scoop/apps/mingw/**`，授权 clangd 执行 gcc 提取 MinGW 头文件路径。
- 三者缺一不可：缺编译数据库或 `.clangd` 时，clangd 找不到 MinGW 的 `stdio.h`（gcc 编译本身不受影响）。

## 用 overseer.nvim 跑任务

在项目里任意位置按 `<leader>R` 选任务、`<leader>O` 开任务面板，或 `:OverseerRun`：

- `CMake: Configure` — 配置 + 装依赖
- `Build myCLI` — 编译（依赖 Configure，编译错误进 quickfix）
- `Run myCLI` — 编译并运行
- `Clean myCLI` — 清理

任务定义在 `.vscode/tasks.json`，overseer 会从 cwd 向上找最近的 `.vscode` 目录；
**新增任务 = 复制一个块，改 `label` 和 `command`**，改完不用重启 nvim。

## 添加 vcpkg 依赖

```jsonc
// vcpkg.json
"dependencies": [
  "libuv"   // 例：库名
]
```

改完重新 `cmake --preset default` 即可。注意当前预设用 MinGW 三元组
`x64-mingw-dynamic`（写在 CMakePresets.json 里），换 MSVC 编译器时需同步改
`VCPKG_TARGET_TRIPLET` 和 `CMAKE_C_COMPILER`。

## 切换生成器 / 编译器

改 `CMakePresets.json` 的 `generator` 与 `CMAKE_C_COMPILER` 后，删掉对应
`build/<preset>` 目录重新 configure 即可。

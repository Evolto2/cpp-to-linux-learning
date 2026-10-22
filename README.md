# C++ 到 Linux：三个递进学习项目

这是一个个人学习仓库：保留开源项目原始代码，另外补充中文阅读指南、练习、构建脚本与实际测试记录。项目代码归原作者所有；整理、构建和检查由 Codex 辅助完成，不能据此声称已独立完成这些项目。

发布前已完成 Windows 本机与 Linux CI 的 23/23 项检查；公开版本的 CI 状态及详情见 [验证记录](docs/验证记录.md)。

## 从哪里开始

| 阶段 | 项目 | 适合练习的内容 | 进入条件 |
|---|---|---|---|
| 01 | [基础计算器](projects/01-calculator/学习指南.md) | 输入输出、switch、函数、输入检查 | 会变量、分支和基本运算 |
| 02 | [学生信息管理](projects/02-student-manager/学习指南.md) | 类、vector、查找、修改、文本文件 | 能独立拆分函数，初步理解类和引用 |
| 03 | [TCP 回显客户端与服务端](projects/03-tcp-echo/学习指南.md) | TCP 收发、端口、线程、资源释放 | 会类、string、数组、引用和基本 Linux 命令 |

按 01 → 02 → 03 学。第三项是后续材料，不要求现在阅读 Asio 库内部。没有固定天数：完成一项练习并解释清楚，再进入下一项。

## 本机依赖与保存位置

本机目录示例（请按实际克隆位置修改）：`C:\Projects\cpp-to-linux-learning`。

| 依赖 | 用途 | 本机已核实位置 / 版本 |
|---|---|---|
| Clang | 编译 C++17 | `clang++`，23.1.3 |
| CMake | 一次构建四个可执行程序 | `cmake (PATH)`，4.4.3 |
| Ninja | Windows 构建后端 | `ninja (PATH)` |
| Git | 版本保存和推送 | `git (PATH)` |
| Python | 运行检查脚本，不是 C++ 程序的运行依赖 | Python 3.10 或以上，脚本只用标准库 |
| Asio | TCP 示例所需头文件 | 随第三个项目保存在 `upstream/include`，无需另外安装 Boost |

整理时复用了已有编译环境。公开文档使用通用目录示例和 PATH 中的工具命令；请根据本机安装情况调整。

## Windows：先编译，再运行

用 VS Code 打开整个仓库文件夹，选择“终端 → 新建终端”。下面只复制代码块内的命令；终端显示的 `PS ...>` 是提示符，不要输入。

```powershell
Set-Location -LiteralPath 'C:\Projects\cpp-to-linux-learning'
& '.\scripts\build-windows.ps1' -Compiler 'clang++'
& '.\build\windows\calculator.exe'
```

构建脚本会生成 calculator、student_manager、tcp_echo_server、tcp_echo_client。修改源代码后必须重新编译；保存 `.cpp` 不会自动改变 `.exe`。

Windows 脚本需要 PowerShell、CMake、Ninja 和可用的 Windows C++ SDK。若执行策略阻止 `.ps1`，不必改系统设置，可以直接执行等价命令：

```powershell
cmake -S . -B build/windows -G Ninja '-DCMAKE_CXX_COMPILER=clang++' -DCMAKE_BUILD_TYPE=Debug
cmake --build build/windows --parallel 2
```

运行完整检查：

```powershell
python .\scripts\smoke_test.py --build-dir build/windows --report build/windows-results.json
```

检查使用临时目录和动态选择的本机端口，结束后释放测试进程，不在原始源码目录写入学生数据。

## Linux：后续练习入口

前提是已有可启动的 Linux，以及 C++17 编译器、CMake、Make 和 Python 3.10+。本仓库不自动安装或修复 WSL。

```bash
git clone https://github.com/Evolto2/cpp-to-linux-learning.git
cd cpp-to-linux-learning
bash scripts/build-linux.sh
./build/linux/calculator
python3 scripts/smoke_test.py --build-dir build/linux --report build/linux-results.json
```

公开仓库可直接克隆。GitHub Actions 的验证结果不能证明本机 WSL 已可运行。

## 目录与原始代码

- `projects/`：项目指南、源码快照与 SOURCE.json，后者包含来源提交、许可证和每个文件的 SHA-256。
- `scripts/`：新增的构建脚本和功能检查，不修改原始源码。
- `examples/`：可复制的输入数据。
- `docs/`：知识点对照、已知问题、来源链接 Word 清单和验证说明。
- `reports/`：本轮 Windows 实测及 Linux CI 证据。
- `build/`：本机生成的程序和构建缓存，已从 Git 排除。
- `practice/`：自己修改的练习副本，默认不提交，避免和原始源码混淆。

复制项目到 practice 后再修改。完成练习后，如果希望把自己的代码保存到 GitHub，可以单独新建明确命名的学习目录并自行提交；不要直接覆盖 upstream。

## 验收方式

每个项目都要做到：能运行、能用自己的话解释数据如何变化、能关闭参考代码重新写核心部分、能完成至少一个变化需求。测试通过只说明所覆盖的行为符合预期，不表示已经掌握知识或项目没有其他缺陷。

来源与许可见 [来源索引](docs/来源索引.md) 和 [Word 链接清单](docs/项目来源与链接清单.docx)，实际验证见 [验证记录](docs/验证记录.md)。

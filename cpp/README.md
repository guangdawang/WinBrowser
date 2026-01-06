
# WinBrowserQt - 轻量级 C++ 浏览器

基于 Qt WebEngine 的现代轻量级浏览器，使用 C++ 和 Qt 框架开发。

## 功能特性

- 🚀 基于 Qt WebEngine (Chromium) 内核
- 🗂️ 多标签页浏览
- 📚 书签管理
- 📖 浏览历史
- 🎨 亮色/暗色主题
- ⚡ 快速导航
- 🔍 地址栏智能提示

## 系统要求

- Windows 10 或更高版本 / Linux / macOS
- Qt 6.2 或更高版本
- CMake 3.16 或更高版本
- C++17 兼容编译器 (MSVC 2019+, GCC 8+, Clang 8+)

## 开发环境

### Windows

1. 安装 Visual Studio 2019 或更高版本
2. 安装 Qt 6.2 或更高版本 (包含 WebEngine 模块)
3. 安装 CMake
4. 克隆项目
5. 打开命令提示符，进入项目目录
6. 运行以下命令：

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux

1. 安装必要的依赖：

```bash
# Ubuntu/Debian
sudo apt-get install qt6-base-dev qt6-webengine-dev cmake build-essential

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel cmake gcc-c++
```

2. 克隆项目并构建：

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### macOS

1. 安装 Homebrew
2. 安装 Qt 6 和 CMake：

```bash
brew install qt@6 cmake
```

3. 克隆项目并构建：

```bash
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

## 运行

### Windows

```bash
cd build/Release
WinBrowserQt.exe
```

### Linux/macOS

```bash
cd build
./WinBrowserQt
```

## 命令行参数

- `-u, --url <url>`: 打开指定的URL
- `-h, --help`: 显示帮助信息
- `-v, --version`: 显示版本信息

示例：

```bash
WinBrowserQt --url https://www.bing.com
```

## 项目结构

```
WinBrowserQt/
├── CMakeLists.txt          # CMake构建配置
├── README.md               # 项目说明
└── src/
    ├── main.cpp            # 程序入口
    ├── mainwindow.h/cpp    # 主窗口
    ├── addressbar.h/cpp    # 地址栏控件
    ├── browsertabwidget.h/cpp  # 标签页控件
    ├── navigationmanager.h/cpp  # 导航管理器
    ├── storagemanager.h/cpp    # 存储管理器
    └── models/             # 数据模型
        ├── browsertab.h/cpp
        ├── historyitem.h
        ├── bookmark.h
        └── settings.h
```

## 数据存储

应用程序数据存储在系统标准位置：

- Windows: `%APPDATA%/WinBrowser/`
- Linux: `~/.local/share/WinBrowser/`
- macOS: `~/Library/Application Support/WinBrowser/`

存储的文件包括：
- `settings.json`: 应用设置
- `bookmarks.json`: 书签数据
- `history.json`: 浏览历史

## 开发说明

### 添加新功能

1. 在相应的类中添加方法
2. 更新头文件和实现文件
3. 在 CMakeLists.txt 中添加新文件（如果有）
4. 重新构建项目

### 调试

在 CMake 配置时添加调试选项：

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

## 许可证

请参考项目根目录的 LICENSE 文件。

## 贡献

欢迎提交问题报告和拉取请求！

## 联系方式

如有问题或建议，请通过项目仓库提交 Issue。


cmake_minimum_required(VERSION 3.16)
project(WinBrowserQt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 链接时优化(LTO)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /LTCG")
elseif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
endif()

# Qt配置
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebEngineWidgets WebEngineCore WebChannel Network Concurrent)

# 源文件
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow_helper.cpp
    src/addressbar.cpp
    src/browsertabwidget.cpp
    src/navigationmanager.cpp
    src/storagemanager.cpp
    src/models/browsertab.cpp
    src/models/historyitem.cpp
    src/models/bookmark.cpp
    src/models/settings.cpp
)

# 头文件
set(HEADERS
    src/mainwindow.h
    src/addressbar.h
    src/browsertabwidget.h
    src/navigationmanager.h
    src/storagemanager.h
    src/models/browsertab.h
    src/models/historyitem.h
    src/models/bookmark.h
    src/models/settings.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 链接Qt库
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebEngineCore
    Qt6::Network
    Qt6::Concurrent
)

# Windows特定设置
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

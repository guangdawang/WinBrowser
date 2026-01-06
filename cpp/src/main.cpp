
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    QApplication::setApplicationName("WinBrowserQt");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("WinBrowser");

    // 命令行参数解析
    QCommandLineParser parser;
    parser.setApplicationDescription("WinBrowserQt - 现代浏览器");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption urlOption(QStringList() << "u" << "url",
                                 "打开指定的URL", "url");
    parser.addOption(urlOption);

    parser.process(app);

    // 检查Qt WebEngine是否可用
    if (!QApplication::instance()->property("qt_webengine_available").toBool()) {
        qCritical() << "Qt WebEngine不可用，请确保安装了Qt WebEngine模块";
        return -1;
    }

    // 创建主窗口
    MainWindow mainWindow;
    mainWindow.show();

    // 如果指定了URL，打开它
    if (parser.isSet(urlOption)) {
        QString url = parser.value(urlOption);
        mainWindow.navigateToUrl(url);
    }

    return app.exec();
}

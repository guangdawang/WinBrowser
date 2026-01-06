
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QHostAddress>
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QMetaObject>
#include <QtConcurrent>

namespace WinBrowserQt {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_currentTab(nullptr)
{
    initializeManagers();
    initializeUI();

    // 创建初始标签页
    createNewTab("https://www.bing.com");

    // 使用 QMetaObject::invokeMethod 延迟执行数据加载
    // 等待 UI 事件循环启动后再加载，避免阻塞启动
    QMetaObject::invokeMethod(this, &MainWindow::loadDataLazy, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    // 保存所有数据（使用异步方式）
    if (m_storageManager) {
        Settings settings = m_storageManager->loadSettings();
        m_storageManager->saveSettingsAsync(settings);
        m_storageManager->saveBookmarksAsync(m_bookmarks);
        m_storageManager->saveHistoryAsync(m_history);
    }
}

void MainWindow::initializeManagers()
{
    m_navigationManager = new NavigationManager(this);
    m_storageManager = new StorageManager(this);

    // 连接历史记录变化信号
    connect(m_navigationManager, &NavigationManager::historyChanged,
            this, &MainWindow::onHistoryChanged);

    // 连接数据保存信号
    connect(m_storageManager, &StorageManager::dataSaved,
            this, &MainWindow::onDataSaved);
    connect(m_storageManager, &StorageManager::saveError,
            this, &MainWindow::onSaveError);
}

void MainWindow::initializeUI()
{
    setWindowTitle("WinBrowserQt - 现代浏览器 (Qt WebEngine)");
    resize(1200, 800);

    // 居中显示窗口
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    createToolbar();
    createAddressBar();
    createTabWidget();
    createStatusBar();
    createMenus();

    // 将组件添加到主布局
    mainLayout->addWidget(m_toolbar);
    mainLayout->addWidget(m_addressBar);
    mainLayout->addWidget(m_tabWidget, 1);
    mainLayout->addWidget(m_statusBar);
}

void MainWindow::createToolbar()
{
    m_toolbar = addToolBar("导航工具栏");
    m_toolbar->setMovable(false);
    m_toolbar->setFloatable(false);

    // 后退按钮
    m_backAction = new QAction("←", this);
    m_backAction->setToolTip("后退");
    m_backAction->setEnabled(false);
    connect(m_backAction, &QAction::triggered, this, &MainWindow::onBackClicked);
    m_toolbar->addAction(m_backAction);

    // 前进按钮
    m_forwardAction = new QAction("→", this);
    m_forwardAction->setToolTip("前进");
    m_forwardAction->setEnabled(false);
    connect(m_forwardAction, &QAction::triggered, this, &MainWindow::onForwardClicked);
    m_toolbar->addAction(m_forwardAction);

    // 刷新按钮
    m_refreshAction = new QAction("↻", this);
    m_refreshAction->setToolTip("刷新");
    connect(m_refreshAction, &QAction::triggered, this, &MainWindow::onRefreshClicked);
    m_toolbar->addAction(m_refreshAction);

    // 主页按钮
    m_homeAction = new QAction("🏠", this);
    m_homeAction->setToolTip("主页");
    connect(m_homeAction, &QAction::triggered, this, &MainWindow::onHomeClicked);
    m_toolbar->addAction(m_homeAction);

    m_toolbar->addSeparator();

    // 开发者工具按钮
    m_devToolsAction = new QAction("🔧", this);
    m_devToolsAction->setToolTip("开发者工具");
    connect(m_devToolsAction, &QAction::triggered, this, &MainWindow::onDevToolsClicked);
    m_toolbar->addAction(m_devToolsAction);
}

void MainWindow::createAddressBar()
{
    m_addressBar = new AddressBar(this);

    connect(m_addressBar, &AddressBar::navigateRequested,
            this, &MainWindow::onNavigateRequested);
    connect(m_addressBar, &AddressBar::searchRequested,
            this, &MainWindow::onSearchRequested);
}

void MainWindow::createTabWidget()
{
    m_tabWidget = new BrowserTabWidget(this);

    connect(m_tabWidget, &BrowserTabWidget::tabCreated,
            this, &MainWindow::onTabCreated);
    connect(m_tabWidget, &BrowserTabWidget::tabClosed,
            this, &MainWindow::onTabClosed);
    connect(m_tabWidget, &BrowserTabWidget::tabChanged,
            this, &MainWindow::onTabChanged);
}

void MainWindow::createStatusBar()
{
    m_statusBar = statusBar();
    m_statusBar->showMessage("就绪");
}

void MainWindow::createMenus()
{
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");

    QAction *newTabAction = fileMenu->addAction("新建标签页(&N)");
    newTabAction->setShortcut(QKeySequence::New);
    connect(newTabAction, &QAction::triggered, this, [this]() { createNewTab(); });

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("退出(&X)");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // 编辑菜单
    QMenu *editMenu = menuBar()->addMenu("编辑(&E)");

    QAction *backAction = editMenu->addAction("后退(&B)");
    backAction->setShortcut(QKeySequence::Back);
    connect(backAction, &QAction::triggered, this, &MainWindow::onBackClicked);

    QAction *forwardAction = editMenu->addAction("前进(&F)");
    forwardAction->setShortcut(QKeySequence::Forward);
    connect(forwardAction, &QAction::triggered, this, &MainWindow::onForwardClicked);

    QAction *refreshAction = editMenu->addAction("刷新(&R)");
    refreshAction->setShortcut(QKeySequence::Refresh);
    connect(refreshAction, &QAction::triggered, this, &MainWindow::onRefreshClicked);

    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");

    QAction *aboutAction = helpMenu->addAction("关于(&A)");
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "关于 WinBrowserQt",
            "WinBrowserQt - 现代浏览器\n\n"
            "基于 Qt WebEngine (Chromium) 内核\n"
            "版本: 1.0\n\n"
            "功能特性:\n"
            "• 多标签页浏览\n"
            "• 书签管理\n"
            "• 浏览历史\n"
            "• 亮色/暗色主题\n"
            "• 快速导航\n"
            "• 地址栏智能提示");
    });
}

void MainWindow::onNavigateRequested(const QString &url)
{
    navigateToUrl(url);
}

void MainWindow::onSearchRequested(const QString &searchTerm)
{
    QString searchUrl = QString("https://www.bing.com/search?q=%1")
        .arg(QString::fromUtf8(QUrl::toPercentEncoding(searchTerm)));
    navigateToUrl(searchUrl);
}

void MainWindow::onTabCreated(BrowserTab *tab)
{
    if (!tab || !tab->webView()) return;

    QWebEngineView *webView = tab->webView();
    QWebEnginePage *page = webView->page();

    // 配置页面设置
    QWebEngineSettings *settings = page->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);

    // 连接页面信号
    connect(page, &QWebEnginePage::loadStarted, this, [this, tab]() {
        if (m_currentTab == tab) {
            updateStatus("正在加载...");
            m_refreshAction->setEnabled(false);
        }
        tab->setIsLoading(true);
    });

    connect(page, &QWebEnginePage::loadFinished, this, [this, tab](bool ok) {
        if (m_currentTab == tab) {
            updateStatus(ok ? "加载完成" : "加载失败");
            m_refreshAction->setEnabled(true);
        }
        tab->setIsLoading(false);
        updateNavigationButtons();
    });

    connect(page, &QWebEnginePage::titleChanged, this, [this, tab](const QString &title) {
        tab->setTitle(title);
        m_tabWidget->updateTabTitle(tab, title);
    });

    connect(page, &QWebEnginePage::urlChanged, this, [this, tab](const QUrl &url) {
        tab->setUrl(url.toString());
        if (m_currentTab == tab) {
            m_addressBar->setUrl(url.toString());
        }
        // 添加到历史记录
        m_navigationManager->addToHistory(url.toString(), tab->title());
    });

    connect(page, &QWebEnginePage::iconChanged, this, [this, tab](const QIcon &icon) {
        // 可以在这里更新标签页图标
    });

    connect(page, &QWebEnginePage::loadProgress, this, [this, tab](int progress) {
        if (m_currentTab == tab) {
            updateStatus(QString("正在加载... %1%").arg(progress));
        }
    });

    // 处理新窗口请求
    connect(page, &QWebEnginePage::newWindowRequested, this, &MainWindow::onNewWindowRequested);

    // 初始导航
    if (!tab->url().isEmpty() && tab->url() != "about:blank") {
        webView->setUrl(QUrl(tab->url()));
    }

    updateStatus("标签页创建完成");
}

void MainWindow::onTabClosed(BrowserTab *tab)
{
    if (tab) {
        // 清理资源
        if (tab->webView()) {
            delete tab->webView();
        }

        // 异步保存书签和历史记录
        m_storageManager->saveBookmarksAsync(m_bookmarks);
        m_storageManager->saveHistoryAsync(m_history);
    }
}

void MainWindow::onTabChanged(BrowserTab *tab)
{
    m_currentTab = tab;

    if (tab) {
        m_addressBar->setUrl(tab->url());
        updateNavigationButtons();
        updateStatus(QString("当前页面: %1").arg(tab->title()));
    }
}

void MainWindow::onHistoryChanged(const HistoryChangedEventArgs &args)
{
    // 历史记录变化处理
    updateNavigationButtons();
}

void MainWindow::onBackClicked()
{
    navigateBack();
}

void MainWindow::onForwardClicked()
{
    navigateForward();
}

void MainWindow::onRefreshClicked()
{
    refreshCurrentTab();
}

void MainWindow::onHomeClicked()
{
    navigateHome();
}

void MainWindow::onDevToolsClicked()
{
    if (m_currentTab && m_currentTab->webView()) {
        QWebEnginePage *page = m_currentTab->webView()->page();
        if (page) {
            page->triggerAction(QWebEnginePage::InspectElement);
        }
    } else {
        QMessageBox::information(this, "提示", "无法打开开发者工具，请确保页面已加载完成");
    }
}

void MainWindow::onNavigationStarted(QWebEnginePage *page)
{
    updateStatus("正在加载...");
    m_refreshAction->setEnabled(false);
}

void MainWindow::onNavigationCompleted(bool ok)
{
    updateStatus(ok ? "加载完成" : "加载失败");
    m_refreshAction->setEnabled(true);
}

void MainWindow::onTitleChanged(const QString &title)
{
    if (m_currentTab) {
        m_tabWidget->updateTabTitle(m_currentTab, title);
    }
}

void MainWindow::onUrlChanged(const QUrl &url)
{
    if (m_currentTab) {
        m_addressBar->setUrl(url.toString());
        m_currentTab->setUrl(url.toString());
    }
}

void MainWindow::onIconChanged(const QIcon &icon)
{
    // 可以在这里更新标签页图标
}

void MainWindow::onLoadProgress(int progress)
{
    updateStatus(QString("正在加载... %1%").arg(progress));
}

void MainWindow::onNewWindowRequested(QWebEnginePage *page)
{
    // 在新标签页中打开
    createNewTab(page->requestedUrl().toString());
}

void MainWindow::navigateToUrl(const QString &url)
{
    if (url.trimmed().isEmpty()) return;

    if (m_currentTab && m_currentTab->webView()) {
        // 处理URL格式 - 自动添加协议头
        QString finalUrl = url;
        if (!url.startsWith("http://") && !url.startsWith("https://") && 
            !url.startsWith("file://") && !url.startsWith("about:")) {
            finalUrl = ensureUrlWithProtocol(url);
        }

        m_currentTab->webView()->setUrl(QUrl(finalUrl));
    } else {
        // 如果没有可用的标签页，创建新标签页
        createNewTab(url);
    }
}

void MainWindow::navigateBack()
{
    if (m_currentTab && m_currentTab->webView()) {
        QWebEnginePage *page = m_currentTab->webView()->page();
        if (page && page->history()->canGoBack()) {
            page->history()->back();
        }
    }
}

void MainWindow::navigateForward()
{
    if (m_currentTab && m_currentTab->webView()) {
        QWebEnginePage *page = m_currentTab->webView()->page();
        if (page && page->history()->canGoForward()) {
            page->history()->forward();
        }
    }
}

void MainWindow::refreshCurrentTab()
{
    if (m_currentTab && m_currentTab->webView()) {
        m_currentTab->webView()->reload();
    }
}

void MainWindow::navigateHome()
{
    navigateToUrl("https://www.bing.com");
}

void MainWindow::createNewTab(const QString &url)
{
    m_tabWidget->createNewTab(url, "新标签页");
}

void MainWindow::updateNavigationButtons()
{
    if (m_currentTab && m_currentTab->webView()) {
        QWebEnginePage *page = m_currentTab->webView()->page();
        if (page) {
            m_backAction->setEnabled(page->history()->canGoBack());
            m_forwardAction->setEnabled(page->history()->canGoForward());
            return;
        }
    }

    m_backAction->setEnabled(false);
    m_forwardAction->setEnabled(false);
}

void MainWindow::updateStatus(const QString &message)
{
    m_statusBar->showMessage(message);
}

QString MainWindow::ensureUrlWithProtocol(const QString &url) const
{
    if (url.trimmed().isEmpty()) {
        return "about:blank";
    }

    // 提取主机名部分进行分析
    QString host = extractHostFromUrl(url);

    if (isLocalOrPrivateAddress(host)) {
        // 对于localhost和私有IP地址，使用HTTP
        return "http://" + url;
    } else {
        // 对于公网域名，使用HTTPS
        return "https://" + url;
    }
}

QString MainWindow::extractHostFromUrl(const QString &url) const
{
    try {
        // 移除可能的路径和查询参数
        QString host = url.split('/')[0].split('?')[0].split(':')[0];
        return host.toLower();
    } catch (...) {
        return url.toLower();
    }
}

bool MainWindow::isLocalOrPrivateAddress(const QString &host) const
{
    if (host.trimmed().isEmpty()) {
        return false;
    }

    QString lowerHost = host.toLower();

    // 检查 localhost
    if (lowerHost == "localhost" || lowerHost.startsWith("localhost.")) {
        return true;
    }

    // 检查环回地址
    if (lowerHost == "127.0.0.1" || lowerHost == "::1" || lowerHost == "[::1]") {
        return true;
    }

    // 检查IPv4地址
    QHostAddress address;
    if (address.setAddress(lowerHost)) {
        // 检查私有IP地址范围
        if (isPrivateIPAddress(address)) {
            return true;
        }

        // 检查环回地址
        if (address.isLoopback()) {
            return true;
        }
    }

    return false;
}

bool MainWindow::isPrivateIPAddress(const QHostAddress &address) const
{
    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        quint32 ipv4 = address.toIPv4Address();

        // 10.0.0.0/8
        if ((ipv4 & 0xFF000000) == 0x0A000000) {
            return true;
        }

        // 172.16.0.0/12
        if ((ipv4 & 0xFFF00000) == 0xAC100000) {
            return true;
        }

        // 192.168.0.0/16
        if ((ipv4 & 0xFFFF0000) == 0xC0A80000) {
            return true;
        }

        // 169.254.0.0/16 (链路本地)
        if ((ipv4 & 0xFFFF0000) == 0xA9FE0000) {
            return true;
        }
    } else if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        // IPv6 私有地址范围检查
        if (address.isLinkLocal() || address.isLoopback()) {
            return true;
        }

        // 检查唯一本地地址 (fc00::/7)
        Q_IPV6ADDR ipv6 = address.toIPv6Address();
        if ((ipv6[0] & 0xFE) == 0xFC) {
            return true;
        }
    }

    return false;
}

void MainWindow::loadDataLazy()
{
    // 延迟加载数据，此时窗口已经显示
    m_bookmarks = m_storageManager->loadBookmarks();
    m_history = m_storageManager->loadHistory();
    updateStatus("数据加载完成");
}

void MainWindow::onDataSaved()
{
    // 数据保存成功
    updateStatus("");
}

void MainWindow::onSaveError(const QString &message)
{
    // 数据保存失败
    qWarning() << "数据保存失败:" << message;
}

} // namespace WinBrowserQt

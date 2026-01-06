
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include "addressbar.h"
#include "browsertabwidget.h"
#include "navigationmanager.h"
#include "storagemanager.h"

namespace WinBrowserQt {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void navigateToUrl(const QString &url);

private slots:
    void onNavigateRequested(const QString &url);
    void onSearchRequested(const QString &searchTerm);
    void onTabCreated(BrowserTab *tab);
    void onTabClosed(BrowserTab *tab);
    void onTabChanged(BrowserTab *tab);
    void onHistoryChanged(const HistoryChangedEventArgs &args);
    void onDataSaved();
    void onSaveError(const QString &message);
    void loadDataLazy();

    void onBackClicked();
    void onForwardClicked();
    void onRefreshClicked();
    void onHomeClicked();
    void onDevToolsClicked();

    void onNavigationStarted(QWebEnginePage *page);
    void onNavigationCompleted(bool ok);
    void onTitleChanged(const QString &title);
    void onUrlChanged(const QUrl &url);
    void onIconChanged(const QIcon &icon);
    void onLoadProgress(int progress);
    void onNewWindowRequested(QWebEngineNewWindowRequest &request);

private:
    void initializeUI();
    void initializeManagers();
    void createToolbar();
    void createAddressBar();
    void createTabWidget();
    void createStatusBar();
    void createMenus();

    void navigateBack();
    void navigateForward();
    void refreshCurrentTab();
    void navigateHome();
    void createNewTab(const QString &url = "about:blank");

    void updateNavigationButtons();
    void updateStatus(const QString &message);

    QString ensureUrlWithProtocol(const QString &url) const;
    QString extractHostFromUrl(const QString &url) const;
    bool isLocalOrPrivateAddress(const QString &host) const;
    bool isPrivateIPAddress(const QHostAddress &address) const;

    // UI组件
    QToolBar *m_toolbar;
    AddressBar *m_addressBar;
    BrowserTabWidget *m_tabWidget;
    QStatusBar *m_statusBar;

    // 工具栏按钮
    QAction *m_backAction;
    QAction *m_forwardAction;
    QAction *m_refreshAction;
    QAction *m_homeAction;
    QAction *m_devToolsAction;

    // 管理器
    NavigationManager *m_navigationManager;
    StorageManager *m_storageManager;

    // 当前标签页
    BrowserTab *m_currentTab;

    // 数据
    QList<Bookmark> m_bookmarks;
    QList<HistoryItem> m_history;
};

} // namespace WinBrowserQt

#endif // MAINWINDOW_H

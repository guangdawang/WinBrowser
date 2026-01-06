
#include "browsertabwidget.h"
#include <QHBoxLayout>
#include <QTabBar>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineProfile>

namespace WinBrowserQt {

BrowserTabWidget::BrowserTabWidget(QWidget *parent)
    : QWidget(parent)
{
    initializeUI();
    setupContextMenu();
}

void BrowserTabWidget::initializeUI()
{
    // 主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 标签控件
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabBarAutoHide(false);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setDocumentMode(true);

    // 设置标签栏样式
    m_tabWidget->tabBar()->setDrawBase(false);
    m_tabWidget->tabBar()->setExpanding(false);
    m_tabWidget->tabBar()->setUsesScrollButtons(true);

    mainLayout->addWidget(m_tabWidget);

    // 新建标签按钮
    m_newTabButton = new QToolButton(this);
    m_newTabButton->setText("+");
    m_newTabButton->setFixedSize(30, 24);
    m_newTabButton->setAutoRaise(true);
    m_newTabButton->setCursor(Qt::PointingHandCursor);

    // 将新建标签按钮添加到标签栏
    m_tabWidget->tabBar()->addTab("");
    m_tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, m_newTabButton);
    m_tabWidget->removeTab(0);

    // 连接信号
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &BrowserTabWidget::onTabCloseRequested);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &BrowserTabWidget::onCurrentChanged);
    connect(m_newTabButton, &QToolButton::clicked, this, &BrowserTabWidget::onNewTabClicked);

    // 设置标签栏右键菜单
    m_tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tabWidget->tabBar(), &QTabBar::customContextMenuRequested, 
            this, &BrowserTabWidget::onTabContextMenuRequested);
}

void BrowserTabWidget::setupContextMenu()
{
    m_tabContextMenu = new QMenu(this);

    m_closeTabAction = m_tabContextMenu->addAction("关闭标签页");
    m_closeOtherTabsAction = m_tabContextMenu->addAction("关闭其他标签页");
    m_closeAllTabsAction = m_tabContextMenu->addAction("关闭所有标签页");
    m_tabContextMenu->addSeparator();
    m_reloadTabAction = m_tabContextMenu->addAction("重新加载");

    connect(m_closeTabAction, &QAction::triggered, this, &BrowserTabWidget::onCloseTabAction);
    connect(m_closeOtherTabsAction, &QAction::triggered, this, &BrowserTabWidget::onCloseOtherTabsAction);
    connect(m_closeAllTabsAction, &QAction::triggered, this, &BrowserTabWidget::onCloseAllTabsAction);
    connect(m_reloadTabAction, &QAction::triggered, this, &BrowserTabWidget::onReloadTabAction);
}

BrowserTab* BrowserTabWidget::createNewTab(const QString &url, const QString &title)
{
    // 创建新的标签页数据对象
    BrowserTab *browserTab = new BrowserTab(QString(), url, title);

    // 创建Web视图，使用共享的 profile 以提高性能
    QWebEngineView *webView = new QWebEngineView(this);
    QWebEnginePage *page = new QWebEnginePage(QWebEngineProfile::defaultProfile(), webView);
    webView->setPage(page);

    // 优化页面设置
    QWebEngineSettings *settings = page->settings();
    settings->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    settings->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false); // 关闭审计，提速

    browserTab->setWebView(webView);

    // 添加到标签控件
    int index = m_tabWidget->addTab(webView, getDisplayText(title));
    m_tabWidget->setCurrentIndex(index);

    // 保存标签页数据
    m_tabWidget->widget(index)->setProperty("browserTab", QVariant::fromValue(browserTab));

    updateNewTabButtonPosition();

    emit tabCreated(browserTab);
    return browserTab;
}

void BrowserTabWidget::closeTab(int index)
{
    if (index >= 0 && index < m_tabWidget->count()) {
        QWidget *widget = m_tabWidget->widget(index);
        BrowserTab *browserTab = widget->property("browserTab").value<BrowserTab*>();

        m_tabWidget->removeTab(index);

        if (browserTab) {
            emit tabClosed(browserTab);
            delete browserTab;
        }

        delete widget;

        updateNewTabButtonPosition();

        // 如果没有标签页了，创建新的
        if (m_tabWidget->count() == 0) {
            createNewTab();
        }
    }
}

void BrowserTabWidget::closeSelectedTab()
{
    int index = m_tabWidget->currentIndex();
    if (index >= 0) {
        closeTab(index);
    }
}

void BrowserTabWidget::closeOtherTabs()
{
    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex < 0) return;

    // 从后往前关闭，避免索引变化
    for (int i = m_tabWidget->count() - 1; i >= 0; --i) {
        if (i != currentIndex) {
            closeTab(i);
        }
    }
}

void BrowserTabWidget::closeAllTabs()
{
    while (m_tabWidget->count() > 0) {
        closeTab(0);
    }
}

void BrowserTabWidget::reloadSelectedTab()
{
    BrowserTab *tab = getSelectedBrowserTab();
    if (tab && tab->webView()) {
        tab->webView()->reload();
    }
}

BrowserTab* BrowserTabWidget::getSelectedBrowserTab() const
{
    int index = m_tabWidget->currentIndex();
    if (index >= 0) {
        QWidget *widget = m_tabWidget->widget(index);
        return widget->property("browserTab").value<BrowserTab*>();
    }
    return nullptr;
}

BrowserTab* BrowserTabWidget::getBrowserTab(int index) const
{
    if (index >= 0 && index < m_tabWidget->count()) {
        QWidget *widget = m_tabWidget->widget(index);
        return widget->property("browserTab").value<BrowserTab*>();
    }
    return nullptr;
}

void BrowserTabWidget::updateTabTitle(BrowserTab *browserTab, const QString &title)
{
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        QWidget *widget = m_tabWidget->widget(i);
        BrowserTab *tab = widget->property("browserTab").value<BrowserTab*>();
        if (tab == browserTab) {
            m_tabWidget->setTabText(i, getDisplayText(title));
            break;
        }
    }
}

void BrowserTabWidget::updateTabUrl(BrowserTab *browserTab, const QString &url)
{
    // 可以在这里更新标签页的其他显示信息
}

int BrowserTabWidget::tabCount() const
{
    return m_tabWidget->count();
}

int BrowserTabWidget::selectedIndex() const
{
    return m_tabWidget->currentIndex();
}

void BrowserTabWidget::setSelectedIndex(int index)
{
    m_tabWidget->setCurrentIndex(index);
}

BrowserTab* BrowserTabWidget::selectedTab() const
{
    return getSelectedBrowserTab();
}

QList<BrowserTab*> BrowserTabWidget::allTabs() const
{
    QList<BrowserTab*> tabs;
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        QWidget *widget = m_tabWidget->widget(i);
        BrowserTab *tab = widget->property("browserTab").value<BrowserTab*>();
        if (tab) {
            tabs.append(tab);
        }
    }
    return tabs;
}

void BrowserTabWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateNewTabButtonPosition();
}

void BrowserTabWidget::onTabCloseRequested(int index)
{
    closeTab(index);
}

void BrowserTabWidget::onCurrentChanged(int index)
{
    BrowserTab *tab = getBrowserTab(index);
    emit tabChanged(tab);
}

void BrowserTabWidget::onNewTabClicked()
{
    createNewTab();
}

void BrowserTabWidget::onTabContextMenuRequested(const QPoint &pos)
{
    int tabIndex = m_tabWidget->tabBar()->tabAt(pos);
    if (tabIndex >= 0) {
        m_tabWidget->setCurrentIndex(tabIndex);
        m_tabContextMenu->exec(m_tabWidget->tabBar()->mapToGlobal(pos));
    }
}

void BrowserTabWidget::onCloseTabAction()
{
    closeSelectedTab();
}

void BrowserTabWidget::onCloseOtherTabsAction()
{
    closeOtherTabs();
}

void BrowserTabWidget::onCloseAllTabsAction()
{
    closeAllTabs();
}

void BrowserTabWidget::onReloadTabAction()
{
    reloadSelectedTab();
}

void BrowserTabWidget::updateNewTabButtonPosition()
{
    // Qt的QTabWidget会自动管理标签按钮的位置
    // 这里可以添加自定义的位置调整逻辑
}

QString BrowserTabWidget::getDisplayText(const QString &text) const
{
    if (text.isEmpty()) {
        return "新标签页";
    }

    // 限制文本长度
    if (text.length() > 25) {
        return text.left(22) + "...";
    }
    return text;
}

} // namespace WinBrowserQt

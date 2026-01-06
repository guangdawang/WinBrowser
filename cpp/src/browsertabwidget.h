
#ifndef BROWSERTABWIDGET_H
#define BROWSERTABWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QToolButton>
#include <QMenu>
#include "models/browsertab.h"

namespace WinBrowserQt {

class BrowserTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BrowserTabWidget(QWidget *parent = nullptr);

    BrowserTab* createNewTab(const QString &url = "about:blank", const QString &title = "新标签页");
    void closeTab(int index);
    void closeSelectedTab();
    void closeOtherTabs();
    void closeAllTabs();
    void reloadSelectedTab();

    BrowserTab* getSelectedBrowserTab() const;
    BrowserTab* getBrowserTab(int index) const;
    void updateTabTitle(BrowserTab *browserTab, const QString &title);
    void updateTabUrl(BrowserTab *browserTab, const QString &url);

    int tabCount() const;
    int selectedIndex() const;
    void setSelectedIndex(int index);
    BrowserTab* selectedTab() const;
    QList<BrowserTab*> allTabs() const;

signals:
    void tabCreated(BrowserTab *tab);
    void tabClosed(BrowserTab *tab);
    void tabChanged(BrowserTab *tab);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTabCloseRequested(int index);
    void onCurrentChanged(int index);
    void onNewTabClicked();
    void onTabContextMenuRequested(const QPoint &pos);
    void onCloseTabAction();
    void onCloseOtherTabsAction();
    void onCloseAllTabsAction();
    void onReloadTabAction();

private:
    void initializeUI();
    void setupContextMenu();
    void updateNewTabButtonPosition();
    QString getDisplayText(const QString &text) const;

    QTabWidget *m_tabWidget;
    QToolButton *m_newTabButton;
    QMenu *m_tabContextMenu;
    QAction *m_closeTabAction;
    QAction *m_closeOtherTabsAction;
    QAction *m_closeAllTabsAction;
    QAction *m_reloadTabAction;
};

} // namespace WinBrowserQt

#endif // BROWSERTABWIDGET_H

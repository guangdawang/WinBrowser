
#ifndef NAVIGATIONMANAGER_H
#define NAVIGATIONMANAGER_H

#include <QObject>
#include <QList>
#include "models/historyitem.h"

namespace WinBrowserQt {

enum class HistoryChangeType {
    Added,
    Removed,
    Cleared
};

class HistoryChangedEventArgs
{
public:
    HistoryItem item;
    HistoryChangeType changeType;

    HistoryChangedEventArgs(const HistoryItem &i, HistoryChangeType type)
        : item(i), changeType(type) {}
};

class NavigationManager : public QObject
{
    Q_OBJECT

public:
    explicit NavigationManager(QObject *parent = nullptr);

    void addToHistory(const QString &url, const QString &title);
    bool canGoBack() const;
    bool canGoForward() const;
    HistoryItem goBack();
    HistoryItem goForward();
    HistoryItem getCurrentHistory() const;
    QList<HistoryItem> getHistory() const;
    void clearHistory();
    bool removeFromHistory(const QString &id);
    QList<HistoryItem> searchHistory(const QString &query) const;

signals:
    void historyChanged(const HistoryChangedEventArgs &args);

private:
    QList<HistoryItem> m_history;
    int m_currentHistoryIndex;
    static const int MAX_HISTORY_SIZE = 100;
};

} // namespace WinBrowserQt

#endif // NAVIGATIONMANAGER_H

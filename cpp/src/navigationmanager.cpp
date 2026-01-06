
#include "navigationmanager.h"
#include <QUuid>
#include <QDateTime>

namespace WinBrowserQt {

NavigationManager::NavigationManager(QObject *parent)
    : QObject(parent)
    , m_currentHistoryIndex(-1)
{
}

void NavigationManager::addToHistory(const QString &url, const QString &title)
{
    // 如果当前不在历史记录末尾，删除当前位置之后的所有记录
    if (m_currentHistoryIndex < m_history.size() - 1) {
        m_history = m_history.mid(0, m_currentHistoryIndex + 1);
    }

    HistoryItem item;
    item.setId(QUuid::createUuid().toString());
    item.setUrl(url);
    item.setTitle(title.isEmpty() ? url : title);
    item.setTimestamp(QDateTime::currentDateTime());
    item.setVisitCount(1);

    m_history.append(item);

    // 限制历史记录大小
    if (m_history.size() > MAX_HISTORY_SIZE) {
        m_history.removeFirst();
    } else {
        m_currentHistoryIndex++;
    }

    emit historyChanged(HistoryChangedEventArgs(item, HistoryChangeType::Added));
}

bool NavigationManager::canGoBack() const
{
    return m_currentHistoryIndex > 0;
}

bool NavigationManager::canGoForward() const
{
    return m_currentHistoryIndex < m_history.size() - 1;
}

HistoryItem NavigationManager::goBack()
{
    if (canGoBack()) {
        m_currentHistoryIndex--;
        return m_history[m_currentHistoryIndex];
    }
    return HistoryItem();
}

HistoryItem NavigationManager::goForward()
{
    if (canGoForward()) {
        m_currentHistoryIndex++;
        return m_history[m_currentHistoryIndex];
    }
    return HistoryItem();
}

HistoryItem NavigationManager::getCurrentHistory() const
{
    if (m_currentHistoryIndex >= 0 && m_currentHistoryIndex < m_history.size()) {
        return m_history[m_currentHistoryIndex];
    }
    return HistoryItem();
}

QList<HistoryItem> NavigationManager::getHistory() const
{
    return m_history;
}

void NavigationManager::clearHistory()
{
    m_history.clear();
    m_currentHistoryIndex = -1;
    emit historyChanged(HistoryChangedEventArgs(HistoryItem(), HistoryChangeType::Cleared));
}

bool NavigationManager::removeFromHistory(const QString &id)
{
    for (int i = 0; i < m_history.size(); ++i) {
        if (m_history[i].id() == id) {
            m_history.removeAt(i);
            if (m_currentHistoryIndex >= i) {
                m_currentHistoryIndex--;
            }
            emit historyChanged(HistoryChangedEventArgs(HistoryItem(), HistoryChangeType::Removed));
            return true;
        }
    }
    return false;
}

QList<HistoryItem> NavigationManager::searchHistory(const QString &query) const
{
    QList<HistoryItem> results;
    QString lowerQuery = query.toLower();

    for (const auto &item : m_history) {
        if (item.url().toLower().contains(lowerQuery) ||
            item.title().toLower().contains(lowerQuery)) {
            results.append(item);
        }
    }

    return results;
}

} // namespace WinBrowserQt

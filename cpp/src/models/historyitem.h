
#ifndef HISTORYITEM_H
#define HISTORYITEM_H

#include <QString>
#include <QDateTime>

namespace WinBrowserQt {

class HistoryItem
{
public:
    HistoryItem() = default;

    QString id() const { return m_id; }
    void setId(const QString &id) { m_id = id; }

    QString url() const { return m_url; }
    void setUrl(const QString &url) { m_url = url; }

    QString title() const { return m_title; }
    void setTitle(const QString &title) { m_title = title; }

    QDateTime timestamp() const { return m_timestamp; }
    void setTimestamp(const QDateTime &timestamp) { m_timestamp = timestamp; }

    int visitCount() const { return m_visitCount; }
    void setVisitCount(int count) { m_visitCount = count; }

private:
    QString m_id;
    QString m_url;
    QString m_title;
    QDateTime m_timestamp;
    int m_visitCount = 0;
};

} // namespace WinBrowserQt

#endif // HISTORYITEM_H


#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QString>
#include <QDateTime>

namespace WinBrowserQt {

class Bookmark
{
public:
    Bookmark() = default;

    QString id() const { return m_id; }
    void setId(const QString &id) { m_id = id; }

    QString title() const { return m_title; }
    void setTitle(const QString &title) { m_title = title; }

    QString url() const { return m_url; }
    void setUrl(const QString &url) { m_url = url; }

    QString folder() const { return m_folder; }
    void setFolder(const QString &folder) { m_folder = folder; }

    QDateTime dateAdded() const { return m_dateAdded; }
    void setDateAdded(const QDateTime &date) { m_dateAdded = date; }

private:
    QString m_id;
    QString m_title;
    QString m_url;
    QString m_folder;
    QDateTime m_dateAdded;
};

} // namespace WinBrowserQt

#endif // BOOKMARK_H


#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QObject>
#include <QList>
#include <QFuture>
#include <QtConcurrent>
#include "models/settings.h"
#include "models/bookmark.h"
#include "models/historyitem.h"

namespace WinBrowserQt {

class StorageManager : public QObject
{
    Q_OBJECT

public:
    explicit StorageManager(QObject *parent = nullptr);

    Settings loadSettings();
    void saveSettings(const Settings &settings);

    QList<Bookmark> loadBookmarks();
    void saveBookmarks(const QList<Bookmark> &bookmarks);

    QList<HistoryItem> loadHistory();
    void saveHistory(const QList<HistoryItem> &history);

    void saveAllData();
    
    // 异步保存方法
    QFuture<void> saveSettingsAsync(const Settings &settings);
    QFuture<void> saveBookmarksAsync(const QList<Bookmark> &bookmarks);
    QFuture<void> saveHistoryAsync(const QList<HistoryItem> &history);
    
signals:
    void dataSaved();
    void saveError(const QString &message);

private:
    QString m_dataDirectory;
    QString m_settingsFile;
    QString m_bookmarksFile;
    QString m_historyFile;

    void initializeDataDirectory();
    Settings getDefaultSettings() const;
    QList<Bookmark> getDefaultBookmarks() const;
};

} // namespace WinBrowserQt

#endif // STORAGEMANAGER_H

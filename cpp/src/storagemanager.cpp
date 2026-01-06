
#include "storagemanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

namespace WinBrowserQt {

StorageManager::StorageManager(QObject *parent)
    : QObject(parent)
{
    initializeDataDirectory();
}

void StorageManager::initializeDataDirectory()
{
    m_dataDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir dir(m_dataDirectory);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    m_settingsFile = m_dataDirectory + "/settings.json";
    m_bookmarksFile = m_dataDirectory + "/bookmarks.json";
    m_historyFile = m_dataDirectory + "/history.json";
}

Settings StorageManager::loadSettings()
{
    try {
        QFile file(m_settingsFile);
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();

            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                Settings settings;
                settings.setHomePage(obj["homePage"].toString("about:blank"));
                settings.setSearchEngine(obj["searchEngine"].toString("bing"));
                settings.setDownloadPath(obj["downloadPath"].toString(
                    QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)));
                settings.setShowBookmarksBar(obj["showBookmarksBar"].toBool(true));
                settings.setBlockPopups(obj["blockPopups"].toBool(true));
                settings.setEnableJavaScript(obj["enableJavaScript"].toBool(true));
                settings.setTheme(obj["theme"].toString("system"));
                return settings;
            }
        }
    } catch (const std::exception &e) {
        qWarning() << "加载设置失败:" << e.what();
    }

    return getDefaultSettings();
}

void StorageManager::saveSettings(const Settings &settings)
{
    try {
        QJsonObject obj;
        obj["homePage"] = settings.homePage();
        obj["searchEngine"] = settings.searchEngine();
        obj["downloadPath"] = settings.downloadPath();
        obj["showBookmarksBar"] = settings.showBookmarksBar();
        obj["blockPopups"] = settings.blockPopups();
        obj["enableJavaScript"] = settings.enableJavaScript();
        obj["theme"] = settings.theme();

        QJsonDocument doc(obj);
        QFile file(m_settingsFile);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson(QJsonDocument::Indented));
            file.close();
        }
    } catch (const std::exception &e) {
        qWarning() << "保存设置失败:" << e.what();
    }
}

QList<Bookmark> StorageManager::loadBookmarks()
{
    try {
        QFile file(m_bookmarksFile);
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();

            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isArray()) {
                QList<Bookmark> bookmarks;
                QJsonArray array = doc.array();
                for (const auto &value : array) {
                    if (value.isObject()) {
                        QJsonObject obj = value.toObject();
                        Bookmark bookmark;
                        bookmark.setId(obj["id"].toString());
                        bookmark.setTitle(obj["title"].toString());
                        bookmark.setUrl(obj["url"].toString());
                        bookmark.setFolder(obj["folder"].toString());
                        bookmark.setDateAdded(QDateTime::fromString(obj["dateAdded"].toString(), Qt::ISODate));
                        bookmarks.append(bookmark);
                    }
                }
                return bookmarks;
            }
        }
    } catch (const std::exception &e) {
        qWarning() << "加载书签失败:" << e.what();
    }

    return getDefaultBookmarks();
}

void StorageManager::saveBookmarks(const QList<Bookmark> &bookmarks)
{
    try {
        QJsonArray array;
        for (const auto &bookmark : bookmarks) {
            QJsonObject obj;
            obj["id"] = bookmark.id();
            obj["title"] = bookmark.title();
            obj["url"] = bookmark.url();
            obj["folder"] = bookmark.folder();
            obj["dateAdded"] = bookmark.dateAdded().toString(Qt::ISODate);
            array.append(obj);
        }

        QJsonDocument doc(array);
        QFile file(m_bookmarksFile);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson(QJsonDocument::Indented));
            file.close();
        }
    } catch (const std::exception &e) {
        qWarning() << "保存书签失败:" << e.what();
    }
}

QList<HistoryItem> StorageManager::loadHistory()
{
    try {
        QFile file(m_historyFile);
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();

            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isArray()) {
                QList<HistoryItem> history;
                QJsonArray array = doc.array();
                for (const auto &value : array) {
                    if (value.isObject()) {
                        QJsonObject obj = value.toObject();
                        HistoryItem item;
                        item.setId(obj["id"].toString());
                        item.setUrl(obj["url"].toString());
                        item.setTitle(obj["title"].toString());
                        item.setTimestamp(QDateTime::fromString(obj["timestamp"].toString(), Qt::ISODate));
                        item.setVisitCount(obj["visitCount"].toInt(1));
                        history.append(item);
                    }
                }
                return history;
            }
        }
    } catch (const std::exception &e) {
        qWarning() << "加载历史记录失败:" << e.what();
    }

    return QList<HistoryItem>();
}

void StorageManager::saveHistory(const QList<HistoryItem> &history)
{
    try {
        QJsonArray array;
        for (const auto &item : history) {
            QJsonObject obj;
            obj["id"] = item.id();
            obj["url"] = item.url();
            obj["title"] = item.title();
            obj["timestamp"] = item.timestamp().toString(Qt::ISODate);
            obj["visitCount"] = item.visitCount();
            array.append(obj);
        }

        QJsonDocument doc(array);
        QFile file(m_historyFile);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson(QJsonDocument::Indented));
            file.close();
        }
    } catch (const std::exception &e) {
        qWarning() << "保存历史记录失败:" << e.what();
    }
}

void StorageManager::saveAllData()
{
    // 保存所有数据的方法，由主程序调用
}

QFuture<void> StorageManager::saveSettingsAsync(const Settings &settings)
{
    return QtConcurrent::run([this, settings]() {
        try {
            QJsonObject obj;
            obj["homePage"] = settings.homePage();
            obj["searchEngine"] = settings.searchEngine();
            obj["downloadPath"] = settings.downloadPath();
            obj["showBookmarksBar"] = settings.showBookmarksBar();
            obj["blockPopups"] = settings.blockPopups();
            obj["enableJavaScript"] = settings.enableJavaScript();
            obj["theme"] = settings.theme();

            QJsonDocument doc(obj);
            QFile file(m_settingsFile);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(doc.toJson(QJsonDocument::Indented));
                file.close();
                QMetaObject::invokeMethod(this, "dataSaved", Qt::QueuedConnection);
            }
        } catch (const std::exception &e) {
            QMetaObject::invokeMethod(this, [this, msg = QString::fromUtf8(e.what())]() {
                emit saveError(msg);
            }, Qt::QueuedConnection);
        }
    });
}

QFuture<void> StorageManager::saveBookmarksAsync(const QList<Bookmark> &bookmarks)
{
    return QtConcurrent::run([this, bookmarks]() {
        try {
            QJsonArray array;
            for (const auto &bookmark : bookmarks) {
                QJsonObject obj;
                obj["id"] = bookmark.id();
                obj["title"] = bookmark.title();
                obj["url"] = bookmark.url();
                obj["folder"] = bookmark.folder();
                obj["dateAdded"] = bookmark.dateAdded().toString(Qt::ISODate);
                array.append(obj);
            }

            QJsonDocument doc(array);
            QFile file(m_bookmarksFile);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(doc.toJson(QJsonDocument::Indented));
                file.close();
                QMetaObject::invokeMethod(this, "dataSaved", Qt::QueuedConnection);
            }
        } catch (const std::exception &e) {
            QMetaObject::invokeMethod(this, [this, msg = QString::fromUtf8(e.what())]() {
                emit saveError(msg);
            }, Qt::QueuedConnection);
        }
    });
}

QFuture<void> StorageManager::saveHistoryAsync(const QList<HistoryItem> &history)
{
    return QtConcurrent::run([this, history]() {
        try {
            QJsonArray array;
            for (const auto &item : history) {
                QJsonObject obj;
                obj["id"] = item.id();
                obj["url"] = item.url();
                obj["title"] = item.title();
                obj["timestamp"] = item.timestamp().toString(Qt::ISODate);
                obj["visitCount"] = item.visitCount();
                array.append(obj);
            }

            QJsonDocument doc(array);
            QFile file(m_historyFile);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(doc.toJson(QJsonDocument::Indented));
                file.close();
                QMetaObject::invokeMethod(this, "dataSaved", Qt::QueuedConnection);
            }
        } catch (const std::exception &e) {
            QMetaObject::invokeMethod(this, [this, msg = QString::fromUtf8(e.what())]() {
                emit saveError(msg);
            }, Qt::QueuedConnection);
        }
    });
}

Settings StorageManager::getDefaultSettings() const
{
    Settings settings;
    settings.setHomePage("about:blank");
    settings.setSearchEngine("bing");
    settings.setDownloadPath(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    settings.setShowBookmarksBar(true);
    settings.setBlockPopups(true);
    settings.setEnableJavaScript(true);
    settings.setTheme("system");
    return settings;
}

QList<Bookmark> StorageManager::getDefaultBookmarks() const
{
    QList<Bookmark> bookmarks;

    Bookmark bing;
    bing.setId(QUuid::createUuid().toString());
    bing.setTitle("必应搜索");
    bing.setUrl("https://www.bing.com");
    bing.setFolder("");
    bing.setDateAdded(QDateTime::currentDateTime());
    bookmarks.append(bing);

    Bookmark github;
    github.setId(QUuid::createUuid().toString());
    github.setTitle("GitHub");
    github.setUrl("https://www.github.com");
    github.setFolder("开发");
    github.setDateAdded(QDateTime::currentDateTime());
    bookmarks.append(github);

    return bookmarks;
}

} // namespace WinBrowserQt

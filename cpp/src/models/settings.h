
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

namespace WinBrowserQt {

class Settings
{
public:
    Settings() = default;

    QString homePage() const { return m_homePage; }
    void setHomePage(const QString &page) { m_homePage = page; }

    QString searchEngine() const { return m_searchEngine; }
    void setSearchEngine(const QString &engine) { m_searchEngine = engine; }

    QString downloadPath() const { return m_downloadPath; }
    void setDownloadPath(const QString &path) { m_downloadPath = path; }

    bool showBookmarksBar() const { return m_showBookmarksBar; }
    void setShowBookmarksBar(bool show) { m_showBookmarksBar = show; }

    bool blockPopups() const { return m_blockPopups; }
    void setBlockPopups(bool block) { m_blockPopups = block; }

    bool enableJavaScript() const { return m_enableJavaScript; }
    void setEnableJavaScript(bool enable) { m_enableJavaScript = enable; }

    QString theme() const { return m_theme; }
    void setTheme(const QString &theme) { m_theme = theme; }

private:
    QString m_homePage = "about:blank";
    QString m_searchEngine = "bing";
    QString m_downloadPath;
    bool m_showBookmarksBar = true;
    bool m_blockPopups = true;
    bool m_enableJavaScript = true;
    QString m_theme = "system";
};

} // namespace WinBrowserQt

#endif // SETTINGS_H

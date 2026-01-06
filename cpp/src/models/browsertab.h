
#ifndef BROWSERTAB_H
#define BROWSERTAB_H

#include <QString>
#include <QSharedPointer>
#include <QWebEngineView>

namespace WinBrowserQt {

class BrowserTab
{
public:
    explicit BrowserTab(const QString &id = QString(),
                       const QString &url = QString(),
                       const QString &title = QString());

    QString id() const { return m_id; }
    void setId(const QString &id) { m_id = id; }

    QString url() const { return m_url; }
    void setUrl(const QString &url) { m_url = url; }

    QString title() const { return m_title; }
    void setTitle(const QString &title) { m_title = title; }

    bool isLoading() const { return m_isLoading; }
    void setIsLoading(bool loading) { m_isLoading = loading; }

    QWebEngineView* webView() const { return m_webView; }
    void setWebView(QWebEngineView* view) { m_webView = view; }

private:
    QString m_id;
    QString m_url;
    QString m_title;
    bool m_isLoading;
    QWebEngineView* m_webView;
};

} // namespace WinBrowserQt

#endif // BROWSERTAB_H

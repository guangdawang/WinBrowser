
#include "browsertab.h"
#include <QUuid>

namespace WinBrowserQt {

BrowserTab::BrowserTab(const QString &id, const QString &url, const QString &title)
    : m_id(id.isEmpty() ? QUuid::createUuid().toString() : id)
    , m_url(url)
    , m_title(title)
    , m_isLoading(false)
    , m_webView(nullptr)
{
}

} // namespace WinBrowserQt

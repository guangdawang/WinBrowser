
#include "mainwindow.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QWebEnginePage>
#include <QWebEngineHistory>

namespace WinBrowserQt {

void MainWindow::updateNavigationButtons()
{
    if (m_currentTab && m_currentTab->webView()) {
        QWebEnginePage *page = m_currentTab->webView()->page();
        if (page) {
            m_backAction->setEnabled(page->history()->canGoBack());
            m_forwardAction->setEnabled(page->history()->canGoForward());
        } else {
            m_backAction->setEnabled(false);
            m_forwardAction->setEnabled(false);
        }
    } else {
        m_backAction->setEnabled(false);
        m_forwardAction->setEnabled(false);
    }
}

void MainWindow::updateStatus(const QString &message)
{
    m_statusBar->showMessage(message);
}

QString MainWindow::ensureUrlWithProtocol(const QString &url) const
{
    if (url.trimmed().isEmpty()) {
        return "about:blank";
    }

    // 提取主机名部分进行分析
    QString host = extractHostFromUrl(url);

    if (isLocalOrPrivateAddress(host)) {
        // 对于localhost和私有IP地址，使用HTTP
        return "http://" + url;
    } else {
        // 对于公网域名，使用HTTPS
        return "https://" + url;
    }
}

QString MainWindow::extractHostFromUrl(const QString &url) const
{
    try {
        // 移除可能的路径和查询参数
        QString host = url.split('/')[0].split('?')[0].split(':')[0];
        return host.toLower();
    } catch (...) {
        return url.toLower();
    }
}

bool MainWindow::isLocalOrPrivateAddress(const QString &host) const
{
    if (host.trimmed().isEmpty()) {
        return false;
    }

    QString lowerHost = host.toLower();

    // 检查 localhost
    if (lowerHost == "localhost" || lowerHost.startsWith("localhost.")) {
        return true;
    }

    // 检查环回地址
    if (lowerHost == "127.0.0.1" || lowerHost == "::1" || lowerHost == "[::1]") {
        return true;
    }

    // 检查IPv4地址
    QHostAddress address;
    if (address.setAddress(host)) {
        // 检查私有IP地址范围
        if (isPrivateIPAddress(address)) {
            return true;
        }

        // 检查环回地址
        if (address.isLoopback()) {
            return true;
        }
    }

    return false;
}

bool MainWindow::isPrivateIPAddress(const QHostAddress &address) const
{
    if (address.protocol() == QAbstractSocket::IPv4Protocol) {
        quint32 ipv4 = address.toIPv4Address();

        // 10.0.0.0/8
        if ((ipv4 & 0xFF000000) == 0x0A000000) {
            return true;
        }

        // 172.16.0.0/12
        if ((ipv4 & 0xFFF00000) == 0xAC100000) {
            return true;
        }

        // 192.168.0.0/16
        if ((ipv4 & 0xFFFF0000) == 0xC0A80000) {
            return true;
        }

        // 169.254.0.0/16 (链路本地)
        if ((ipv4 & 0xFFFF0000) == 0xA9FE0000) {
            return true;
        }
    } else if (address.protocol() == QAbstractSocket::IPv6Protocol) {
        // IPv6 私有地址范围检查
        if (address.isLinkLocal() || address.isLoopback()) {
            return true;
        }

        // 检查唯一本地地址 (fc00::/7)
        Q_IPV6ADDR ipv6 = address.toIPv6Address();
        if ((ipv6[0] & 0xFE) == 0xFC) {
            return true;
        }
    }

    return false;
}

} // namespace WinBrowserQt

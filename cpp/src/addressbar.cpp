
#include "addressbar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QUrl>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QApplication>
#include <QScreen>
#include <QGuiApplication>
#include <QUuid>

namespace WinBrowserQt {

AddressBar::AddressBar(QWidget *parent)
    : QWidget(parent)
    , m_selectedSuggestionIndex(-1)
    , m_isShowingSuggestions(false)
{
    initializeUI();
    setupEventHandlers();
}

void AddressBar::initializeUI()
{
    setMinimumHeight(35);
    setMaximumHeight(35);

    // 主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(3, 3, 3, 3);
    mainLayout->setSpacing(5);

    // 地址文本框
    m_addressTextBox = new QLineEdit(this);
    m_addressTextBox->setFont(QFont("Segoe UI", 10));
    m_addressTextBox->setPlaceholderText("输入网址或搜索内容");
    mainLayout->addWidget(m_addressTextBox, 1);

    // 转到按钮
    m_goButton = new QPushButton("转到", this);
    m_goButton->setFixedSize(60, 24);
    m_goButton->setFlat(true);
    mainLayout->addWidget(m_goButton);

    // 建议面板
    m_suggestionsPanel = new QWidget(this);
    m_suggestionsPanel->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    m_suggestionsPanel->setAttribute(Qt::WA_TranslucentBackground);

    QVBoxLayout *panelLayout = new QVBoxLayout(m_suggestionsPanel);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(0);

    // 建议列表
    m_suggestionsList = new QListWidget(m_suggestionsPanel);
    m_suggestionsList->setFont(QFont("Segoe UI", 9));
    m_suggestionsList->setMaximumHeight(200);
    m_suggestionsList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    panelLayout->addWidget(m_suggestionsList);

    // 建议定时器
    m_suggestionsTimer = new QTimer(this);
    m_suggestionsTimer->setInterval(300);
    m_suggestionsTimer->setSingleShot(true);

    // 网络管理器
    m_networkManager = new QNetworkAccessManager(this);
}

void AddressBar::setupEventHandlers()
{
    m_addressTextBox->installEventFilter(this);
    connect(m_addressTextBox, &QLineEdit::textChanged, this, &AddressBar::onTextChanged);

    connect(m_goButton, &QPushButton::clicked, this, &AddressBar::onGoButtonClicked);
    connect(m_suggestionsList, &QListWidget::itemClicked, this, &AddressBar::onSuggestionSelected);

    connect(m_suggestionsTimer, &QTimer::timeout, this, &AddressBar::onSuggestionsTimerTimeout);
}

bool AddressBar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_addressTextBox) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

            if (m_isShowingSuggestions) {
                if (keyEvent->key() == Qt::Key_Down) {
                    selectNextSuggestion();
                    return true;
                } else if (keyEvent->key() == Qt::Key_Up) {
                    selectPreviousSuggestion();
                    return true;
                } else if (keyEvent->key() == Qt::Key_Escape) {
                    hideSuggestions();
                    return true;
                }
            }

            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                navigate();
                return true;
            }
        } else if (event->type() == QEvent::FocusIn) {
            m_addressTextBox->selectAll();
            showSuggestions();
        } else if (event->type() == QEvent::FocusOut) {
            // 延迟隐藏建议，以便处理建议列表的点击事件
            QTimer::singleShot(100, this, [this]() {
                if (!m_suggestionsList->hasFocus() && !m_suggestionsPanel->hasFocus()) {
                    hideSuggestions();
                }
            });
        }
    }

    return QWidget::eventFilter(watched, event);
}

void AddressBar::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_suggestionsPanel->isVisible()) {
        m_suggestionsPanel->setGeometry(0, height(), width(), m_suggestionsPanel->height());
    }
}

void AddressBar::onTextChanged(const QString &text)
{
    m_suggestionsTimer->start();
}

void AddressBar::onGoButtonClicked()
{
    navigate();
}

void AddressBar::onSuggestionSelected()
{
    if (m_selectedSuggestionIndex >= 0 && m_selectedSuggestionIndex < m_suggestions.size()) {
        m_addressTextBox->setText(m_suggestions[m_selectedSuggestionIndex].url);
        hideSuggestions();
        navigateTo(m_suggestions[m_selectedSuggestionIndex].url);
    }
}

void AddressBar::onSuggestionsTimerTimeout()
{
    generateSuggestions(m_addressTextBox->text());
}

void AddressBar::setUrl(const QString &url)
{
    m_addressTextBox->setText(url);
}

QString AddressBar::getUrl() const
{
    return m_addressTextBox->text().trimmed();
}

void AddressBar::focusAddressBox()
{
    m_addressTextBox->setFocus();
    m_addressTextBox->selectAll();
}

void AddressBar::clear()
{
    m_addressTextBox->clear();
    hideSuggestions();
}

void AddressBar::generateSuggestions(const QString &input)
{
    if (input.trimmed().isEmpty()) {
        hideSuggestions();
        return;
    }

    m_suggestions.clear();

    // 添加搜索建议
    if (input.length() > 2) {
        SuggestionItem searchSuggestion;
        searchSuggestion.type = SuggestionType::Search;
        searchSuggestion.title = QString("搜索 \"%1\"").arg(input);
        searchSuggestion.url = QString("https://www.bing.com/search?q=%1")
            .arg(QString::fromUtf8(QUrl::toPercentEncoding(input)));
        m_suggestions.append(searchSuggestion);
    }

    // 添加URL建议
    if (isValidUrl(input) || input.contains('.')) {
        QString url = input.startsWith("http") ? input : "https://" + input;
        SuggestionItem urlSuggestion;
        urlSuggestion.type = SuggestionType::Url;
        urlSuggestion.title = input;
        urlSuggestion.url = url;
        m_suggestions.append(urlSuggestion);
    }

    updateSuggestionsList();

    if (!m_suggestions.isEmpty()) {
        showSuggestions();
    } else {
        hideSuggestions();
    }
}

void AddressBar::updateSuggestionsList()
{
    m_suggestionsList->clear();

    int maxSuggestions = qMin(8, m_suggestions.size());
    for (int i = 0; i < maxSuggestions; ++i) {
        const auto &suggestion = m_suggestions[i];
        QString displayText = QString("%1\n%2")
            .arg(suggestion.title)
            .arg(suggestion.url);
        m_suggestionsList->addItem(displayText);
    }
}

void AddressBar::showSuggestions()
{
    if (m_suggestions.isEmpty()) return;

    m_suggestionsList->clear();
    for (const auto &suggestion : m_suggestions) {
        QString iconText;
        QColor iconColor;

        switch (suggestion.type) {
        case SuggestionType::Search:
            iconText = "🔍";
            iconColor = QColor("#4285f4");
            break;
        case SuggestionType::Url:
            iconText = "🌐";
            iconColor = QColor("#34a853");
            break;
        case SuggestionType::History:
            iconText = "🕐";
            iconColor = QColor("#fbbc05");
            break;
        }

        QString itemText = QString("%1 %2\n%3")
            .arg(iconText)
            .arg(suggestion.title)
            .arg(suggestion.url);

        QListWidgetItem *item = new QListWidgetItem(itemText);
        m_suggestionsList->addItem(item);
    }

    m_suggestionsPanel->setGeometry(0, height(), width(), qMin(200, m_suggestionsList->sizeHintForRow(0) * m_suggestions.size() + 4));
    m_suggestionsPanel->show();
    m_suggestionsPanel->raise();
    m_isShowingSuggestions = true;
    m_selectedSuggestionIndex = -1;
}

void AddressBar::hideSuggestions()
{
    m_suggestionsPanel->hide();
    m_isShowingSuggestions = false;
    m_selectedSuggestionIndex = -1;
}

void AddressBar::selectNextSuggestion()
{
    if (m_suggestions.isEmpty()) return;

    m_selectedSuggestionIndex++;
    if (m_selectedSuggestionIndex >= m_suggestionsList->count()) {
        m_selectedSuggestionIndex = 0;
    }

    m_suggestionsList->setCurrentRow(m_selectedSuggestionIndex);
    m_addressTextBox->setText(m_suggestions[m_selectedSuggestionIndex].url);
}

void AddressBar::selectPreviousSuggestion()
{
    if (m_suggestions.isEmpty()) return;

    m_selectedSuggestionIndex--;
    if (m_selectedSuggestionIndex < 0) {
        m_selectedSuggestionIndex = m_suggestionsList->count() - 1;
    }

    m_suggestionsList->setCurrentRow(m_selectedSuggestionIndex);
    m_addressTextBox->setText(m_suggestions[m_selectedSuggestionIndex].url);
}

void AddressBar::navigate()
{
    QString url = getUrl();
    if (!url.isEmpty()) {
        navigateTo(url);
    }
}

void AddressBar::navigateTo(const QString &url)
{
    if (url.isEmpty()) return;

    // 检查是否为本地地址或私有IP地址
    if (isLocalOrPrivateAddress(url)) {
        emit navigateRequested(url);
        hideSuggestions();
        return;
    }

    // 检查是否是有效的URL或域名
    if (isValidUrl(url) || isLikelyDomain(url)) {
        emit navigateRequested(url);
    } else {
        // 否则进行搜索
        QString searchUrl = QString("https://www.bing.com/search?q=%1")
            .arg(QString::fromUtf8(QUrl::toPercentEncoding(url)));
        emit searchRequested(url);
        navigateRequested.emit(searchUrl);
    }
    hideSuggestions();
}

bool AddressBar::isLocalOrPrivateAddress(const QString &input) const
{
    if (input.trimmed().isEmpty()) return false;

    QString host = extractHostFromInput(input);

    // 检查 localhost
    if (host == "localhost" || host.startsWith("localhost.")) {
        return true;
    }

    // 检查环回地址
    if (host == "127.0.0.1" || host == "::1" || host == "[::1]") {
        return true;
    }

    // 检查IPv4地址
    QHostAddress address;
    if (address.setAddress(host)) {
        if (isPrivateIPAddress(host)) {
            return true;
        }
        if (address.isLoopback()) {
            return true;
        }
    }

    return false;
}

QString AddressBar::extractHostFromInput(const QString &input) const
{
    try {
        // 移除可能的路径和查询参数
        QString host = input.split('/')[0].split('?')[0].split(':')[0];
        return host.toLower();
    } catch (...) {
        return input.toLower();
    }
}

bool AddressBar::isPrivateIPAddress(const QString &host) const
{
    QHostAddress address;
    if (!address.setAddress(host)) {
        return false;
    }

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

bool AddressBar::isValidUrl(const QString &url) const
{
    QUrl qurl(url);
    return qurl.isValid() && !qurl.scheme().isEmpty();
}

bool AddressBar::isLikelyDomain(const QString &text) const
{
    // 简单的域名检查：包含点且不包含空格
    return text.contains('.') && !text.contains(' ');
}

} // namespace WinBrowserQt

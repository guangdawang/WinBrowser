
#ifndef ADDRESSBAR_H
#define ADDRESSBAR_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <QNetworkAccessManager>

namespace WinBrowserQt {

enum class SuggestionType {
    Search,
    Url,
    History
};

class SuggestionItem
{
public:
    QString title;
    QString url;
    SuggestionType type;
};

class AddressBar : public QWidget
{
    Q_OBJECT

public:
    explicit AddressBar(QWidget *parent = nullptr);

    void setUrl(const QString &url);
    QString getUrl() const;
    void focusAddressBox();
    void clear();

signals:
    void navigateRequested(const QString &url);
    void searchRequested(const QString &searchTerm);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTextChanged(const QString &text);
    void onGoButtonClicked();
    void onSuggestionSelected();
    void onSuggestionsTimerTimeout();

private:
    void initializeUI();
    void setupEventHandlers();
    void generateSuggestions(const QString &input);
    void updateSuggestionsList();
    void showSuggestions();
    void hideSuggestions();
    void selectNextSuggestion();
    void selectPreviousSuggestion();
    void navigate();
    void navigateTo(const QString &url);
    bool isLocalOrPrivateAddress(const QString &input) const;
    QString extractHostFromInput(const QString &input) const;
    bool isPrivateIPAddress(const QString &host) const;
    bool isValidUrl(const QString &url) const;
    bool isLikelyDomain(const QString &text) const;

    QLineEdit *m_addressTextBox;
    QPushButton *m_goButton;
    QListWidget *m_suggestionsList;
    QWidget *m_suggestionsPanel;
    QTimer *m_suggestionsTimer;
    QNetworkAccessManager *m_networkManager;

    QList<SuggestionItem> m_suggestions;
    int m_selectedSuggestionIndex;
    bool m_isShowingSuggestions;
};

} // namespace WinBrowserQt

#endif // ADDRESSBAR_H

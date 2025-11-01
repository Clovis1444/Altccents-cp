#pragma once

#include <QChar>
#include <QList>
#include <QObject>
#include <QWidget>

namespace Altccents {
class Popup : public QWidget {
    Q_OBJECT

   public:
    explicit Popup(QWidget* parent = nullptr);

    void show(const QList<QChar>& chars, unsigned int active_char,
              const QList<QChar>& tabs, unsigned int active_tab);

   signals:
    void hidden();
    // Note: Popup::hide() should be explicitly called after this signal was
    // emited
    void accentChosen();
    void nextAccent(bool forward);
    void nextTab(bool forward);
    void capitalChanged(bool is_capital);

   private:
    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;
    void hideEvent(QHideEvent* e) override;

    struct CharCollection {
        QList<QChar> chars;
        unsigned int active_index;
    };
    struct TabCollection {
        QList<QChar> tabs;
        unsigned int active_index;
    };

    CharCollection charCollection_;
    TabCollection tabCollection_;
};
}  // namespace Altccents

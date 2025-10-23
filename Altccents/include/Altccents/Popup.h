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

   private:
    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent* e) override;

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

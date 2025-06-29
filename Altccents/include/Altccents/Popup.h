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

    void show(const QList<QChar>& chars, unsigned int active_index);

   private:
    void paintEvent(QPaintEvent*) override;
};
}  // namespace Altccents

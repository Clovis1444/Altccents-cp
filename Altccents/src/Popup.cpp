#include "Altccents/Popup.h"

#include <qminmax.h>
#include <qtypes.h>

#include <QApplication>
#include <QPainter>
#include <QScreen>
#include <QtGlobal>

#include "Altccents/Altccents.h"

namespace Altccents {
Popup::Popup(QWidget* parent) : QWidget{parent} {
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    // Make the window not appear in taskbar
    setWindowFlag(Qt::Tool);
    // Enable click through the window to other apps
    setWindowFlag(Qt::WindowTransparentForInput);
    setWindowFlag(Qt::WindowDoesNotAcceptFocus);

    setAttribute(Qt::WA_TranslucentBackground);
    // setAttribute(Qt::WA_NoSystemBackground);
}

void Popup::show(const QList<QChar>& chars, unsigned int active_index) {
    if (chars.isEmpty() || active_index >= chars.count()) {
        return;
    }

    charCollection_ = {chars, active_index};

    // Opacity
    setWindowOpacity(
        qBound(0.0F, Settings::get(Settings::kPopupOpacity).toFloat(), 1.0F));

    // TODO(clovis): create setting option for popup outline border
    // TODO(clovis): create setting option for popup margin
    // TODO(clovis): create setting option for char box margin
    // Size
    int char_box_size{Settings::get(Settings::kCharBoxSize).toInt()};
    resize(static_cast<int>(char_box_size * chars.count()), char_box_size);

    // Position
    QRect screen_geometry{QApplication::primaryScreen()->availableGeometry()};

    float x_pos{Settings::get(Settings::kPopupX).toFloat()};
    float y_pos{Settings::get(Settings::kPopupY).toFloat()};

    int x{static_cast<int>((screen_geometry.width() - width()) * x_pos)};
    int y{static_cast<int>((screen_geometry.height() - height()) * y_pos)};

    move(x, y);

    update();
    QWidget::show();
}

void Popup::paintEvent(QPaintEvent*) {
    QPainter p{this};

    // TODO(clovis): create setting option for corners rounding
    qreal rounding_radius{0.5};

    int char_box_size{Settings::get(Settings::kCharBoxSize).toInt()};

    for (int i{}; i < charCollection_.chars.count(); ++i) {
        // TODO(clovis): create settings options for colors
        if (i == charCollection_.active_index) {
            p.fillRect(i * char_box_size, 0, char_box_size, char_box_size,
                       Qt::white);
        } else {
            p.fillRect(i * char_box_size, 0, char_box_size, char_box_size,
                       Qt::blue);
        }

        // p.drawRoundedRect(i * char_box_size, 0, char_box_size, char_box_size,
        //                   rounding_radius, rounding_radius);
    }

    QPen pen{Qt::red};
    pen.setWidth(1);
    p.setPen(pen);
    p.drawRect(0, 0, width() - 1, height() - 1);
}
}  // namespace Altccents

#include "Altccents/Popup.h"

#include <qminmax.h>

#include <QApplication>
#include <QPainter>
#include <QPainterPath>
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

    charCollection_ = {.chars = chars, .active_index = active_index};

    // Opacity
    setWindowOpacity(
        qBound(0.0, Settings::get(Settings::kPopupOpacity).toDouble(), 1.0));

    // TODO(clovis): create setting option for popup outline border
    // TODO(clovis): create setting option for popup margin
    // TODO(clovis): create setting option for char box margin
    // Size
    int char_box_size{Settings::get(Settings::kCharBoxSize).toInt()};
    int border_width{Settings::get(Settings::kPopupBorderWidth).toInt()};

    int w{static_cast<int>((char_box_size * chars.count()) + border_width)};
    int h{char_box_size + border_width};
    resize(w, h);

    // Position
    QRect screen_geometry{QApplication::primaryScreen()->availableGeometry()};

    double x_pos{Settings::get(Settings::kPopupX).toDouble()};
    double y_pos{Settings::get(Settings::kPopupY).toDouble()};

    int x{static_cast<int>((screen_geometry.width() - width()) * x_pos)};
    int y{static_cast<int>((screen_geometry.height() - height()) * y_pos)};

    move(x, y);

    update();
    QWidget::show();
}

void Popup::paintEvent(QPaintEvent*) {
    QPainter p{this};
    p.setRenderHint(QPainter::Antialiasing);

    int char_box_size{Settings::get(Settings::kCharBoxSize).toInt()};
    QColor background_color{
        Settings::get(Settings::kPopupBackgorundColor).value<QColor>()};
    QColor char_box_color{
        Settings::get(Settings::kCharBoxColor).value<QColor>()};
    QColor char_box_active_color{
        Settings::get(Settings::kCharBoxActiveColor).value<QColor>()};
    double rounding{Settings::get(Settings::kPopupRounding).toDouble()};
    int border_width{Settings::get(Settings::kPopupBorderWidth).toInt()};
    QColor border_color{
        Settings::get(Settings::kPopupBorderColor).value<QColor>()};
    QColor char_box_border_color{
        Settings::get(Settings::kCharBoxBorderColor).value<QColor>()};
    QColor char_box_active_border_color{
        Settings::get(Settings::kCharBoxActiveBorderColor).value<QColor>()};

    // Draw Popup box
    QPainterPath popup_box{};

    int offset{border_width / 2};

    QRect popup_rect{offset, offset, width() - (offset * 2),
                     height() - (offset * 2)};

    popup_box.addRoundedRect(popup_rect, rounding, rounding);

    QPen popup_pen{};
    popup_pen.setColor(border_color);
    popup_pen.setWidth(border_width);

    p.setPen(popup_pen);
    p.setBrush(background_color);

    p.drawPath(popup_box);
    //

    // TODO(clovis): add margins options
    // TODO(clovis): add font, font size, font color options

    // Draw char boxes
    for (int i{}; i < charCollection_.chars.count(); ++i) {
        QPainterPath char_box{};

        QRect char_box_rect{offset + (i * char_box_size), offset, char_box_size,
                            char_box_size};

        char_box.addRoundedRect(char_box_rect, rounding, rounding);

        QPen char_box_pen{};
        char_box_pen.setWidth(border_width);

        if (i == charCollection_.active_index) {
            p.setBrush(char_box_active_color);
            char_box_pen.setColor(char_box_active_border_color);
        } else {
            p.setBrush(char_box_color);
            char_box_pen.setColor(char_box_border_color);
        }

        p.setPen(char_box_pen);

        p.drawPath(char_box);
    }
}
}  // namespace Altccents

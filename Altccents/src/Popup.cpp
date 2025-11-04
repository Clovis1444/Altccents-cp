#include "Altccents/Popup.h"

#include <qminmax.h>

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QtGlobal>

#include "Altccents/Settings.h"

namespace Altccents {
Popup::Popup(QWidget* parent) : QWidget{parent} {
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    // Make the window not appear in taskbar
    setWindowFlag(Qt::Popup);
    // Enable click through the window to other apps
    setWindowFlag(Qt::WindowTransparentForInput);
    // setWindowFlag(Qt::WindowDoesNotAcceptFocus);

    setAttribute(Qt::WA_TranslucentBackground);
    // setAttribute(Qt::WA_NoSystemBackground);
}

void Popup::show(const QList<QChar>& chars, unsigned int active_char,
                 const QList<QChar>& tabs, unsigned int active_tab) {
    if (chars.isEmpty() || chars.count() <= active_char ||
        (tabs.count() <= active_tab && !tabs.isEmpty())) {
        return;
    }

    charCollection_ = {.chars = chars, .active_index = active_char};
    tabCollection_ = {.tabs = tabs, .active_index = active_tab};

    // Opacity
    setWindowOpacity(
        qBound(0.0, Settings::get(Settings::kPopupOpacity).toDouble(), 1.0));

    int margin{Settings::get(Settings::kPopupMargins).toInt()};
    int char_box_size{Settings::get(Settings::kCharBoxSize).toInt()};
    int tab_size{
        qMin(Settings::get(Settings::kPopupTabSize).toInt(), char_box_size)};
    int border_width{Settings::get(Settings::kPopupBorderWidth).toInt()};

    int t_h{tabs.isEmpty() ? 0 : tab_size};

    int cb_w{static_cast<int>((char_box_size * chars.count()) + border_width +
                              (margin * (chars.count() + 1)))};
    int cb_h{char_box_size + border_width + (margin * 2)};

    int w{cb_w};
    int h{t_h + cb_h};
    resize(w, h);

    // Position
    QRect screen_geometry{QApplication::primaryScreen()->availableGeometry()};

    double x_pos{qBound(0.0, Settings::get(Settings::kPopupX).toDouble(), 1.0)};
    double y_pos{qBound(0.0, Settings::get(Settings::kPopupY).toDouble(), 1.0)};

    int x{static_cast<int>((screen_geometry.width() - width()) * x_pos)};
    int y{static_cast<int>((screen_geometry.height() - height()) * y_pos)};

    move(x, y);

    update();
    QWidget::show();
}

// TODO(clovis): implement case where tabs width > chars width
void Popup::paintEvent(QPaintEvent*) {
    QPainter p{this};
    p.setRenderHint(QPainter::Antialiasing);

    int margin{Settings::get(Settings::kPopupMargins).toInt()};
    int tab_margin{
        qMin(Settings::get(Settings::kPopupTabMargins).toInt(), margin)};
    int char_box_size{Settings::get(Settings::kCharBoxSize).toInt()};
    int tab_size{
        qMin(Settings::get(Settings::kPopupTabSize).toInt(), char_box_size)};
    QColor background_color{
        Settings::get(Settings::kPopupBackgorundColor).value<QColor>()};
    QColor char_box_color{
        Settings::get(Settings::kCharBoxColor).value<QColor>()};
    QColor char_box_active_color{
        Settings::get(Settings::kCharBoxActiveColor).value<QColor>()};
    double rounding{
        qBound(0.0, Settings::get(Settings::kPopupRounding).toDouble(), 1.0)};
    int border_width{Settings::get(Settings::kPopupBorderWidth).toInt()};
    QColor border_color{
        Settings::get(Settings::kPopupBorderColor).value<QColor>()};
    QColor char_box_border_color{
        Settings::get(Settings::kCharBoxBorderColor).value<QColor>()};
    QColor char_box_active_border_color{
        Settings::get(Settings::kCharBoxActiveBorderColor).value<QColor>()};
    QColor text_color{Settings::get(Settings::kPopupFontColor).value<QColor>()};
    QColor active_text_color{
        Settings::get(Settings::kPopupActiveFontColor).value<QColor>()};
    QString font_family{Settings::get(Settings::kPopupFontFamily).toString()};
    int font_point_size{Settings::get(Settings::kPopupFontPointSize).toInt()};
    int tab_font_point_size{
        Settings::get(Settings::kPopupTabFontPointSize).toInt()};
    int font_weight{Settings::get(Settings::kPopupFontWeight).toInt()};
    bool font_italic{Settings::get(Settings::kPopupFontItalic).toBool()};

    QFont font{font_family, font_point_size, font_weight, font_italic};
    p.setFont(font);

    int offset{border_width / 2};

    // popup_rect.height() * rounding
    qreal popup_rect_radius{(char_box_size + (margin * 2) + offset) * rounding};
    // Use qMin() to prevent bizzare shapes
    qreal tab_radius{tab_size * qMin(rounding, 0.5)};
    qreal char_box_rect_radius{char_box_size * rounding};

    // p.fillRect(0, 0, width(), height(), Qt::green);

    font.setPointSize(tab_font_point_size);
    p.setFont(font);

    // [1] Draw tabs
    for (int i{}; i < tabCollection_.tabs.count(); ++i) {
        QPainterPath tab{};

        // Draw tab rect
        QRect tab_rect{
            static_cast<int>(popup_rect_radius + ((tab_size + tab_margin) * i)),
            offset, tab_size, tab_size};

        tab.moveTo(tab_rect.bottomLeft());
        tab.lineTo(tab_rect.bottomRight());
        tab.lineTo(tab_rect.right(), tab_rect.top() + tab_radius);
        tab.arcTo(QRectF(tab_rect.right() - (tab_radius * 2), tab_rect.top(),
                         tab_radius * 2, (tab_radius * 2)),
                  0, 90);
        tab.lineTo(tab_rect.left() + tab_radius, tab_rect.top());
        tab.arcTo(QRectF(tab_rect.left(), tab_rect.top(), tab_radius * 2,
                         tab_radius * 2),
                  90, 90);
        tab.lineTo(tab_rect.bottomLeft());
        //
        QPen tab_pen{};
        tab_pen.setColor(i == tabCollection_.active_index
                             ? char_box_active_border_color
                             : char_box_border_color);
        tab_pen.setWidth(border_width);
        p.setPen(tab_pen);
        p.setBrush(i == tabCollection_.active_index ? char_box_active_color
                                                    : background_color);

        tab.closeSubpath();
        p.drawPath(tab);

        // Draw tab text
        QPen text_pen;
        text_pen.setColor(i == tabCollection_.active_index ? active_text_color
                                                           : text_color);
        p.setPen(text_pen);
        QTextOption text_options{Qt::AlignCenter};
        p.drawText(tab_rect, tabCollection_.tabs[i], text_options);
    }

    font.setPointSize(font_point_size);
    p.setFont(font);

    // [2] Draw Popup box
    QPainterPath popup_box{};

    QRect popup_rect{
        offset, (tabCollection_.tabs.isEmpty() ? 0 : tab_size) + offset,
        width() - (offset * 2), char_box_size + (margin * 2) + offset};

    popup_box.addRoundedRect(popup_rect, popup_rect_radius, popup_rect_radius);

    QPen popup_pen{};
    popup_pen.setColor(border_color);
    popup_pen.setWidth(border_width);

    p.setPen(popup_pen);
    p.setBrush(background_color);

    p.drawPath(popup_box);
    //

    // [3] Draw char boxes
    for (int i{}; i < charCollection_.chars.count(); ++i) {
        QPainterPath char_box{};

        QRect char_box_rect{offset + (char_box_size * i) + (margin * (i + 1)),
                            popup_rect.top() + margin, char_box_size,
                            char_box_size};

        char_box.addRoundedRect(char_box_rect, char_box_rect_radius,
                                char_box_rect_radius);

        QPen char_box_pen{};
        QPen text_pen{};
        char_box_pen.setWidth(border_width);

        if (i == charCollection_.active_index) {
            p.setBrush(char_box_active_color);
            char_box_pen.setColor(char_box_active_border_color);
            text_pen.setColor(active_text_color);
        } else {
            p.setBrush(char_box_color);
            char_box_pen.setColor(char_box_border_color);
            text_pen.setColor(text_color);
        }

        p.setPen(char_box_pen);
        // Draw char_box
        p.drawPath(char_box);

        p.setPen(text_pen);
        QTextOption text_options{Qt::AlignCenter};
        // Draw text
        p.drawText(char_box_rect, charCollection_.chars[i], text_options);
    }
}

void Popup::keyPressEvent(QKeyEvent* e) {
    switch (e->key()) {
        case Qt::Key_Shift: {
            emit capitalChanged(true);
            break;
        }
        default:
            break;
    }

    QWidget::keyPressEvent(e);
}

void Popup::keyReleaseEvent(QKeyEvent* e) {
    if (Settings::kNextAccentKeys.contains(e->key())) {
        emit nextAccent(true);
    }
    if (Settings::kPrevAccentKeys.contains(e->key())) {
        emit nextAccent(false);
    }
    if (Settings::kNextTabKeys.contains(e->key())) {
        emit nextTab(true);
    }
    if (Settings::kPrevTabKeys.contains(e->key())) {
        emit nextTab(false);
    }

    switch (e->key()) {
        case Qt::Key_Shift: {
            emit capitalChanged(false);
            break;
        }
        case Qt::Key_Return: {
            emit accentChosen();
            break;
        }
        case Qt::Key_C: {
            if (QApplication::keyboardModifiers().testFlag(
                    Qt::ControlModifier)) {
                emit accentCopyChosen();
            }
            break;
        }
        default:
            break;
    }

    QWidget::keyReleaseEvent(e);
}

void Popup::hideEvent(QHideEvent* e) {
    emit hidden();
    QWidget::hideEvent(e);
}
}  // namespace Altccents

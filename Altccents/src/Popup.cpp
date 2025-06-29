#include "Altccents/Popup.h"

#include <QApplication>
#include <QPainter>
#include <QScreen>

namespace Altccents {
Popup::Popup(QWidget* parent) : QWidget{parent} {
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    // Make the window not appear in taskbar
    setWindowFlag(Qt::Tool);
    // Enable click through the window to other apps
    setWindowFlag(Qt::WindowTransparentForInput);
    setWindowFlag(Qt::WindowDoesNotAcceptFocus);

    // setAttribute(Qt::WA_TranslucentBackground);
    // setAttribute(Qt::WA_NoSystemBackground);
}

void Popup::show(const QList<QChar>& chars, unsigned int active_index) {
    if (chars.isEmpty() || active_index >= chars.count()) {
        return;
    }

    // TODO(clovis): make setting for popup opacity
    // Opacity
    setWindowOpacity(0.9);  // 0 - transparent; 1 - opaque

    // TODO(clovis): make setting for popup position and size
    // Size
    resize(800, 100);

    // Position
    QRect screen_geometry{QApplication::primaryScreen()->availableGeometry()};

    double x_ration{0.5};  // 0 - center left; 1 - center right
    double y_ration{0.9};  // 0 - center up; 1 - center bottom

    int x{static_cast<int>((screen_geometry.width() - width()) * x_ration)};
    int y{static_cast<int>((screen_geometry.height() - height()) * y_ration)};

    move(x, y);

    update();
    QWidget::show();
}

void Popup::paintEvent(QPaintEvent*) {
    QPainter p{this};
    p.drawRect(10, 10, 50, 50);
}
}  // namespace Altccents

#include "fake3dchart.h"
#include <QPainter>

Fake3DChart::Fake3DChart(QWidget *parent) : QWidget(parent) {
    setMinimumSize(400, 300);
    memset(frequency, 0, sizeof(frequency));
}

void Fake3DChart::addDataPoint(int x, int y) {
    if (x < 0 || x >= MAX_X || y < 0 || y >= MAX_Y) return;
    frequency[x][y]++;
    if (frequency[x][y] > maxFrequency) {
        maxFrequency = frequency[x][y];
    }
    update();
}

void Fake3DChart::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int cellWidth = width() / MAX_X;
    int cellHeight = height() / MAX_Y;

    for (int x = 0; x < MAX_X; ++x) {
        for (int y = 0; y < MAX_Y; ++y) {
            int freq = frequency[x][y];
            if (freq > 0) {
                float ratio = float(freq) / maxFrequency;
                QColor color = QColor::fromHsvF(0.6 - ratio * 0.6, 1.0, 1.0);
                painter.fillRect(x * cellWidth, height() - (y + 1) * cellHeight, cellWidth, cellHeight, color);
            }
        }
    }
}

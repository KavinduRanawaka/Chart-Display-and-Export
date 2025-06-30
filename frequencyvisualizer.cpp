#include "frequencyvisualizer.h"
#include <QPainter>
#include <QLinearGradient>
#include <QDebug>

FrequencyVisualizer::FrequencyVisualizer(QWidget *parent)
    : QWidget(parent) {
    setMinimumSize(300, 300);
}

void FrequencyVisualizer::setData(const std::vector<std::vector<int>> &data) {
    frequencyGrid = data;

    maxFrequency = 1;
    for (const auto &row : data) {
        for (int val : row) {
            if (val > maxFrequency) maxFrequency = val;
        }
    }

    update();
}

void FrequencyVisualizer::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int rows = frequencyGrid.size();
    if (rows == 0) return;

    int cols = frequencyGrid[0].size();
    if (cols == 0) return;

    float cellW = width() / float(cols);
    float cellH = height() / float(rows);

    for (int x = 0; x < rows; ++x) {
        for (int y = 0; y < cols; ++y) {
            float intensity = frequencyGrid[x][y] / float(maxFrequency);
            QColor color = QColor::fromHsvF(0.6 - intensity * 0.6, 1.0, 1.0);
            painter.setBrush(color);
            painter.setPen(Qt::NoPen);
            painter.drawRect(y * cellW, x * cellH, cellW, cellH);
        }
    }
}

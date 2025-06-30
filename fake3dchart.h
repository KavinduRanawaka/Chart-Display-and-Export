#ifndef FAKE3DCHART_H
#define FAKE3DCHART_H

#include <QWidget>
#include <vector>

class Fake3DChart : public QWidget {
    Q_OBJECT

public:
    explicit Fake3DChart(QWidget *parent = nullptr);

    void addDataPoint(int x, int y);  // Accept X and Y, count frequency for Z

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static const int MAX_X = 100;
    static const int MAX_Y = 100;

    int frequency[MAX_X][MAX_Y] = {{0}};
    int maxFrequency = 1;
};

#endif // FAKE3DCHART_H

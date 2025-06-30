#ifndef FREQUENCYVISUALIZER_H
#define FREQUENCYVISUALIZER_H

#include <QWidget>
#include <vector>

class FrequencyVisualizer : public QWidget {
    Q_OBJECT

public:
    explicit FrequencyVisualizer(QWidget *parent = nullptr);

    void setData(const std::vector<std::vector<int>> &data);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<std::vector<int>> frequencyGrid;
    int maxFrequency = 1;
};

#endif // FREQUENCYVISUALIZER_H

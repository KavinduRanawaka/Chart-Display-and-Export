/********************************************************************************
** Form generated from reading UI file 'frequencyvisualizer.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FREQUENCYVISUALIZER_H
#define UI_FREQUENCYVISUALIZER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FrequencyVisualizer
{
public:

    void setupUi(QWidget *FrequencyVisualizer)
    {
        if (FrequencyVisualizer->objectName().isEmpty())
            FrequencyVisualizer->setObjectName("FrequencyVisualizer");
        FrequencyVisualizer->resize(400, 300);

        retranslateUi(FrequencyVisualizer);

        QMetaObject::connectSlotsByName(FrequencyVisualizer);
    } // setupUi

    void retranslateUi(QWidget *FrequencyVisualizer)
    {
        FrequencyVisualizer->setWindowTitle(QCoreApplication::translate("FrequencyVisualizer", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FrequencyVisualizer: public Ui_FrequencyVisualizer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FREQUENCYVISUALIZER_H

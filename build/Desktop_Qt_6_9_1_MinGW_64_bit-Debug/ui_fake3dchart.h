/********************************************************************************
** Form generated from reading UI file 'fake3dchart.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FAKE3DCHART_H
#define UI_FAKE3DCHART_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Fake3DChart
{
public:

    void setupUi(QWidget *Fake3DChart)
    {
        if (Fake3DChart->objectName().isEmpty())
            Fake3DChart->setObjectName("Fake3DChart");
        Fake3DChart->resize(400, 300);

        retranslateUi(Fake3DChart);

        QMetaObject::connectSlotsByName(Fake3DChart);
    } // setupUi

    void retranslateUi(QWidget *Fake3DChart)
    {
        Fake3DChart->setWindowTitle(QCoreApplication::translate("Fake3DChart", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Fake3DChart: public Ui_Fake3DChart {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FAKE3DCHART_H

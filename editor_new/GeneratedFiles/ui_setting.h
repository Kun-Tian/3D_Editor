/********************************************************************************
** Form generated from reading UI file 'setting.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTING_H
#define UI_SETTING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_setting
{
public:
    QFrame *line;
    QFrame *line_2;
    QFrame *line_3;
    QLabel *label;
    QComboBox *Quality;
    QLabel *label_2;
    QDoubleSpinBox *LayerHeight;
    QLabel *label_3;
    QLabel *label_4;
    QSpinBox *NumofShells;
    QLabel *label_5;
    QComboBox *Meterial;
    QLabel *label_6;
    QSpinBox *Temperature;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    QSpinBox *Infill;
    QCheckBox *Raft;
    QCheckBox *Support;
    QLabel *label_7;
    QSpinBox *ModelHeight;

    void setupUi(QWidget *setting)
    {
        if (setting->objectName().isEmpty())
            setting->setObjectName(QStringLiteral("setting"));
        setting->resize(694, 201);
        line = new QFrame(setting);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(10, 30, 331, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(setting);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setGeometry(QRect(330, 20, 41, 141));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(setting);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setGeometry(QRect(360, 30, 321, 20));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        label = new QLabel(setting);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 40, 51, 41));
        label->setTextFormat(Qt::AutoText);
        Quality = new QComboBox(setting);
        Quality->setObjectName(QStringLiteral("Quality"));
        Quality->setGeometry(QRect(80, 50, 101, 22));
        label_2 = new QLabel(setting);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 80, 101, 41));
        label_2->setTextFormat(Qt::AutoText);
        LayerHeight = new QDoubleSpinBox(setting);
        LayerHeight->setObjectName(QStringLiteral("LayerHeight"));
        LayerHeight->setGeometry(QRect(120, 90, 62, 22));
        label_3 = new QLabel(setting);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(190, 80, 51, 41));
        label_3->setTextFormat(Qt::AutoText);
        label_4 = new QLabel(setting);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 120, 101, 41));
        label_4->setTextFormat(Qt::AutoText);
        NumofShells = new QSpinBox(setting);
        NumofShells->setObjectName(QStringLiteral("NumofShells"));
        NumofShells->setGeometry(QRect(190, 130, 141, 22));
        label_5 = new QLabel(setting);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(360, 40, 101, 41));
        label_5->setTextFormat(Qt::AutoText);
        Meterial = new QComboBox(setting);
        Meterial->setObjectName(QStringLiteral("Meterial"));
        Meterial->setGeometry(QRect(500, 50, 161, 22));
        label_6 = new QLabel(setting);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(360, 80, 121, 41));
        label_6->setTextFormat(Qt::AutoText);
        Temperature = new QSpinBox(setting);
        Temperature->setObjectName(QStringLiteral("Temperature"));
        Temperature->setGeometry(QRect(500, 90, 161, 22));
        saveButton = new QPushButton(setting);
        saveButton->setObjectName(QStringLiteral("saveButton"));
        saveButton->setGeometry(QRect(130, 170, 75, 23));
        cancelButton = new QPushButton(setting);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setGeometry(QRect(500, 170, 75, 23));
        Infill = new QSpinBox(setting);
        Infill->setObjectName(QStringLiteral("Infill"));
        Infill->setGeometry(QRect(260, 90, 61, 22));
        Raft = new QCheckBox(setting);
        Raft->setObjectName(QStringLiteral("Raft"));
        Raft->setGeometry(QRect(190, 50, 71, 21));
        Support = new QCheckBox(setting);
        Support->setObjectName(QStringLiteral("Support"));
        Support->setGeometry(QRect(260, 50, 71, 21));
        label_7 = new QLabel(setting);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(360, 120, 101, 41));
        label_7->setTextFormat(Qt::AutoText);
        ModelHeight = new QSpinBox(setting);
        ModelHeight->setObjectName(QStringLiteral("ModelHeight"));
        ModelHeight->setGeometry(QRect(500, 130, 161, 22));

        retranslateUi(setting);

        QMetaObject::connectSlotsByName(setting);
    } // setupUi

    void retranslateUi(QWidget *setting)
    {
        setting->setWindowTitle(QApplication::translate("setting", "setting", 0));
        label->setText(QApplication::translate("setting", "Quality:", 0));
        Quality->clear();
        Quality->insertItems(0, QStringList()
         << QApplication::translate("setting", "Low", 0)
         << QApplication::translate("setting", "Standard", 0)
         << QApplication::translate("setting", "High", 0)
        );
        label_2->setText(QApplication::translate("setting", "Layer Height(mm)", 0));
#ifndef QT_NO_TOOLTIP
        LayerHeight->setToolTip(QApplication::translate("setting", "<html><head/><body><p><br/></p></body></html>", 0));
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("setting", "Infill", 0));
        label_4->setText(QApplication::translate("setting", "Number of Shells", 0));
        label_5->setText(QApplication::translate("setting", "Meterial:", 0));
        Meterial->clear();
        Meterial->insertItems(0, QStringList()
         << QApplication::translate("setting", "MakerBot PLA", 0)
        );
        label_6->setText(QApplication::translate("setting", "Extruder Temperature", 0));
        saveButton->setText(QApplication::translate("setting", "Save ", 0));
        cancelButton->setText(QApplication::translate("setting", "Cancel", 0));
        Raft->setText(QApplication::translate("setting", "Raft", 0));
        Support->setText(QApplication::translate("setting", "Support", 0));
        label_7->setText(QApplication::translate("setting", "ModelHeight:", 0));
    } // retranslateUi

};

namespace Ui {
    class setting: public Ui_setting {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTING_H

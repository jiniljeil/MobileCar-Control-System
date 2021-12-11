/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDial>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *lbDispTime;
    QLabel *lbDispDate;
    QLabel *lbDirection;
    QLabel *lbLamp;
    QWidget *grbDirLamp;
    QPushButton *btnLampDirLeft;
    QPushButton *btnLampDirOff;
    QPushButton *btnLampDirRight;
    QDial *diaDirection;
    QDial *diaLamp;
    QGroupBox *grbDrive;
    QPushButton *btnAccel;
    QPushButton *btnBreak;
    QGroupBox *grbLamp;
    QPushButton *btnLampTail;
    QPushButton *btnLampAuto;
    QPushButton *btnLampLow;
    QPushButton *btnLampFront;
    QPushButton *btnLampOff;
    QGroupBox *grbGearMode;
    QPushButton *btnParking;
    QPushButton *btnReversing;
    QPushButton *btnDriving;
    QGroupBox *grbRate;
    QLCDNumber *lcdDigitNumber;
    QLabel *lbDispUnit;
    QLabel *lbDispLabelTop;
    QLabel *lbDispLabelMiddle;
    QLabel *lbBoxName;
    QLabel *lbUltraSonicWave;
    QGroupBox *grbStatus;
    QPushButton *btnLampEmergency;
    QLabel *lbStateEmergency;
    QPushButton *btnTraceMode;
    QGroupBox *grbEngine;
    QPushButton *btnEngine;
    QGroupBox *groupBox;
    QTextEdit *txtSystemMessage;
    QPushButton *btnMsgClear;
    QLabel *lbDispSystemMsg;
    QPushButton *btnClarkson;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(790, 464);
        MainWindow->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);\n"
"background-color: rgb(16, 172, 255);"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        lbDispTime = new QLabel(centralWidget);
        lbDispTime->setObjectName(QString::fromUtf8("lbDispTime"));
        lbDispTime->setGeometry(QRect(372, 30, 151, 31));
        QFont font;
        font.setPointSize(16);
        font.setBold(true);
        font.setItalic(true);
        font.setWeight(75);
        lbDispTime->setFont(font);
        lbDispTime->setStyleSheet(QString::fromUtf8("background-color: rgb(16, 172, 255);"));
        lbDispTime->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lbDispDate = new QLabel(centralWidget);
        lbDispDate->setObjectName(QString::fromUtf8("lbDispDate"));
        lbDispDate->setGeometry(QRect(412, 10, 111, 22));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setItalic(true);
        font1.setWeight(75);
        lbDispDate->setFont(font1);
        lbDispDate->setStyleSheet(QString::fromUtf8("background-color: rgb(16, 172, 255);"));
        lbDispDate->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lbDirection = new QLabel(centralWidget);
        lbDirection->setObjectName(QString::fromUtf8("lbDirection"));
        lbDirection->setGeometry(QRect(202, 396, 68, 22));
        lbDirection->setStyleSheet(QString::fromUtf8("background-color: rgb(16, 172, 255);\n"
"font-weight: bold;\n"
""));
        lbLamp = new QLabel(centralWidget);
        lbLamp->setObjectName(QString::fromUtf8("lbLamp"));
        lbLamp->setGeometry(QRect(491, 396, 40, 22));
        lbLamp->setStyleSheet(QString::fromUtf8("background-color: rgb(16, 172, 255);\n"
"font-weight: bold;\n"
""));
        grbDirLamp = new QWidget(centralWidget);
        grbDirLamp->setObjectName(QString::fromUtf8("grbDirLamp"));
        grbDirLamp->setGeometry(QRect(12, 130, 531, 271));
        grbDirLamp->setStyleSheet(QString::fromUtf8("background-color: white; border:0;\n"
"background-color: rgb(16, 172, 255);"));
        btnLampDirLeft = new QPushButton(grbDirLamp);
        btnLampDirLeft->setObjectName(QString::fromUtf8("btnLampDirLeft"));
        btnLampDirLeft->setGeometry(QRect(250, 20, 71, 31));
        btnLampDirLeft->setStyleSheet(QString::fromUtf8("font-weight: bold ;\n"
"border: 2px solid grey;\n"
"background-color: rgb(255,255,255);"));
        btnLampDirOff = new QPushButton(grbDirLamp);
        btnLampDirOff->setObjectName(QString::fromUtf8("btnLampDirOff"));
        btnLampDirOff->setGeometry(QRect(330, 20, 71, 31));
        btnLampDirOff->setStyleSheet(QString::fromUtf8("font-weight: bold ;\n"
"border: 2px solid grey; background-color: rgb(255,255,255);"));
        btnLampDirRight = new QPushButton(grbDirLamp);
        btnLampDirRight->setObjectName(QString::fromUtf8("btnLampDirRight"));
        btnLampDirRight->setGeometry(QRect(410, 20, 71, 31));
        btnLampDirRight->setStyleSheet(QString::fromUtf8("font-weight: bold ;\n"
"border: 2px solid grey; background-color: rgb(255,255,255);"));
        diaDirection = new QDial(grbDirLamp);
        diaDirection->setObjectName(QString::fromUtf8("diaDirection"));
        diaDirection->setGeometry(QRect(188, 200, 71, 71));
        diaDirection->setStyleSheet(QString::fromUtf8("background-color: rgb(255,255,255);"));
        diaDirection->setMaximum(5);
        diaLamp = new QDial(grbDirLamp);
        diaLamp->setObjectName(QString::fromUtf8("diaLamp"));
        diaLamp->setGeometry(QRect(465, 200, 71, 71));
        diaLamp->setStyleSheet(QString::fromUtf8("background-color: rgb(255,255,255);"));
        diaLamp->setMaximum(7);
        diaLamp->setPageStep(10);
        grbDrive = new QGroupBox(grbDirLamp);
        grbDrive->setObjectName(QString::fromUtf8("grbDrive"));
        grbDrive->setGeometry(QRect(265, 190, 191, 81));
        grbDrive->setStyleSheet(QString::fromUtf8("background-color: white; border:0; font-weight: bold; background-color: rgb(16, 172, 255);"));
        btnAccel = new QPushButton(grbDrive);
        btnAccel->setObjectName(QString::fromUtf8("btnAccel"));
        btnAccel->setGeometry(QRect(120, 30, 71, 51));
        btnAccel->setStyleSheet(QString::fromUtf8("background-color: black;color:white; font-weight: bold;"));
        btnBreak = new QPushButton(grbDrive);
        btnBreak->setObjectName(QString::fromUtf8("btnBreak"));
        btnBreak->setGeometry(QRect(0, 30, 101, 51));
        btnBreak->setStyleSheet(QString::fromUtf8("background-color: black;color:white; font-weight: bold;"));
        grbLamp = new QGroupBox(grbDirLamp);
        grbLamp->setObjectName(QString::fromUtf8("grbLamp"));
        grbLamp->setGeometry(QRect(7, 150, 181, 121));
        grbLamp->setStyleSheet(QString::fromUtf8("background-color: white; border:0; font-weight: bold; background-color: rgb(16, 172, 255);"));
        btnLampTail = new QPushButton(grbLamp);
        btnLampTail->setObjectName(QString::fromUtf8("btnLampTail"));
        btnLampTail->setGeometry(QRect(120, 30, 61, 51));
        btnLampTail->setStyleSheet(QString::fromUtf8("background-color: rgb(255,255,255);\n"
"border: 2px solid grey;\n"
""));
        btnLampAuto = new QPushButton(grbLamp);
        btnLampAuto->setObjectName(QString::fromUtf8("btnLampAuto"));
        btnLampAuto->setGeometry(QRect(98, 90, 81, 30));
        btnLampAuto->setStyleSheet(QString::fromUtf8("background-color: rgb(255,255,255);\n"
"border: 2px solid grey;\n"
""));
        btnLampLow = new QPushButton(grbLamp);
        btnLampLow->setObjectName(QString::fromUtf8("btnLampLow"));
        btnLampLow->setGeometry(QRect(0, 30, 61, 51));
        btnLampLow->setStyleSheet(QString::fromUtf8("background-color: rgb(255,255,255);\n"
"border: 2px solid grey;\n"
""));
        btnLampFront = new QPushButton(grbLamp);
        btnLampFront->setObjectName(QString::fromUtf8("btnLampFront"));
        btnLampFront->setGeometry(QRect(60, 30, 61, 51));
        btnLampFront->setStyleSheet(QString::fromUtf8("background-color: rgb(255,255,255);\n"
"border: 2px solid grey;\n"
""));
        btnLampOff = new QPushButton(grbLamp);
        btnLampOff->setObjectName(QString::fromUtf8("btnLampOff"));
        btnLampOff->setGeometry(QRect(0, 90, 81, 31));
        btnLampOff->setStyleSheet(QString::fromUtf8("background-color: rgb(255,255,255);\n"
"border: 2px solid grey;\n"
""));
        grbGearMode = new QGroupBox(grbDirLamp);
        grbGearMode->setObjectName(QString::fromUtf8("grbGearMode"));
        grbGearMode->setGeometry(QRect(10, 20, 181, 101));
        grbGearMode->setStyleSheet(QString::fromUtf8("background-color: white; border:0; font-weight: bold;\n"
"background-color: rgb(16, 172, 255);"));
        btnParking = new QPushButton(grbGearMode);
        btnParking->setObjectName(QString::fromUtf8("btnParking"));
        btnParking->setGeometry(QRect(0, 40, 51, 51));
        btnParking->setStyleSheet(QString::fromUtf8("border: 2px solid grey ; background-color: rgb(255,255,255);"));
        btnReversing = new QPushButton(grbGearMode);
        btnReversing->setObjectName(QString::fromUtf8("btnReversing"));
        btnReversing->setGeometry(QRect(50, 40, 51, 51));
        btnReversing->setStyleSheet(QString::fromUtf8("border: 2px solid grey; background-color: rgb(255,255,255);"));
        btnDriving = new QPushButton(grbGearMode);
        btnDriving->setObjectName(QString::fromUtf8("btnDriving"));
        btnDriving->setGeometry(QRect(100, 40, 51, 51));
        btnDriving->setStyleSheet(QString::fromUtf8("border: 2px solid grey; background-color: rgb(255,255,255);"));
        grbRate = new QGroupBox(grbDirLamp);
        grbRate->setObjectName(QString::fromUtf8("grbRate"));
        grbRate->setGeometry(QRect(235, 66, 261, 121));
        grbRate->setStyleSheet(QString::fromUtf8("background-color: white; font-weight: bold;\n"
"background-color: rgb(166, 232, 255);\n"
"background-color: rgb(255,255,255);\n"
"border: 2px solid grey;"));
        grbRate->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        lcdDigitNumber = new QLCDNumber(grbRate);
        lcdDigitNumber->setObjectName(QString::fromUtf8("lcdDigitNumber"));
        lcdDigitNumber->setGeometry(QRect(10, 30, 111, 81));
        QFont font2;
        font2.setPointSize(18);
        font2.setBold(true);
        font2.setItalic(true);
        font2.setWeight(75);
        lcdDigitNumber->setFont(font2);
        lcdDigitNumber->setStyleSheet(QString::fromUtf8("border: 0px solid ;"));
        lcdDigitNumber->setFrameShape(QFrame::NoFrame);
        lcdDigitNumber->setSmallDecimalPoint(false);
        lcdDigitNumber->setDigitCount(3);
        lbDispUnit = new QLabel(grbRate);
        lbDispUnit->setObjectName(QString::fromUtf8("lbDispUnit"));
        lbDispUnit->setGeometry(QRect(123, 70, 41, 21));
        QFont font3;
        font3.setPointSize(12);
        font3.setBold(true);
        font3.setItalic(true);
        font3.setWeight(75);
        lbDispUnit->setFont(font3);
        lbDispUnit->setStyleSheet(QString::fromUtf8("border: 0px solid ;"));
        lbDispLabelTop = new QLabel(grbRate);
        lbDispLabelTop->setObjectName(QString::fromUtf8("lbDispLabelTop"));
        lbDispLabelTop->setGeometry(QRect(180, 14, 61, 31));
        lbDispLabelTop->setFont(font3);
        lbDispLabelTop->setStyleSheet(QString::fromUtf8("border: 0px solid ;"));
        lbDispLabelMiddle = new QLabel(grbRate);
        lbDispLabelMiddle->setObjectName(QString::fromUtf8("lbDispLabelMiddle"));
        lbDispLabelMiddle->setGeometry(QRect(180, 46, 71, 31));
        lbDispLabelMiddle->setFont(font3);
        lbDispLabelMiddle->setStyleSheet(QString::fromUtf8("border: 0px solid ;"));
        lbBoxName = new QLabel(grbRate);
        lbBoxName->setObjectName(QString::fromUtf8("lbBoxName"));
        lbBoxName->setGeometry(QRect(10, 5, 81, 22));
        lbBoxName->setStyleSheet(QString::fromUtf8("background-color: rgb(255,255,255);\n"
"border: 0px ; "));
        lbUltraSonicWave = new QLabel(grbRate);
        lbUltraSonicWave->setObjectName(QString::fromUtf8("lbUltraSonicWave"));
        lbUltraSonicWave->setGeometry(QRect(180, 81, 61, 21));
        lbUltraSonicWave->setStyleSheet(QString::fromUtf8("border: 0px solid ;"));
        grbStatus = new QGroupBox(centralWidget);
        grbStatus->setObjectName(QString::fromUtf8("grbStatus"));
        grbStatus->setGeometry(QRect(129, 10, 241, 121));
        grbStatus->setStyleSheet(QString::fromUtf8("background-color: white; border:0; font-weight: bold;\n"
"background-color: rgb(16, 172, 255);"));
        btnLampEmergency = new QPushButton(grbStatus);
        btnLampEmergency->setObjectName(QString::fromUtf8("btnLampEmergency"));
        btnLampEmergency->setGeometry(QRect(3, 28, 101, 41));
        btnLampEmergency->setStyleSheet(QString::fromUtf8("background-color: rgb(0,0,0); font-weight: bold; color: white; font-size: 10pt; border-radius: 10px;"));
        lbStateEmergency = new QLabel(grbStatus);
        lbStateEmergency->setObjectName(QString::fromUtf8("lbStateEmergency"));
        lbStateEmergency->setGeometry(QRect(120, 30, 81, 81));
        btnTraceMode = new QPushButton(grbStatus);
        btnTraceMode->setObjectName(QString::fromUtf8("btnTraceMode"));
        btnTraceMode->setGeometry(QRect(3, 74, 101, 41));
        btnTraceMode->setStyleSheet(QString::fromUtf8("background-color: rgb(0,0,0); font-weight: bold; color: red; font-size: 10pt; border-radius: 10px;"));
        grbEngine = new QGroupBox(centralWidget);
        grbEngine->setObjectName(QString::fromUtf8("grbEngine"));
        grbEngine->setGeometry(QRect(20, 10, 111, 131));
        grbEngine->setStyleSheet(QString::fromUtf8("background-color: white; border:0; font-weight: bold;\n"
"background-color: rgb(16, 172, 255);"));
        btnEngine = new QPushButton(grbEngine);
        btnEngine->setObjectName(QString::fromUtf8("btnEngine"));
        btnEngine->setGeometry(QRect(0, 30, 81, 81));
        btnEngine->setStyleSheet(QString::fromUtf8("border: 1px solid black; border-radius: 20px; max-width: 100px; max-height: 100px; font-size: 10pt; background-color: white; color:black;"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(550, 30, 231, 391));
        groupBox->setStyleSheet(QString::fromUtf8("border: 2px solid grey; background-color: rgb(255,255,255);"));
        txtSystemMessage = new QTextEdit(groupBox);
        txtSystemMessage->setObjectName(QString::fromUtf8("txtSystemMessage"));
        txtSystemMessage->setGeometry(QRect(10, 40, 211, 341));
        txtSystemMessage->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);\n"
"color: rgb(255, 255, 255);"));
        btnMsgClear = new QPushButton(groupBox);
        btnMsgClear->setObjectName(QString::fromUtf8("btnMsgClear"));
        btnMsgClear->setGeometry(QRect(140, 6, 71, 31));
        lbDispSystemMsg = new QLabel(groupBox);
        lbDispSystemMsg->setObjectName(QString::fromUtf8("lbDispSystemMsg"));
        lbDispSystemMsg->setGeometry(QRect(10, 7, 111, 31));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Piboto [Goog]"));
        font4.setPointSize(10);
        font4.setBold(false);
        font4.setItalic(false);
        font4.setWeight(50);
        lbDispSystemMsg->setFont(font4);
        lbDispSystemMsg->setStyleSheet(QString::fromUtf8("border: 0px solid white;"));
        btnClarkson = new QPushButton(centralWidget);
        btnClarkson->setObjectName(QString::fromUtf8("btnClarkson"));
        btnClarkson->setGeometry(QRect(410, 70, 71, 71));
        btnClarkson->setStyleSheet(QString::fromUtf8("border: 1px solid black; border-radius: 20px; max-width: 100px; max-height: 100px; font-size: 10pt; background-color: white; color:black; font-weight: bold;"));
        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        lbDispTime->setText(QApplication::translate("MainWindow", "11:12:13 AM", nullptr));
        lbDispDate->setText(QApplication::translate("MainWindow", "2021/11/09", nullptr));
        lbDirection->setText(QApplication::translate("MainWindow", "Direction", nullptr));
        lbLamp->setText(QApplication::translate("MainWindow", "Lamp", nullptr));
        btnLampDirLeft->setText(QApplication::translate("MainWindow", "LEFT", nullptr));
        btnLampDirOff->setText(QApplication::translate("MainWindow", "OFF", nullptr));
        btnLampDirRight->setText(QApplication::translate("MainWindow", "RIGHT", nullptr));
        grbDrive->setTitle(QApplication::translate("MainWindow", "Drive", nullptr));
        btnAccel->setText(QApplication::translate("MainWindow", "Accel", nullptr));
        btnBreak->setText(QApplication::translate("MainWindow", "Break", nullptr));
        grbLamp->setTitle(QApplication::translate("MainWindow", "Lamp", nullptr));
        btnLampTail->setText(QApplication::translate("MainWindow", "TAIL", nullptr));
        btnLampAuto->setText(QApplication::translate("MainWindow", "Auto", nullptr));
        btnLampLow->setText(QApplication::translate("MainWindow", "LOW", nullptr));
        btnLampFront->setText(QApplication::translate("MainWindow", "FRONT", nullptr));
        btnLampOff->setText(QApplication::translate("MainWindow", "Off", nullptr));
        grbGearMode->setTitle(QApplication::translate("MainWindow", "Gear Mode", nullptr));
        btnParking->setText(QApplication::translate("MainWindow", "P", nullptr));
        btnReversing->setText(QApplication::translate("MainWindow", "R", nullptr));
        btnDriving->setText(QApplication::translate("MainWindow", "D", nullptr));
        grbRate->setTitle(QString());
        lbDispUnit->setText(QApplication::translate("MainWindow", "km", nullptr));
        lbDispLabelTop->setText(QApplication::translate("MainWindow", "12.C", nullptr));
        lbDispLabelMiddle->setText(QApplication::translate("MainWindow", "100%", nullptr));
        lbBoxName->setText(QApplication::translate("MainWindow", "Rate", nullptr));
        lbUltraSonicWave->setText(QApplication::translate("MainWindow", "0 cm", nullptr));
        grbStatus->setTitle(QApplication::translate("MainWindow", "Status", nullptr));
        btnLampEmergency->setText(QApplication::translate("MainWindow", "Emergency", nullptr));
        lbStateEmergency->setText(QString());
        btnTraceMode->setText(QApplication::translate("MainWindow", "Trace Mode", nullptr));
        grbEngine->setTitle(QApplication::translate("MainWindow", "Engine", nullptr));
        btnEngine->setText(QApplication::translate("MainWindow", "ON/OFF", nullptr));
        groupBox->setTitle(QString());
        btnMsgClear->setText(QApplication::translate("MainWindow", "CLEAR", nullptr));
        lbDispSystemMsg->setText(QApplication::translate("MainWindow", "System Message", nullptr));
        btnClarkson->setText(QApplication::translate("MainWindow", "Clarkson", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

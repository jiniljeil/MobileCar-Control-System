#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "devices.h"
#include "gpios.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <QDebug>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    statLampEmergency = false;
    m_trace_mode_color=0;

    putSystemMessage("[System] GUI Initialization - OK.");


    monTimer = new QTimer(this);
    tmrEmergency = new QTimer(this);
    tmrDirection = new QTimer(this);
    tmrTraceMode = new QTimer(this);

    monTimer->start(1000); // 1초마다 monTimer가 작동되게해
    tmrEmergency->start(1000);
    tmrDirection->start(500);
    putSystemMessage("[System] Main timer Created!");


    recvCanData = new thCanRecv();
    recvCanData->start();
    putSystemMessage("[System] Data Recv Thread Start!");



    // Timer Event
    connect(recvCanData, SIGNAL(sendCanData(char*)),this, SLOT(slotRecvCanDataPacket(char*)));
    connect(monTimer, SIGNAL(timeout()), SLOT(slotTimerGetSensorState()));
    connect(tmrEmergency, SIGNAL(timeout()), SLOT(slotTimerLampEmerState()));
    connect(tmrDirection, SIGNAL(timeout()), SLOT(slotTimerLampDirectionState()));

    connect(tmrTraceMode, SIGNAL(timeout()), SLOT(slotTimerTraceModeState()));
}

void MainWindow::slotRecvCanDataPacket(char* recvCData)
{
    QString strData;
    int color = 1;
    char data[13];
    memcpy(data,recvCData,sizeof(data));
    strData.sprintf("%04x %02x | %02x %02x %02x %02x %02x %02x %02x %02x",data[0]<<24|data[1]<<16|data[2]<<8|data[3],data[4],data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12]);

    // Data : 5(0) | 6(1) | 7(2) | 8(3) | 9(4) | 10(5) | 11(6) | 12(7)
    // TICK(0) : not used.
    // CMD (1) : not used. (only cmd)
    // FUNC(2) : 10 TP_LAMP_OFF / 11 TAIL / 12 FRONT / 13 LOW
    //           20 TP_DIR_OFF  / 21 LIGHT / 22 RIGHT
    //           30 TP_EMERGENCY
    // VALUE   : ON/OFF/VALUE
    putSystemMessage(strData);

    if (data[6] == 0x01){
    switch(data[7])
    {
        case 0x10:   //bscode
            switch(data[8]) {
            // off
            case 0x00:
                ui->btnProgStart->setStyleSheet("QPushButton { background-color: rgb(255,255,255); color: black; font-weight:bold; border:2px solid rgb(255, 232, 102);}");
                ui->bntProgStartStop->setStyleSheet("QPushButton { background-color: green ; color: white; font-weight:bold; border:2px solid rgb(0, 255, 0);}");
            break;
            // engine on
            case 0x01:
                ui->btnProgStart->setStyleSheet("QPushButton { background-color: green ; color: white; font-weight:bold; border:2px solid rgb(0, 255, 0);}");
                ui->bntProgStartStop->setStyleSheet("QPushButton { background-color: rgb(255,255,255); color: black; font-weight:bold; border:2px solid rgb(255, 232, 102);}");
            break;
            // error
            case 0x02:
                ui->btnProgStart->setStyleSheet("QPushButton { background-color : red; border:2px solid rgb(255, 0, 0);}");
                ui->bntProgStartStop->setStyleSheet("QPushButton { background-color : red; border:2px solid rgb(255, 232, 102);}");
            break;


            default:
                break;
            }
        break;

        case 0x11:
            // trace mode on
            switch(data[8]){
            case 0x0:
                tmrTraceMode->stop();
                ui->btnEmgState_1->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
                ui->btnEmgState_2->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
                ui->btnTracemode->setStyleSheet("QPushButton { background-color: rgb(255, 255, 255); border-top: 1px solid rgb(182, 182, 182); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");

                break;
            case 0x1:
                //(data[9]!=0)?stateTraceEnable=true:stateTraceEnable=false;
                ui->btnTracemode->setStyleSheet("QPushButton { background-color : yellow; border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
                tmrTraceMode->start(300);
                stateTraceEnable=true;
            break;
            }
        break;

        //Gear Part
        case 0x12:
            switch(data[8]) {
            //Gear P
            case 0x00:
                ui->btnGearP->setStyleSheet("QPushButton { background-color : yellow; border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 4px solid rgb(255, 232, 102); }");
                ui->btnGearR->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182);}");
                ui->btnGearD->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182); border-bottom:1px solid rgb(182, 182, 182); }");
//                ui->btnTracemode->setStyleSheet("QPushButton { background-color: rgb(255, 255, 255); border-top: 1px solid rgb(182, 182, 182); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");

            break;
            //Gear R
            case 0x01:
                ui->btnGearR->setStyleSheet("QPushButton { background-color : yellow; border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ; }");
                ui->btnGearP->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(255, 232, 102);}");
                ui->btnGearD->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182); border-bottom: 1px solid rgb(182, 182, 182);}");           
                if(data[9]){
                    ui->btnwarning->setStyleSheet("QPushButton {background-color: red; border:2px solid rgb(255, 0, 0);}");
                }else{
                    ui->btnwarning->setStyleSheet("QPushButton {background-color: rgb(255, 255, 255); border:2px solid rgb(255, 232, 102);}");
                }
            break;
            //Gear D
            case 0x02:
                ui->btnGearD->setStyleSheet("QPushButton { background-color : yellow;border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182); border-bottom: 1px solid rgb(182, 182, 182); }");
                ui->btnGearR->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182);}");
                ui->btnGearP->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(255, 232, 102);}");
            break;

            default:
                break;
        }
        break;

        case 0x14:
            if(data[8]!=0) {
                if (data[9] != 0) {
                    if(statEmergencyBlink)
                    {
                        statEmergencyBlink=false;
                        ui->btnEmergency->setStyleSheet("QPushButton { background-color : yellow; border:2px solid rgb(255, 232, 102);}");
                    }else {
                        statEmergencyBlink=true;
                        ui->btnEmergency->setStyleSheet("QPushButton { background-color : rgb(255, 255, 255); border:2px solid rgb(255, 232, 102);}");
                    }
                }
                statLampEmergency = true;
            }else{
                 statLampEmergency = false;
                 ui->btnEmergency->setStyleSheet("QPushButton { background-color : rgb(255, 255, 255); border:2px solid rgb(255, 232, 102);}");

                 ui->btnHeadlight1->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                 ui->btnHeadlight2->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                 ui->btnBreaklight1->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
                 ui->btnBreaklight2->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
                 ui->btnBreaklight3->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
                 ui->btnBreaklight4->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
            }
        break;

        //Headlight part
        case 0x20:
            switch(data[8]) {
            //off
            case 0x00:
                ui->btnHeadlight1->setStyleSheet("QPushButton { background-color:rgb(255,255,255); border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                ui->btnHeadlight2->setStyleSheet("QPushButton { background-color:rgb(255,255,255); border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                ui->btnLEDOff->setStyleSheet("QPushButton { background-color : yellow; border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 4px solid rgb(255, 232, 102);}");
                ui->btnLEDLow->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDFront->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDTail->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDAuto->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
            break;
            //low
            case 0x01:
                ui->btnHeadlight1->setStyleSheet("QPushButton { background-color : #FFC133; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                ui->btnHeadlight2->setStyleSheet("QPushButton { background-color : #FFC133; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                ui->btnLEDOff->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 4px solid rgb(255, 232, 102);}");
                ui->btnLEDLow->setStyleSheet("QPushButton { background-color : yellow; border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDFront->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDTail->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDAuto->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
            break;
            //front
            case 0x02:
                ui->btnHeadlight1->setStyleSheet("QPushButton { background-color : #FFE933; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                ui->btnHeadlight2->setStyleSheet("QPushButton { background-color : #FFE933; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                ui->btnLEDOff->setStyleSheet("QPushButton { background-color : rgb(255,255,255);border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 4px solid rgb(255, 232, 102); }");
                ui->btnLEDLow->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDFront->setStyleSheet("QPushButton { background-color : yellow; border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDTail->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDAuto->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
            break;
            //tail
            case 0x03:
                ui->btnHeadlight1->setStyleSheet("QPushButton { background-color : #F0FF33; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                ui->btnHeadlight2->setStyleSheet("QPushButton { background-color : #F0FF33; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                ui->btnLEDOff->setStyleSheet("QPushButton { background-color : rgb(255,255,255);border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 4px solid rgb(255, 232, 102);}");
                ui->btnLEDLow->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDFront->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDTail->setStyleSheet("QPushButton { background-color : yellow; border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                ui->btnLEDAuto->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
            break;
            //Auto
            case 0x04:
                switch(data[9]){
                //off
                case 0x00:
//                    ui->SpeedNumber->display(data[10]);
                    ui->btnHeadlight1->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                    ui->btnHeadlight2->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");

                    ui->btnLEDOff->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 4px solid rgb(255, 232, 102);}");
                    ui->btnLEDLow->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDFront->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDTail->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDAuto->setStyleSheet("QPushButton { background-color : yellow; border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
                    break;
                //low
                case 0x01:
//                    ui->SpeedNumber->display(data[10]);
                    ui->btnHeadlight1->setStyleSheet("QPushButton { background-color : #FFC133; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                    ui->btnHeadlight2->setStyleSheet("QPushButton { background-color : #FFC133; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");

                    ui->btnLEDOff->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 4px solid rgb(255, 232, 102);}");
                    ui->btnLEDLow->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDFront->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDTail->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDAuto->setStyleSheet("QPushButton { background-color : yellow; border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
                    break;
                //front
                case 0x02:
//                    ui->SpeedNumber->display(data[10]);
                    ui->btnHeadlight1->setStyleSheet("QPushButton { background-color : #FFE933; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                    ui->btnHeadlight2->setStyleSheet("QPushButton { background-color : #FFE933; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");

                    ui->btnLEDOff->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 4px solid rgb(255, 232, 102);}");
                    ui->btnLEDLow->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDFront->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDTail->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDAuto->setStyleSheet("QPushButton { background-color : yellow; border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
                    break;
                //tail
                case 0x03:
//                    ui->SpeedNumber->display(data[10]);
                    ui->btnHeadlight1->setStyleSheet("QPushButton { background-color : #F0FF33; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
                    ui->btnHeadlight2->setStyleSheet("QPushButton { background-color : #F0FF33; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");

                    ui->btnLEDOff->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 4px solid rgb(255, 232, 102);}");
                    ui->btnLEDLow->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDFront->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDTail->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDAuto->setStyleSheet("QPushButton { background-color : yellow; border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
                    break;

                default:

                    ui->btnLEDOff->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 4px solid rgb(255, 232, 102);}");
                    ui->btnLEDLow->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDFront->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDTail->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102);border-top: 1px solid rgb(182, 182, 182) ;}");
                    ui->btnLEDAuto->setStyleSheet("QPushButton { background-color : yellow; border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102);border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
                break;
                }
            //default:
                //break;
            }
        break;

    case 0x21:
        switch(data[8])
        {
        case 0x00:
            statLampDirection = 0x00;
            (data[9]!=0)?stateLampDirEnable=true:stateLampDirEnable=false;
            break;
        case 0x01:
            statLampDirection = 0x01;
            stateLampDirEnable = true;
            break;
        case 0x02:
            statLampDirection = 0x02;
            (data[9]!=0)?stateLampDirEnable=true:stateLampDirEnable=false;
            break;
        default :
            break;
        }
        break;

    case 0x22:
        ui->SpeedNumber->display(data[8]);
        if(data[9] == 1){
            putSystemMessage("BREAK");
            ui->btnBreaklight1->setStyleSheet("QPushButton { background-color: rgb(255,131,51); }");
            ui->btnBreaklight2->setStyleSheet("QPushButton { background-color : rgb(255,131,51); }");
            ui->btnBreaklight3->setStyleSheet("QPushButton { background-color : rgb(255,131,51); }");
            ui->btnBreaklight4->setStyleSheet("QPushButton { background-color : rgb(255,131,51); }");
        }else{
            ui->btnBreaklight2->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
            ui->btnBreaklight1->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
            ui->btnBreaklight3->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
            ui->btnBreaklight4->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
        }
        break;

    case 0x23:
        if(data[9]!=0){
            ui->SpeedNumber->display(data[8]);
            ui->btnTracemode->setStyleSheet("QPushButton { background-color : yellow; border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
        }else{
            ui->SpeedNumber->display(data[8]);
            ui->btnTracemode->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-top: 1px solid rgb(182, 182, 182);border-right: 4px solid rgb(255, 232, 102); border-left: 4px solid rgb(255, 232, 102); border-bottom: 4px solid rgb(255, 232, 102);}");
        }
        break;
    case 0x24:
        if (data[8] != 0) {
            ui->btnClarkson->setStyleSheet("QPushButton { background-color : yellow; border:2px solid rgb(255, 232, 102);}");
        }else{
            ui->btnClarkson->setStyleSheet("QPushButton { background-color : white; border:2px solid rgb(255, 232, 102); }");
        }
    default:
    break;
    }
    } else if (data[6] == 0x02){
        unsigned char integer = data[7];
        unsigned char point = data[8];
        putSystemMessage(QString::number(integer));
        QString str;
        ui->lbTemp->setText(str.sprintf("%u.%u C", integer, point));
    }
}

void MainWindow::slotTimerTraceModeState(void)
{
    m_trace_mode_color++;
    QString str;
    str.sprintf("%d", m_trace_mode_color);
    putSystemMessage(str);
    if (m_trace_mode_color == 1 ) {
        ui->btnEmgState_1->setStyleSheet("QPushButton { background-color : red; }");
        ui->btnEmgState_2->setStyleSheet("QPushButton { background-color : red; }");
    } else if (m_trace_mode_color == 2){
        ui->btnEmgState_1->setStyleSheet("QPushButton { background-color : orange; }");
        ui->btnEmgState_2->setStyleSheet("QPushButton { background-color : orange; }");
    } else if (m_trace_mode_color == 3){
        ui->btnEmgState_1->setStyleSheet("QPushButton { background-color : yellow; }");
        ui->btnEmgState_2->setStyleSheet("QPushButton { background-color : yellow; }");
    } else if (m_trace_mode_color == 4){
        ui->btnEmgState_1->setStyleSheet("QPushButton { background-color : green; }");
        ui->btnEmgState_2->setStyleSheet("QPushButton { background-color : green; }");
    } else if (m_trace_mode_color == 5){
        ui->btnEmgState_1->setStyleSheet("QPushButton { background-color : blue; }");
        ui->btnEmgState_2->setStyleSheet("QPushButton { background-color : blue; }");
    } else if (m_trace_mode_color == 6){
        ui->btnEmgState_1->setStyleSheet("QPushButton { background-color : navy; }");
        ui->btnEmgState_2->setStyleSheet("QPushButton { background-color : navy; }");
    } else if (m_trace_mode_color == 7){
        ui->btnEmgState_1->setStyleSheet("QPushButton { background-color :purple; }");
        ui->btnEmgState_2->setStyleSheet("QPushButton { background-color : purple; }");
        m_trace_mode_color = 0 ;
    }
}

void MainWindow::slotTimerGetSensorState(void)
{


}



void MainWindow::putSystemMessage(QString msg)
{
    ui->teDebugMessage->append(msg);
}

void MainWindow::slotTimerLampDirectionState(void)
{

    if(stateLampDirEnable!=0)
    {
        if(statLampDirBlinck)
        {
            statLampDirBlinck=false;
            switch(statLampDirection)
            {
            case 0:
                ui->pbDirLampLeftState->setStyleSheet("QPushButton { background-color : yellow; }");
                ui->pbDirLampRightState->setStyleSheet("QPushButton { background-color : white; }");
                ui->btnDirLeftState->setStyleSheet("QPushButton { background-color : yellow; border:2px solid rgb(255, 232, 102);}");
                ui->bntDirOffState->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border:2px solid rgb(255, 232, 102);}");
                ui->btnDirRightState->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border:2px solid rgb(255, 232, 102);}");
                break;
            case 1:
                ui->btnDirLeftState->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border:2px solid rgb(255, 232, 102);}");
                ui->bntDirOffState->setStyleSheet("QPushButton { background-color : yellow; border:2px solid rgb(255, 232, 102);}");
                ui->btnDirRightState->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border:2px solid rgb(255, 232, 102);}");
                break;
            case 2:
                ui->pbDirLampLeftState->setStyleSheet("QPushButton { background-color : white; }");
                ui->pbDirLampRightState->setStyleSheet("QPushButton { background-color : yellow; }");
                ui->btnDirRightState->setStyleSheet("QPushButton { background-color : yellow; border:2px solid rgb(255, 232, 102);}");
                ui->bntDirOffState->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border:2px solid rgb(255, 232, 102);}");
                ui->btnDirLeftState->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border:2px solid rgb(255, 232, 102);}");
                break;
            default :
                break;
            }
        } else {
            statLampDirBlinck=true;
            ui->pbDirLampLeftState->setStyleSheet("QPushButton { background-color : white; }");
            ui->pbDirLampRightState->setStyleSheet("QPushButton { background-color : white; }");
        }
    } else {
        ui->pbDirLampLeftState->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
        ui->pbDirLampRightState->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
    }

}

void MainWindow::slotTimerLampEmerState(void)
{
      if(statLampEmergency) {
      if(statLampEmerBlank) {   //blinking하기 위한 code
            ui->btnHeadlight1->setStyleSheet("QPushButton { background-color : yellow; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
            ui->btnHeadlight2->setStyleSheet("QPushButton { background-color : yellow; border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
            ui->btnBreaklight1->setStyleSheet("QPushButton { background-color : yellow; }");
            ui->btnBreaklight2->setStyleSheet("QPushButton { background-color : yellow; }");
            ui->btnBreaklight3->setStyleSheet("QPushButton { background-color : yellow; }");
            ui->btnBreaklight4->setStyleSheet("QPushButton { background-color : yellow; }");

            statLampEmerBlank=false;
        } else {
            ui->btnHeadlight1->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
            ui->btnHeadlight2->setStyleSheet("QPushButton { background-color : rgb(255,255,255); border-style:solid; border-width: 1px; border-radius: 50px;border-color:black; max-width: 100px; max-height:100px;max-width:100px;max-height:100px;border-radius:20px;}");
            ui->btnBreaklight1->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
            ui->btnBreaklight2->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
            ui->btnBreaklight3->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
            ui->btnBreaklight4->setStyleSheet("QPushButton { background-color : rgb(255,255,255); }");
            statLampEmerBlank=true;
        }
      }
}


MainWindow::~MainWindow()
{
    tmrEmergency->stop();
    tmrDirection->stop();
    recvCanData->exit();
    tmrTraceMode->stop();
    /* Implement a destructor. */



    delete ui;
}



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "thcanrecv.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTimer* monTimer;
    QTimer* tmrEmergency;
    QTimer* tmrDirection;
    //bs code
    QTimer* tmrEngine;
    QTimer* tmrTraceMode;

    thCanRecv *recvCanData;

    // trace mode color
    int m_trace_mode_color ;

    bool statLampEmergency;
    bool statLampEmerBlank;
    bool m_LampDirState;
    bool m_LampDirTimerState;

    unsigned char statLampDirection;
    bool stateLampDirEnable;
    bool statLampDirBlinck;
    bool statEmergencyBlink;
    bool stateTraceEnable;

    unsigned char m_stat_rotary_led;
    unsigned char m_stat_gpio_led;


    void putSystemMessage(QString);
    void refreshTime(void);




public slots:
    void slotTimerTraceModeState(void);
    void slotTimerGetSensorState(void);
    void slotTimerLampDirectionState(void);
    void slotTimerLampEmerState(void);
    void slotRecvCanDataPacket(char*);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <QProcess>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

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
    QTimer* inputTimer;
    QTimer* tmrEmergency;
    QTimer* tmrAccel ;
    QTimer* tmrTraceMode ;
    QTimer* tmrTemp ;
    void putSystemMessage(QString);
    void refreshTime(void);

    // initialize
    bool m_initial_state ;

    unsigned char m_LampValue;
    bool m_EmergencyValue;
    bool m_EmergencyLampValue;
    bool m_LampDirState;
    bool m_LampDirTimerState;
    unsigned char m_LampDirValue;
    unsigned char m_RotaryLedValue;

    // ultraSonicWave
    bool m_ultra_sensor_state ;
    int m_trig ;
    int m_echo;
    int start_time;
    int end_time;
    float distance ;

    char regs[2] ;
    char led_dir_lamp;

    // Engine
    bool m_engine_state;
    char m_break_state;
    bool m_trace_mode_state;
    bool m_lamp_state;

    // Gear
    char m_gear_state;

    // Temperature
    double m_temp_value;

    // Clarkson
    bool m_clarkson_state ;

    // Speed
    unsigned short m_time_count ;
    bool m_accel_state;
    unsigned char m_speed ;
    bool m_toggle_speed ;

    int m_CanFd;
    struct sockaddr_can m_CanAddr;
    struct can_frame m_CanSendFrame;
    struct ifreq m_Canifr;

    char mI2cReg;
    char mCledData = 0;
    char mValues[2] ;

    // I2C R/W
    int mI2cFd;
    int setI2cDevice(int *fd, int addr);
    int setI2cRegister(int *fd, unsigned char reg);
    int setI2cRegister(int *fd, unsigned char hb, unsigned char lb);

    // Temperature
    int getI2cTempRegValue(int *fd, double* value,  size_t size);
    QString getI2cTemperatureValue(void);

    // Illumination
    unsigned char getI2cIlluminRegValue(int *fd);
    QString getI2cIlluminationValue(void);

    // UltraSonicWave
    QString getUltraSonicWaveValue(void);
    void setGpioUltraSonicWaveInitialization(void);

    // Rotary Switch
    int parseCanPacketData(void);
    unsigned char getI2cExpGpioRotaryValue(int *fd);
    void setGpioLedControl(unsigned char led, bool state);
    void setGpioLedInitialization(void);

public slots:
    // Temperature
    void slotTimeTemperature(void);

    // Clarkson
    void slotBtnClarksonPressed(void);
    void slotBtnClarksonReleased(void);
    void setGpio26Initialization(void);

    // Engine
    void slotBtnEngineState(void);

    // break
    void slotBtnBreakPressed(void);
    // Trace mode
    void slotBtnTraceModeState(void);
    void slotTraceModeState(void);
    // Speed Accel
    void slotBtnAccelPressed(void);
    void slotBtnAccelState(void);
    void slotBtnAccelReleased(void);
    void slotBtnBreakReleased(void);

    // gear
    void slotBtnParkingClicked(void);
    void slotReversingClicked(void);
    void slotBtnDrivingClicked(void);
    void slotBtnGearColorReset(void);

    // lamp
    void slotBtnEmergencyClicked(void);
    void slotBtnLampAutoClicked(void);
    void slotBtnLampLowClicked(void);
    void slotBtnLampFrontClicked(void);
    void slotBtnLampTailClicked(void);
    void slotBtnLampOffClicked(void);
    void slotDialLampValueChange(int value);
    void slotBtnDirColorReset(void);

    // dir
    void slotBtnDirLeftClicked(void);
    void slotBtnDirOffClicked(void);
    void slotBtnDirRightClicked(void);
    void slotDialDirectionValueChange(int value);

    void slotBtnLampColorReset(void);
    void slotTimeDisplay(void);
    void slotBtnMsgClearClicked(void);

    void slotLampEmerState(void);

    int setI2cExpGpioRotaryInit(int *fd);
    int setI2cExpGpioRotaryLed(int *fd);

    int openCanDevice(void);
    void sendCanDataPacket(unsigned char id, unsigned char type, unsigned char func, unsigned char value1, unsigned char value2);

    void setGpioPushInitialization();
    void setGpioLedOff();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

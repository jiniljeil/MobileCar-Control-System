#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "devices.h"
#include "cmd_type.h"

#include "gpios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ultrasonic wave
#include <wiringPi.h>

#include <fcntl.h>
#include <unistd.h>
#include <QRandomGenerator>
#include <QDebug>
#include <QFile>
#include <QProcess>

// If you use the ultrasonic wave function and clarksion function, it should not be commented.
//#define CLARKSON
//#define ULTRASONIC_WAVE

struct tm * g_SysTimeinfo;
time_t g_SysRawTime;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->btnEngine->setStyleSheet("QPushButton {background-color: rgb(255,255,255); border-style: solid ;border-width: 1px ;border-radius: 20px; border-color: black ; max-width: 100px; max-height: 100px; max-width: 100px; max-height: 100px; font-size: 10pt;}");

    m_EmergencyValue = false;
    m_break_state = 0;
    m_speed = 0 ;
    distance = 100;// default

    int retv;
    m_time_count = 0 ;
    m_engine_state = false;
    m_initial_state = true;
    m_toggle_speed = false;
    m_RotaryLedValue = 0xfe ;
    led_dir_lamp = 0xff;
    m_LampValue = 0 ;

    // GEAR
    sendCanDataPacket(0x20, 0x01, 0x12, 0x00, 0x00);

    // engine stop
    sendCanDataPacket(0x20, 0x01, 0x10, 0x00, 0x00);
    usleep(2000);

    setGpioLedOff();
    setGpioLedControl(GP_LED0, B_ON);


    monTimer = new QTimer(this);
    monTimer->start(1500);
    tmrEmergency = new QTimer(this);
    tmrAccel = new QTimer(this);
    tmrTraceMode = new QTimer(this);
    tmrTemp = new QTimer(this);
    tmrTemp->start(5000);

    m_trace_mode_state = false;
    m_accel_state = false;

    putSystemMessage("[System] GUI Initialization - OK.");

    mI2cFd = open(I2C_DEVICE, O_RDWR);
    if(mI2cFd<0) {
        putSystemMessage("[System] I2C Line Open failed!");
    }

    retv = openCanDevice();
    if(retv<0)
        putSystemMessage("[System] CAN Device Failed!");

    // gpio swgroup off
    setI2cDevice(&mI2cFd, LEDSW_ADDR);

    mI2cReg = 0x07;
    mValues[0] = mI2cReg ;
    mValues[1] = 0x00 ;
    write(mI2cFd, mValues, 2);

    mI2cReg = 0x03;
    mValues[0] = mI2cReg ;
    mValues[1] = 0xff;
    write(mI2cFd, mValues, 2);

    // Init
    slotBtnLampOffClicked();
    slotBtnDirOffClicked();
    setGpioLedInitialization();
    setGpioLedControl(GP_LEDA, B_OFF);


    sendCanDataPacket(0x20, 0x01, 0x12, 0x00, 0x00);
#ifdef ULTRASONIC_WAVE
        setGpioUltraSonicWaveInitialization();
        if (wiringPiSetupSys() == -1) exit(1);

        m_trig = TRIGGER;
        m_echo = ECHO;

        pinMode(m_trig, OUTPUT);
        pinMode(m_echo, INPUT);
#endif

#ifdef CLARKSON
       setGpio26Initialization();

       // Clarkson
       connect(ui->btnClarkson, SIGNAL(pressed()), SLOT(slotBtnClarksonPressed()));
       connect(ui->btnClarkson, SIGNAL(released()), SLOT(slotBtnClarksonReleased()));
#endif
    // Engine
    connect(ui->btnEngine, SIGNAL(clicked()), SLOT(slotBtnEngineState()));

    // Trace mode
    connect(ui->btnTraceMode, SIGNAL(clicked()), SLOT(slotBtnTraceModeState()));

    // Speed
    connect(ui->btnAccel, SIGNAL(pressed()), SLOT(slotBtnAccelPressed()));
    connect(ui->btnAccel, SIGNAL(released()), SLOT(slotBtnAccelReleased()));
    connect(ui->btnBreak, SIGNAL(pressed()), SLOT(slotBtnBreakPressed()));
    connect(ui->btnBreak, SIGNAL(released()), SLOT(slotBtnBreakReleased()));

    connect(monTimer, SIGNAL(timeout()), SLOT(slotTimeDisplay()));
    connect(tmrEmergency, SIGNAL(timeout()), SLOT(slotLampEmerState()));
    connect(tmrAccel, SIGNAL(timeout()), SLOT(slotBtnAccelState()));
    connect(tmrTraceMode, SIGNAL(timeout()), SLOT(slotTraceModeState()));

    // Temperature
    connect(tmrTemp, SIGNAL(timeout()), SLOT(slotTimeTemperature()));

    connect(ui->btnMsgClear, SIGNAL(clicked()), SLOT(slotBtnMsgClearClicked(void)));
    connect(ui->btnLampEmergency, SIGNAL(clicked()), SLOT(slotBtnEmergencyClicked(void)));

    // Gear mode
    connect(ui->btnParking, SIGNAL(clicked()), SLOT(slotBtnParkingClicked()));
    connect(ui->btnReversing, SIGNAL(clicked()), SLOT(slotReversingClicked()));
    connect(ui->btnDriving, SIGNAL(clicked()), SLOT(slotBtnDrivingClicked()));

    // Lamp
    connect(ui->btnLampLow, SIGNAL(clicked()), SLOT(slotBtnLampLowClicked()));
    connect(ui->btnLampFront, SIGNAL(clicked()), SLOT(slotBtnLampFrontClicked()));
    connect(ui->btnLampTail, SIGNAL(clicked()), SLOT(slotBtnLampTailClicked()));
    connect(ui->btnLampOff, SIGNAL(clicked()), SLOT(slotBtnLampOffClicked()));
    connect(ui->btnLampAuto, SIGNAL(clicked()), SLOT(slotBtnLampAutoClicked()));
    connect(ui->diaLamp, SIGNAL(valueChanged(int)), SLOT(slotDialLampValueChange(int)));

    // Dir
    connect(ui->btnLampDirLeft, SIGNAL(clicked()), SLOT(slotBtnDirLeftClicked()));
    connect(ui->btnLampDirOff, SIGNAL(clicked()), SLOT(slotBtnDirOffClicked()));
    connect(ui->btnLampDirRight, SIGNAL(clicked()), SLOT(slotBtnDirRightClicked()));
    connect(ui->diaDirection, SIGNAL(valueChanged(int)), SLOT(slotDialDirectionValueChange(int)));
}

void MainWindow::slotTimeTemperature(void)
{
    QString m_temp_value_str = getI2cTemperatureValue();
    ui->lbDispLabelTop->setText(m_temp_value_str);
    double div = 0.0;
    double mod ;
    mod = modf(m_temp_value, &div);
    mod *= 10 ;

    sendCanDataPacket(0x20, 0x2, (unsigned char)div, (unsigned char)mod, 0x0);
}

void MainWindow::slotBtnClarksonPressed(void)
{
    if ( m_engine_state == ENGINE_ON ) {
#ifdef CLARKSON
        system("echo 1 > /sys/class/gpio/gpio26/value");
#endif
        ui->btnClarkson->setStyleSheet("border: 1px solid black; border-radius: 20px; max-width: 100px; max-height: 100px; font-size: 10pt; background-color: black; color:white; font-weight: bold; ");

        if ( !m_clarkson_state ) {
            m_clarkson_state = true;
            sendCanDataPacket(0x20, 0x1, 0x24, 0x1, 0x0);
        }
    }
}

void MainWindow::slotBtnClarksonReleased(void)
{
    if ( m_engine_state == ENGINE_ON ) {
        m_clarkson_state = false;
#ifdef CLARKSON
        system("echo 0 > /sys/class/gpio/gpio26/value");
#endif
        ui->btnClarkson->setStyleSheet("border: 1px solid black; border-radius: 20px; max-width: 100px; max-height: 100px; font-size: 10pt; background-color: white; color:black; font-weight: bold;");
        sendCanDataPacket(0x20, 0x1, 0x24, 0x0, 0x0);
    }
}

int MainWindow::openCanDevice(void)
{
    if ((m_CanFd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1)
    {
        putSystemMessage("CAN Device Open Failed");
        return -1;
    } else {
        putSystemMessage("CAN Device Opened!");
    }

    strcpy(m_Canifr.ifr_name, CAN_DEVICE);
    ioctl(m_CanFd, SIOCGIFINDEX, &m_Canifr);

    m_CanAddr.can_family  = AF_CAN;
    m_CanAddr.can_ifindex = m_Canifr.ifr_ifindex;


    if (bind(m_CanFd, (struct sockaddr *)&m_CanAddr, sizeof(m_CanAddr)) == -1) {
        putSystemMessage("CAN Binding Error");
        return -1;
    }

    return 0;
}


void MainWindow::sendCanDataPacket(unsigned char id, unsigned char type, unsigned char func, unsigned char value1, unsigned char value2)
{
    int nbytes;

    m_CanSendFrame.can_id  = id;
    m_CanSendFrame.can_dlc = CAN_DEF_PACKET_SIZE;
    memset(m_CanSendFrame.data,0,sizeof(m_CanSendFrame.data));
    m_CanSendFrame.data[TICK] = 0; // Not used
    m_CanSendFrame.data[TYPE] = type;
    m_CanSendFrame.data[FUNC] = func;
    m_CanSendFrame.data[VAL1] = value1;
    m_CanSendFrame.data[VAL2] = value2;
    nbytes = write(m_CanFd, &m_CanSendFrame, sizeof(struct can_frame));
    if(nbytes<1)
    {
       putSystemMessage("Can data send failed!");
    }
}

void MainWindow::setGpioLedOff(void)
{
    system("echo 1 > /sys/class/gpio/gpio6/value");
    system("echo 1 > /sys/class/gpio/gpio16/value");
    system("echo 1 > /sys/class/gpio/gpio17/value");
    system("echo 1 > /sys/class/gpio/gpio18/value");
}

void MainWindow::setGpio26Initialization(void)
{
    system("echo 26 > /sys/class/gpio/export");
    usleep(10000);

    system("echo out > /sys/class/gpio/gpio26/direction");
    usleep(2000);
}

void MainWindow::setGpioUltraSonicWaveInitialization(void)
{
    system("echo 23 > /sys/class/gpio/export");
    usleep(2000);
    system("echo 24 > /sys/class/gpio/export");
    usleep(2000);

    system("echo out > /sys/class/gpio/gpio23/direction");
    usleep(2000);
    system("echo in > /sys/class/gpio/gpio24/direction");
    usleep(2000);
}

void MainWindow::setGpioLedInitialization(void)
{
    system("echo 6 > /sys/class/gpio/export");
    usleep(2000);
    system("echo 16 > /sys/class/gpio/export");
    usleep(2000);
    system("echo 17 > /sys/class/gpio/export");
    usleep(2000);
    system("echo 18 > /sys/class/gpio/export");
    usleep(2000);

    system("echo out > /sys/class/gpio/gpio6/direction");
    usleep(2000);
    system("echo out > /sys/class/gpio/gpio16/direction");
    usleep(2000);
    system("echo out > /sys/class/gpio/gpio17/direction");
    usleep(2000);
    system("echo out > /sys/class/gpio/gpio18/direction");
    usleep(2000);
}

void MainWindow::setGpioPushInitialization(void)
{
    system("echo 0 > /sys/class/gpio/export");
    usleep(2000);
    system("echo 5 > /sys/class/gpio/export");
    usleep(2000);

    system("echo in > /sys/class/gpio/gpio0/direction");
    usleep(2000);
    system("echo in > /sys/class/gpio/gpio5/direction");
    usleep(2000);
}



void MainWindow::setGpioLedControl(unsigned char led, bool state)
{
    if(state) // Off
    {
        switch(led)
        {
        case 0x00:
            system("echo 1 > /sys/class/gpio/gpio6/value");

            break;
        case 0x01:
            system("echo 1 > /sys/class/gpio/gpio16/value");

            break;
        case 0x02:
            system("echo 1 > /sys/class/gpio/gpio17/value");

            break;
        case 0x03:
            system("echo 1 > /sys/class/gpio/gpio18/value");

            break;
        case 0x04:
            system("echo 1 > /sys/class/gpio/gpio6/value");
            system("echo 1 > /sys/class/gpio/gpio16/value");
            system("echo 1 > /sys/class/gpio/gpio17/value");
            system("echo 1 > /sys/class/gpio/gpio18/value");

            break;
        default :
            break;

        }

    } else { // On
        switch(led)
        {
        case 0x00:
            system("echo 0 > /sys/class/gpio/gpio6/value");
            break;
        case 0x01:
            system("echo 0 > /sys/class/gpio/gpio16/value");
            break;
        case 0x02:
            system("echo 0 > /sys/class/gpio/gpio17/value");
            break;
        case 0x03:
            system("echo 0 > /sys/class/gpio/gpio18/value");
            break;
        case 0x04:
            system("echo 0 > /sys/class/gpio/gpio6/value");
            system("echo 0 > /sys/class/gpio/gpio16/value");
            system("echo 0 > /sys/class/gpio/gpio17/value");
            system("echo 0 > /sys/class/gpio/gpio18/value");
            break;
        default :
            break;
        }

    }
}

void MainWindow::slotBtnEngineState(void)
{

    setI2cDevice(&mI2cFd, LEDSW_ADDR);

    mI2cReg = 0x06;
    mValues[0] = mI2cReg ;
    mValues[1] = 0x00 ;
    write(mI2cFd, mValues, 2);

    mI2cReg = 0x02;
    mValues[0] = mI2cReg ;
    mValues[1] = 0xff;
    write(mI2cFd, mValues, 2);

    mCledData = 0b10001111;

    if (m_break_state != 0) {
        m_engine_state = (m_engine_state) ? false : true ;
        if (m_engine_state) putSystemMessage("ON");
        else putSystemMessage("OFF");

        if (m_engine_state == ENGINE_ON) {
            // parking color
            setGpioLedOff();
            setGpioLedControl(GP_LED0, B_ON);

            putSystemMessage("Engine ON");
            ui->btnEngine->setStyleSheet("QPushButton{ border: 1px solid black; border-radius: 20px; max-width: 100px; max-height: 100px; font-size: 10pt; background-color: black; color:white;}");
            sendCanDataPacket(0x20, 0x01, 0x10, 0x01,0x01);

            if ( mI2cFd < 0 ) {
                putSystemMessage("[System] Ioctl Open Failed!");
            } else {

                mValues[1] = mCledData + (0x05 << 4) ; // green
                write(mI2cFd, mValues, 2);
            }
        // EIGINE_OFF
        } else if (m_engine_state == ENGINE_OFF){
            putSystemMessage("Engine OFF");
            ui->btnEngine->setStyleSheet("QPushButton{ background-color : rgb(255,255,255); color: black; font-weight: bold ; border-style: solid ;border-width: 1px ;border-radius: 20px; border-color: black ; max-width: 100px; max-height: 100px; font-size: 10pt;}");

            mValues[1] = 0xff;
            write(mI2cFd, mValues, 2);
#ifdef CLARKSON
            system("echo 0 > /sys/class/gpio/gpio26");
            system("echo 26 > /sys/class/gpio/unexport");
#endif

#ifdef ULTRASONIC_WAVE
            system("echo 23 > /sys/class/gpio/unexport");
            system("echo 24 > /sys/class/gpio/unexport");
#endif

            sendCanDataPacket(0x20, 0x01, 0x10, 0x00, 0x00);
        }
    } else {
        putSystemMessage("Engine Error");
        ui->btnEngine->setStyleSheet("QPushButton{ background-color : red; color: white; font-weight:bold ; border-style: solid ;border-width: 1px ;border-radius: 20px; border-color: black ; max-width: 100px; max-height: 100px; max-width: 100px; max-height: 100px; font-size: 10pt; }");

        mValues[1] = mCledData + (0x06 << 4) ; // red
        write(mI2cFd, mValues, 2);

        sendCanDataPacket(0x20, 0x01, 0x10, 0x02, 0x01);
    }
}


void MainWindow::slotBtnTraceModeState(void)
{

    if ( m_engine_state == ENGINE_ON) {
        m_trace_mode_state = (m_trace_mode_state) ? false : true;
        if (m_trace_mode_state) {
            tmrTraceMode->start(4000);

            ui->btnTraceMode->setStyleSheet("QPushButton { background-color: red; font-weight: bold; color: white; font-size: 10pt; border-radius: 10px; }");
            putSystemMessage("Trace Mode Turn On !!!");

            sendCanDataPacket(0x20, 0x01, 0x11, 0x01, 0x00);
        } else {
            setI2cDevice(&mI2cFd, LEDSW_ADDR);

            mI2cReg = 0x06;
            mValues[0] = mI2cReg ;
            mValues[1] = 0x00 ;
            write(mI2cFd, mValues, 2);

            mI2cReg = 0x02;
            mValues[0] = mI2cReg ;
            mValues[1] = 0xff;
            write(mI2cFd, mValues, 2);

            mCledData = 0b10001111;
            mValues[1] = mCledData + (5 << 4) ; // green
            write(mI2cFd, mValues, 2);

            tmrTraceMode->stop();
            ui->btnTraceMode->setStyleSheet("QPushButton { background-color: rgb(0,0,0); font-weight: bold; color: red; font-size: 10pt; border-radius: 10px; } ");
            putSystemMessage("Trace Mode Turn Off !!!");
            sendCanDataPacket(0x20, 0x01, 0x11, 0x00, 0x00);
        }
    }
}

void MainWindow::slotTraceModeState(void)
{
    setI2cDevice(&mI2cFd, LEDSW_ADDR);

    mI2cReg = 0x06;
    mValues[0] = mI2cReg ;
    mValues[1] = 0x00 ;
    write(mI2cFd, mValues, 2);

    mI2cReg = 0x02;
    mValues[0] = mI2cReg ;
    mValues[1] = 0xff;
    write(mI2cFd, mValues, 2);

    mCledData = 0b10001111;

    for (int i = 0 ; i < 8; i++) {
#ifdef CLARKSON
        if( i % 2 == 0 ) system("echo 1 > /sys/class/gpio/gpio26/value");
        else {
            system("echo 0 > /sys/class/gpio/gpio26/value");
        }
#endif
        mValues[1] = mCledData + (i << 4) ;
        write(mI2cFd, mValues, 2);
        usleep(100000);
    }
}

void MainWindow::slotBtnAccelState(void)
{
    if (m_gear_state == DRIVE_MODE) {
        setI2cDevice(&mI2cFd, LEDSW_ADDR);

        mI2cReg = 0x07;
        mValues[0] = mI2cReg ;
        mValues[1] = 0x00 ;
        write(mI2cFd, mValues, 2);

        mI2cReg = 0x03;
        mValues[0] = mI2cReg ;
        mValues[1] = 0xff;
        write(mI2cFd, mValues, 2);

        if ( m_speed < 20) {
            mValues[1] = 0xff;
        } else if (m_speed < 40) {
            mValues[1] = 0xfe;
        } else if (m_speed < 60) {
            mValues[1] = 0xfc;
        } else if (m_speed < 80) {
            mValues[1] = 0xf8;
        } else if (m_speed < 100) {
            mValues[1] = 0xf0;
        } else if (m_speed < 120) {
            mValues[1] = 0xe0;
        } else if (m_speed < 140) {
            mValues[1] = 0xc0;
        } else if (m_speed < 160) {
            mValues[1] = 0x80;
        } else {
            // if the value of speed is higher than 160km,
            m_toggle_speed = (m_toggle_speed) ? false : true ;
            if (m_toggle_speed) {
                mValues[1] = 0x0 ;
            } else {
                mValues[1] = 0xff ;
            }
        }

        write(mI2cFd, mValues, 2);

        if (m_accel_state) {    //((0b10001111 + (5 << 4)) | ~(0xFF & ~0x08));

            if (m_trace_mode_state) {
                // 10 ~ 17
                m_speed += (QRandomGenerator::global()->bounded(3) + 5);
            } else {
                // 5 ~ 10
                m_speed += (QRandomGenerator::global()->bounded(5) + 1);
            }

            // max-speed 160
            if (m_speed > 160) {
                m_speed = 160;
            }

            ui->lcdDigitNumber->display(m_speed);
        } else {
            // Break button press
            if (m_break_state == 2) {

                if (m_speed > 0) {
                    // 5 ~ 10
                    m_speed -= (QRandomGenerator::global()->bounded(6) + 5) ;

                    // m_speed > 250: for underflow case.
                    if (m_speed < 5 || m_speed > 250){
                        m_speed = 0 ;
                        m_time_count = 0 ;
                        tmrAccel->stop();
                    }
                    ui->lcdDigitNumber->display(m_speed);
                } else if (m_speed == 0) {
                    tmrAccel->stop();
                }
                sendCanDataPacket(0x20, 0x01, 0x22, m_speed, 0x01);
            // Don't anything
            } else {
                if (m_speed > 0) {
                    m_speed-- ;

                    // m_speed > 250: for underflow case.
                    if (m_speed < 5 || m_speed > 250) {
                        m_speed = 0;
                        m_time_count = 0 ;
                        tmrAccel->stop();
                    }
                    ui->lcdDigitNumber->display(m_speed);
                } else if (m_speed == 0) {
                    tmrAccel->stop();
                }
            }
        }

        if (m_break_state == 1 || m_accel_state == true) {

            if (m_trace_mode_state) {
                sendCanDataPacket(0x20, 0x01, 0x23, m_speed, 0x01);
            } else {
                sendCanDataPacket(0x20, 0x01, 0x23, m_speed, 0x00);
            }

        }
    }
}

void MainWindow::slotBtnAccelPressed(void)
{
    // timer on
    m_accel_state = true;
    tmrAccel->start(500);
    ui->btnAccel->setStyleSheet("QPushButton { background-color : white; color: black; font-weight: bold; }");
}

void MainWindow::slotBtnAccelReleased(void)
{

   m_accel_state = false;
   ui->btnAccel->setStyleSheet("QPushButton { background-color : black; color: white; font-weight: bold;  }");
}

void MainWindow::slotBtnBreakPressed(void)
{
    if ( m_engine_state == ENGINE_OFF)
    {
        m_break_state = 1;
    } else {
        m_break_state = 2 ;

        setI2cDevice(&mI2cFd, LEDSW_ADDR);

        char led_state = mValues[1];

        mValues[0] = 0x06 ;
        mValues[1] = 0x03 ;
        write(mI2cFd, mValues, 2);

        mValues[0] = 0x02 ;
        mValues[1] = 0xff;
        write(mI2cFd, mValues, 2);

        mValues[0] = 0x02 ;

//      0b11011011 (Green color LED + SW4)
        mValues[1] = ((0b10001111 + (0x05 << 4)) & (0xFF & ~0x04))  ;

        write(mI2cFd, mValues, 2);
    }
}

void MainWindow::slotBtnBreakReleased(void)
{
    m_break_state = 1;
    ui->btnAccel->setStyleSheet("QPushButton { background-color : black; color: white; font-weight: bold;  }");

    if ( m_engine_state == ENGINE_ON ) {
        setI2cDevice(&mI2cFd, LEDSW_ADDR);

        mI2cReg = 0x06;
        mValues[0] = mI2cReg ;
        mValues[1] = 0x00 ;
        write(mI2cFd, mValues, 2);

        mI2cReg = 0x02;
        mValues[0] = mI2cReg ;
        mValues[1] = 0xff;
        write(mI2cFd, mValues, 2);

        mCledData = 0b10001111;

        mValues[1] = mCledData + (0x05 << 4);

        write(mI2cFd, mValues, 2);
    }
    sendCanDataPacket(0x20, 0x01, 0x22, m_speed, 0x00);
}

void MainWindow::slotBtnGearColorReset(void)
{
    ui->btnParking->setStyleSheet("QPushButton { border: 2px solid grey ; background-color: rgb(255,255,255);}");
    ui->btnReversing->setStyleSheet("QPushButton { border: 2px solid grey ; background-color: rgb(255,255,255);}");
    ui->btnDriving->setStyleSheet("QPushButton { border: 2px solid grey ; background-color: rgb(255,255,255);}");
}

void MainWindow::slotBtnParkingClicked(void)
{
//    if ( m_engine_state == ENGINE_ON ) {
        if ( m_ultra_sensor_state ) m_ultra_sensor_state = false;
        slotBtnGearColorReset();
        ui->grbDrive->setTitle("Park");
        ui->btnParking->setStyleSheet("QPushButton { background-color : rgb(0,0,0); color: white; font-weight: bold; } ");

        setGpioLedOff();
        setGpioLedControl(GP_LED0, B_ON);

        m_gear_state = PARK_MODE;

        sendCanDataPacket(0x20, 0x01, 0x12, 0x00, 0x00);
//    }

}

void MainWindow::slotReversingClicked(void)
{
    if ( m_engine_state == ENGINE_ON ) {
        slotBtnGearColorReset();
        ui->grbDrive->setTitle("Reverse");
        ui->btnReversing->setStyleSheet("QPushButton { background-color : rgb(0,0,0); color: white; font-weight: bold; } ");

        m_ultra_sensor_state = true;

        setGpioLedOff();
        setGpioLedControl(GP_LED1, B_ON);
        m_gear_state = REVERSE_MODE;
    }
}

void MainWindow::slotBtnDrivingClicked(void)
{
    if ( m_engine_state == ENGINE_ON ) {
        if ( m_ultra_sensor_state ) m_ultra_sensor_state = false;
        slotBtnGearColorReset();
        ui->grbDrive->setTitle("Drive");
        ui->btnDriving->setStyleSheet("QPushButton { background-color : rgb(0,0,0); color: white; font-weight: bold; }");

        setGpioLedOff();
        setGpioLedControl(GP_LED2, B_ON);
        setGpioLedControl(GP_LED3, B_ON);

        m_gear_state = DRIVE_MODE;

        sendCanDataPacket(0x20, 0x01, 0x12, 0x02, 0x00);
   }
}

void MainWindow::slotLampEmerState(void)
{
    if ( m_engine_state == ENGINE_ON ) {
        setI2cDevice(&mI2cFd, LEDSW_ADDR);

        mValues[0] = 0x06 ;
        mValues[1] = 0x03 ;
        write(mI2cFd, mValues, 2);

        mValues[0] = 0x02 ;
        mValues[1] = 0xff;
        write(mI2cFd, mValues, 2);

        if(m_EmergencyLampValue) {

            mValues[0] = 0x02 ;

    //      0b11010111 (Green color LED + SW5)
            mValues[1] = ((0b10001111 + (0x05 << 4)) & (0xFF & ~0x08))  ;

            write(mI2cFd, mValues, 2);

            ui->lbStateEmergency->setStyleSheet("QLabel { background-color : yellow; }");

            m_EmergencyLampValue=false;
        } else {


            mValues[0] = 0x02 ;

    //      0b11011111 (Green color LED)
            mValues[1] = ((0b10001111 + (0x05 << 4)) & 0xFF)  ;

            write(mI2cFd, mValues, 2);

            ui->lbStateEmergency->setStyleSheet("QLabel { background-color : rgb(255,255,255); }");

            m_EmergencyLampValue=true;
        }
        sendCanDataPacket(0x20, 0x01, 0x14, 0x01, 0x01);
    }
}

void MainWindow::slotBtnEmergencyClicked(void)
{
    if ( m_engine_state == ENGINE_ON ) {
        m_EmergencyValue = (m_EmergencyValue) ? false : true;
        if(m_EmergencyValue) {
            ui->btnLampEmergency->setStyleSheet("QPushButton { background-color: red; font-weight: bold; color: white; font-size: 10pt; border-radius: 10px;}");
            sendCanDataPacket(0x20, 0x01, 0x14, 0x01,0x00);
            tmrEmergency->start(1000);
        } else {
            setI2cDevice(&mI2cFd, LEDSW_ADDR);

            mValues[0] = 0x06 ;
            mValues[1] = 0x03 ;
            write(mI2cFd, mValues, 2);

            mValues[0] = 0x02 ;
            mValues[1] = 0xff;
            write(mI2cFd, mValues, 2);

            mValues[0] = 0x02 ;

            // 0b11011111 (Green color LED)
            mValues[1] = ((0b10001111 + (0x05 << 4)) & 0xFF)  ;

            write(mI2cFd, mValues, 2);

            ui->btnLampEmergency->setStyleSheet("QPushButton { background-color: rgb(0,0,0); font-weight: bold; color: white; font-size: 10pt; border-radius: 10px;}");
            ui->lbStateEmergency->setStyleSheet("QLabel { background-color : rgb(255,255,255); }");
            sendCanDataPacket(0x20, 0x01, 0x14, 0x00,0x00);
            tmrEmergency->stop();
        }
    }
}

void MainWindow::slotBtnMsgClearClicked(void)
{
    ui->txtSystemMessage->clear();
}


void MainWindow::slotBtnDirOffClicked()
{
    slotBtnDirColorReset();
    ui->btnLampDirOff->setStyleSheet("QPushButton { background-color : yellow; }");
    ui->diaDirection->setValue(1);
    m_LampDirValue = 0x01;
}

void MainWindow::slotBtnDirLeftClicked()
{
    if ( m_engine_state == ENGINE_ON ) {
        slotBtnDirColorReset();
        ui->btnLampDirLeft->setStyleSheet("QPushButton { background-color : yellow; }");
        ui->diaDirection->setValue(0);
        m_LampDirValue = 0x00;
    }
}

void MainWindow::slotBtnDirRightClicked()
{
    if ( m_engine_state == ENGINE_ON ) {
        slotBtnDirColorReset();
        ui->btnLampDirRight->setStyleSheet("QPushButton { background-color : yellow; }");
        ui->diaDirection->setValue(2);
        m_LampDirValue = 0x02;
    }
}

void MainWindow::slotDialDirectionValueChange(int value)
{
    if ( m_engine_state == ENGINE_ON ) {
        int setBtnValue;
        value = ui->diaDirection->value();
        setBtnValue = value % 3;

        setI2cDevice(&mI2cFd, MOD_ROTARY);

        regs[0] = 0x07 ; // GPIO Register - TCA9535 Data sheet 23p
        regs[1] = 0x00 ; // Setting Value
        write(mI2cFd, regs, 2) ;

        // LED Initial
        regs[0] = 0x03 ; // Output Port 1
        regs[1] = 0xff ; // All off
        write(mI2cFd, regs, 2);


        switch(setBtnValue)
        {
        case 0:
            slotBtnDirLeftClicked();
            led_dir_lamp = 0xcf;
            regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
            write(mI2cFd, regs, 2);

            m_LampDirValue = 0x00;
            break;
        case 1:
            slotBtnDirOffClicked();
            m_LampDirValue = 0x01;
            led_dir_lamp = 0xff;
            regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
            write(mI2cFd, regs, 2);
            break;
        case 2:
            slotBtnDirRightClicked();
            led_dir_lamp = 0x3f;
            regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
            write(mI2cFd, regs, 2);

            m_LampDirValue = 0x02;
            break;
        default:
            break;
        }

        sendCanDataPacket(0x20,0x01,0x21,(char)setBtnValue,0x01);
    }
}

void MainWindow::slotBtnLampAutoClicked(void)
{
    if ( m_engine_state == ENGINE_ON ) {
        slotBtnLampColorReset();
        ui->btnLampAuto->setStyleSheet("QPushButton { background-color : yellow; }");
        ui->diaLamp->setValue(4);
        m_LampValue = 0x04;

        sendCanDataPacket(0x20, 0x01, 0x20, 0x04, 0x00);
    }
}

void MainWindow::slotBtnLampLowClicked(void)
{
    if ( m_engine_state == ENGINE_ON ) {
        slotBtnLampColorReset();
        ui->btnLampLow->setStyleSheet("QPushButton { background-color : yellow; }");
        ui->diaLamp->setValue(1);
        m_LampValue = 0x01;

        setI2cDevice(&mI2cFd, MOD_ROTARY);
        m_RotaryLedValue = 0xfc ;

        regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
        write(mI2cFd, regs, 2);

        sendCanDataPacket(0x20, 0x01, 0x20, 0x01, 0x00);
    }
}

void MainWindow::slotBtnLampFrontClicked(void)
{
    if ( m_engine_state == ENGINE_ON ) {
        slotBtnLampColorReset();
        ui->btnLampFront->setStyleSheet("QPushButton { background-color : yellow; }");
        ui->diaLamp->setValue(2);
        m_LampValue = 0x02;

        setI2cDevice(&mI2cFd, MOD_ROTARY);
        m_RotaryLedValue = 0xf8 ;

        regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
        write(mI2cFd, regs, 2);
        sendCanDataPacket(0x20, 0x01, 0x20, 0x02, 0x00);
    }
}

void MainWindow::slotBtnLampTailClicked(void)
{
    if ( m_engine_state == ENGINE_ON ) {
        slotBtnLampColorReset();
        ui->btnLampTail->setStyleSheet("QPushButton { background-color : yellow; }");
        ui->diaLamp->setValue(3);
        m_LampValue = 0x03;

        setI2cDevice(&mI2cFd, MOD_ROTARY);
        m_RotaryLedValue = 0xf0 ;


        regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
        write(mI2cFd, regs, 2);

        sendCanDataPacket(0x20, 0x01, 0x20, 0x03, 0x00);
    }
}

void MainWindow::slotBtnLampOffClicked(void)
{

    slotBtnLampColorReset();
    ui->btnLampOff->setStyleSheet("QPushButton { background-color : yellow; }");
    ui->diaLamp->setValue(0);
    m_LampValue = 0x00;

    setI2cDevice(&mI2cFd, MOD_ROTARY);
    m_RotaryLedValue = 0xfe;

    regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
    write(mI2cFd, regs, 2);

    sendCanDataPacket(0x20, 0x01, 0x20, 0x00, 0x00);
}

void MainWindow::slotDialLampValueChange(int value)
{
    if ( m_engine_state == ENGINE_ON ) {
        int setBtnValue;
        value = ui->diaLamp->value();
        setBtnValue = value % 5;

        m_lamp_state = false;

        setI2cDevice(&mI2cFd, MOD_ROTARY);

        regs[0] = 0x07 ; // GPIO Register - TCA9535 Data sheet 23p
        regs[1] = 0x00 ; // Setting Value
        write(mI2cFd, regs, 2) ;

        // LED Initial
        regs[0] = 0x03 ; // Output Port 1
        regs[1] = 0xff ; // All off
        write(mI2cFd, regs, 2);

        switch(setBtnValue)
        {
        case 0:
            slotBtnLampOffClicked();
            putSystemMessage("Lamp Off Clicked");
            break;
        case 1:
            slotBtnLampLowClicked();
            putSystemMessage("Lamp Low Clicked");
            break;
        case 2:
            slotBtnLampFrontClicked();
            putSystemMessage("Lamp Front Clicked");
            break;
        case 3:
            slotBtnLampTailClicked();
            putSystemMessage("Lamp Tail Clicked");
            break;
        case 4:
            slotBtnLampAutoClicked();
            putSystemMessage("Lamp Auto Clicked");
            m_lamp_state = true;
        default:
            break;
        }
    }
}


int MainWindow::setI2cDevice(int *fd, int addr)
{
    return ioctl(*fd, I2C_SLAVE, addr);
}

int MainWindow::getI2cTempRegValue(int *fd, double* value,  size_t size)
{
    int retv;
    char regv[2]={0,0};
    retv = read(*fd, regv, size);

    *value = (((regv[0]<<8) | regv[1])>>4) * 0.0625;

    return retv;
}

QString MainWindow::getI2cTemperatureValue(void)
{
    QString strValue;
    double tempValue;
    unsigned char regs[2]={0x00,0x00};
    // Get temperature data
    setI2cDevice(&mI2cFd, TEMP_ADDR);
    setI2cRegister(&mI2cFd,regs[0],regs[1]);
    getI2cTempRegValue(&mI2cFd, &tempValue, 2);

    m_temp_value = tempValue;

    return strValue.sprintf("%.1f C",tempValue);
}

// ADC로부터 조도 값을 가져온다.
unsigned char MainWindow::getI2cIlluminRegValue(int *fd)
{
    double temp;

    unsigned char regv;
    read(*fd, &regv, sizeof(regv));

    temp = ((255-regv)/255.0)*100;  // 백분율 계산

    return (unsigned char)temp;
}

// 조도값을 화면에 출력한다
QString MainWindow::getI2cIlluminationValue(void)
{
    int retv;
    QString strValue;

    unsigned char reg = 0;  // Chnnel selected - 0

    setI2cDevice(&mI2cFd, ADC_CDS);  // 0x4C
    retv=setI2cRegister(&mI2cFd, reg);
    if(retv!=1)
        putSystemMessage("[System] ADC error!.");
    retv=getI2cIlluminRegValue(&mI2cFd);

    return strValue.sprintf("%2d %%",retv);
}

int MainWindow::setI2cRegister(int *fd, unsigned char reg)
{
    return write(*fd, &reg,sizeof(reg));
}

int MainWindow::setI2cRegister(int *fd, unsigned char hb, unsigned char lb)
{
    unsigned char regs[2]={0,};
    regs[0] = hb;
    regs[1] = lb;
    write(*fd, regs,sizeof(regs));
}

int MainWindow::setI2cExpGpioRotaryInit(int *fd)
{

    int retv;
    unsigned char regs[2]={0,};

    retv = ioctl(*fd, I2C_SLAVE, MOD_ROTARY);
    usleep(1000);

    regs[0]=0x07;
    regs[1]=0x00; // I2C Ch1 - Output
    write(*fd, regs, sizeof(regs));
    usleep(1000);

    regs[0]=0x03; // Output Register for Ch1
    regs[1]=0x00; // 8Bit(LED0~LED8);
    write(*fd, regs, sizeof(regs));

}


int MainWindow::setI2cExpGpioRotaryLed(int *fd)
{

    int retv;
    unsigned char regs[2]={0,};

    retv = ioctl(*fd, I2C_SLAVE, MOD_ROTARY);
    usleep(1000);

    regs[0]=0x03; // Output Register for Ch1
    regs[1]=m_RotaryLedValue; // 8Bit(LED0~LED8);
    write(*fd, regs, sizeof(regs));

}


unsigned char MainWindow::getI2cExpGpioRotaryValue(int *fd)
{

    int retv;
    unsigned char reg;

    retv = ioctl(*fd, I2C_SLAVE, MOD_ROTARY);
    usleep(1000);

    reg=0x00; // Output Register for Ch1

    read(*fd, &reg, sizeof(reg));

    return reg;

}

void MainWindow::putSystemMessage(QString msg)
{
    ui->txtSystemMessage->append(msg);
}

QString MainWindow::getUltraSonicWaveValue() {
    digitalWrite(m_trig, LOW);
    delay(500);

    digitalWrite(m_trig, HIGH);

    delayMicroseconds(10);
    digitalWrite(m_trig, LOW);

    while( digitalRead(m_echo) == LOW) start_time = micros();

    while( digitalRead(m_echo) == HIGH) end_time = micros();
    if ((end_time - start_time) / 29. / 2. > 1000) {
        distance = 1000;
    } else {
        distance = (end_time - start_time) / 29. / 2. ;
    }

    QString str;
    return str.sprintf("%.2f cm", distance);
}

void MainWindow::slotTimeDisplay(void)
{
    QString illumationValue = getI2cIlluminationValue();
    ui->lbDispLabelMiddle->setText(illumationValue);


#ifdef ULTRASONIC_WAVE
    QString ultraSonicWaveValue = getUltraSonicWaveValue();
    ui->lbUltraSonicWave->setText(ultraSonicWaveValue);
#endif
    // if the mode is reverse, give the ultraSonic information
    if (m_gear_state == REVERSE_MODE) {
        if ( distance <= 20 ) {
#ifdef CLARKSON
            system("echo 1 > /sys/class/gpio/gpio26/value");
#endif

            sendCanDataPacket(0x20, 0x01, 0x12, 0x01, 0x01);
        } else {
#ifdef CLARKSON
            system("echo 0 > /sys/class/gpio/gpio26/value");
#endif
            sendCanDataPacket(0x20, 0x01, 0x12, 0x01, 0x00);
        }
    }

    int value = illumationValue.split(" ")[0].toInt();



        setI2cDevice(&mI2cFd, MOD_ROTARY);

        switch(m_LampValue)
        {
            case 0: // OFF               1111 1111
                m_RotaryLedValue = 0xfe;
                regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
                write(mI2cFd, regs, 2);
            break;

            case 1: // Low Lamp          0110 0110
                m_RotaryLedValue = 0xfc;
                regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
                write(mI2cFd, regs, 2);
            break;

            case 2: // Front Lamp        1010 0101
                m_RotaryLedValue = 0xf8;
                regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
                write(mI2cFd, regs, 2);
            break;

            case 3: // Tail Lamp         1110 0111
                m_RotaryLedValue = 0xf0;
                regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
                write(mI2cFd, regs, 2);
            break;

            case 4: // Auto Lamp
                setI2cDevice(&mI2cFd, MOD_ROTARY);

                if (value >= 97) {
                    m_RotaryLedValue = 0xfe;
                    regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
                    write(mI2cFd, regs, 2);
                    sendCanDataPacket(0x20, 0x01, 0x20, 0x04, 0x00);

                } else if (value >= 92) {
                    m_RotaryLedValue = 0xfc;
                    regs[1] = (m_RotaryLedValue & led_dir_lamp) ;
                    write(mI2cFd, regs, 2);
                    sendCanDataPacket(0x20, 0x01, 0x20, 0x04, 0x01);

                } else if (value >= 83) {
                    m_RotaryLedValue = 0xf8;
                    regs[1] = (m_RotaryLedValue & led_dir_lamp)  ;
                    write(mI2cFd, regs, 2);
                    sendCanDataPacket(0x20, 0x01, 0x20, 0x04, 0x02);

                } else {
                    m_RotaryLedValue = 0xf0;
                    regs[1] = (m_RotaryLedValue & led_dir_lamp)  ;
                    write(mI2cFd, regs, 2);
                    sendCanDataPacket(0x20, 0x01, 0x20, 0x04, 0x03);
                }
            default:
            break;
        }

        if (m_initial_state) {
            ui->btnParking->setStyleSheet("QPushButton { background-color : rgb(0,0,0); color: white; font-weight: bold; } ");
            m_initial_state = false;

            // Direction
            sendCanDataPacket(0x20, 0x01, 0x21, 0x01, 0x00);


            setGpioLedOff();
            setGpioLedControl(GP_LED0, B_ON);

            setI2cDevice(&mI2cFd, LEDSW_ADDR);

            mI2cReg = 0x07;
            mValues[0] = mI2cReg ;
            mValues[1] = 0x00 ;
            write(mI2cFd, mValues, 2);

            mI2cReg = 0x03;
            mValues[0] = mI2cReg ;
            mValues[1] = 0xff;
            write(mI2cFd, mValues, 2);

            mI2cReg = 0x06;
            mValues[0] = mI2cReg ;
            mValues[1] = 0x00 ;
            write(mI2cFd, mValues, 2);

            mI2cReg = 0x02;
            mValues[0] = mI2cReg ;
            mValues[1] = 0xff;
            write(mI2cFd, mValues, 2);
            setI2cExpGpioRotaryLed(&mI2cFd);

            sendCanDataPacket(0x20, 0x01, 0x10, 0x00, 0x00);
            putSystemMessage("Initialization");
        }

    refreshTime();
}

void MainWindow::refreshTime()
{
    char strTempArray[30];

    time(&g_SysRawTime);
    g_SysTimeinfo = localtime(&g_SysRawTime);
    sprintf(strTempArray,"%04d/%02d/%02d", g_SysTimeinfo->tm_year+1900,g_SysTimeinfo->tm_mon,g_SysTimeinfo->tm_mday);
    ui->lbDispDate->setText(strTempArray);
    sprintf(strTempArray,"%02d:%02d:%02d",g_SysTimeinfo->tm_hour,g_SysTimeinfo->tm_min,g_SysTimeinfo->tm_sec);
    ui->lbDispTime->setText(strTempArray);
}




void MainWindow::slotBtnLampColorReset(void)
{
    ui->btnLampLow->setStyleSheet("QPushButton { background-color: rgb(255,255,255); border: 2px solid grey; }");
    ui->btnLampFront->setStyleSheet("QPushButton { background-color: rgb(255,255,255); border: 2px solid grey; }");
    ui->btnLampTail->setStyleSheet("QPushButton { background-color: rgb(255,255,255); border: 2px solid grey; }");
    ui->btnLampOff->setStyleSheet("QPushButton { background-color: rgb(255,255,255); border: 2px solid grey; }");
    ui->btnLampAuto->setStyleSheet("QPushButton { background-color: rgb(255,255,255); border: 2px solid grey; }");
}

void MainWindow::slotBtnDirColorReset(void)
{
    ui->btnLampDirOff->setStyleSheet("QPushButton { background-color: rgb(255,255,255); border: 2px solid grey; }");
    ui->btnLampDirLeft->setStyleSheet("QPushButton { background-color: rgb(255,255,255); border: 2px solid grey; }");
    ui->btnLampDirRight->setStyleSheet("QPushButton { background-color: rgb(255,255,255); border: 2px solid grey; }");
}

MainWindow::~MainWindow()
{
    /* Implement a destructor. */

    // GPIO LED OFF
    system("echo 1 > /sys/class/gpio/gpio6/value");
    system("echo 1 > /sys/class/gpio/gpio16/value");
    system("echo 1 > /sys/class/gpio/gpio17/value");
    system("echo 1 > /sys/class/gpio/gpio18/value");

#ifdef CLARKSON
    system("echo 0 > /sys/class/gpio/gpio26/value");
    system("echo 26 > /sys/class/gpio/unexport");
#endif

    // Device close
    system("echo 6 > /sys/class/gpio/unexport");
    system("echo 16 > /sys/class/gpio/unexport");
    system("echo 17 > /sys/class/gpio/unexport");
    system("echo 18 > /sys/class/gpio/unexport");

#ifdef ULTRASONIC_WAVE
    system("echo 23 > /sys/class/gpio/unexport");
    system("echo 24 > /sys/class/gpio/unexport");
#endif
    // Color LED
    mValues[1] = 0xff;
    write(mI2cFd, mValues, 2);
    ::close(mI2cFd);

    delete ui;
}

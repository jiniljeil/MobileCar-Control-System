#ifndef DEVICES_H
#define DEVICES_H

#define I2C_DEVICE "/dev/i2c-1"
#define CAN_DEVICE "can0"

#define CAN_OWN_ID 0x10
#define CAN_DEV_ID 0x20

#define CAN_DEF_PACKET_SIZE 0x8

#define LEDSW_ADDR  0x20
#define MOD_ROTARY  0x21
#define TEMP_ADDR   0x48
#define ADC_CDS     0x4C
#define EPR_ADDR    0x50
#define RTC_ADDR    0x68
#define IMU_ADDR    0x69
#define IXP_ADDR    0x73

#define GP_LED0 0
#define GP_LED1 1
#define GP_LED2 2
#define GP_LED3 3
#define GP_LEDA 4

#define B_ON false
#define B_OFF true

#define ENGINE_ON true
#define ENGINE_OFF false

#define BREAK_ON 2
#define BREAK_OFF 1

#define PARK_MODE 0
#define REVERSE_MODE 1
#define DRIVE_MODE 2

#define TRIGGER 23
#define ECHO 24

#endif // DEVICES_H

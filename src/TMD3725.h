/*
  TMD3725.h - Adafruit_BusIO compatible library for ams TMD3725.
  Based on Datasheet v2-00 (2023-Mar-21).
*/

#ifndef TMD3725_H
#define TMD3725_H

#include <Arduino.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>

[cite_start]// I2C Address [cite: 215]
#define TMD3725_I2C_ADDRESS    0x39

[cite_start]// Register Addresses [cite: 250, 251, 252]
#define TMD3725_REG_ENABLE     0x80
#define TMD3725_REG_ATIME      0x81
#define TMD3725_REG_PRATE      0x82
#define TMD3725_REG_WTIME      0x83
#define TMD3725_REG_AILTL      0x84
#define TMD3725_REG_AILTH      0x85
#define TMD3725_REG_AIHTL      0x86
#define TMD3725_REG_AIHTH      0x87
#define TMD3725_REG_PILT       0x88
#define TMD3725_REG_PIHT       0x8A
#define TMD3725_REG_PERS       0x8C
#define TMD3725_REG_CFG0       0x8D
#define TMD3725_REG_PCFG0      0x8E
#define TMD3725_REG_PCFG1      0x8F
#define TMD3725_REG_CFG1       0x90
#define TMD3725_REG_REVID      0x91
#define TMD3725_REG_ID         0x92
#define TMD3725_REG_STATUS     0x93
#define TMD3725_REG_CDATAL     0x94
#define TMD3725_REG_CDATAH     0x95
#define TMD3725_REG_RDATAL     0x96
#define TMD3725_REG_RDATAH     0x97
#define TMD3725_REG_GDATAL     0x98
#define TMD3725_REG_GDATAH     0x99
#define TMD3725_REG_BDATAL     0x9A
#define TMD3725_REG_BDATAH     0x9B
#define TMD3725_REG_PDATA      0x9C
#define TMD3725_REG_CFG2       0x9F
#define TMD3725_REG_CFG3       0xAB

[cite_start]// Bit Masks [cite: 256, 258, 259, 260]
#define TMD3725_ENABLE_PON     0x01
#define TMD3725_ENABLE_AEN     0x02
#define TMD3725_ENABLE_PEN     0x04
#define TMD3725_ENABLE_WEN     0x08

[cite_start]// ID Value [cite: 344]
#define TMD3725_CHIP_ID        0xE4 

// Configuration Enums
typedef enum {
    TMD3725_GAIN_1X   = 0x00,
    TMD3725_GAIN_4X   = 0x01,
    TMD3725_GAIN_16X  = 0x02,
    TMD3725_GAIN_64X  = 0x03
} tmd3725_gain_t;

typedef enum {
    TMD3725_LDRIVE_6MA   = 0x00,
    TMD3725_LDRIVE_12MA  = 0x01,
    TMD3725_LDRIVE_18MA  = 0x02,
    TMD3725_LDRIVE_192MA = 0x1F
} tmd3725_ldrive_t;

struct TMD3725_Data {
    uint16_t clear;
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint8_t  proximity;
};

class TMD3725 {
public:
    TMD3725();
    ~TMD3725();

    bool begin(TwoWire *wire = &Wire);
    bool isConnected();

    // Power and Enable
    void enable(); 
    void disable();
    void enableProximity(bool enable);
    void enableALS(bool enable);

    // Configuration
    void setALSGain(tmd3725_gain_t gain);
    void setIntegrationTime(uint8_t atime);
    void setProximityPulses(uint8_t pulseCount);
    void setProximityDrive(tmd3725_ldrive_t drive);

    // Data Acquisition
    bool getData(TMD3725_Data &data);
    uint8_t getProximity();
    uint16_t getClear();
    
    // Interrupts
    uint8_t getStatus();
    void clearInterrupts();

private:
    Adafruit_I2CDevice *i2c_dev = NULL;
};

#endif

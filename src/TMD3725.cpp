/*
  TMD3725.cpp - Implementation using Adafruit_BusIO
*/

#include "TMD3725.h"

TMD3725::TMD3725() {}

TMD3725::~TMD3725() {
    if (i2c_dev) delete i2c_dev;
}

bool TMD3725::begin(TwoWire *wire) {
    if (i2c_dev) delete i2c_dev;
    i2c_dev = new Adafruit_I2CDevice(TMD3725_I2C_ADDRESS, wire);

    if (!i2c_dev->begin()) {
        return false;
    }

    return isConnected();
}

bool TMD3725::isConnected() {
    Adafruit_I2CRegister idReg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_ID);
    uint8_t id;
    
    if (!idReg.read(&id)) return false;

    [cite_start]// Mask reserved bits (1:0), check against 0xE4 [cite: 344]
    if ((id & 0xFC) == TMD3725_CHIP_ID) {
        return true;
    }
    return false;
}

void TMD3725::enable() {
    [cite_start]// Set PON, AEN, PEN [cite: 256-262]
    Adafruit_I2CRegister enableReg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_ENABLE);
    enableReg.write(TMD3725_ENABLE_PON | TMD3725_ENABLE_AEN | TMD3725_ENABLE_PEN);
    delay(5); // Allow wake up time
}

void TMD3725::disable() {
    Adafruit_I2CRegister enableReg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_ENABLE);
    enableReg.write(0x00);
}

void TMD3725::enableProximity(bool enable) {
    Adafruit_I2CRegister enableReg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_ENABLE);
    uint8_t val;
    enableReg.read(&val);
    
    if (enable) {
        val |= (TMD3725_ENABLE_PEN | TMD3725_ENABLE_PON);
    } else {
        val &= ~TMD3725_ENABLE_PEN;
    }
    enableReg.write(val);
}

void TMD3725::enableALS(bool enable) {
    Adafruit_I2CRegister enableReg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_ENABLE);
    uint8_t val;
    enableReg.read(&val);
    
    if (enable) {
        val |= (TMD3725_ENABLE_AEN | TMD3725_ENABLE_PON);
    } else {
        val &= ~TMD3725_ENABLE_AEN;
    }
    enableReg.write(val);
}

void TMD3725::setALSGain(tmd3725_gain_t gain) {
    [cite_start]// CFG1 Register [cite: 340]
    // Bits 1:0 are AGAIN
    Adafruit_I2CRegister cfg1Reg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_CFG1);
    uint8_t val;
    cfg1Reg.read(&val);
    val &= 0xFC; // Clear bottom 2 bits
    val |= (gain & 0x03);
    cfg1Reg.write(val);
}

void TMD3725::setIntegrationTime(uint8_t atime) {
    [cite_start]// ATIME Register [cite: 263]
    Adafruit_I2CRegister atimeReg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_ATIME);
    atimeReg.write(atime);
}

void TMD3725::setProximityPulses(uint8_t pulseCount) {
    [cite_start]// PCFG0 Register [cite: 332]
    // Bits 5:0 are PPULSE
    if (pulseCount > 63) pulseCount = 63;
    
    Adafruit_I2CRegister pcfg0Reg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_PCFG0);
    uint8_t val;
    pcfg0Reg.read(&val);
    val &= 0xC0; // Preserve PPULSE_LEN
    val |= pulseCount;
    pcfg0Reg.write(val);
}

void TMD3725::setProximityDrive(tmd3725_ldrive_t drive) {
    [cite_start]// PCFG1 Register [cite: 335]
    // Bits 4:0 are PLDRIVE
    Adafruit_I2CRegister pcfg1Reg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_PCFG1);
    uint8_t val;
    pcfg1Reg.read(&val);
    val &= 0xE0; // Preserve PGAIN
    val |= (drive & 0x1F);
    pcfg1Reg.write(val);
}

bool TMD3725::getData(TMD3725_Data &data) {
    // Read 9 bytes starting at 0x94 (CDATAL)
    [cite_start]// 2 bytes Clear, 2 Red, 2 Green, 2 Blue, 1 Prox [cite: 352-363]
    uint8_t buffer[9];
    
    // Use write_then_read for atomic transaction
    uint8_t startReg = TMD3725_REG_CDATAL;
    if (!i2c_dev->write_then_read(&startReg, 1, buffer, 9)) {
        return false;
    }

    data.clear = buffer[0] | (buffer[1] << 8);
    data.red   = buffer[2] | (buffer[3] << 8);
    data.green = buffer[4] | (buffer[5] << 8);
    data.blue  = buffer[6] | (buffer[7] << 8);
    data.proximity = buffer[8];

    return true;
}

uint8_t TMD3725::getProximity() {
    Adafruit_I2CRegister pdataReg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_PDATA);
    uint8_t val;
    pdataReg.read(&val);
    return val;
}

uint16_t TMD3725::getClear() {
    Adafruit_I2CRegister cdataReg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_CDATAL, 2, LSBFIRST);
    uint16_t val;
    cdataReg.read(&val);
    return val;
}

uint8_t TMD3725::getStatus() {
    [cite_start]// [cite: 345]
    Adafruit_I2CRegister statusReg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_STATUS);
    uint8_t val;
    statusReg.read(&val);
    return val;
}

void TMD3725::clearInterrupts() {
    [cite_start]// Clear by writing 1s to status register [cite: 342]
    Adafruit_I2CRegister statusReg = Adafruit_I2CRegister(i2c_dev, TMD3725_REG_STATUS);
    statusReg.write(0xFF);
}

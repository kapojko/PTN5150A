#include "PTN5150A.h"

#define PTN5150A_REG_ID 0x01
#define PTN5150A_REG_CONTROL 0x02
#define PTN5150A_REG_INTERRUPT_CABLE_STATUS 0x03
#define PTN5150A_REG_CC_STATUS 0x04
#define PTN5150A_REG_CON_DET_CONFIG 0x09
#define PTN5150A_REG_VCONN_STATUS 0x0A
#define PTN5150A_REG_RESET 0x10
#define PTN5150A_REG_INTERRUPT_MASK 0x18
#define PTN5150A_REG_INTERRUPT_STATUS 0x19

#define PTN5150A_I2C_TIMEOUT 1

bool PTN5150A_ReadID(struct PTN5150A_Platform *platform, uint8_t *deviceVersionId, uint8_t *vendorId) {
    uint8_t data;
    if (platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_ID, &data, 1, PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    *deviceVersionId = data >> 3;
    *vendorId = data & 0x07;

    return true;
}

bool PTN5150A_ReadControl(struct PTN5150A_Platform *platform, enum PTN5150A_RpSelection_DFP *rpSelection,
    enum PTN5150A_ModeSelection *modeSelection, bool *maskInterruptCable) {
    uint8_t data;
    if (platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_CONTROL, &data, 1, PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    *rpSelection = (enum PTN5150A_RpSelection_DFP)((data >> 3) & 0x03);
    *modeSelection = (enum PTN5150A_ModeSelection)((data >> 1) & 0x03);
    *maskInterruptCable = (data & 0x01) != 0;

    return true;
}

bool PTN5150A_WriteControl(struct PTN5150A_Platform *platform, enum PTN5150A_RpSelection_DFP rpSelection,
    enum PTN5150A_ModeSelection modeSelection, bool maskInterruptCable) {
    uint8_t data = ((uint8_t)rpSelection << 3) | ((uint8_t)modeSelection << 1) | (maskInterruptCable ? 1 : 0);
    return platform->i2cWriteReg(platform->i2cAddress, PTN5150A_REG_CONTROL, &data, 1, PTN5150A_I2C_TIMEOUT) >= 0;
}

bool PTN5150A_ReadClearInterruptCableStatus(struct PTN5150A_Platform *platform, uint8_t *interruptCableStatus) {
    return platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_INTERRUPT_CABLE_STATUS, interruptCableStatus, 1, PTN5150A_I2C_TIMEOUT) >= 0;
}

bool PTN5150A_ReadCCStatus(struct PTN5150A_Platform *platform, enum PTN5150A_CCVBusDetection_UFP *vBusDetection,
    enum PTN5150A_CCRpDetection_UFP *rpDetection, enum PTN5150A_CCPortAttachmentStatus *portAttachmentStatus,
    enum PTN5150A_CCPolarity *polarity) {
    uint8_t data;
    if (platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_CC_STATUS, &data, 1, PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    *vBusDetection = (enum PTN5150A_CCVBusDetection_UFP)((data >> 7) & 0x01);
    *rpDetection = (enum PTN5150A_CCRpDetection_UFP)((data >> 5) & 0x03);
    *portAttachmentStatus = (enum PTN5150A_CCPortAttachmentStatus)((data >> 2) & 0x07);
    *polarity = (enum PTN5150A_CCPolarity)(data & 0x03);

    return true;
}

bool PTN5150A_ReadConDetConfiguration(struct PTN5150A_Platform *platform, bool *disableConDetOutput) {
    uint8_t data;
    if (platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_CON_DET_CONFIG, &data, 1, PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    *disableConDetOutput = (data & 0x01) != 0;

    return true;
}

bool PTN5150A_WriteConDetConfiguration(struct PTN5150A_Platform *platform, bool disableConDetOutput) {
    uint8_t data = disableConDetOutput ? 0x01 : 0x00;
    if (platform->i2cWriteReg(platform->i2cAddress, PTN5150A_REG_CON_DET_CONFIG, &data, 1, PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    return true;
}

bool PTN5150A_ReadVConnStatus(struct PTN5150A_Platform *platform, enum PTN5150A_VConnDetectedStatus *vConnStatus) {
    uint8_t data;
    if (platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_VCONN_STATUS, &data, 1, PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    *vConnStatus = (enum PTN5150A_VConnDetectedStatus)(data & 0x03);

    return true;
}

bool PTN5150A_Reset(struct PTN5150A_Platform *platform) {
    uint8_t data = 0x01;
    if (platform->i2cWriteReg(platform->i2cAddress, PTN5150A_REG_RESET, &data, 1, PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    return true;
}

bool PTN5150A_ReadInterruptMask(struct PTN5150A_Platform *platform, uint8_t *interruptMask) {
    return platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_INTERRUPT_MASK, interruptMask, 1, PTN5150A_I2C_TIMEOUT) >= 0;
}

bool PTN5150A_WriteInterruptMask(struct PTN5150A_Platform *platform, uint8_t interruptMask) {
    return platform->i2cWriteReg(platform->i2cAddress, PTN5150A_REG_INTERRUPT_MASK, &interruptMask, 1, PTN5150A_I2C_TIMEOUT) >= 0;
}

bool PTN5150A_ReadClearInterruptStatus(struct PTN5150A_Platform *platform, uint8_t *interruptStatus) {
    return platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_INTERRUPT_STATUS, interruptStatus, 1, PTN5150A_I2C_TIMEOUT) >= 0;
}

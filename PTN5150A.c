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

#define PTN5150A_REG_VCONN_STATUS_ENABLE 0x43

#define PTN5150A_I2C_TIMEOUT 1

const char *PTN5150A_RpSelection_DFP_str[] = {
    "80uA Default", // PTN5150A_RP_SELECTION_DFP_80UA_DEFAULT = 0x0,
    "180uA Medium", // PTN5150A_RP_SELECTION_DFP_180UA_MEDIUM = 0x1,
    "330uA High"    // PTN5150A_RP_SELECTION_DFP_330UA_HIGH = 0x2,
};

const char *PTN5150A_ModeSelection_str[] = {
    "Device UFP",   // PTN5150A_MODE_SELECTION_DEVICE_UFP = 0x0,
    "Host DFP",     // PTN5150A_MODE_SELECTION_HOST_DFP = 0x1,
    "Dual Role DRP" // PTN5150A_MODE_SELECTION_DUAL_ROLE_DRP = 0x2,
};

const char *PTN5150A_CCVBusDetection_UFP_str[] = {
    "Not Detected", // PTN5150A_CC_VBUS_NOT_DETECTED = 0x0,
    "Detected"      // PTN5150A_CC_VBUS_DETECTED = 0x1,
};

const char *PTN5150A_CCRpDetection_UFP_str[] = {
    "Standby",      // PTN5150A_CC_RP_STANDBY = 0x0,
    "Standard USB", // PTN5150A_CC_RP_STD_USB = 0x1,
    "1.5A",         // PTN5150A_CC_RP_1_5A = 0x2,
    "3.0A"          // PTN5150A_CC_RP_3_0A = 0x3,
};

const char *PTN5150A_CCPortAttachmentStatus_str[] = {
    "Not Connected",                   // PTN5150A_CC_PORT_NOT_CONNECTED = 0x0,
    "DFP Attached",                    // PTN5150A_CC_PORT_DFP_ATTACHED = 0x1,
    "UFP Attached",                    // PTN5150A_CC_PORT_UFP_ATTACHED = 0x2,
    "Analog Audio Accessory Attached", // PTN5150A_CC_PORT_ANALOG_AUDIO_ACCESSORY_ATTACHED = 0x3,
    "Debug Accessory Attached"         // PTN5150A_CC_PORT_DEBUG_ACCESSORY_ATTACHED = 0x4,
};

const char *PTN5150A_CCPolarity_str[] = {
    "Cable Not Attached", // PTN5150A_CC_POLARITY_CABLE_NOT_ATTACHED = 0x0,
    "CC1",                // PTN5150A_CC_POLARITY_CC1 = 0x1,
    "CC2"                 // PTN5150A_CC_POLARITY_CC2 = 0x2,
};

const char *PTN5150A_VConnDetectedStatus_str[] = {
    "Standby",   // PTN5150A_VCONN_STANDBY = 0x0,
    "Power CC1", // PTN5150A_VCONN_POWER_CC1 = 0x1,
    "Power CC2"  // PTN5150A_VCONN_POWER_CC2 = 0x2,
};

bool PTN5150A_ReadID(struct PTN5150A_Platform *platform, uint8_t *deviceVersionId,
                     uint8_t *vendorId) {
    uint8_t data;
    if (platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_ID, &data, 1,
                             PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    *deviceVersionId = data >> 3;
    *vendorId = data & 0x07;

    return true;
}

bool PTN5150A_ReadControl(struct PTN5150A_Platform *platform, struct PTN5150A_Control *control) {
    uint8_t data;
    if (platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_CONTROL, &data, 1,
                             PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    control->rpSelection = (enum PTN5150A_RpSelection_DFP)((data >> 3) & 0x03);
    control->modeSelection = (enum PTN5150A_ModeSelection)((data >> 1) & 0x03);
    control->maskInterruptCable = (data & 0x01) != 0;

    return true;
}

bool PTN5150A_WriteControl(struct PTN5150A_Platform *platform,
                           const struct PTN5150A_Control *control) {
    uint8_t data = ((uint8_t)control->rpSelection << 3) | ((uint8_t)control->modeSelection << 1) |
                   (control->maskInterruptCable ? 1 : 0);
    return platform->i2cWriteReg(platform->i2cAddress, PTN5150A_REG_CONTROL, &data, 1,
                                 PTN5150A_I2C_TIMEOUT) >= 0;
}

void PTN5150A_DebugPrintControl(struct PTN5150A_Platform *platform,
                                const struct PTN5150A_Control *control) {
    platform->debugPrint("PTN5150A %02XH: Rp_Sel=%s Mode_Sel=%s Mask_Int_Cable=%d\r\n",
                         PTN5150A_REG_CONTROL, PTN5150A_RpSelection_DFP_str[control->rpSelection],
                         PTN5150A_ModeSelection_str[control->modeSelection],
                         control->maskInterruptCable);
}

bool PTN5150A_ReadClearInterruptCableStatus(struct PTN5150A_Platform *platform,
                                            uint8_t *interruptCableStatus) {
    return platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_INTERRUPT_CABLE_STATUS,
                                interruptCableStatus, 1, PTN5150A_I2C_TIMEOUT) >= 0;
}

bool PTN5150A_ReadCCStatus(struct PTN5150A_Platform *platform, struct PTN5150A_CCStatus *ccStatus) {
    uint8_t data;
    if (platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_CC_STATUS, &data, 1,
                             PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    ccStatus->vBusDetection = (enum PTN5150A_CCVBusDetection_UFP)((data >> 7) & 0x01);
    ccStatus->rpDetection = (enum PTN5150A_CCRpDetection_UFP)((data >> 5) & 0x03);
    ccStatus->portAttachmentStatus = (enum PTN5150A_CCPortAttachmentStatus)((data >> 2) & 0x07);
    ccStatus->polarity = (enum PTN5150A_CCPolarity)(data & 0x03);

    return true;
}

void PTN5150A_DebugPrintCCStatus(struct PTN5150A_Platform *platform,
                                 const struct PTN5150A_CCStatus *ccStatus) {
    platform->debugPrint("PTN5150A %02XH: VBus_Det=%s Rp_Det=%s Port_Attach=%s Polarity=%s\r\n",
                         PTN5150A_REG_CC_STATUS,
                         PTN5150A_CCVBusDetection_UFP_str[ccStatus->vBusDetection],
                         PTN5150A_CCRpDetection_UFP_str[ccStatus->rpDetection],
                         PTN5150A_CCPortAttachmentStatus_str[ccStatus->portAttachmentStatus],
                         PTN5150A_CCPolarity_str[ccStatus->polarity]);
}

bool PTN5150A_ReadConDetConfiguration(struct PTN5150A_Platform *platform,
                                      bool *disableConDetOutput) {
    uint8_t data;
    if (platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_CON_DET_CONFIG, &data, 1,
                             PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    *disableConDetOutput = (data & 0x01) != 0;

    return true;
}

bool PTN5150A_WriteConDetConfiguration(struct PTN5150A_Platform *platform,
                                       bool disableConDetOutput) {
    uint8_t data = disableConDetOutput ? 0x01 : 0x00;
    if (platform->i2cWriteReg(platform->i2cAddress, PTN5150A_REG_CON_DET_CONFIG, &data, 1,
                              PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    return true;
}

bool PTN5150A_EnableVConnStatus(struct PTN5150A_Platform *platform) {
    // NOTE: The register is not described in the datasheet, but mentioned (see below)
    /*
    Ra detect happens in all modes. VCONN enable
    happens autonomously when as DFP (including in DRP
    mode).
    Prior to accessing this register, system must write
    register offset 43H with value of 0xe0 to enable VCONN
    detected status. If register offset 43H is not set to 0xe0,
    VCONN detected status read out is always 00.
    */
    uint8_t data = 0xe0;
    if (platform->i2cWriteReg(platform->i2cAddress, PTN5150A_REG_VCONN_STATUS_ENABLE, &data, 1,
                              PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    return true;
}

bool PTN5150A_ReadVConnStatus(struct PTN5150A_Platform *platform,
                              enum PTN5150A_VConnDetectedStatus *vConnStatus) {
    uint8_t data;
    if (platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_VCONN_STATUS, &data, 1,
                             PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    *vConnStatus = (enum PTN5150A_VConnDetectedStatus)(data & 0x03);

    return true;
}

void PTN5150A_DebugPrintVConnStatus(struct PTN5150A_Platform *platform,
                                    enum PTN5150A_VConnDetectedStatus vConnStatus) {
    platform->debugPrint("PTN5150A %02XH: VConn_Det=%s\r\n", PTN5150A_REG_VCONN_STATUS,
                         PTN5150A_VConnDetectedStatus_str[vConnStatus]);
}

bool PTN5150A_Reset(struct PTN5150A_Platform *platform) {
    uint8_t data = 0x01;
    if (platform->i2cWriteReg(platform->i2cAddress, PTN5150A_REG_RESET, &data, 1,
                              PTN5150A_I2C_TIMEOUT) < 0) {
        return false;
    }

    return true;
}

bool PTN5150A_ReadInterruptMask(struct PTN5150A_Platform *platform, uint8_t *interruptMask) {
    return platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_INTERRUPT_MASK, interruptMask, 1,
                                PTN5150A_I2C_TIMEOUT) >= 0;
}

bool PTN5150A_WriteInterruptMask(struct PTN5150A_Platform *platform, uint8_t interruptMask) {
    return platform->i2cWriteReg(platform->i2cAddress, PTN5150A_REG_INTERRUPT_MASK, &interruptMask,
                                 1, PTN5150A_I2C_TIMEOUT) >= 0;
}

bool PTN5150A_ReadClearInterruptStatus(struct PTN5150A_Platform *platform,
                                       uint8_t *interruptStatus) {
    return platform->i2cReadReg(platform->i2cAddress, PTN5150A_REG_INTERRUPT_STATUS,
                                interruptStatus, 1, PTN5150A_I2C_TIMEOUT) >= 0;
}

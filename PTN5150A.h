#ifndef PTN5150A_H
#define PTN5150A_H

#include <stdbool.h>
#include <stdint.h>

#define PTN5150A_I2C_ADDR_ADR_0 0x1D // I2C address when ADR pulled down; 8bit: 0x3A
#define PTN5150A_I2C_ADDR_ADR_1 0x3D // I2C address when ADR pulled upl; 8bit: 0x7A

#define PTN5150A_DEVICE_VERSION_ID 0x01
#define PTN5150A_VENDOR_ID 0x03

#define PTN5150A_INTERRUPT_CABLE_DETACH 0x02
#define PTN5150A_INTERRUPT_CABLE_ATTACH 0x01

#define PTN5150A_INTERRUPT_CC_COMPARATOR_CHANGE 0x10
#define PTN5150A_INTERRUPT_ROLE_CHANGE 0x08
#define PTN5150A_INTERRUPT_ORIENTATION_FOUND 0x04
#define PTN5150A_INTERRUPT_DEBUG_ACCESSORY_FOUND 0x02
#define PTN5150A_INTERRUPT_AUDIO_ACCESSORY_FOUND 0x01

enum PTN5150A_RpSelection_DFP {
    PTN5150A_RP_SELECTION_DFP_80uA_DEFAULT = 0x0,
    PTN5150A_RP_SELECTION_DFP_180uA_MEDIUM = 0x1,
    PTN5150A_RP_SELECTION_DFP_330uA_HIGH = 0x2,
};

enum PTN5150A_ModeSelection {
    PTN5150A_MODE_SELECTION_DEVICE_UFP = 0x0,
    PTN5150A_MODE_SELECTION_HOST_DFP = 0x1,
    PTN5150A_MODE_SELECTION_DUAL_ROLE_DRP = 0x2,
};

// NOTE: UFP mode after valid CC detection
enum PTN5150A_CCVBusDetection_UFP {
    PTN5150A_CC_V_BUS_NOT_DETECTED = 0x0,
    PTN5150A_CC_V_BUS_DETECTED = 0x1,
};

// NOTE: In UFP mode
enum PTN5150A_CCRpDetection_UFP {
    PTN5150A_CC_RP_STANDBY = 0x0,
    PTN5150A_CC_RP_STD_USB = 0x1,
    PTN5150A_CC_RP_1_5A = 0x2,
    PTN5150A_CC_RP_3_0A = 0x3,
};

enum PTN5150A_CCPortAttachmentStatus {
    PTN5150A_CC_PORT_NOT_CONNECTED = 0x0,
    PTN5150A_CC_PORT_DFP_ATTACHED = 0x1,
    PTN5150A_CC_PORT_UFP_ATTACHED = 0x2,
    PTN5150A_CC_PORT_ANALOG_AUDIO_ACCESSORY_ATTACHED = 0x3,
    PTN5150A_CC_PORT_DEBUG_ACCESSORY_ATTACHED = 0x4,
};

enum PTN5150A_CCPolarity {
    PTN5150A_CC_POLARITY_CABLE_NOT_ATTACHED = 0x0,
    PTN5150A_CC_POLARITY_CC1 = 0x1,
    PTN5150A_CC_POLARITY_CC2 = 0x2,
};

enum PTN5150A_VConnDetectedStatus {
    PTN5150A_VCONN_STANDBY = 0x0,
    PTN5150A_VCONN_POWER_CC1 = 0x1,
    PTN5150A_VCONN_POWER_CC2 = 0x2,
};

struct PTN5150A_Platform {
    int (*i2cWriteReg)(uint8_t addr7bit, uint8_t regNum, const uint8_t *data, uint8_t length, uint8_t wait);
    int (*i2cReadReg)(uint8_t addr7bit, uint8_t regNum, uint8_t *data, uint8_t length, int timeout);

    uint8_t i2cAddress;
};

bool PTN5150A_ReadID(struct PTN5150A_Platform *platform, uint8_t *deviceVersionId, uint8_t *vendorId);

bool PTN5150A_ReadControl(struct PTN5150A_Platform *platform, enum PTN5150A_RpSelection_DFP *rpSelection,
    enum PTN5150A_ModeSelection *modeSelection, bool *maskInterruptCable);
bool PTN5150A_WriteControl(struct PTN5150A_Platform *platform, enum PTN5150A_RpSelection_DFP rpSelection,
    enum PTN5150A_ModeSelection modeSelection, bool maskInterruptCable);

bool PTN5150A_ReadClearInterruptCableStatus(struct PTN5150A_Platform *platform, uint8_t *interruptCableStatus);

bool PTN5150A_ReadCCStatus(struct PTN5150A_Platform *platform, enum PTN5150A_CCVBusDetection_UFP *vBusDetection,
    enum PTN5150A_CCRpDetection_UFP *rpDetection, enum PTN5150A_CCPortAttachmentStatus *portAttachmentStatus,
    enum PTN5150A_CCPolarity *polarity);

bool PTN5150A_ReadConDetConfiguration(struct PTN5150A_Platform *platform, bool *disableConDetOutput);
bool PTN5150A_WriteConDetConfiguration(struct PTN5150A_Platform *platform, bool disableConDetOutput);

bool PTN5150A_ReadVConnStatus(struct PTN5150A_Platform *platform, enum PTN5150A_VConnDetectedStatus *vConnStatus);

bool PTN5150A_Reset(struct PTN5150A_Platform *platform);

bool PTN5150A_ReadInterruptMask(struct PTN5150A_Platform *platform, uint8_t *interruptMask);
bool PTN5150A_WriteInterruptMask(struct PTN5150A_Platform *platform, uint8_t interruptMask);

bool PTN5150A_ReadClearInterruptStatus(struct PTN5150A_Platform *platform, uint8_t *interruptStatus);

#endif // PTN5150A_H

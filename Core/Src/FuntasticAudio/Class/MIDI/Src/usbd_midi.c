/* Includes ------------------------------------------------------------------*/
#include <usbd_midi.h>
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_MIDI
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_MIDI_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_MIDI_Private_Defines
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_MIDI_Private_Macros
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_MIDI_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_MIDI_Init(USBD_HandleTypeDef *pdev,  uint8_t cfgidx);

static uint8_t  USBD_MIDI_DeInit(USBD_HandleTypeDef *pdev,  uint8_t cfgidx);

static uint8_t  USBD_MIDI_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t  USBD_MIDI_DataIn(USBD_HandleTypeDef *pdev,  uint8_t epnum);

static uint8_t  USBD_MIDI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_MIDI_EP0_RxReady(USBD_HandleTypeDef *pdev);

static uint8_t  *USBD_MIDI_GetFSCfgDesc(uint16_t *length);

static uint8_t  *USBD_MIDI_GetHSCfgDesc(uint16_t *length);

static uint8_t  *USBD_MIDI_GetOtherSpeedCfgDesc(uint16_t *length);

static uint8_t  *USBD_MIDI_GetOtherSpeedCfgDesc(uint16_t *length);

uint8_t  *USBD_MIDI_GetDeviceQualifierDescriptor(uint16_t *length);

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MIDI_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */

/** @defgroup USBD_MIDI_Private_Variables
  * @{
  */


/* MIDI interface class callbacks structure */
USBD_ClassTypeDef  USBD_MIDI =
{
  USBD_MIDI_Init,
  USBD_MIDI_DeInit,
  USBD_MIDI_Setup,
  NULL,                 /* EP0_TxSent, */
  USBD_MIDI_EP0_RxReady,
  USBD_MIDI_DataIn,
  USBD_MIDI_DataOut,
  NULL,
  NULL,
  NULL,
  USBD_MIDI_GetHSCfgDesc,
  USBD_MIDI_GetFSCfgDesc,
  USBD_MIDI_GetOtherSpeedCfgDesc,
  USBD_MIDI_GetDeviceQualifierDescriptor,
};

/* USB MIDI device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_MIDI_CfgHSDesc[USB_AUDIO_CONFIG_DESC_SIZ] __ALIGN_END =
{
	/* Configuration 1 */
	//0x09, 0x02, 0x65, 0x00, 0x02, 0x01, 0x00, 0xc0, 0x50,
	0x09,                                 /* bLength */
	USB_DESC_TYPE_CONFIGURATION,          /* bDescriptorType */
	LOBYTE(USB_MIDI_CONFIG_DESC_SIZ),    /* wTotalLength  109 bytes*/
	HIBYTE(USB_MIDI_CONFIG_DESC_SIZ),
	0x02,                                 /* bNumInterfaces */
	0x01,                                 /* bConfigurationValue */
	0x00,                                 /* iConfiguration */
	0xC0,                                 /* bmAttributes  BUS Powred*/
	0x32,                                 /* bMaxPower = 100 mA*/
	/* 09 byte*/

	/* USB MIDI Standard interface descriptor */
	//0x09, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, // Standard AC Interface Descriptor
	AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
	USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
	0x00,                                 /* bInterfaceNumber */
	0x00,                                 /* bAlternateSetting */
	0x00,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
	AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
	0x00,                                 /* iInterface */
	/* 09 byte*/

	/* USB Speaker Class-specific AC Interface Descriptor */
    //0x09, 0x24, 0x01, 0x00, 0x01, 0x09, 0x00, 0x01, 0x01, // Class-specific AC Interface Descriptor
	AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
	0x00,          /* 1.00 */             /* bcdADC */
	0x01,
	0x09,                                 /* wTotalLength = 9*/
	0x00,
	0x01,                                 /* bInCollection */
	0x01,                                 /* baInterfaceNr */
	/* 09 byte*/
	/* MIDIStreaming Interface Descriptor */
    //0x09, 0x04, 0x01, 0x00, 0x02, 0x01, 0x03, 0x00, 0x00, // MIDIStreaming Interface Descriptors
	MIDI_INPUT_TERMINAL_DESC_SIZE,        /* bLength */
	MIDI_INTERFACE_DESCRIPTOR_TYPE,       /* bDescriptorType */
	0x01,         						  /* bDescriptorSubtype */
	0x00,                                 /* bTerminalID */
	0x02,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
	0x01,
	0x03,                                 /* bAssocTerminal */
	0x00,                                 /* bNrChannels */
	0x00,                                 /* wChannelConfig 0x0000  Mono */
	/* 09 byte*/
	/* Class-Specific MS Interface Unit Descriptor */
    //0x07, 0x24, 0x01, 0x00, 0x01, 0x41, 0x00,             // Class-Specific MS Interface Header Descriptor
	0x07,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	0x01,           /* bDescriptorSubtype */
	0x00,             /* bUnitID */
	0x01,                                 /* bSourceID */
	0x41,                                 /* bControlSize */
	0x00,                                 /* iTerminal */
	/* 07 byte*/
	//0x06, 0x24, 0x02, 0x01, 0x01, 0x00,
	/* MIDI IN 1 Unit Descriptor */
	0x06,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	0x02,           /* bDescriptorSubtype */
	0x01,             /* bUnitID */
	0x01,                                 /* bSourceID */
	0x00,                                 /* iTerminal */
	/* 06 byte*/
	/* MIDI IN 2 Unit Descriptor */
    //0x06, 0x24, 0x02, 0x02, 0x02, 0x00,
	0x06,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	0x02,           /* bDescriptorSubtype */
	0x02,             /* bUnitID */
	0x02,                                 /* bSourceID */
	0x00,                                 /* iTerminal */
	/* 06 byte*/

	/* MIDI OUT 1 Interface Descriptor */
	//0x09, 0x24, 0x03, 0x01, 0x03, 0x01, 0x02, 0x01, 0x00,
	MIDI_INPUT_TERMINAL_DESC_SIZE,        /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,       /* bDescriptorType */
	0x03,         						  /* bDescriptorSubtype */
	0x01,                                 /* bTerminalID */
	0x03,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
	0x01,
	0x02,                                 /* bAssocTerminal */
	0x01,                                 /* bNrChannels */
	0x00,                                 /* iTerminal */
	/* 09 byte*/

	/* MIDI OUT 2 Interface Descriptor */
	//0x09, 0x24, 0x03, 0x02, 0x06, 0x01, 0x01, 0x01, 0x00,
	MIDI_INPUT_TERMINAL_DESC_SIZE,        /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,       /* bDescriptorType */
	0x03,         						  /* bDescriptorSubtype */
	0x02,                                 /* bTerminalID */
	0x06,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
	0x01,
	0x01,                                 /* bAssocTerminal */
	0x01,                                 /* bNrChannels */
	0x00,                                 /* iTerminal */
	/* 09 byte*/

	// OUT endpoint descriptor
	//0x09, 0x05, AUDIO_OUT_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
	/* OUT Endpoint - Standard Descriptor */
	AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
	USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
	MIDI_OUT_EP,                         /* bEndpointAddress 1 out endpoint*/
	USBD_EP_TYPE_BULK,                    /* bmAttributes */
	0x40,								  /* wMaxPacketSize in Bytes */
	0x00,                                 /* bInterval */
	0x00,                                 /* bRefresh */
	0x00,                                 /* bSynchAddress */
	0x00,                                 /* iTerminal */
	/* 09 byte*/
	0x05, 0x25, 0x01, 0x01, 0x01,

	/* IN Endpoint - Standard Descriptor */
    //0x09, 0x05, AUDIO_OUT_EP+1, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
	AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
	USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
	MIDI_IN_EP,                   		  /* bEndpointAddress 1 out endpoint*/
	USBD_EP_TYPE_BULK,                    /* bmAttributes */
	0x40,								  /* wMaxPacketSize in Bytes */
	0x00,                                 /* bInterval */
	0x00,                                 /* bRefresh */
	0x00,                                 /* bSynchAddress */
	0x00,                                 /* iTerminal */
	/* 09 byte*/
	0x05, 0x25, 0x01, 0x01, 0x03,
};


/* USB MIDI device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_MIDI_CfgFSDesc[USB_AUDIO_CONFIG_DESC_SIZ] __ALIGN_END =
{
	/* Configuration 1 */
	//0x09, 0x02, 0x65, 0x00, 0x02, 0x01, 0x00, 0xc0, 0x50,
	0x09,                                 /* bLength */
	USB_DESC_TYPE_CONFIGURATION,          /* bDescriptorType */
	LOBYTE(USB_MIDI_CONFIG_DESC_SIZ),    /* wTotalLength  109 bytes*/
	HIBYTE(USB_MIDI_CONFIG_DESC_SIZ),
	0x02,                                 /* bNumInterfaces */
	0x01,                                 /* bConfigurationValue */
	0x00,                                 /* iConfiguration */
	0xC0,                                 /* bmAttributes  BUS Powred*/
	0x32,                                 /* bMaxPower = 100 mA*/
	/* 09 byte*/

	/* USB MIDI Standard interface descriptor */
	//0x09, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, // Standard AC Interface Descriptor
	AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
	USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
	0x00,                                 /* bInterfaceNumber */
	0x00,                                 /* bAlternateSetting */
	0x00,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
	AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
	0x00,                                 /* iInterface */
	/* 09 byte*/

	/* USB Speaker Class-specific AC Interface Descriptor */
    //0x09, 0x24, 0x01, 0x00, 0x01, 0x09, 0x00, 0x01, 0x01, // Class-specific AC Interface Descriptor
	AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
	0x00,          /* 1.00 */             /* bcdADC */
	0x01,
	0x09,                                 /* wTotalLength = 9*/
	0x00,
	0x01,                                 /* bInCollection */
	0x01,                                 /* baInterfaceNr */
	/* 09 byte*/
	/* MIDIStreaming Interface Descriptor */
    //0x09, 0x04, 0x01, 0x00, 0x02, 0x01, 0x03, 0x00, 0x00, // MIDIStreaming Interface Descriptors
	MIDI_INPUT_TERMINAL_DESC_SIZE,        /* bLength */
	MIDI_INTERFACE_DESCRIPTOR_TYPE,       /* bDescriptorType */
	0x01,         						  /* bDescriptorSubtype */
	0x00,                                 /* bTerminalID */
	0x02,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
	0x01,
	0x03,                                 /* bAssocTerminal */
	0x00,                                 /* bNrChannels */
	0x00,                                 /* wChannelConfig 0x0000  Mono */
	/* 09 byte*/
	/* Class-Specific MS Interface Unit Descriptor */
    //0x07, 0x24, 0x01, 0x00, 0x01, 0x41, 0x00,             // Class-Specific MS Interface Header Descriptor
	0x07,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	0x01,           /* bDescriptorSubtype */
	0x00,             /* bUnitID */
	0x01,                                 /* bSourceID */
	0x41,                                 /* bControlSize */
	0x00,                                 /* iTerminal */
	/* 07 byte*/
	//0x06, 0x24, 0x02, 0x01, 0x01, 0x00,
	/* MIDI IN 1 Unit Descriptor */
	0x06,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	0x02,           /* bDescriptorSubtype */
	0x01,             /* bUnitID */
	0x01,                                 /* bSourceID */
	0x00,                                 /* iTerminal */
	/* 06 byte*/
	/* MIDI IN 2 Unit Descriptor */
    //0x06, 0x24, 0x02, 0x02, 0x02, 0x00,
	0x06,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	0x02,           /* bDescriptorSubtype */
	0x02,             /* bUnitID */
	0x02,                                 /* bSourceID */
	0x00,                                 /* iTerminal */
	/* 06 byte*/

	/* MIDI OUT 1 Interface Descriptor */
	//0x09, 0x24, 0x03, 0x01, 0x03, 0x01, 0x02, 0x01, 0x00,
	MIDI_INPUT_TERMINAL_DESC_SIZE,        /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,       /* bDescriptorType */
	0x03,         						  /* bDescriptorSubtype */
	0x01,                                 /* bTerminalID */
	0x03,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
	0x01,
	0x02,                                 /* bAssocTerminal */
	0x01,                                 /* bNrChannels */
	0x00,                                 /* iTerminal */
	/* 09 byte*/

	/* MIDI OUT 2 Interface Descriptor */
	//0x09, 0x24, 0x03, 0x02, 0x06, 0x01, 0x01, 0x01, 0x00,
	MIDI_INPUT_TERMINAL_DESC_SIZE,        /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,       /* bDescriptorType */
	0x03,         						  /* bDescriptorSubtype */
	0x02,                                 /* bTerminalID */
	0x06,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
	0x01,
	0x01,                                 /* bAssocTerminal */
	0x01,                                 /* bNrChannels */
	0x00,                                 /* iTerminal */
	/* 09 byte*/

	// OUT endpoint descriptor
	//0x09, 0x05, AUDIO_OUT_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
	/* OUT Endpoint - Standard Descriptor */
	AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
	USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
	MIDI_OUT_EP,                         /* bEndpointAddress 1 out endpoint*/
	USBD_EP_TYPE_BULK,                    /* bmAttributes */
	0x40,								  /* wMaxPacketSize in Bytes */
	0x00,                                 /* bInterval */
	0x00,                                 /* bRefresh */
	0x00,                                 /* bSynchAddress */
	0x00,                                 /* iTerminal */
	/* 09 byte*/
	0x05, 0x25, 0x01, 0x01, 0x01,

	/* IN Endpoint - Standard Descriptor */
    //0x09, 0x05, AUDIO_OUT_EP+1, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
	AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
	USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
	MIDI_IN_EP,                   		  /* bEndpointAddress 1 out endpoint*/
	USBD_EP_TYPE_BULK,                    /* bmAttributes */
	0x40,								  /* wMaxPacketSize in Bytes */
	0x00,                                 /* bInterval */
	0x00,                                 /* bRefresh */
	0x00,                                 /* bSynchAddress */
	0x00,                                 /* iTerminal */
	/* 09 byte*/
	0x05, 0x25, 0x01, 0x01, 0x03,
};

__ALIGN_BEGIN uint8_t USBD_MIDI_OtherSpeedCfgDesc[USB_AUDIO_CONFIG_DESC_SIZ] __ALIGN_END =
{
	/* Configuration 1 */
	//0x09, 0x02, 0x65, 0x00, 0x02, 0x01, 0x00, 0xc0, 0x50,
	0x09,                                 /* bLength */
	USB_DESC_TYPE_CONFIGURATION,          /* bDescriptorType */
	LOBYTE(USB_MIDI_CONFIG_DESC_SIZ),    /* wTotalLength  109 bytes*/
	HIBYTE(USB_MIDI_CONFIG_DESC_SIZ),
	0x02,                                 /* bNumInterfaces */
	0x01,                                 /* bConfigurationValue */
	0x00,                                 /* iConfiguration */
	0xC0,                                 /* bmAttributes  BUS Powred*/
	0x32,                                 /* bMaxPower = 100 mA*/
	/* 09 byte*/

	/* USB MIDI Standard interface descriptor */
	//0x09, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, // Standard AC Interface Descriptor
	AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
	USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
	0x00,                                 /* bInterfaceNumber */
	0x00,                                 /* bAlternateSetting */
	0x00,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
	AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
	0x00,                                 /* iInterface */
	/* 09 byte*/

	/* USB Speaker Class-specific AC Interface Descriptor */
    //0x09, 0x24, 0x01, 0x00, 0x01, 0x09, 0x00, 0x01, 0x01, // Class-specific AC Interface Descriptor
	AUDIO_INTERFACE_DESC_SIZE,            /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
	0x00,          /* 1.00 */             /* bcdADC */
	0x01,
	0x09,                                 /* wTotalLength = 9*/
	0x00,
	0x01,                                 /* bInCollection */
	0x01,                                 /* baInterfaceNr */
	/* 09 byte*/
	/* MIDIStreaming Interface Descriptor */
    //0x09, 0x04, 0x01, 0x00, 0x02, 0x01, 0x03, 0x00, 0x00, // MIDIStreaming Interface Descriptors
	MIDI_INPUT_TERMINAL_DESC_SIZE,        /* bLength */
	MIDI_INTERFACE_DESCRIPTOR_TYPE,       /* bDescriptorType */
	0x01,         						  /* bDescriptorSubtype */
	0x00,                                 /* bTerminalID */
	0x02,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
	0x01,
	0x03,                                 /* bAssocTerminal */
	0x00,                                 /* bNrChannels */
	0x00,                                 /* wChannelConfig 0x0000  Mono */
	/* 09 byte*/
	/* Class-Specific MS Interface Unit Descriptor */
    //0x07, 0x24, 0x01, 0x00, 0x01, 0x41, 0x00,             // Class-Specific MS Interface Header Descriptor
	0x07,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	0x01,           /* bDescriptorSubtype */
	0x00,             /* bUnitID */
	0x01,                                 /* bSourceID */
	0x41,                                 /* bControlSize */
	0x00,                                 /* iTerminal */
	/* 07 byte*/
	//0x06, 0x24, 0x02, 0x01, 0x01, 0x00,
	/* MIDI IN 1 Unit Descriptor */
	0x06,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	0x02,           /* bDescriptorSubtype */
	0x01,             /* bUnitID */
	0x01,                                 /* bSourceID */
	0x00,                                 /* iTerminal */
	/* 06 byte*/
	/* MIDI IN 2 Unit Descriptor */
    //0x06, 0x24, 0x02, 0x02, 0x02, 0x00,
	0x06,                                 /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
	0x02,           /* bDescriptorSubtype */
	0x02,             /* bUnitID */
	0x02,                                 /* bSourceID */
	0x00,                                 /* iTerminal */
	/* 06 byte*/

	/* MIDI OUT 1 Interface Descriptor */
	//0x09, 0x24, 0x03, 0x01, 0x03, 0x01, 0x02, 0x01, 0x00,
	MIDI_INPUT_TERMINAL_DESC_SIZE,        /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,       /* bDescriptorType */
	0x03,         						  /* bDescriptorSubtype */
	0x01,                                 /* bTerminalID */
	0x03,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
	0x01,
	0x02,                                 /* bAssocTerminal */
	0x01,                                 /* bNrChannels */
	0x00,                                 /* iTerminal */
	/* 09 byte*/

	/* MIDI OUT 2 Interface Descriptor */
	//0x09, 0x24, 0x03, 0x02, 0x06, 0x01, 0x01, 0x01, 0x00,
	MIDI_INPUT_TERMINAL_DESC_SIZE,        /* bLength */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE,       /* bDescriptorType */
	0x03,         						  /* bDescriptorSubtype */
	0x02,                                 /* bTerminalID */
	0x06,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */
	0x01,
	0x01,                                 /* bAssocTerminal */
	0x01,                                 /* bNrChannels */
	0x00,                                 /* iTerminal */
	/* 09 byte*/

	// OUT endpoint descriptor
	//0x09, 0x05, AUDIO_OUT_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
	/* OUT Endpoint - Standard Descriptor */
	AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
	USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
	MIDI_OUT_EP,                         /* bEndpointAddress 1 out endpoint*/
	USBD_EP_TYPE_BULK,                    /* bmAttributes */
	0x40,								  /* wMaxPacketSize in Bytes */
	0x00,                                 /* bInterval */
	0x00,                                 /* bRefresh */
	0x00,                                 /* bSynchAddress */
	0x00,                                 /* iTerminal */
	/* 09 byte*/
	0x05, 0x25, 0x01, 0x01, 0x01,

	/* IN Endpoint - Standard Descriptor */
    //0x09, 0x05, AUDIO_OUT_EP+1, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
	AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
	USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
	MIDI_IN_EP,                   		  /* bEndpointAddress 1 out endpoint*/
	USBD_EP_TYPE_BULK,                    /* bmAttributes */
	0x40,								  /* wMaxPacketSize in Bytes */
	0x00,                                 /* bInterval */
	0x00,                                 /* bRefresh */
	0x00,                                 /* bSynchAddress */
	0x00,                                 /* iTerminal */
	/* 09 byte*/
	0x05, 0x25, 0x01, 0x01, 0x03,
};

/**
  * @brief  USBD_MIDI_Init
  *         Initialize the MIDI interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_MIDI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = 0U;
  USBD_MIDI_HandleTypeDef   *hmidi;

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Open EP IN */
    USBD_LL_OpenEP(pdev, MIDI_IN_EP, USBD_EP_TYPE_BULK,
                   MIDI_DATA_HS_IN_PACKET_SIZE);

    pdev->ep_in[MIDI_IN_EP & 0xFU].is_used = 1U;

    /* Open EP OUT */
    USBD_LL_OpenEP(pdev, MIDI_OUT_EP, USBD_EP_TYPE_BULK,
                   MIDI_DATA_HS_OUT_PACKET_SIZE);

    pdev->ep_out[MIDI_OUT_EP & 0xFU].is_used = 1U;

  }
  else
  {
    /* Open EP IN */
    USBD_LL_OpenEP(pdev, MIDI_IN_EP, USBD_EP_TYPE_BULK,
                   MIDI_DATA_FS_IN_PACKET_SIZE);

    pdev->ep_in[MIDI_IN_EP & 0xFU].is_used = 1U;

    /* Open EP OUT */
    USBD_LL_OpenEP(pdev, MIDI_OUT_EP, USBD_EP_TYPE_BULK,
                   MIDI_DATA_FS_OUT_PACKET_SIZE);

    pdev->ep_out[MIDI_OUT_EP & 0xFU].is_used = 1U;
  }
  /* Open Command IN EP */
  USBD_LL_OpenEP(pdev, MIDI_CMD_EP, USBD_EP_TYPE_INTR, MIDI_CMD_PACKET_SIZE);
  pdev->ep_in[MIDI_CMD_EP & 0xFU].is_used = 1U;

  pdev->pClassData = USBD_malloc(sizeof(USBD_MIDI_HandleTypeDef));

  if (pdev->pClassData == NULL)
  {
    ret = 1U;
  }
  else
  {
    hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;

    /* Init  physical Interface components */
    ((USBD_MIDI_ItfTypeDef *)pdev->pUserData)->Init();

    /* Init Xfer states */
    hmidi->TxState = 0U;
    hmidi->RxState = 0U;

    if (pdev->dev_speed == USBD_SPEED_HIGH)
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev, MIDI_OUT_EP, hmidi->RxBuffer,
                             MIDI_DATA_HS_OUT_PACKET_SIZE);
    }
    else
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev, MIDI_OUT_EP, hmidi->RxBuffer,
                             MIDI_DATA_FS_OUT_PACKET_SIZE);
    }
  }
  return ret;
}

/**
  * @brief  USBD_MIDI_Init
  *         DeInitialize the MIDI layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_MIDI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = 0U;

  /* Close EP IN */
  USBD_LL_CloseEP(pdev, MIDI_IN_EP);
  pdev->ep_in[MIDI_IN_EP & 0xFU].is_used = 0U;

  /* Close EP OUT */
  USBD_LL_CloseEP(pdev, MIDI_OUT_EP);
  pdev->ep_out[MIDI_OUT_EP & 0xFU].is_used = 0U;

  /* Close Command IN EP */
  USBD_LL_CloseEP(pdev, MIDI_CMD_EP);
  pdev->ep_in[MIDI_CMD_EP & 0xFU].is_used = 0U;

  /* DeInit  physical Interface components */
  if (pdev->pClassData != NULL)
  {
    ((USBD_MIDI_ItfTypeDef *)pdev->pUserData)->DeInit();
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }

  return ret;
}

/**
  * @brief  USBD_MIDI_Setup
  *         Handle the MIDI specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_MIDI_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req)
{
  USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;
  uint8_t ifalt = 0U;
  uint16_t status_info = 0U;
  uint8_t ret = USBD_OK;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS :
      if (req->wLength)
      {
        if (req->bmRequest & 0x80U)
        {
          ((USBD_MIDI_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
                                                            (uint8_t *)(void *)hmidi->data,
                                                            req->wLength);

          USBD_CtlSendData(pdev, (uint8_t *)(void *)hmidi->data, req->wLength);
        }
        else
        {
          hmidi->CmdOpCode = req->bRequest;
          hmidi->CmdLength = (uint8_t)req->wLength;

          USBD_CtlPrepareRx(pdev, (uint8_t *)(void *)hmidi->data, req->wLength);
        }
      }
      else
      {
        ((USBD_MIDI_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
                                                          (uint8_t *)(void *)req, 0U);
      }
      break;

    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            USBD_CtlSendData(pdev, (uint8_t *)(void *)&status_info, 2U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            USBD_CtlSendData(pdev, &ifalt, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE:
          if (pdev->dev_state != USBD_STATE_CONFIGURED)
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  return ret;
}

/**
  * @brief  USBD_MIDI_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_MIDI_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_MIDI_HandleTypeDef *hmidi = (USBD_MIDI_HandleTypeDef *)pdev->pClassData;
  PCD_HandleTypeDef *hpcd = pdev->pData;

  if (pdev->pClassData != NULL)
  {
    if ((pdev->ep_in[epnum].total_length > 0U) && ((pdev->ep_in[epnum].total_length % hpcd->IN_ep[epnum].maxpacket) == 0U))
    {
      /* Update the packet total length */
      pdev->ep_in[epnum].total_length = 0U;

      /* Send ZLP */
      USBD_LL_Transmit(pdev, epnum, NULL, 0U);
    }
    else
    {
      hmidi->TxState = 0U;
    }
    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}

/**
  * @brief  USBD_MIDI_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  USBD_MIDI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;

  /* Get the received data length */
  hmidi->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

  /* USB data will be immediately processed, this allow next USB traffic being
  NAKed till the end of the application Xfer */
  if (pdev->pClassData != NULL)
  {
    ((USBD_MIDI_ItfTypeDef *)pdev->pUserData)->Receive(hmidi->RxBuffer, &hmidi->RxLength);

    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}

/**
  * @brief  USBD_MIDI_EP0_RxReady
  *         Handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_MIDI_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;

  if ((pdev->pUserData != NULL) && (hmidi->CmdOpCode != 0xFFU))
  {
    ((USBD_MIDI_ItfTypeDef *)pdev->pUserData)->Control(hmidi->CmdOpCode,
                                                      (uint8_t *)(void *)hmidi->data,
                                                      (uint16_t)hmidi->CmdLength);
    hmidi->CmdOpCode = 0xFFU;

  }
  return USBD_OK;
}

/**
  * @brief  USBD_MIDI_GetFSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_MIDI_GetFSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_MIDI_CfgFSDesc);
  return USBD_MIDI_CfgFSDesc;
}

/**
  * @brief  USBD_MIDI_GetHSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_MIDI_GetHSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_MIDI_CfgHSDesc);
  return USBD_MIDI_CfgHSDesc;
}

/**
  * @brief  USBD_MIDI_GetCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_MIDI_GetOtherSpeedCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_MIDI_OtherSpeedCfgDesc);
  return USBD_MIDI_OtherSpeedCfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_MIDI_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = sizeof(USBD_MIDI_DeviceQualifierDesc);
  return USBD_MIDI_DeviceQualifierDesc;
}

/**
* @brief  USBD_MIDI_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CD  Interface callback
  * @retval status
  */
uint8_t  USBD_MIDI_RegisterInterface(USBD_HandleTypeDef   *pdev,
                                    USBD_MIDI_ItfTypeDef *fops)
{
  uint8_t  ret = USBD_FAIL;

  if (fops != NULL)
  {
    pdev->pUserData = fops;
    ret = USBD_OK;
  }

  return ret;
}

/**
  * @brief  USBD_MIDI_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
uint8_t  USBD_MIDI_SetTxBuffer(USBD_HandleTypeDef   *pdev,
                              uint8_t  *pbuff,
                              uint16_t length)
{
  USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;

  hmidi->TxBuffer = pbuff;
  hmidi->TxLength = length;

  return USBD_OK;
}


/**
  * @brief  USBD_MIDI_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
uint8_t  USBD_MIDI_SetRxBuffer(USBD_HandleTypeDef   *pdev,
                              uint8_t  *pbuff)
{
  USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;

  hmidi->RxBuffer = pbuff;

  return USBD_OK;
}

/**
  * @brief  USBD_MIDI_TransmitPacket
  *         Transmit packet on IN endpoint
  * @param  pdev: device instance
  * @retval status
  */
uint8_t  USBD_MIDI_TransmitPacket(USBD_HandleTypeDef *pdev)
{
  USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;

  if (pdev->pClassData != NULL)
  {
    if (hmidi->TxState == 0U)
    {
      /* Tx Transfer in progress */
      hmidi->TxState = 1U;

      /* Update the packet total length */
      pdev->ep_in[MIDI_IN_EP & 0xFU].total_length = hmidi->TxLength;

      /* Transmit next packet */
      USBD_LL_Transmit(pdev, MIDI_IN_EP, hmidi->TxBuffer,
                       (uint16_t)hmidi->TxLength);

      return USBD_OK;
    }
    else
    {
      return USBD_BUSY;
    }
  }
  else
  {
    return USBD_FAIL;
  }
}


/**
  * @brief  USBD_MIDI_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t  USBD_MIDI_ReceivePacket(USBD_HandleTypeDef *pdev)
{
  USBD_MIDI_HandleTypeDef   *hmidi = (USBD_MIDI_HandleTypeDef *) pdev->pClassData;

  /* Suspend or Resume USB Out process */
  if (pdev->pClassData != NULL)
  {
    if (pdev->dev_speed == USBD_SPEED_HIGH)
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev,
                             MIDI_OUT_EP,
                             hmidi->RxBuffer,
                             MIDI_DATA_HS_OUT_PACKET_SIZE);
    }
    else
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev,
                             MIDI_OUT_EP,
                             hmidi->RxBuffer,
                             MIDI_DATA_FS_OUT_PACKET_SIZE);
    }
    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

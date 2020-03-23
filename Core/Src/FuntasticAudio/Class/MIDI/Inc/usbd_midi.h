/**
  ******************************************************************************
  * @file    usbd_cdc.h
  * @author  MCD Application Team
  * @brief   header file for the usbd_cdc.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_MIDI_H
#define __USB_MIDI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup usbd_cdc
  * @brief This file is the Header file for usbd_cdc.c
  * @{
  */


/** @defgroup usbd_cdc_Exported_Defines
  * @{
  */
#define USB_AUDIO_CONFIG_DESC_SIZ                     0x6DU
#define USB_MIDI_CONFIG_DESC_SIZ                      0x65U
#define AUDIO_INTERFACE_DESC_SIZE                     0x09U
#define AUDIO_DESCRIPTOR_TYPE                         0x21U
#define USB_DEVICE_CLASS_AUDIO                        0x01U
#define AUDIO_SUBCLASS_AUDIOCONTROL                   0x01U
#define AUDIO_PROTOCOL_UNDEFINED                      0x00U
#define MIDI_INTERFACE_DESCRIPTOR_TYPE                0x04U
#define AUDIO_CONTROL_HEADER                          0x01U
#define MIDI_INPUT_TERMINAL_DESC_SIZE                 0x09U
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE               0x24U
#define MIDI_INTERFACE_DESCRIPTOR_TYPE                0x04U
#define AUDIO_STANDARD_ENDPOINT_DESC_SIZE             0x09U
#define MIDI_OUT_EP									  0x01U
#define MIDI_IN_EP									  0x81U

#define MIDI_IN_EP                                   0x81U  /* EP1 for data IN */
#define MIDI_OUT_EP                                  0x01U  /* EP1 for data OUT */
#define MIDI_CMD_EP                                  0x82U  /* EP2 for MIDI commands */

#ifndef MIDI_HS_BINTERVAL
#define MIDI_HS_BINTERVAL                          0x10U
#endif /* MIDI_HS_BINTERVAL */

#ifndef MIDI_FS_BINTERVAL
#define MIDI_FS_BINTERVAL                          0x10U
#endif /* MIDI_FS_BINTERVAL */

/* MIDI Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
#define MIDI_DATA_HS_MAX_PACKET_SIZE                 512U  /* Endpoint IN & OUT Packet size */
#define MIDI_DATA_FS_MAX_PACKET_SIZE                 64U  /* Endpoint IN & OUT Packet size */
#define MIDI_CMD_PACKET_SIZE                         8U  /* Control Endpoint Packet size */

#define MIDI_DATA_HS_IN_PACKET_SIZE                  MIDI_DATA_HS_MAX_PACKET_SIZE
#define MIDI_DATA_HS_OUT_PACKET_SIZE                 MIDI_DATA_HS_MAX_PACKET_SIZE

#define MIDI_DATA_FS_IN_PACKET_SIZE                  MIDI_DATA_FS_MAX_PACKET_SIZE
#define MIDI_DATA_FS_OUT_PACKET_SIZE                 MIDI_DATA_FS_MAX_PACKET_SIZE


typedef struct _USBD_MIDI_Itf
{
  int8_t (* Init)(void);
  int8_t (* DeInit)(void);
  int8_t (* Control)(uint8_t cmd, uint8_t *pbuf, uint16_t length);
  int8_t (* Receive)(uint8_t *Buf, uint32_t *Len);

} USBD_MIDI_ItfTypeDef;


typedef struct
{
  uint32_t data[MIDI_DATA_HS_MAX_PACKET_SIZE / 4U];      /* Force 32bits alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;
  uint8_t  *RxBuffer;
  uint8_t  *TxBuffer;
  uint32_t RxLength;
  uint32_t TxLength;

  __IO uint32_t TxState;
  __IO uint32_t RxState;
}
USBD_MIDI_HandleTypeDef;



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef  USBD_MIDI;
#define USBD_MIDI_CLASS    &USBD_MIDI
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
uint8_t  USBD_MIDI_RegisterInterface(USBD_HandleTypeDef   *pdev,
                                    USBD_MIDI_ItfTypeDef *fops);

uint8_t  USBD_MIDI_SetTxBuffer(USBD_HandleTypeDef   *pdev,
                              uint8_t  *pbuff,
                              uint16_t length);

uint8_t  USBD_MIDI_SetRxBuffer(USBD_HandleTypeDef   *pdev,
                              uint8_t  *pbuff);

uint8_t  USBD_MIDI_ReceivePacket(USBD_HandleTypeDef *pdev);

uint8_t  USBD_MIDI_TransmitPacket(USBD_HandleTypeDef *pdev);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_MIDI_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/*!
    \file    usbd_conf.h
    \brief   usb device driver basic configuration

    \version 2024-12-20, V2.5.0, firmware for GD32F10x
*/

#ifndef USBD_CONF_H
#define USBD_CONF_H

#include "gd32f10x.h"


#define USBD_CFG_MAX_NUM                   1U
#define USBD_ITF_MAX_NUM                   1U

//共4个通信端点(含端点0)
#define EP_COUNT                           (4)
/* USB user string supported */
//#define USB_SUPPORT_USER_STRING_DESC

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                        (0x40U)
#define EP0_TX_ADDR                        (0x80U)
#define INT_IN_ADDR                        (0xC0U)
#define BULK_RX_ADDR                       (0x100U)
#define BULK_TX_ADDR                       (0x140U)


/* base address offset of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                      (0x0000U)



#endif /* USBD_CONF_H */

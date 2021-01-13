/*---------------------------------------------------------------------------
 *
 * Copyright (c) 2016, congatec AG. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the BSD 2-clause license which 
 * accompanies this distribution. 
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the BSD 2-clause license for more details.
 *
 * The full text of the license may be found at:        
 * http://opensource.org/licenses/BSD-2-Clause   
 *
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 *
 * $Header:   S:/CG/archives/CGTOOLS/INC/CGBC.H-arc   1.28   Sep 06 2016 15:45:56   congatec  $
 *
 * Contents: Board controller common definitions.
 *
 *---------------------------------------------------------------------------
 *
 *                      R E V I S I O N   H I S T O R Y
 *
 * $Log:   S:/CG/archives/CGTOOLS/INC/CGBC.H-arc  $
 * 
 *    Rev 1.28   Sep 06 2016 15:45:56   congatec
 * Added BSD header.
 * 
 *    Rev 1.27   Jun 03 2016 11:17:24   ess
 * MOD041: added new host commands for second LFP control, diagnostic output
 *         interface configuration and GPI IRQ support
 * 
 *    Rev 1.26   Apr 05 2016 16:55:28   ess
 * MOD040: update for latest BC functionality including POST code redirection,
 *          LFP backlight delay and HWM configuration
 * 
 *    Rev 1.25   Feb 26 2014 10:20:44   ess
 * MOD039: more changes concerning 64bit porting
 * MOD038: added support for new commands CGBC_CMD_AVR_SPM,
 *         CGBC_CMD_DEVICE_ID and CGBC_CMD_HWM_SENSOR
 * 
 *    Rev 1.24   Nov 05 2013 16:38:54   ess
 * MOD037: HIF_ID corrected
 * MOD036: added CGBC_CMD_BLT_PWM
 * 
 *    Rev 1.23   Oct 18 2013 12:06:32   ess
 * MOD035: added conditional #defines for uint8_t, uint16_t and uint32_t in
 *         case they are not defined yet
 * MOD034: added support for battery systems on SMBus
 * MOD033: added more SMBus numbers
 * MOD032: added error code CGBC_ERR_ABORT
 * 
 *    Rev 1.22   Aug 19 2013 23:14:56   ess
 * MOD031: added CGBC_CMD_COMX_CFG
 * 
 *    Rev 1.21   Jun 26 2013 10:51:08   ess
 * MOD030: added support for TM4E1231H6ZRB
 * MOD029: added CGBC_CMD_AVR_SPM
 * 
 *    Rev 1.20   Mar 25 2013 14:49:34   ess
 * MOD028: added conditional #defines for UINT8 and UINT32 in case they are
 *         not defined yet
 * MOD027: added CM41 BC family declarations
 * 
 *    Rev 1.19   Jun 27 2012 11:06:26   ess
 * MOD026: more changes concerning 64bit porting
 * MOD025: added support for system configuration pins
 * MOD024: added support for SMBus gate
 * 
 *    Rev 1.18   May 31 2012 15:45:34   gartner
 * Updated variable definition to ease 64bit porting.
 * 
 *    Rev 1.16   Mar 21 2011 09:25:54   ess
 * MOD023: added PECI support
 * MOD022: added RTC support
 * 
 *    Rev 1.15   Dec 02 2010 11:32:04   ess
 * MOD021: completed board controller command parameter union
 * MOD020: added fan control support
 * MOD019: added support for hot S5
 * MOD018: added support for STM32F100R8
 * 
 *    Rev 1.14   Jul 22 2009 16:33:52   ess
 * MOD017: update according to the latest CGBC functionality including
 *         button control support
 * 
 *    Rev 1.13   May 12 2009 09:29:42   ess
 * MOD016: added GPIO commands
 * 
 *    Rev 1.12   Dec 18 2008 15:46:34   ess
 * MOD015: added support for ATmega325P
 * 
 *    Rev 1.11   Aug 26 2008 21:56:00   ess
 * MOD014: update according to the latest CGBC functionality including
 *           support for ATmega165P
 * MOD013: changes to allow compilation in the AVR GCC environment
 * 
 *    Rev 1.10   Aug 07 2007 11:21:24   ess
 * MOD012: added feature byte 1 and the SBSM emulation flag
 * 
 *    Rev 1.9   Apr 13 2007 15:55:58   ess
 * MOD011: added RTC_PRESENT bit in power loss parameter
 * MOD010: added support for CGBC_CMD_SCI_STOP command
 * 
 *    Rev 1.8   Oct 30 2006 21:54:10   ess
 * MOD009: added declaration for CGBC_CMD_CPU_TEMP
 * 
 *    Rev 1.7   Mar 02 2006 20:52:40   ess
 * MOD008: changed watchdog parameter declarations from integer to short to
 *         avoid wrong structure in WIN32 environment
 * 
 *    Rev 1.6   Mar 01 2006 19:18:08   ess
 * MOD007: further corrections on battery management declarations
 * 
 *    Rev 1.5   Jan 19 2006 15:06:02   ess
 * MOD006: corrected declaration of battery flags
 * 
 *    Rev 1.4   Jan 03 2006 17:20:30   ess
 * MOD005: declarations updated for latest board controller enhancements
 * 
 *    Rev 1.3   Dec 23 2005 01:57:20   ess
 * MOD004: declarations updated for latest board controller enhancements
 * 
 *    Rev 1.2   Nov 25 2005 18:30:16   ess
 * MOD003: declarations updated for latest board controller enhancements
 * 
 *    Rev 1.1   Oct 20 2005 11:26:32   ess
 * MOD002: updates from latest board controller firmware revision CGBCP121
 * MOD001: added pragma pack(1) to ensure correct structure offsets
 * 
 *    Rev 1.0   Sep 30 2005 16:29:08   gartner
 * Initial revision.
 *
 *---------------------------------------------------------------------------
 */



#ifndef CGBC_H_INCLUDED
#define CGBC_H_INCLUDED                 /* This avoids multiple inclusions. */



#ifndef __AVR__                                                      //MOD013
#pragma pack(1)                                                      //MOD001
#endif //!__AVR__                                                    //MOD013
                                                                   /*MOD039v*/
#ifndef  SINT16
#define  SINT16 signed short
#endif /*SINT16*/
                                                                   /*MOD039^*/
                                                                   /*MOD026v*/
#ifndef  UINT16
#define  UINT16 unsigned short
#endif /*UINT16*/
                                                                   /*MOD026^*/
                                                                   /*MOD028v*/
#ifndef  UINT8
#define  UINT8 unsigned char
#endif /*UINT8*/

#ifndef  UINT32
#define  UINT32 unsigned long
#endif /*UINT32*/
                                                                   /*MOD028^*/
                                                                   /*MOD035v*/
#ifndef  uint8_t
#define  uint8_t unsigned char
#endif /*uint8_t*/

#ifndef  uint16_t
#define  uint16_t unsigned short
#endif /*uint16_t*/

#ifndef  uint32_t
#define  uint32_t unsigned long
#endif /*uint32_t*/
                                                                   /*MOD035^*/


/*--------------
 * SPI commands
 *--------------
 */

#define CGBC_CMD_GET_STATUS    0x00
#define CGBC_CMD_GET_DATA_1    0x01
#define CGBC_CMD_GET_DATA_2    0x02
#define CGBC_CMD_GET_DATA_3    0x03
#define CGBC_CMD_GET_DATA_4    0x04
#define CGBC_CMD_GET_DATA_5    0x05
#define CGBC_CMD_GET_DATA_6    0x06
#define CGBC_CMD_GET_DATA_7    0x07
#define CGBC_CMD_GET_DATA_8    0x08
#define CGBC_CMD_GET_DATA_9    0x09
#define CGBC_CMD_GET_DATA_10   0x0A
#define CGBC_CMD_GET_DATA_11   0x0B
#define CGBC_CMD_GET_DATA_12   0x0C
#define CGBC_CMD_GET_DATA_13   0x0D
#define CGBC_CMD_GET_DATA_14   0x0E
#define CGBC_CMD_GET_DATA_15   0x0F
#define CGBC_CMD_GET_DATA_16   0x10
#define CGBC_CMD_GET_DATA_17   0x11
#define CGBC_CMD_GET_DATA_18   0x12
#define CGBC_CMD_GET_DATA_19   0x13
#define CGBC_CMD_GET_DATA_20   0x14
#define CGBC_CMD_GET_DATA_21   0x15
#define CGBC_CMD_GET_DATA_22   0x16
#define CGBC_CMD_GET_DATA_23   0x17
#define CGBC_CMD_GET_DATA_24   0x18
#define CGBC_CMD_GET_DATA_25   0x19
#define CGBC_CMD_GET_DATA_26   0x1A
#define CGBC_CMD_GET_DATA_27   0x1B
#define CGBC_CMD_GET_DATA_28   0x1C
#define CGBC_CMD_GET_DATA_29   0x1D
#define CGBC_CMD_GET_DATA_30   0x1E
#define CGBC_CMD_GET_DATA_31   0x1F
#define CGBC_CMD_GET_DATA_32   0x20
#define CGBC_CMD_GET_FW_REV    0x21
#define CGBC_CMD_AUX_INIT      0x22
#define CGBC_CMD_AUX_OUT       0x23
#define CGBC_CMD_AUX_IN        0x24
#define CGBC_CMD_AUX_STAT      0x25
#define CGBC_CMD_WD_EVENT      0x26
#define CGBC_CMD_WD_TRIGGER    0x27
#define CGBC_CMD_WD_INIT       0x28
#define CGBC_CMD_WD_INIT_POST  0x29
#define CGBC_CMD_WD_INIT_EARLY 0x2A
#define CGBC_CMD_SYS_FLAGS_1   0x2B
#define CGBC_CMD_SCI_EVT_QUERY 0x2C
#define CGBC_CMD_STACK_INFO    0x2D
#define CGBC_CMD_INFO_1        0x2E
#define CGBC_CMD_LFP_CONTROL   0x2F                                  //MOD040
#define CGBC_CMD_VID_CONTROL   0x2F /* Deprecated */                 //MOD040
#define CGBC_CMD_TEST_COUNTER  0x30
#define CGBC_CMD_BOOT_TIME     0x31
#define CGBC_CMD_SPI_TIMING    0x32
#define CGBC_CMD_DUMP          0x33                                  //MOD002
#define CGBC_CMD_RESET_INFO    0x34                                  //MOD002
#define CGBC_CMD_POST_CODE     0x35                                  //MOD002
#define CGBC_CMD_PERFORMANCE   0x36                                  //MOD002
#define CGBC_CMD_POWER_LOSS    0x37                                  //MOD003
#define CGBC_CMD_XINT_CONTROL  0x38                                  //MOD004
#define CGBC_CMD_USER_LOCK     0x39                                  //MOD004
#define CGBC_CMD_OSCCAL        0x3A                                  //MOD005
#define CGBC_CMD_CPU_TEMP      0x3B                                  //MOD009
#define CGBC_CMD_SBSM_DATA     0x3C                           //MOD004 MOD005
#define CGBC_CMD_I2C_START     0x40
#define CGBC_CMD_I2C_STAT      0x48
#define CGBC_CMD_I2C_DATA      0x50
#define CGBC_CMD_I2C_SPEED     0x58                                  //MOD002
#define CGBC_CMD_SCI_STOP      0x60                                  //MOD010
#define CGBC_CMD_BTN_CONTROL   0x61                                  //MOD017
#define CGBC_CMD_FAN_CONTROL   0x62                                  //MOD020
#define CGBC_CMD_RTC           0x63                                  //MOD022
#define CGBC_CMD_GPIO_DAT_RD   0x64                                  //MOD016
#define CGBC_CMD_GPIO_DAT_WR   0x65                                  //MOD016
#define CGBC_CMD_GPIO_CFG_RD   0x66                                  //MOD016
#define CGBC_CMD_GPIO_CFG_WR   0x67                                  //MOD016
#define CGBC_CMD_I2C_COMBINED  0x68                                  //MOD014
#define CGBC_CMD_PECI          0x70                                  //MOD023
#define CGBC_CMD_I2C_GATE      0x71                                  //MOD024
#define CGBC_CMD_CFG_PINS      0x72                                  //MOD025
#define CGBC_CMD_AVR_SPM       0x73                                  //MOD029
#define CGBC_CMD_COMX_CFG      0x74                                  //MOD031
#define CGBC_CMD_BLT_PWM       0x75                                  //MOD036
#define CGBC_CMD_DEVICE_ID     0x76                                  //MOD038
#define CGBC_CMD_HWM_SENSOR    0x77                                  //MOD038
#define CGBC_CMD_POST_CFG      0x78                                  //MOD040
#define CGBC_CMD_LFP_DELAY     0x79                                  //MOD040
#define CGBC_CMD_TEST_MODE     0x7A                                  //MOD040
#define CGBC_CMD_HWM_CONFIG    0x7B                                  //MOD040
#define CGBC_CMD_LF2_CONTROL   0x7C                                  //MOD041
#define CGBC_CMD_LF2_DELAY     0x7D                                  //MOD041
#define CGBC_CMD_BL2_PWM       0x7E                                  //MOD041
#define CGBC_CMD_DIAG_CFG      0x7F                                  //MOD041
#define CGBC_CMD_GPIO_IER_RD   0x80                                  //MOD041
#define CGBC_CMD_GPIO_IER_WR   0x81                                  //MOD041
#define CGBC_CMD_GPIO_IRQ_RD   0x82                                  //MOD041
#define CGBC_CMD_GPIO_IRQ_WR   0x83                                  //MOD041
#define CGBC_CMD_SYNC          0xFE                                  //MOD014



/*------------------
 * CGBC status byte
 *------------------
 */

#define CGBC_BSY_BIT     7                             /* busy flag         */
#define CGBC_RDY_BIT     6                             /* ready flag        */
#define CGBC_STAT_MSK    ((1<<CGBC_BSY_BIT)|(1<<CGBC_RDY_BIT)) /* state msk */
#define CGBC_IDL_STAT    ((0<<CGBC_BSY_BIT)|(0<<CGBC_RDY_BIT)) /* IDLE      */
#define CGBC_BSY_STAT    ((1<<CGBC_BSY_BIT)|(0<<CGBC_RDY_BIT)) /* BUSY      */
#define CGBC_RDY_STAT    ((0<<CGBC_BSY_BIT)|(1<<CGBC_RDY_BIT)) /* READY     */
#define CGBC_ERR_STAT    ((1<<CGBC_BSY_BIT)|(1<<CGBC_RDY_BIT)) /* ERROR     */

#define CGBC_DAT_BIT     5                             /* data pending flag */
#define CGBC_DAT_PENDING (1<<CGBC_DAT_BIT)

#define CGBC_DAT_CNT_MSK 0x1F                          /* data count        */
#define CGBC_RET_COD_MSK 0x1F                          /* return code       */
#define CGBC_ERR_COD_MSK 0x1F                          /* error code        */
#define CGBC_IDL_FLG_MSK 0x1F                          /* idle flags        */


                                                                    /*MOD027v*/
/*-------------------------------------------
 * CGBC command status byte / CM41 BC family
 *-------------------------------------------
 */

#define CGBC_ERR_BIT     7                                     /* error flag */
/*efine CGBC_RDY_BIT     6                                        ready flag */
/*efine CGBC_STAT_MSK    ((1<<CGBC_ERR_BIT)|(1<<CGBC_RDY_BIT))    state mask */
#define CGBC_DAT_STAT    ((0<<CGBC_ERR_BIT)|(0<<CGBC_RDY_BIT)) /* DATA READY */
/*efine CGBC_RDY_STAT    ((0<<CGBC_ERR_BIT)|(1<<CGBC_RDY_BIT))    CMD READY  */
/*efine CGBC_ERR_STAT    ((1<<CGBC_ERR_BIT)|(1<<CGBC_RDY_BIT))    ERROR      */
                                                                    /*MOD027^*/


/*-------------
 * Error codes
 *-------------
 */

#define CGBC_ERR_UNKNOWN       0x00     /* unexpected or unknown error       */
#define CGBC_ERR_BAD_COMMAND   0x01     /* command not supported             */
#define CGBC_ERR_OVERFLOW      0x02     /* buffer overflow                   */
#define CGBC_ERR_OUT_OF_SYNC   0x03     /* protocol out of sync              */
#define CGBC_ERR_NO_DATA       0x04     /* no data available                 */
#define CGBC_ERR_BAD_PROTOCOL  0x05     /* protocol not supported            */
#define CGBC_ERR_DEVICE_NAK    0x06     /* device address not acknowledged   */
#define CGBC_ERR_DATA_NAK      0x07     /* data byte not acknowledged        */
#define CGBC_ERR_BAD_INTERRUPT 0x08     /* unexpected or unknown interrupt   */
#define CGBC_ERR_ARBITRATION   0x09     /* device arbitration failed         */
#define CGBC_ERR_DEVICE_BUSY   0x0A     /* device is busy and cannot respond */
#define CGBC_ERR_NOT_SUPPORTED 0x0B     /* feature or function not supported */
#define CGBC_ERR_BAD_PARAMETER 0x0C     /* command parameter invalid         */
#define CGBC_ERR_TIMEOUT       0x0D     /* timeout condition       */ //MOD002
#define CGBC_ERR_ABORT         0x0E     /* command aborted         */ //MOD032
#define CGBC_ERR_BAD_CHECKSUM  0x0F     /* checksum error          */ //MOD040
                                                                    /*MOD027v*/
#define CGBC_ERR_HIF_RESET     0x1E     /* host interface was reset          */
#define CGBC_ERR_HIF_DOWN      0x1F     /* host interface is down            */
                                                                    /*MOD027^*/

                                                                     //MOD014v
/*-------------------------------
 * Info returned from GET_FW_REV
 *-------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_FW_REV_STRUCT
   {
    unsigned char feat;                         /* firmware feature number  */
    unsigned char majv;                         /* major revision number    */
    unsigned char minv;                         /* minor revision number    */
    unsigned char cid;                          /* compatibility ID         */
   } CGBC_FW_REV, *P_CGBC_FW_REV;

typedef struct CGBC_CMD_FW_REV_OUT_STRUCT
   {
    unsigned char sts;                          /* command status           */
    CGBC_FW_REV   rev;                          /* revision information     */
   } CGBC_CMD_FW_REV_OUT, *P_CGBC_CMD_FW_REV_OUT;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
                                                                     
                                                                     
/*------------------------------------
 * Info returned from CGBC_CMD_INFO_1
 *------------------------------------
 */

#ifndef __ASSEMBLER__                                                //MOD013
                                                                     //MOD002v
typedef struct CGBC_INFO_1_STRUCT
   {
    unsigned char cgbcType;          /* congatec board controller type      */
    unsigned char wdInfo;            /* watchdog information                */
    unsigned char cgbcFeatures;      /* supported features                  */
    unsigned char i2cFreq;           /* S/F-mode I2C bus frequency          */
    unsigned char smbFreq;           /* ASM-bus frequency                   */
    unsigned char ddcFreq;           /* EPI-bus frequency                   */
    unsigned char sdaEepromInfo;     /* secure data area EEPROM information */
    unsigned char cgbcEepromInfo;    /* CGBC parameter EEPROM information   */
    unsigned char userEepromInfo;    /* user data EEPROM information        */
    unsigned char biosEepromInfo;    /* BIOS parameter EEPROM information   */
    unsigned char cgbcRamInfo;       /* CGBC parameter RAM information      */
    unsigned char userRamInfo;       /* user data RAM information           */
    unsigned char biosRamInfo;       /* BIOS parameter RAM information      */
    unsigned char reserved;                                          //MOD014
   } CGBC_INFO_1, *P_CGBC_INFO_1;
                                                                     //MOD002^
                                                                     //MOD014v
typedef struct CGBC_CMD_INFO_1_OUT_STRUCT
   {
    unsigned char sts;                          /* command status           */
    CGBC_INFO_1   info1;                        /* info 1 structure         */
   } CGBC_CMD_INFO_1_OUT, *P_CGBC_CMD_INFO_1_OUT;
                                                                     //MOD014^
#endif /*!__ASSEMBLER__*/                                            //MOD013

/* congatec board controller types */

#define CGBC_ATMEGA48      1
#define CGBC_ATMEGA88      2
#define CGBC_ATMEGA168     3
#define CGBC_ATMEGA165P    4                                         //MOD014
#define CGBC_ATMEGA325P    5                                         //MOD015
#define CGBC_STM32F100R8   6                                         //MOD018
#define CGBC_TM4E1231H6ZRB 7                                         //MOD030
#define CGBC_MEC1705QSZ    8                                         //MOD0042

/* watchdog information */

#define CGBC_WD_TYPE_MSK           0x0F    /* mask for controller type field */
 #define CGBC_WD_TYPE_NONE          0
 #define CGBC_WD_TYPE_STANDARD      1

#define CGBC_WD_EVENT_SUPPORT_MSK  0xF0    /* mask for supported watchdog events */
 #define CGBC_WD_INT_SUPPORT        0x10    /* NMI/IRQ supported */
 #define CGBC_WD_SCI_SUPPORT        0x20    /* SMI/SCI supported */
 #define CGBC_WD_RST_SUPPORT        0x40    /* system reset supported */
 #define CGBC_WD_BTN_SUPPORT        0x80    /* power button supported */

/* supported features */

#define CGBC_BACKLIGHT_SUPPORT     0x01    /* LCD backlight control supported */
#define CGBC_LFP_BACKLIGHT_SUPPORT 0x01    /* LFP backlight control */ //MOD040
#define CGBC_LCD_POWER_SUPPORT     0x02    /* LCD power control supported */
#define CGBC_LFP_POWER_SUPPORT     0x02    /* LFP power control */     //MOD040
#define CGBC_BOOT_COUNTER_SUPPORT  0x04    /* boot counter supported */
#define CGBC_RUNNIG_TIME_SUPPORT   0x08    /* running time meter supported */
#define CGBC_BOOT_TIME_SUPPORT     0x10    /* boot time meter supported */
#define CGBC_AUX_SUPPORT           0x20    /* aux. serial port supported */

/* I2C bus frequency */

#define CGBC_I2C_FREQ_VALUE_MSK    0x3F    /* mask for I2C bus frequency value */
#define CGBC_I2C_FREQ_UNIT_MASK    0xC0    /* mask for I2C bus frequency unit */
 #define CGBC_I2C_FREQ_UNIT_100HZ   0x00    /* unit of frequency value is 100Hz */
 #define CGBC_I2C_FREQ_UNIT_1KHZ    0x40    /* unit of frequency value is 1kHz */
 #define CGBC_I2C_FREQ_UNIT_10KHZ   0x80    /* unit of frequency value is 10kHz */
 #define CGBC_I2C_FREQ_UNIT_100KHZ  0xC0    /* unit of frequency value is 100kHz */

/* I2C device information */

#define CGBC_I2C_DEV_SIZE_MASK     0x1F    /* I2C device size coded in power of 2 */
#define CGBC_I2C_DEV_TYPE_MASK     0xE0    /* mask for I2C device type */
 #define CGBC_I2C_DEV_TYPE_EEP8     0x00    /* I2C EEPROM with 8-bit index */
 #define CGBC_I2C_DEV_TYPE_EEP16    0x20    /* I2C EEPROM with 16-bit index */
 #define CGBC_I2C_DEV_TYPE_RAM8     0x40    /* I2C RAM with 8-bit index */
 #define CGBC_I2C_DEV_TYPE_RAM16    0x60    /* I2C RAM with 16-bit index */


                                                                     //MOD014v
/*---------------------------------------
 * Info returned from CGBC_CMD_BOOT_TIME
 *---------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_BOOT_TIME_OUT_STRUCT
   {
    unsigned char  sts;                         /* command status           */
    UINT16 systemUpTime;                /* system up time in ms     */
    UINT16 lastBootTime;                /* last boot time in ms     */
   } CGBC_CMD_BOOT_TIME_OUT, *P_CGBC_CMD_BOOT_TIME_OUT;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
                                                                     

/*----------------
 * System flags 1
 *----------------
 */
                                                                     //MOD021v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_SYS_FLAGS_1_IN_STRUCT   // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_SYS_FLAGS_1
    unsigned char andMask;                      // system flags 1 AND mask
    unsigned char orMask;                       // system flags 1 OR mask
   } CGBC_CMD_SYS_FLAGS_1_IN, *P_CGBC_CMD_SYS_FLAGS_1_IN;

typedef struct CGBC_CMD_SYS_FLAGS_1_OUT_STRUCT  // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char flags;                        // current system flags 1
   } CGBC_CMD_SYS_FLAGS_1_OUT, *P_CGBC_CMD_SYS_FLAGS_1_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD021^
#define CGBC_SYS_PWR_AT         0x08    /* 1 = AT power supply */    //MOD003
#define CGBC_SYS_POST_END       0x04    /* 1 = POST complete */
#define CGBC_SYS_POST_START     0x02    /* 1 = system has started POST */
#define CGBC_SYS_PWR_ON         0x01    /* 1 = system power is on */



/*---------------------------------------------------------------------------
 * CGBC_CMD_VID_CONTROL 
 *
 * Input:  bits 7-6 - reserved
 *         bit  5   - 1 = enable LCD power control
 *         bit  4   - 1 = enable LCD backlight control
 *         bits 3-2 - reserved
 *         bit  1   - new LCD power state if bit 5 = 1 (1=on, 0=off)
 *         bit  0   - new LCD backlight state if bit 4 = 1 (1=on, 0=off)
 *
 * Output: bits 7-2 - reserved
 *         bit  1   - current LCD power state (1=on, 0=off)
 *         bit  0   - current LCD backlight state (1=on, 0=off)
 */
                                                                     //MOD021v
#ifndef __ASSEMBLER__
                                                                     //MOD040v
typedef struct CGBC_CMD_LFP_CONTROL_IN_STRUCT   // cmd packet struct
   {
    unsigned char cmd;                          // CGBC_CMD_LFP_CONTROL
    unsigned char parm;                         // video control parameter
   } CGBC_CMD_LFP_CONTROL_IN, *P_CGBC_CMD_LFP_CONTROL_IN;

typedef struct CGBC_CMD_LFP_CONTROL_OUT_STRUCT  // response pkt struct
   {
    unsigned char sts;                          // command status
    unsigned char flags;                        // current video ctrl flags
   } CGBC_CMD_LFP_CONTROL_OUT, *P_CGBC_CMD_LFP_CONTROL_OUT;

typedef struct CGBC_CMD_VID_CONTROL_IN_STRUCT   // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_VID_CONTROL
    unsigned char parm;                         // video control parameter
   } CGBC_CMD_VID_CONTROL_IN, *P_CGBC_CMD_VID_CONTROL_IN; /* Deprecated */

typedef struct CGBC_CMD_VID_CONTROL_OUT_STRUCT  // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char flags;                        // current video ctrl flags
   } CGBC_CMD_VID_CONTROL_OUT, *P_CGBC_CMD_VID_CONTROL_OUT; /* Deprecated */
                                                                     //MOD040^
#endif //!__ASSEMBLER__
                                                                     //MOD021^
                                                                     //MOD040v
/* Local flat panel control register */
#define CGBC_LFP_VDD_INP_FORCED  0x80
#define CGBC_LFP_BLT_INP_FORCED  0x40
#define CGBC_LFP_VDD_CTL_UPDATE  0x20
#define CGBC_LFP_BLT_CTL_UPDATE  0x10
#define CGBC_LFP_VDD_INP_STATUS  0x08
#define CGBC_LFP_VDD_INP_ON      0x08
#define CGBC_LFP_VDD_INP_OFF     0x00
#define CGBC_LFP_BLT_INP_STATUS  0x04
#define CGBC_LFP_BLT_INP_ON      0x04
#define CGBC_LFP_BLT_INP_OFF     0x00
#define CGBC_LFP_VDD_GATE_STATUS 0x02
#define CGBC_LFP_VDD_GATE_OPEN   0x02
#define CGBC_LFP_VDD_GATE_CLOSED 0x00
#define CGBC_LCD_POWER_ON        0x02
#define CGBC_LFP_BLT_GATE_STATUS 0x01
#define CGBC_LFP_BLT_GATE_OPEN   0x01
#define CGBC_LFP_BLT_GATE_CLOSED 0x00
#define CGBC_LCD_BACKLIGHT_ON    0x01

/* Deprecated definitions */
#define CGBC_LCD_BACKLIGHT_OFF  0x00
#define CGBC_LCD_POWER_OFF      0x00
#define CGBC_SET_LCD_BACKLIGHT  0x10
#define CGBC_SET_LCD_POWER      0x20
                                                                     //MOD040^
                                                                     //MOD002v
/*-----------------
 * I2C bus numbers
 *-----------------
 */

#define CGBC_I2C_BUS_MSK      0x07  /* I2C bus mask for SPI commands        */
#define CGBC_I2C_BUS_EXTERNAL 0x00  /* physical 400kHz multi-master I2C bus */
#define CGBC_I2C_BUS_SMB1     0x01  /* secondary SMBus                      */ //MOD033
#define CGBC_I2C_BUS_SMB0     0x02  /* primary SMBus                        */ //MOD033
#define CGBC_I2C_BUS_SMB      0x02  /* system management bus                */
#define CGBC_I2C_BUS_ASM      0x02  /* auxiliary system management bus      */ //MOD004
#define CGBC_I2C_BUS_INTERNAL 0x03  /* virtual I2C bus for internal devices */
#define CGBC_I2C_BUS_DDC      0x04  /* video DDC bus                        */
#define CGBC_I2C_BUS_EPI      0x04  /* embedded panel interface bus         */ //MOD004
#define CGBC_I2C_BUS_SMB2     0x07  /* auxiliary SMBus                      */ //MOD033
                                                                     //MOD002^


/*-------------------------
 * I2C_START command flags
 *-------------------------
 */
                                                                     //MOD014v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_I2C_START_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char wrCnt;
                   unsigned char rdCnt;
                   unsigned char i2cData[35];
                  } CGBC_CMD_I2C_START_IN,
                 *P_CGBC_CMD_I2C_START_IN;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
#define CGBC_I2C_SEND_START     0x80    /* send START condition */
#define CGBC_I2C_SEND_STOP      0x40    /* send STOP condition */
#define CGBC_I2C_SEND_LAST_ACK  0x80    /* send ACK on last read byte */
#define CGBC_I2C_SEND_ALL_NAK   0x40    /* send NAK on all read bytes */
#define CGBC_I2C_CNT_MSK        0x3F    /* write/read count mask */  //MOD014



/*-----------------------
 * I2C_STAT return codes
 *-----------------------
 */

#define CGBC_I2C_STAT_IDL       0x00    /* I2C bus is idle. */
#define CGBC_I2C_STAT_DAT       0x01    /* I2C bus controller data pending. */
#define CGBC_I2C_STAT_BSY       0x02    /* I2C bus is busy. */


                                                                     //MOD014v
/*---------------------------
 * I2C_DATA return structure
 *---------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_I2C_DATA_OUT_STRUCT
                  {
                   unsigned char sts;
                   unsigned char i2cData[32];
                  } CGBC_CMD_I2C_DATA_OUT, *P_CGBC_CMD_I2C_DATA_OUT;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^

                                                                     //MOD021v
/*------------------------------------
 * I2C_SPEED command/response packets
 *------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_I2C_SPEED_IN_STRUCT
   {
    unsigned char cmd;                          // command code
    unsigned char speed;                        // desired I2C-bus frequency
   } CGBC_CMD_I2C_SPEED_IN, *P_CGBC_CMD_I2C_SPEED_IN;

typedef struct CGBC_CMD_I2C_SPEED_OUT_STRUCT
   {
    unsigned char sts;                          // command status
    unsigned char speed;                        // current I2C-bus frequency
   } CGBC_CMD_I2C_SPEED_OUT, *P_CGBC_CMD_I2C_SPEED_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD021^


/*--------------------
 * I2C device numbers
 *--------------------
 */

#define CG_SDA_EEP_I2C_ADDR     0x80    /* secure data area EEPROM */
#define CG_BC_EEP_I2C_ADDR      0x90    /* board controller parameter EEPROM */
#define CG_USER_EEP_I2C_ADDR    0xA0    /* user data EEPROM */
#define CG_BIOS_EEP_I2C_ADDR    0xB0    /* BIOS control parameter EEPROM */
#define CG_BC_RAM_I2C_ADDR      0xC0    /* board controller parameter RAM */
#define CG_USER_RAM_I2C_ADDR    0xD0    /* user scratch RAM */
#define CG_BIOS_RAM_I2C_ADDR    0xE0    /* BIOS scratch RAM */


                                                                     //MOD002v
/*-------------------------------------------------
 * Layout of the board controller parameter EEPROM
 *-------------------------------------------------
 */

#ifndef __ASSEMBLER__                                                //MOD013

typedef struct CG_BC_EEP_STRUCT
   {
    unsigned char  i2cExtSpeed;       /* I2C-bus power-up default frequency */
    UINT16 i2cExtTimeout;     /* I2C-bus power-up default timeout   */
    unsigned char  i2cSmbSpeed;       /* ASM-bus power-up default frequency */
    UINT16 i2cSmbTimeout;     /* ASM-bus power-up default timeout   */
    unsigned char  i2cDdcSpeed;       /* EPI-bus power-up default frequency */
    UINT16 i2cDdcTimeout;     /* EPI-bus power-up default timeout   */
    unsigned char  postCodeEnable;    /* POST code relay enable             */
                                                                     //MOD004v
    unsigned char  vidControl;        /* video control power-up default     */
    unsigned char  vidResetDelay;     /* video signal suppression time      */
                                                                     //MOD004^
    unsigned char  pwrOffDelay;       /* LastStateOff saving dly */  //MOD003
    unsigned char  featByte1;         /* feature byte 1          */  //MOD012
    unsigned char  lf2Control;        /* LF2 ctrl at pwr-up dflt */  //MOD041
    unsigned char  lf2ResetDelay;     /* LF2 signal suppr. time  */  //MOD041
   } CG_BC_EEP, *P_CG_BC_EEP;
                                                                     //MOD002^
#endif /*!__ASSEMBLER__*/                                            //MOD013


                                                                     //MOD014v
/*----------------------------------------------
 * Layout of the board controller parameter RAM
 *----------------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CG_BC_RAM_STRUCT
   {
    unsigned char  reserved[4];                         /* reserved         */
    unsigned char  postCodeEnable;                      /* POST code enable */
                                                                     //MOD017v
    unsigned char  flagsByte1;                          /* flags byte 1     */
    unsigned char  padding[2];
                                                                     //MOD017^
   } CG_BC_RAM, *P_CG_BC_RAM;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
                                                                     //MOD017v
/* CG_BC_RAM.flagsByte1 */

#define CGBC_COLD_RESET_REQUEST      0x10                            //MOD040
#define CGBC_RSMRST_REQUEST          0x20                            //MOD040
#define CGBC_HARD_OFF_INHIBIT        0x40
#define CGBC_BOOTBLOCK_UPDATE_ENABLE 0x80
                                                                     //MOD017^

                                                                     //MOD012v
/*----------------
 * Feature byte 1
 *----------------
 */

#define CGBC_FORCE_SBSM_EMULATION 0x01          /* 1 = force SBSM emulation */
                                                                     //MOD012^
#define CGBC_HOT_S5_ENABLE        0x02          // 1 = hot S5 enabled//MOD019
#define CGBC_FORCE_BAT_ON_SMBUS   0x04          // 1 = bat on SMBus  //MOD040
#define CGBC_MPP_FLAG             0x08          // multi purpose     //MOD040
                                                // power flag        //MOD040
                                                                     //MOD017v
#define CGBC_RST_BTN_INHIBIT      0x10          // 1 = reset button disabled
#define CGBC_LID_BTN_INHIBIT      0x20          // 1 = LID button disabled
#define CGBC_SLP_BTN_INHIBIT      0x40          // 1 = sleep button disabled
#define CGBC_PWR_BTN_INHIBIT      0x80          // 1 = power button disabled
                                                                     //MOD017^


/*------------------------------------
 * CGBC_CMD_WD_INIT command structure
 *------------------------------------
 */

#ifndef __ASSEMBLER__                                                //MOD013
                                                                     //MOD002v
typedef struct CGBC_WD_PARMS_STRUCT
   {
    unsigned char  wdInitCmd;                /* CGBC_CMD_WD_INIT             */
    unsigned char  wdMode;                   /* watchdog mode                */
    unsigned char  wdCtrl;                   /* watchdog control byte        */
    UINT16 wdSt1TimL;                /* watchdog stage 1 time (msec) */ //MOD008
    unsigned char  wdSt1TimH;
    UINT16 wdSt2TimL;                /* watchdog stage 2 time (msec) */ //MOD008
    unsigned char  wdSt2TimH;
    UINT16 wdSt3TimL;                /* watchdog stage 3 time (msec) */ //MOD008
    unsigned char  wdSt3TimH;
    UINT16 wdDlyTimL;                /* watchdog delay time   (msec) */ //MOD008
    unsigned char  wdDlyTimH;
   } CGBC_WD_PARMS, *P_CGBC_WD_PARMS;
                                                                     //MOD002^
#endif /*!__ASSEMBLER__*/                                            //MOD013



/*--------------------
 * Watchdog mode byte
 *--------------------
 */

#define CGBC_WD_DISABLED           0x00    /* watchdog disabled */
#define CGBC_WD_ONETIME_TRIG_MODE  0x01    /* watchdog one-time-trigger mode */
#define CGBC_WD_SINGLE_EVENT_MODE  0x02    /* watchdog single event mode */
#define CGBC_WD_EVENT_REPEAT_MODE  0x03    /* watchdog event repeat mode */
#define CGBC_WD_MODE_MSK           0x03                              //MOD014



/*-----------------
 * Watchdog events
 *-----------------
 */

#define CGBC_WD_EVENT_INT       0x00    /* NMI/IRQ */
#define CGBC_WD_EVENT_SCI       0x01    /* SMI/SCI */
#define CGBC_WD_EVENT_RST       0x02    /* system reset */
#define CGBC_WD_EVENT_BTN       0x03    /* power button */



/*-----------------------
 * Watchdog control byte
 *-----------------------
 */

#define CGBC_WD_STAGES_MSK      0x03
#define CGBC_WD_EVENT1_MSK      0x0C
#define CGBC_WD_EVENT2_MSK      0x30
#define CGBC_WD_EVENT3_MSK      0xC0


                                                                     //MOD014v
/*---------------------------------
 * CGBC_CMD_WD_INIT_POST parameter
 *---------------------------------
 */

#define CGBC_POST_WATCHDOG_EVENT_MSK   0x8000
 #define CGBC_POST_WATCHDOG_EVENT_RST   0x0000
 #define CGBC_POST_WATCHDOG_EVENT_BTN   0x8000

#define CGBC_POST_WATCHDOG_TIMEOUT_MSK 0x7FFF



/*----------------------------------
 * CGBC_CMD_WD_INIT_EARLY parameter
 *----------------------------------
 */

#define CGBC_EARLY_WATCHDOG_EVENT_MSK   0x80
 #define CGBC_EARLY_WATCHDOG_EVENT_RST   0x00
 #define CGBC_EARLY_WATCHDOG_EVENT_BTN   0x80

#define CGBC_EARLY_WATCHDOG_TIMEOUT_MSK 0x7F
                                                                     //MOD014^

                                                                     //MOD002v
/*---------------------
 * AUX_INIT parameters
 *---------------------
 */
                                                                     //MOD014v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_AUX_INIT_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char parm;
                  } CGBC_CMD_AUX_INIT_IN,
                 *P_CGBC_CMD_AUX_INIT_IN;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
#define CGBC_UART_BAUDRATE_MSK 0x07
#define CGBC_UART_BAUDRATE_S   0                                     //MOD040
#define CGBC_UART_DISABLE      0x00
#define CGBC_UART_1200         0x01
#define CGBC_UART_2400         0x02
#define CGBC_UART_4800         0x03
#define CGBC_UART_9600         0x04
#define CGBC_UART_19200        0x05
#define CGBC_UART_38400        0x06
#define CGBC_UART_115200       0x07                                  //MOD040

#define CGBC_UART_DATABITS_MSK 0x18
#define CGBC_UART_DATABITS_S   3                                     //MOD040
#define CGBC_UART_DATA_5       0x00
#define CGBC_UART_DATA_6       0x08
#define CGBC_UART_DATA_7       0x10
#define CGBC_UART_DATA_8       0x18

#define CGBC_UART_STOPBITS_MSK 0x20
#define CGBC_UART_STOPBITS_S   5                                     //MOD040
#define CGBC_UART_STOP_1       0x00
#define CGBC_UART_STOP_2       0x20

#define CGBC_UART_PARITY_MSK   0xC0
#define CGBC_UART_PARITY_S     6                                     //MOD040
#define CGBC_UART_PARITY_NONE  0x00
#define CGBC_UART_PARITY_EVEN  0x80
#define CGBC_UART_PARITY_ODD   0xC0
                                                                     //MOD002^
#define CGBC_AUX_62500_N_6_1   0x48                                  //MOD014



/*-----------------------
 * AUX_STAT return code
 *-----------------------
 */

#define CGBC_UART_IBF   0x01    /* RX data input buffer filled */
#define CGBC_UART_OBF   0x02    /* TX data output buffer full */
#define CGBC_UART_OBE   0x04    /* TX data output buffer empty */
#define CGBC_UART_ERR   0x08    /* transmission error */             //MOD002



/*------------
 * SCI events
 *------------
 */

#define CGBC_SCI_EVT_NONE               0
#define CGBC_SCI_EVT_WATCHDOG           1
#define CGBC_SCI_EVT_TEMPERATURE        2
#define CGBC_SCI_EVT_XINT               3                            //MOD004
#define CGBC_SCI_EVT_BAT_0              4                            //MOD005
#define CGBC_SCI_EVT_BAT_1              5                            //MOD005
#define CGBC_SCI_EVT_BAT_2              6                            //MOD005
#define CGBC_SCI_EVT_BAT_3              7                            //MOD005
#define CGBC_SCI_EVT_LID_OPEN           8                            //MOD017
#define CGBC_SCI_EVT_LID_CLOSE          9                            //MOD017
#define CGBC_SCI_EVT_SLP_BTN            10                           //MOD017



/*----------------------------------
 * CGBC_CMD_TEST_COUNTER parameters
 *----------------------------------
 */
                                                                     //MOD021v
#ifndef __ASSEMBLER__

typedef struct CGBC_TCNT_STRUCT
   {
    unsigned char good;
    unsigned char bad;
   } CGBC_TCNT, *P_CGBC_TCNT;

typedef struct CGBC_CMD_TCNT_IN_STRUCT          // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_TEST_COUNTER
    unsigned char parm;                         // test counter parameter
   } CGBC_CMD_TCNT_IN, *P_CGBC_CMD_TCNT_IN;

typedef struct CGBC_CMD_TCNT_OUT_STRUCT         // response packet structure
   {
    unsigned char sts;                          // command status
    CGBC_TCNT     tcnt;                         // test counters
   } CGBC_CMD_TCNT_OUT, *P_CGBC_CMD_TCNT_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD021^
#define CGBC_TEST_PASSED        0x55
#define CGBC_TEST_FAILED        0xAA

#define CGBC_LAST_FAILED        0x80


                                                                     //MOD002v
/*--------------------------
 * CGBC_CMD_DUMP parameters
 *--------------------------
 */
                                                                     //MOD014v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_DUMP_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char typeCnt;
                   unsigned char addrLow;
                   unsigned char addrMid;
                   unsigned char addrHigh;
                  } CGBC_CMD_DUMP_IN, *P_CGBC_CMD_DUMP_IN;

#endif /*!__ASSEMBLER__*/
                                                                     //MOD014^
#define CGBC_DUMP_COUNT_MSK   0xF8
#define CGBC_MEM_TYPE_MSK     0x07
#define CGBC_MEM_TYPE_FLASH   0x00
#define CGBC_MEM_TYPE_EEPROM  0x01
#define CGBC_MEM_TYPE_RAM     0x02


                                                                     //MOD014v
/*---------------------------------------
 * CGBC_CMD_RESET_INFO return parameters
 *---------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_RESET_INFO_STRUCT
   {
    unsigned char latestReset;
    unsigned char externalResetCounter;
    unsigned char brownoutResetCounter;
    unsigned char watchdogResetCounter;
    unsigned char softwareResetCounter;
   } CGBC_RESET_INFO, *P_CGBC_RESET_INFO;
                                                                     //MOD021v
typedef struct CGBC_CMD_RESET_INFO_OUT_STRUCT
   {
    unsigned char   sts;                        // command status
    CGBC_RESET_INFO info;                       // reset information
   } CGBC_CMD_RESET_INFO_OUT, *P_CGBC_CMD_RESET_INFO_OUT;
                                                                     //MOD021^
#endif /*!__ASSEMBLER__*/

/* bit declarations within latestResetSource */
                                                                     //MOD014^
#define CGBC_RESET_SWR  0x10
#define CGBC_RESET_WDG  0x08
#define CGBC_RESET_BOD  0x04
#define CGBC_RESET_EXT  0x02
#define CGBC_RESET_POR  0x01
                                                                     //MOD002^
                                                                     //MOD040v
/*-------------------------------
 * CGBC_CMD_POST_CODE parameters
 *-------------------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_POST_CODE_IN_STRUCT     // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_POST_CODE
    unsigned char postCode;                     // POST code
   } CGBC_CMD_POST_CODE_IN, *P_CGBC_CMD_POST_CODE_IN;

#endif //!__ASSEMBLER__
                                                                     //MOD040^

                                                                     //MOD003v
/*--------------------------------
 * CGBC_CMD_POWER_LOSS parameters
 *--------------------------------
 */

#define CGBC_POWER_LOSS_MSK   0x03
#define  CGBC_POWER_LOSS_OFF   0x00
#define  CGBC_POWER_LOSS_ON    0x01
#define  CGBC_POWER_LOSS_LAST  0x02
                                                                     //MOD003^
#define CGBC_RTC_PRESENT      0x80                                   //MOD011


                                                                     //MOD004v
/*----------------------------------
 * CGBC_CMD_XINT_CONTROL parameters
 *----------------------------------
 */
                                                                     //MOD021v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_XINT_CONTROL_IN_STRUCT  // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_XINT_CONTROL
    unsigned char parm;                         // control parameter
   } CGBC_CMD_XINT_CONTROL_IN, *P_CGBC_CMD_XINT_CONTROL_IN;

typedef struct CGBC_CMD_XINT_CONTROL_OUT_STRUCT // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char parm;                         // current control parameter
   } CGBC_CMD_XINT_CONTROL_OUT, *P_CGBC_CMD_XINT_CONTROL_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD021^
#define  CGBC_XINT_GET_STATUS 0x00           
#define  CGBC_XINT_SET_MODE   0x01           // 1 = set parms / 0 = get status
#define  CGBC_XINT_NMI        0x02           // Generate IRQ/NMI on EXT_INT.
#define  CGBC_XINT_SCI        0x04           // Generate SCI/SMI on EXT_INT.
#define  CGBC_XINT_POLLING    0x08           // EXT_INT polling mode.
#define  CGBC_XINT_MODE_MSK   (CGBC_XINT_NMI|CGBC_XINT_SCI|CGBC_XINT_POLLING)
#define  CGBC_XINT_ACTIVE     0x10           // 1 = EXT_INT was active.



/*----------------------------------
 * CGBC_CMD_USER_LOCK return status
 *----------------------------------
 */
                                                                     //MOD021v
#define CGBC_USER_LOCK_KEY_SIZE 6

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_USER_LOCK_IN_STRUCT
                  {
                   unsigned char cmd;
                   unsigned char key[CGBC_USER_LOCK_KEY_SIZE];
                  } CGBC_CMD_USER_LOCK_IN, *P_CGBC_CMD_USER_LOCK_IN;

#endif //!__ASSEMBLER__
                                                                     //MOD021^
#define  CGBC_LOCKED    0x01                // User lock is closed.
#define  CGBC_TAMPERED  0x02                // Someone tampered with the lock.
                                                                     //MOD004^

                                                                     //MOD017v
/*---------------------------------------------------------------------------
 * CGBC_CMD_BTN_CONTROL 
 *
 * Input:  bit  7   - power button inhibit
 *         bit  6   - sleep button inhibit
 *         bit  5   -   LID button inhibit
 *         bit  4   - reset button inhibit
 *         bits 3-0 - reserved
 *
 * Output: bit  7   - 1 = power button disabled / 0 = power button enabled
 *         bit  6   - 1 = sleep button disabled / 0 = sleep button enabled
 *         bit  5   - 1 = LID button disabled   / 0 = LID button enabled
 *         bit  4   - 1 = reset button disabled / 0 = reset button enabled
 *         bit  3   - current power button state (1=inactive, 0=active)
 *         bit  2   - current sleep button state (1=inactive, 0=active)
 *         bit  1   - current LID state          (1=open,     0=closed)
 *         bit  0   - current reset button state (1=inactive, 0=active)
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_BTN_CONTROL_IN_STRUCT   // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_BTN_CONTROL
    unsigned char parm;                         // button control parameter
   } CGBC_CMD_BTN_CONTROL_IN, *P_CGBC_CMD_BTN_CONTROL_IN;

typedef struct CGBC_CMD_BTN_CONTROL_OUT_STRUCT  // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char flags;                        // current button ctrl flags
   } CGBC_CMD_BTN_CONTROL_OUT, *P_CGBC_CMD_BTN_CONTROL_OUT;

#endif //!__ASSEMBLER__

/*
 * Button control bit mapping
 */
#define CGBC_RST_BTN_INACTIVE 0x01              // 0 = reset button pressed
#define CGBC_LID_OPEN         0x02              // 0 = LID closed
#define CGBC_SLP_BTN_INACTIVE 0x04              // 0 = sleep button pressed
#define CGBC_PWR_BTN_INACTIVE 0x08              // 0 = power button pressed
#if 0                                           // same as in feature byte 1
#define CGBC_RST_BTN_INHIBIT  0x10              // 1 = reset button disabled
#define CGBC_LID_BTN_INHIBIT  0x20              // 1 = LID button disabled
#define CGBC_SLP_BTN_INHIBIT  0x40              // 1 = sleep button disabled
#define CGBC_PWR_BTN_INHIBIT  0x80              // 1 = power button disabled
#endif


                                                                     //MOD020v
/*-----------------------------------------------
 * CGBC_CMD_FAN_CONTROL command/response packets
 *-----------------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_FAN_CONTROL_IN_STRUCT   // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_FAN_CONTROL
    unsigned char fanCmd;                       // fan command
   } CGBC_CMD_FAN_CONTROL_IN, *P_CGBC_CMD_FAN_CONTROL_IN;

typedef struct CGBC_CMD_FAN_CONTROL_OUT_STRUCT  // response packet structure
   {
    unsigned char  sts;                         // command status
    UINT16 speed;                       // current fan speed count
   } CGBC_CMD_FAN_CONTROL_OUT, *P_CGBC_CMD_FAN_CONTROL_OUT;

#endif //!__ASSEMBLER__

/*
 * Fan commands
 */
#define CGBC_FAN_SET_PWM      0x00 // - 0x64    // set PWM on/off ratio 0-100
#define CGBC_FAN_ON           0x65              // turn fan on
#define CGBC_FAN_OFF          0x66              // turn fan off
                                                                     //MOD040v
#define CGBC_FAN_SET_FREQ_HI  0x80 // - 0xBF    // set high PWM frequency
#define CGBC_FAN_FREQ_HI_MASK 0x3F
#define CGBC_FAN_FREQ_HI_MIN  0x01
#define CGBC_FAN_FREQ_1kHz    0x01
#define CGBC_FAN_FREQ_63kHz   0x3F
#define CGBC_FAN_FREQ_HI_MAX  0x3F
#define CGBC_FAN_SET_FREQ_LO  0xC0 // - 0xC7    // set low PWM frequency
#define CGBC_FAN_FREQ_LO_MASK 0x07
#define CGBC_FAN_FREQ_LO_MIN  0x00
#define CGBC_FAN_FREQ_11p0Hz  0x00
#define CGBC_FAN_FREQ_14p7Hz  0x01
#define CGBC_FAN_FREQ_22p1Hz  0x02
#define CGBC_FAN_FREQ_29p4Hz  0x03
#define CGBC_FAN_FREQ_35p3Hz  0x04
#define CGBC_FAN_FREQ_44p1Hz  0x05
#define CGBC_FAN_FREQ_58p8Hz  0x06
#define CGBC_FAN_FREQ_88p2Hz  0x07
#define CGBC_FAN_FREQ_LO_MAX  0x07
                                                                     //MOD040^
#define CGBC_FAN_GET_SPEED    0xFF              // get fan speed count
                                                                     //MOD020^

                                                                     //MOD022v
/*---------------------------------------
 * CGBC_CMD_RTC command/response packets
 *---------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_RTC_IN_STRUCT           // command packet structure
   {
    unsigned char  cmd;                         // CGBC_CMD_RTC
    UINT16 year;                        // year to be set or 0
    unsigned char  month;                       // month to be set
    unsigned char  day;                         // day to be set
    unsigned char  hour;                        // hour to be set
    unsigned char  min;                         // minute to be set
    unsigned char  sec;                         // second to be set
   } CGBC_CMD_RTC_IN, *P_CGBC_CMD_RTC_IN;

typedef struct CGBC_CMD_RTC_OUT_STRUCT          // response packet structure
   {
    unsigned char  sts;                         // command status
    UINT16 yearSts;                     // current year and flags
    unsigned char  month;                       // current month
    unsigned char  day;                         // current day
    unsigned char  hour;                        // current hour
    unsigned char  min;                         // current minute
    unsigned char  sec;                         // current second
   } CGBC_CMD_RTC_OUT, *P_CGBC_CMD_RTC_OUT;

#endif //!__ASSEMBLER__

/*
 * RTC year and status flags
 */
#define CGBC_RTC_LOST_POWER   0x8000            // RTC lost power flag
#define CGBC_RTC_G3           0x4000            // first RTC read since G3
#define CGBC_RTC_YEAR_MSK     0x0FFF            // RTC year date
                                                                     //MOD022^


/*------------------------------------------
 * CGBC_CMD_GPIO_x command/response packets
 *------------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_GPIO_IN_STRUCT
   {
    unsigned char cmd;                          // command code
    unsigned char adr;                          // GPIO byte address
    unsigned char dat;                          // data/direction byte
   } CGBC_CMD_GPIO_IN, *P_CGBC_CMD_GPIO_IN;

typedef struct CGBC_CMD_GPIO_OUT_STRUCT
   {
    unsigned char sts;                          // command status
    unsigned char dat;                          // data/direction byte
   } CGBC_CMD_GPIO_OUT, *P_CGBC_CMD_GPIO_OUT;

#endif //!__ASSEMBLER__



/*-----------------------------------
 * CGBC_CMD_CPU_TEMP response packet
 *-----------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_CPU_TEMP_OUT_STRUCT
   {
    unsigned char sts;                          // command status
      signed char temp;                         // latest CPU temperature
   } CGBC_CMD_CPU_TEMP_OUT, *P_CGBC_CMD_CPU_TEMP_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD017^


/*-------------------------------
 * CGBC_CMD_SBSM_DATA parameters
 *-------------------------------
 */

#define  CGBC_SBSM_BAT_MSK 0x03
#define  CGBC_SBSM_BAT_0   0
#define  CGBC_SBSM_BAT_1   1
#define  CGBC_SBSM_BAT_2   2
#define  CGBC_SBSM_BAT_3   3
                                                                     //MOD007
#define  CGBC_BAT_PRESENT      0x01                                  //MOD006
#define  CGBC_BAT_CHARGING     0x02
#define  CGBC_BAT_DISCHARGING  0x04                                  //MOD007
#define  CGBC_BAT_ON_SMBUS     0x08                                  //MOD034
#define  CGBC_SBSM_REDIRECTION 0x10                                  //MOD017
#define  CGBC_SBSM_EMULATION   0x20                                  //MOD012
#define  CGBC_AC_ONLINE        0x40                           //MOD006 MOD007
#define  CGBC_SBSM_PRESENT     0x80                                  //MOD007
                                                                     //MOD005^
                                                                     //MOD021v
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_SBSM_DATA_OUT_STRUCT
   {
    unsigned char sts;                          // command status
    unsigned char pwrSts;                       // AC and battery status
   } CGBC_CMD_SBSM_DATA_OUT, *P_CGBC_CMD_SBSM_DATA_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD021^

                                                                   /*MOD038v*/
/*------------------------------------
 * Hardware monitoring data structure
 *------------------------------------
 */

typedef struct CGBC_HWM_DATA_HDR_STRUCT
    {
     uint8_t sts;                       /* hardware monitoring status byte  */
     uint8_t siz;                       /* size of HWM data space in bytes  */
     uint8_t rev;                       /* BC firmware revision byte        */
     uint8_t res;                       /* reserved                         */
    } CGBC_HWM_DATA_HDR, *P_CGBC_HWM_DATA_HDR;

typedef struct CGBC_HWM_SENSOR_STRUCT
    {
     uint8_t typ;                       /* hardware monitoring sensor type  */
     SINT16  val;                       /* hardware monitoring sensor value */
     uint8_t res;                       /* reserved                         */
    } CGBC_HWM_SENSOR, *P_CGBC_HWM_SENSOR;

//typedef struct CGBC_HWM_DATA_STRUCT
//    {
//     CGBC_HWM_DATA_HDR hdr;             /* HWM data space header            */
//     CGBC_HWM_SENSOR   sensorTbl[];     /* hardware monitoring sensor table */
//    } CGBC_HWM_DATA, *P_CGBC_HWM_DATA;
                                                                   /*MOD038^*/

                                                                     //MOD014v
/*--------------------------------------------
 * Hardware monitoring status byte (HWM_STAT)
 *--------------------------------------------
 */

#define CGBC_HWM_ACTIVE   0x80
#define CGBC_HWM_FEAT_MSK 0x0F



/*------------------------------------------------------
 * Board controller firmware revision byte (HWM_FW_REV)
 *------------------------------------------------------
 */

#define CGBC_HWM_MAJ_REV_MSK 0xF0
#define CGBC_HWM_MIN_REV_MSK 0x0F



/*-----------------------------------------------------
 * Hardware monitoring sensor type field (HWM_Sx_TYPE)
 *-----------------------------------------------------
 */

#define CGBC_SENSOR_ACTIVE      0x80

#define CGBC_SENSOR_GROUP_MSK   0x60
 #define CGBC_NO_SENSOR          0x00
 #define CGBC_TEMP_SENSOR        0x20
 #define CGBC_VOLTAGE_SENSOR     0x40
 #define CGBC_CURRENT_SENSOR     0x40                               /*MOD038*/
 #define CGBC_FAN_SENSOR         0x60

#define CGBC_SENSOR_TYPE_MSK    0x1F
 #define CGBC_SENSOR_RESERVED    0x00

 #define CGBC_TEMP_CPU           0x01
 #define CGBC_TEMP_BOX           0x02
 #define CGBC_TEMP_ENV           0x03
 #define CGBC_TEMP_BOARD         0x04
 #define CGBC_TEMP_BACKPLANE     0x05
 #define CGBC_TEMP_CHIPSETS      0x06
 #define CGBC_TEMP_VIDEO         0x07
 #define CGBC_TEMP_OTHER         0x08
 #define CGBC_TEMP_DIMM_TOP      0x09
 #define CGBC_TEMP_DIMM_BOTTOM   0x0A

 #define CGBC_VOLTAGE_CPU        0x01
 #define CGBC_VOLTAGE_DC         0x02
 #define CGBC_VOLTAGE_DC_STANDBY 0x03
 #define CGBC_VOLTAGE_BAT_CMOS   0x04
 #define CGBC_VOLTAGE_BAT_POWER  0x05
 #define CGBC_VOLTAGE_AC         0x06
 #define CGBC_VOLTAGE_OTHER      0x07
 #define CGBC_VOLTAGE_5V_S0      0x08
 #define CGBC_VOLTAGE_5V_S5      0x09
 #define CGBC_VOLTAGE_33V_S0     0x0A
 #define CGBC_VOLTAGE_33V_S5     0x0B
 #define CGBC_VOLTAGE_VCOREA     0x0C
 #define CGBC_VOLTAGE_VCOREB     0x0D
 #define CGBC_VOLTAGE_12V_S0     0x0E
 #define CGBC_VOLTAGE_12V_S5     0x0F                              /* MOD040 */
                                                                   /*MOD038v*/
 #define CGBC_CURRENT_DC         0x12
 #define CGBC_CURRENT_5V_S0      0x18
 #define CGBC_CURRENT_12V_S0     0x1E
                                                                   /*MOD038^*/
 #define CGBC_FAN_CPU            0x01
 #define CGBC_FAN_BOX            0x02
 #define CGBC_FAN_ENV            0x03
 #define CGBC_FAN_CHIPSET        0x04
 #define CGBC_FAN_VIDEO          0x05
 #define CGBC_FAN_OTHER          0x06
                                                                   /*MOD038v*/
/*---------------------------------------------------------------------
 * Hardware monitoring error codes in sensor value field (HWM_Sx_VALUE)
 *---------------------------------------------------------------------
 */

#define CGBC_HWM_NOT_PRESENT     0x00              /* sensor not present    */
#define CGBC_HWM_NOT_SUPPORTED   0x01              /* sensor not supported  */
#define CGBC_HWM_INACTIVE        0x02              /* sensor inactive       */
#define CGBC_HWM_BROKEN          0x03              /* sensor broken or open */
#define CGBC_HWM_SHORTCUT        0x04              /* sensor shortcut       */
                                                                   /*MOD038^*/
                                                                   /*MOD040v*/
/*------------------------
 * HWM reporting commands
 *------------------------
 */
#define HWM_CMD_BYTE_PON 0x02                            /* Power ON request */
#define HWM_CMD_BYTE_RSD 0x01                            /* ReSenD request   */

/*---------------------------------------------
 * CGBC_CMD_HWM_CONFIG command/response packet
 *---------------------------------------------
 */
typedef struct
CGBC_HWM_UNIT_COMMON_CONFIG_STRUCT /* This structure defines the common part of
 * the HWM unit config parameters supported by all types of HWM units. */
{
  uint8_t   ui8UnitMode;             /* see HWM unit mode byte               */
  uint8_t   ui8ErrorStatus;          /* see hardware monitoring error codes  */
  uint16_t ui16SampleRateMs;         /* HWM unit sample rate in milliseconds */
  uint32_t ui32Reserved;             /* reserved for future extensions       */
}
CGBC_HU_CFG, *P_CGBC_HU_CFG;

/* Bit definitions for the HWM unit mode byte (CGBC_HU_CFG.ui8UnitMode) */
#define CGBC_HU_ACTIVE            0x80   /* activated and operating          */
#define CGBC_HU_RUNTIME           0x40   /* S0 runtime power state supported */
#define CGBC_HU_STANDBY           0x20   /* Sx standby power state supported */
#define CGBC_HU_HOST_CONTROLLED   0x10   /* host controlled, no auto-on      */
#define CGBC_HU_FILTER_DEPTH_MASK 0x07   /* sensor/actuator I/O filter depth */
#define CGBC_HU_FILTER_DEPTH_128  0x07
#define CGBC_HU_FILTER_DEPTH_64   0x06
#define CGBC_HU_FILTER_DEPTH_32   0x05
#define CGBC_HU_FILTER_DEPTH_16   0x04
#define CGBC_HU_FILTER_DEPTH_8    0x03
#define CGBC_HU_FILTER_DEPTH_4    0x02
#define CGBC_HU_FILTER_DEPTH_2    0x01
#define CGBC_HU_FILTER_OFF        0x00

typedef struct
CGBC_HWM_SENSOR_UNIT_CONFIG_STRUCT /* This is the structure of the config
 * parameters for HWM sensor units. */
{
  CGBC_HU_CFG xCmn;                       /* see common config parameters    */
  uint8_t   ui8LogType;                   /* logical sensor type             */
  uint8_t   ui8PhyType;                   /* physical sensor type            */
  uint8_t   ui8BusNum;                    /* bus on which the sensor resides */
  uint8_t   ui8DevAddr;                   /* sensor device address           */
  uint32_t ui32Reserved;                  /* reserved for future extensions  */
}
CGBC_HSU_CFG, *P_CGBC_HSU_CFG;

/* Values for the logical sensor type (CGBC_HSU_CFG.ui8LogType) */
/* See sensor groups and types as for HWM_Sx_TYPE. */

/* Values for the physical sensor type (CGBC_HSU_CFG.ui8PhyType) */
#define CGBC_HSU_UNKNOWN    0x00
#define CGBC_HSU_INTEL_PECI 0x01
#define CGBC_HSU_AMD_SBTSI  0x02
#define CGBC_HSU_MCU_INT    0x03
#define CGBC_HSU_DIMM_SMB   0x04
#define CGBC_HSU_ISL28022   0x05
#define CGBC_HSU_NONE       0xFF

typedef struct
CGBC_HWM_ACTUATOR_UNIT_CONFIG_STRUCT /* This is the structure of the config
 * parameters for HWM actuator units. */
{
  CGBC_HU_CFG xCmn;                     /* see common config parameters      */
  uint8_t   ui8LogType;                 /* logical actuator type             */
  uint8_t   ui8PhyType;                 /* physical actuator type            */
  uint8_t   ui8BusNum;                  /* bus on which the actuator resides */
  uint8_t   ui8DevAddr;                 /* actuator device address           */
  uint16_t ui16SetMin;                  /* set point minimum                 */
  uint16_t ui16SetMax;                  /* set point maximum                 */
  uint16_t ui16SetDflt;                 /* default set point                 */
  uint16_t ui16SetCur;                  /* current set point                 */
  uint32_t ui32Reserved;                /* reserved for future extensions    */
}
CGBC_HAU_CFG, *P_CGBC_HAU_CFG;

/* Values for the logical actuator type (CGBC_HAU_CFG.ui8LogType) */
#define CGBC_HAU_RESERVED 0x00
#define CGBC_HAU_FAN_CPU  0x01

/* Values for the physical actuator type (CGBC_HAU_CFG.ui8PhyType) */
#define CGBC_HAU_UNKNOWN    0x00
#define CGBC_HAU_FAN_EC_100 0x01
#define CGBC_HAU_FAN_EC_RPM 0x02
#define CGBC_HAU_NONE       0xFF

typedef struct
CGBC_HWM_CONTROL_UNIT_CONFIG_STRUCT /* This is the structure of the config
 * parameters for HWM control units. */
{
  CGBC_HU_CFG xCmn;             /* see common config parameters              */
  uint16_t ui16LimitHi;         /* upper sensor limit                        */
  uint16_t ui16LimitLo;         /* lower sensor limit                        */
  uint8_t   ui8SetHi;           /* set point when sensor above upper limit   */
  uint8_t   ui8SetMidMax;       /* set point when sensor at mid range top    */
  uint8_t   ui8SetMidMin;       /* set point when sensor at mid range bottom */
  uint8_t   ui8SetLo;           /* set point when sensor below lower limit   */
  uint8_t   ui8SenSel;          /* HWM sensor selector                       */
  uint8_t   ui8ActSel;          /* HWM actuator selector                     */
  uint32_t ui32Reserved;        /* reserved for future extensions            */
}
CGBC_HCU_CFG, *P_CGBC_HCU_CFG;

typedef union
CGBC_HWM_UNIT_CONFIG_UNION /* This is the overall union of config parameters
 * for all types of HWM units. */
{
  CGBC_HU_CFG  xCmn;           /* see common config parameters               */
  CGBC_HSU_CFG xSen;           /* HWM sensor unit config parameters          */
  CGBC_HAU_CFG xAct;           /* HWM actuator unit config parameters        */
  CGBC_HCU_CFG xCtl;           /* HWM control unit config parameters         */
}
CGBC_HWM_UNIT_CFG, *P_CGBC_HWM_UNIT_CFG;

typedef struct
CGBC_CMD_HWM_CFG_IN_STRUCT /* command packet structure for CGBC_CMD_HWM_CFG */
{
  unsigned char   ui8Cmd;                      /* CGBC_CMD_HWM_CONFIG        */
  unsigned char   ui8Sel;                      /* HWM unit selector          */
  CGBC_HWM_UNIT_CFG xCfg;                      /* HWM unit config parameters */
} CGBC_CMD_HWM_CFG_IN, *P_CGBC_CMD_HWM_CFG_IN;

typedef struct
CGBC_CMD_HWM_CFG_OUT_STRUCT /* response data structure for CGBC_CMD_HWM_CFG */
{
  unsigned char   ui8Sts;                      /* command status             */
  unsigned char   ui8Sel;                      /* HWM unit selector          */
  CGBC_HWM_UNIT_CFG xCfg;                      /* HWM unit config parameters */
} CGBC_CMD_HWM_CFG_OUT, *P_CGBC_CMD_HWM_CFG_OUT;

/* Bit definitions for the HWM unit selector (CGBC_CMD_HWM_CFG_IO.ui8Sel) */
#define CGBC_HWM_CUP      0x80  /* configuration update request/status flag */

#define CGBC_HUT_MASK     0x60  /* HWM unit type */
#define CGBC_HUT_RESERVED 0x00
#define CGBC_HUT_SENSOR   0x20
#define CGBC_HUT_ACTUATOR 0x40
#define CGBC_HUT_CONTROL  0x60
#define CGBC_HUT_MASK_S   5

#define CGBC_HUN_MASK     0x1F  /* HWM unit number */
                                                                     //MOD040^
                                                                     //MOD023v
/*----------------------------------------
 * CGBC_CMD_PECI command/response packets
 *----------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_PECI_IN_STRUCT          // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_PECI
    unsigned char addr;                         // PECI client address
    unsigned char wrCnt;                        // PECI write count
    unsigned char rdCnt;                        // PECI read count
    unsigned char wrDat[32];                    // TxD
   } CGBC_CMD_PECI_IN, *P_CGBC_CMD_PECI_IN;

typedef struct CGBC_CMD_PECI_OUT_STRUCT         // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char rdDat[32];                    // RxD
   } CGBC_CMD_PECI_OUT, *P_CGBC_CMD_PECI_OUT;

#endif //!__ASSEMBLER__
                                                                     //MOD023^

                                                                   /*MOD024v*/
/*--------------------------------------------
 * CGBC_CMD_I2C_GATE command/response packets
 *--------------------------------------------
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_I2C_GATE_IN_STRUCT      // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_I2C_GATE
    unsigned char parm;                         // I2C gate control parameter
   } CGBC_CMD_I2C_GATE_IN, *P_CGBC_CMD_I2C_GATE_IN;

typedef struct CGBC_CMD_I2C_GATE_OUT_STRUCT     // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char parm;                         // I2C gate control parameter
   } CGBC_CMD_I2C_GATE_OUT, *P_CGBC_CMD_I2C_GATE_OUT;

#endif //!__ASSEMBLER__

/*
 * I2C gate control parameter bit mapping
 */
#define CGBC_I2C_GATE_GSB     0x80
#define CGBC_I2C_GATE_GS0     0x40
#define CGBC_I2C_GATE_GBT     0x20
#define CGBC_I2C_GATE_GAT     0x10
#define CGBC_I2C_GATE_GUP     0x08
#define CGBC_I2C_GATE_BUS_MSK 0x07
                                                                   /*MOD024^*/

                                                                   /*MOD025v*/
/*---------------------------------------------------------------------------
 * CGBC_CMD_CFG_PINS
 *
 * Input:  bits 7-6 - control mode
 *                    00 = no change / read current pin output states only
 *                    01 = update current pin states only
 *                    10 = update EEPROM content only
 *                    11 = update current pin states and EEPROM content
 *         bit  5-2 - reserved for more config pins
 *         bit  1   - new CONFIG_PIN_1 state
 *         bit  0   - new CONFIG_PIN_0 state
 *
 * Output: bits 7-6 - reserved
 *         bit  5-2 - reserved for more config pins
 *         bit  1   - current CONFIG_PIN_1 output state
 *         bit  0   - current CONFIG_PIN_0 output state
 */

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_CFG_PINS_IN_STRUCT      // command packet structure
   {
    unsigned char cmd;                          // CGBC_CMD_CFG_PINS
    unsigned char parm;                         // config pins parameter
   } CGBC_CMD_CFG_PINS_IN, *P_CGBC_CMD_CFG_PINS_IN;

typedef struct CGBC_CMD_CFG_PINS_OUT_STRUCT     // response packet structure
   {
    unsigned char sts;                          // command status
    unsigned char state;                        // current pin output state
   } CGBC_CMD_CFG_PINS_OUT, *P_CGBC_CMD_CFG_PINS_OUT;

#endif //!__ASSEMBLER__

/*
 * CGBC_CMD_CFG_PINS parameter/response bit mapping
 */
#define CGBC_CFG_PINS_MODE_MSK     0xC0
#define CGBC_CFG_PINS_MODE_EEP_CUR 0xC0
#define CGBC_CFG_PINS_MODE_EEP     0x80
#define CGBC_CFG_PINS_MODE_CUR     0x40
#define CGBC_CFG_PINS_MODE_READ    0x00

#define CGBC_CFG_PINS_MSK          0x3F
#define CGBC_CFG_PIN_5             0x20
#define CGBC_CFG_PIN_4             0x10
#define CGBC_CFG_PIN_3             0x08
#define CGBC_CFG_PIN_2             0x04
#define CGBC_CFG_PIN_1             0x02
#define CGBC_CFG_PIN_0             0x01
                                                                   /*MOD025^*/

                                                                    /*MOD038v*/
/*------------------
 * CGBC_CMD_AVR_SPM
 *------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_AVR_SPM_IN_STRUCT       /* command packet structure  */
  {
    uint8_t  cmd;                               /* CGBC_CMD_AVR_SPM          */
    uint32_t spmCmd;                            /* AVR SPM command           */
  } CGBC_CMD_AVR_SPM_IN, *P_CGBC_CMD_AVR_SPM_IN;

typedef struct CGBC_CMD_AVR_SPM_OUT_STRUCT      /* response packet structure */
   {
    uint8_t  sts;                               /* command status            */
    uint32_t spmRes;                            /* AVR SPM command response  */
   } CGBC_CMD_AVR_SPM_OUT, *P_CGBC_CMD_AVR_SPM_OUT;

#endif /* not __ASSEMBLER__ */
                                                                    /*MOD038^*/

                                                                   /*MOD031v*/
/*-------------------
 * CGBC_CMD_COMX-CFG
 *-------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_COMX_CFG_IN_STRUCT      /* command packet structure  */
  {
    uint8_t  cmd;                               /* CGBC_CMD_COMX_CFG         */
    uint8_t  parm;                              /* COMX config parameter     */
    uint16_t portBase;                          /* LPC I/O base address      */
    uint32_t baudrate;                          /* baudrate                  */
    uint8_t  lineCtrl;                          /* line control              */
    uint8_t  modemCtrl;                         /* modem control             */
  } CGBC_CMD_COMX_CFG_IN, *P_CGBC_CMD_COMX_CFG_IN;

typedef struct CGBC_CMD_COMX_CFG_OUT_STRUCT     /* response packet structure */
  {
    uint8_t  sts;                               /* command status            */
    uint8_t  parm;                              /* COMX config parameter     */
    uint16_t portBase;                          /* LPC I/O base address      */
    uint32_t baudrate;                          /* baudrate                  */
    uint8_t  lineCtrl;                          /* line control              */
    uint8_t  modemCtrl;                         /* modem control             */
  } CGBC_CMD_COMX_CFG_OUT, *P_CGBC_CMD_COMX_CFG_OUT;

#endif /* not __ASSEMBLER__ */

/* COMX configuration parameter */
#define COMX_CTRL_MODE_M       0xC0             /* control mode              */
#define COMX_CTRL_MODE_READ    0x00             /* read configuration only   */
#define COMX_CTRL_MODE_DISABLE 0x40             /* disable COM port          */
#define COMX_CTRL_MODE_ENABLE  0xC0             /* enable COM port           */
#define COMX_CTRL_MODE_S       6

#define COMX_PORT_SELECT_M     0x20             /* COM port select           */
#define COMX_PORT_SELECT_0     0x00             /* COM port 0                */
#define COMX_PORT_SELECT_1     0x20             /* COM port 1                */
#define COMX_PORT_SELECT_S     5

#define COMX_IRQ_MODE_M        0x10             /* IRQ mode                  */
#define COMX_IRQ_MODE_DISABLE  0x00             /* no IRQ allocated          */
#define COMX_IRQ_MODE_ENABLE   0x10             /* enabled / IRQ allocated   */
#define COMX_IRQ_MODE_S        4

#define COMX_IRQ_SELECT_M      0x0F             /* IRQ select                */
#define COMX_IRQ_SELECT_IRQ0   0
#define COMX_IRQ_SELECT_IRQ1   1
#define COMX_IRQ_SELECT_IRQ2   2
#define COMX_IRQ_SELECT_IRQ3   3
#define COMX_IRQ_SELECT_IRQ4   4
#define COMX_IRQ_SELECT_IRQ5   5
#define COMX_IRQ_SELECT_IRQ6   6
#define COMX_IRQ_SELECT_IRQ7   7
#define COMX_IRQ_SELECT_IRQ8   8
#define COMX_IRQ_SELECT_IRQ9   9
#define COMX_IRQ_SELECT_IRQ10  10
#define COMX_IRQ_SELECT_IRQ11  11
#define COMX_IRQ_SELECT_IRQ12  12
#define COMX_IRQ_SELECT_IRQ13  13
#define COMX_IRQ_SELECT_IRQ14  14
#define COMX_IRQ_SELECT_IRQ15  15
#define COMX_IRQ_SELECT_S      0
                                                                   /*MOD031^*/
                                                                   /*MOD040v*/
/*---------------------------------------------
 * Second COM port 1 from host LPC perspective
 *---------------------------------------------
 */
/* register offsets from the LPC base address BC_COM1_BASE */
#define BC_COM1_INDEX  0                       /* COM port 1 index register  */
#define BC_COM1_DATA   1                       /* COM port 1 data register   */
#define BC_COM1_STROBE 3                       /* COM port 1 strobe register */

/* COM port 1 index register BC_COM1_IDX */
#define BC_COM1_IDX_CMD_M 0xF8       /* command field */
#define BC_COM1_IDX_CMD_S 3

#define BC_COM1_IDX_IDX_M 0x07       /* index field */
#define BC_COM1_IDX_IDX_S 0

/* COM port 1 commands */
#define BC_COM1_CMD_READ_NS16550_REG  0x00
#define BC_COM1_CMD_WRITE_NS16550_REG 0x08
#define BC_COM1_CMD_READ_ID           0x10
                                                                   /*MOD040^*/

                                                                   /*MOD036v*/
/*------------------
 * CGBC_CMD_BLT_PWM
 *------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_BLT_PWM_IN_STRUCT       /* command packet structure  */
  {
    uint8_t  cmd;                               /* CGBC_CMD_COMX_CFG         */
    uint8_t  pwmDuty;                           /* backlight PWM duty cycle  */
    uint16_t pwmFreq;                           /* backlight PWM frequency   */
  } CGBC_CMD_BLT_PWM_IN, *P_CGBC_CMD_BLT_PWM_IN;

typedef struct CGBC_CMD_BLT_PWM_OUT_STRUCT      /* response packet structure */
  {
    uint8_t  sts;                               /* command status            */
    uint8_t  pwmDuty;                           /* backlight PWM duty cycle  */
    uint16_t pwmFreq;                           /* backlight PWM frequency   */
  } CGBC_CMD_BLT_PWM_OUT, *P_CGBC_CMD_BLT_PWM_OUT;

#endif /* not __ASSEMBLER__ */

/* backlight PWM duty cycle parameter bit masks */
#define BLT_PWM_DUTY_M      0x7F
#define BLT_PWM_DUTY_S      0

#define BLT_PWM_INVERTED_M  0x80
#define BLT_PWM_INVERTED_S  7
                                                                   /*MOD036^*/

                                                                   /* MOD038 */
                                                                   /*      v */
/*--------------------
 * CGBC_CMD_DEVICE_ID
 *--------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_DEVICE_ID_OUT_STRUCT    /* response packet structure */
  {
    uint8_t  sts;                               /* command status            */
    uint32_t did0;                              /* device identification 0   */
    uint32_t did1;                              /* device identification 1   */
  } CGBC_CMD_DEVICE_ID_OUT, *P_CGBC_CMD_DEVICE_ID_OUT;

#endif /* not __ASSEMBLER__ */

/*---------------------
 * CGBC_CMD_HWM_SENSOR
 *---------------------
 */
#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_HWM_SENSOR_IN_STRUCT    /* command packet structure  */
  {
    uint8_t cmd;                                /* CGBC_CMD_AVR_SPM          */
    uint8_t sensorNum;                          /* sensor number             */
  } CGBC_CMD_HWM_SENSOR_IN, *P_CGBC_CMD_HWM_SENSOR_IN;

typedef struct CGBC_CMD_HWM_SENSOR_OUT_STRUCT   /* response packet structure */
   {
    uint8_t         sts;                        /* command status            */
    uint8_t         sensorCount;                /* # of available sensors    */
    CGBC_HWM_SENSOR sensorDat;                  /* sensor data               */
   } CGBC_CMD_HWM_SENSOR_OUT, *P_CGBC_CMD_HWM_SENSOR_OUT;

#endif /* not __ASSEMBLER__ */
                                                                   /*      ^ */
                                                                   /* MOD038 */
                                                                   /* MOD040 */
                                                                   /*      v */
/*------------------------------
 * CGBC_CMD_POST_CFG parameters
 *------------------------------
 */
#ifndef __ASSEMBLER__

typedef struct
CGBC_POST_CFG_PARMS_STRUCT         /* POST Code Feature parameter structure  */
{
  uint8_t  ctrl;                                /* control parameter         */
  uint16_t ch1Addr;                             /* channel 1 I/O address     */
  uint8_t  relayDevAddr;                        /* relay device address      */
} CGBC_POST_CFG_PARMS, *P_CGBC_POST_CFG_PARMS;

typedef struct
CGBC_CMD_POST_CFG_IN_STRUCT                     /* command packet structure  */
{
  uint8_t             cmd;                      /* CGBC_CMD_POST_CFG         */
  CGBC_POST_CFG_PARMS parms;                    /* feature parameters        */
} CGBC_CMD_POST_CFG_IN, *P_CGBC_CMD_POST_CFG_IN;

typedef struct
CGBC_CMD_POST_CFG_OUT_STRUCT                    /* response packet structure */
{
  uint8_t  sts;                                 /* command status            */
  CGBC_POST_CFG_PARMS parms;                    /* feature parameters        */
} CGBC_CMD_POST_CFG_OUT, *P_CGBC_CMD_POST_CFG_OUT;

#endif //!__ASSEMBLER__

/* CGBC_CMD_POST_CFG control parameter bit mapping */
#define CGBC_POST_CFG_UPDATE    0x80   /* current config update request      */
#define CGBC_POST_EEP_UPDATE    0x40   /* EEPROM parameter update request    */
#define CGBC_POST_RAW_MODE      0x20   /* raw relay data mode                */

#define CGBC_POST_RELAY_IF_MSK  0x18   /* relay interface selection          */
#define  CGBC_POST_RELAY_OFF     0x00  /*   - relay off                      */
#define  CGBC_POST_RELAY_DIAG    0x08  /*   - diagnostic console             */
#define  CGBC_POST_RELAY_I2C     0x10  /*   - I2C-bus                        */
#define  CGBC_POST_RELAY_SMB     0x18  /*   - SMBus                          */

#define CGBC_POST_CH3_ENABLE    0x04   /* 3rd POST code input channel enable */
#define CGBC_POST_CH2_ENABLE    0x02   /* 2nd POST code input channel enable */
#define CGBC_POST_CH1_ENABLE    0x01   /* 1st POST code input channel enable */

/*-------------------------------
 * CGBC_CMD_LFP_DELAY parameters
 *-------------------------------
 */
typedef struct
CGBC_CMD_LFP_DELAY_IN_STRUCT                    /* command packet structure  */
{
  uint8_t cmd;                                  /* CGBC_CMD_LFP_DELAY        */
  uint8_t ui8RstDelay250ms;                     /* delay in units of 250ms   */
  uint8_t ui8BltDelay25ms;                      /* delay in units of 25ms    */
} CGBC_CMD_LFP_DELAY_IN, *P_CGBC_CMD_LFP_DELAY_IN;

typedef struct
CGBC_CMD_LFP_DELAY_OUT_STRUCT                   /* response packet structure */
{
  uint8_t sts;                                  /* command status            */
  uint8_t ui8RstDelay250ms;                     /* delay in units of 250ms   */
  uint8_t ui8BltDelay25ms;                      /* delay in units of 25ms    */
} CGBC_CMD_LFP_DELAY_OUT, *P_CGBC_CMD_LFP_DELAY_OUT;

#define CGBC_LFP_DELAY_READ 0xFF   /* reserved delay value for read function */
                                                                   /*      ^ */
                                                                   /* MOD040 */
                                                                   /* MOD041 */
                                                                   /*      v */
/*------------------------------
 * CGBC_CMD_DIAG_CFG parameters
 *------------------------------
 */
#ifndef __ASSEMBLER__

typedef struct
CGBC_DIAG_CON_CFG_STRUCT       /* diagnostic console configuration structure */
{
  uint8_t ui8Ctrl;                              /* control byte              */
  uint8_t ui8Parm;                              /* interface parameter       */
} CGBC_DIAG_CON_CFG, *P_CGBC_DIAG_CON_CFG;

typedef struct
CGBC_CMD_DIAG_CFG_IN_STRUCT                     /* command packet structure  */
{
  uint8_t         ui8Cmd;                       /* CGBC_CMD_DIAG_CFG         */
  CGBC_DIAG_CON_CFG xCfg;                       /* diagnostic console config */
} CGBC_CMD_DIAG_CFG_IN, *P_CGBC_CMD_DIAG_CFG_IN;

typedef struct
CGBC_CMD_DIAG_CFG_OUT_STRUCT                    /* response packet structure */
{
  uint8_t  sts;                                 /* command status            */
  CGBC_DIAG_CON_CFG xCfg;                       /* diagnostic console config */
} CGBC_CMD_DIAG_CFG_OUT, *P_CGBC_CMD_DIAG_CFG_OUT;

#endif //!__ASSEMBLER__

/* Diagnostic console control byte bit mapping */
#define CGBC_DIAG_CFG_UPDATE    0x80   /* current config update request      */
#define CGBC_DIAG_EEP_UPDATE    0x40   /* EEPROM parameter update request    */

#define CGBC_DIAG_RES_MSK       0x38   /* reserved / must be zero            */

#define CGBC_DIAG_IF_MSK        0x07   /* diagnostic console I/F select      */
#define  CGBC_DIAG_CON_OFF       0x00  /*   - diagnostic console disabled    */
#define  CGBC_DIAG_CON_AUX       0x01  /*   - auxiliary system serial port   */
#define  CGBC_DIAG_CON_SER0      0x02  /*   - system COM port 0              */
#define  CGBC_DIAG_CON_SER1      0x03  /*   - system COM port 1              */
                                                                   /*      ^ */
                                                                   /* MOD041 */
/*-------------------
 * SYNC return codes
 *-------------------
 */

#define CGBC_SYNC_BYTE0 0xAC
#define CGBC_SYNC_BYTE1 0x53

#ifndef __ASSEMBLER__

typedef struct CGBC_CMD_SYNC_OUT_STRUCT
   {
    unsigned char sts;                          // command status
    unsigned char byte0;                        // CGBC_SYNC_BYTE0
    unsigned char byte1;                        // CGBC_SYNC_BYTE1
   } CGBC_CMD_SYNC_OUT, *P_CGBC_CMD_SYNC_OUT;

#endif //!__ASSEMBLER__



/*--------------------
 * CGBD command union
 *--------------------
 */

#define MAX_HST_CMD_SIZ 38

#ifndef __ASSEMBLER__

typedef union CGBC_CMD_STRUCT
               {
                unsigned char             cmd;
                unsigned char             sts;
                unsigned char             cmdPkt[MAX_HST_CMD_SIZ];
                unsigned char             resPkt[CGBC_DAT_CNT_MSK+2];
                CGBC_CMD_FW_REV_OUT       revOut;
                CGBC_CMD_AUX_INIT_IN      auxInitIn;
                CGBC_WD_PARMS             wdParms;
                CGBC_CMD_SYS_FLAGS_1_IN   sysFlags1In;               //MOD021
                CGBC_CMD_SYS_FLAGS_1_OUT  sysFlags1Out;              //MOD021
                CGBC_CMD_INFO_1_OUT       info1Out;
                CGBC_CMD_LFP_CONTROL_IN   lfpControlIn;              //MOD040
                CGBC_CMD_VID_CONTROL_IN   vidControlIn;              //MOD021
                CGBC_CMD_LFP_CONTROL_OUT  lfpControlOut;             //MOD040
                CGBC_CMD_VID_CONTROL_OUT  vidControlOut;             //MOD021
                CGBC_CMD_TCNT_IN          tcntIn;                    //MOD021
                CGBC_CMD_TCNT_OUT         tcntOut;                   //MOD021
                CGBC_CMD_BOOT_TIME_OUT    btimOut;
                CGBC_CMD_DUMP_IN          dumpParms;
                CGBC_CMD_RESET_INFO_OUT   resetOut;                  //MOD021
                CGBC_CMD_POST_CODE_IN     postCodeIn;                //MOD040
                CGBC_CMD_XINT_CONTROL_IN  xintControlIn;             //MOD021
                CGBC_CMD_XINT_CONTROL_OUT xintControlOut;            //MOD021
                CGBC_CMD_USER_LOCK_IN     userLockIn;                //MOD021
                CGBC_CMD_CPU_TEMP_OUT     cpuTempOut;                //MOD021
                CGBC_CMD_SBSM_DATA_OUT    sbsmDataOut;               //MOD021
                CGBC_CMD_I2C_START_IN     i2cStartIn;
                CGBC_CMD_I2C_DATA_OUT     i2cDataOut;
                CGBC_CMD_I2C_SPEED_IN     i2cSpeedIn;                //MOD021
                CGBC_CMD_I2C_SPEED_OUT    i2cSpeedOut;               //MOD021
                CGBC_CMD_BTN_CONTROL_IN   btnControlIn;              //MOD017
                CGBC_CMD_BTN_CONTROL_OUT  btnControlOut;             //MOD017
                CGBC_CMD_FAN_CONTROL_IN   fanControlIn;              //MOD020
                CGBC_CMD_FAN_CONTROL_OUT  fanControlOut;             //MOD020
                CGBC_CMD_RTC_IN           rtcIn;                     //MOD022
                CGBC_CMD_RTC_OUT          rtcOut;                    //MOD022
                CGBC_CMD_GPIO_IN          gpioIn;                    //MOD021
                CGBC_CMD_GPIO_OUT         gpioOut;                   //MOD021
                CGBC_CMD_PECI_IN          peciIn;                    //MOD023
                CGBC_CMD_PECI_OUT         peciOut;                   //MOD023
                CGBC_CMD_I2C_GATE_IN      i2cGateIn;                 //MOD024
                CGBC_CMD_I2C_GATE_OUT     i2cGateOut;                //MOD024
                CGBC_CMD_CFG_PINS_IN      cfgPinsIn;                 //MOD025
                CGBC_CMD_CFG_PINS_OUT     cfgPinsOut;                //MOD025
                CGBC_CMD_AVR_SPM_IN       avrSpmIn;                  //MOD038
                CGBC_CMD_AVR_SPM_OUT      avrSpmOut;                 //MOD038
                CGBC_CMD_COMX_CFG_IN      comxCfgIn;                 //MOD031
                CGBC_CMD_COMX_CFG_OUT     comxCfgOut;                //MOD031
                CGBC_CMD_BLT_PWM_IN       bltPwmIn;                  //MOD036
                CGBC_CMD_BLT_PWM_OUT      bltPwmOut;                 //MOD036
                CGBC_CMD_DEVICE_ID_OUT    deviceIdOut;               //MOD038
                CGBC_CMD_HWM_SENSOR_IN    hwmSensorIn;               //MOD038
                CGBC_CMD_HWM_SENSOR_OUT   hwmSensorOut;              //MOD038
                CGBC_CMD_POST_CFG_IN      postCfgIn;                 //MOD040
                CGBC_CMD_POST_CFG_OUT     postCfgOut;                //MOD040
                CGBC_CMD_LFP_DELAY_IN     bltDelayIn;                //MOD040
                CGBC_CMD_LFP_DELAY_OUT    bltDelayOut;               //MOD040
                CGBC_CMD_HWM_CFG_IN       hwmCfgIn;                  //MOD040
                CGBC_CMD_HWM_CFG_OUT      hwmCfgOut;                 //MOD040
                CGBC_CMD_DIAG_CFG_IN      diagCfgIn;                 //MOD041
                CGBC_CMD_DIAG_CFG_OUT     diagCfgOut;                //MOD041
                CGBC_CMD_SYNC_OUT         sync;
               } CGBC_CMD, *P_CGBC_CMD;

#endif //!__ASSEMBLER__
                                                                     //MOD014^

                                                                     /*MOD027v*/
/*-----------------------------------------------
 * Host interface information structure HIF_INFO
 *-----------------------------------------------
 */
#ifndef __ASSEMBLER__

typedef struct HIF_INFO_STRUCT
{
  UINT32      id;                  /* HIF_ID                                 */
  UINT8       infoRev;             /* info structure revision                */
  UINT8       infoSize;            /* structure size in bytes                */
  UINT8       flags;               /* host interface control flags           */
  UINT8       res7;                /* reserved (offset 0x07)                 */
  UINT32      ctrlBase;            /* control block base address             */
  UINT32      shBase;              /* shared command channel base address    */
  UINT32      exBase;              /* exclusive command channel base address */
  UINT8       shSize;              /* shared command channel size            */
  UINT8       shIrq;               /* shared command channel IRQ control     */
  UINT8       exSize;              /* exclusive command channel size         */
  UINT8       exIrq;               /* exclusive command channel IRQ control  */
  CGBC_FW_REV fwRev;               /* CGBC firmware revision information     */
  UINT32      res1C;               /* reserved (offset 0x1C)                 */
} HIF_INFO, *P_HIF_INFO;

#endif //!__ASSEMBLER__

/* host interface identifier */
#define HIF_ID 0x434263DE       /* CGBC_ERR_HIF_RESET, 'c', 'B', 'C' **MOD037*/

/* Info structure revision */
#define HIF_INFO_REV_MAJ_MASK  0xF0                 /* major revision number */
#define HIF_INFO_REV_MAJ_SHIFT 4
#define HIF_INFO_REV_MIN_MASK  0x0F                 /* minor revision number */
#define HIF_INFO_REV_MIN_SHIFT 0
#define HIF_INFO_REV ((0<<HIF_INFO_REV_MAJ_SHIFT)|(1<<HIF_INFO_REV_MIN_SHIFT))

/* Host interface information structure size */
#define HIF_INFO_SIZE (sizeof (HIF_INFO))

/* host interface control flags */
#define HIF_FLAGS_INV 0x80                 /* control flags invalid          */
#define HIF_UPD       0x01                 /* request to update HIF defaults */

/* default LPC base addresses */
#define HIF_CTRL_BASE_DFLT 0x0C38       /* HIF control block base address    */
#define HIF_SH_BASE_DFLT   0x0C40       /* shared command channel base addr. */
#define HIF_EX_BASE_DFLT   0x0C80       /* excl. command channel base addr.  */

/* supported command channel sizes */
#define HIF_CH_SIZE_64  6
#define HIF_CH_SIZE_128 7
#define HIF_CH_SIZE_256 8
#define HIF_CH_SIZE_MIN HIF_CH_SIZE_64
#define HIF_CH_SIZE_MAX HIF_CH_SIZE_256

/* default command channel sizes */
#define HIF_SH_SIZE_DFLT HIF_CH_SIZE_64
#define HIF_EX_SIZE_DFLT HIF_CH_SIZE_64

/* IRQ control parameter */
#define HIF_IRQ_INV      0x80              /* IRQ control parameter invalid  */
#define HIF_IRQ_ENA      0x10              /* 0: IRQ enabled / 1: IRQ masked */
#define HIF_IRQ_NUM_MASK 0x0F              /* IRQ0 - IRQ15                   */

/*------------------------------
 * Host interface control block
 *------------------------------
 */
/* register offsets from control block base address HIF_CTRL_BASE */
#define HIF_ACCESS  0              /* host interface access control register */
#define HIF_RELEASE 0              /* host interface release register        */
#define HIF_STATUS  4              /* host interface status register         */

/* host interface status register HIF_ACCESS */
#define HIF_OCC_SHFT 4                    /* shared command channel occupied */
#define HIF_MIS_SHFT 5                    /* access number mismatch          */

#define HIF_OCC_MASK (1 << HIF_OCC_SHFT)
#define HIF_MIS_MASK (1 << HIF_OCC_SHFT)
                                                                     /*MOD027^*/


#ifndef __AVR__                                                      //MOD013
#pragma pack()                                                       //MOD001
#endif //!__AVR__                                                    //MOD013



#endif /*CGBC_H_INCLUDED*/

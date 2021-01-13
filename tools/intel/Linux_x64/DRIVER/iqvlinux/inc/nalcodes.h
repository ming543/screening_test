/*

  This file is provided under a dual BSD/GPLv2 license.  When using or
  redistributing this file, you may do so under either license.

  GPL LICENSE SUMMARY

  Copyright(c) 1999 - 2015 Intel Corporation. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
  The full GNU General Public License is included in this distribution
  in the file called LICENSE.GPL.

  Contact Information:
  e1000-devel Mailing List <e1000-devel@lists.sourceforge.net>
  Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497

  BSD LICENSE

  Copyright(c) 1999 - 2015 Intel Corporation. All rights reserved.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/*
 *
 * Module Name:
 *   nalcodes.h
 *
 * Abstract:
 *   This file contains the status codes.
 *
 */


#ifndef _NALCODES_H_
#define _NALCODES_H_ 

#include <naltypes.h>

#define NAL_CODE_SUCCESS 0x0
#define NAL_CODE_INFORMATIONAL 0x1
#define NAL_CODE_WARNING 0x2
#define NAL_CODE_ERROR 0x3

#define OEM_NONE 0x00
#define OEM_INTEL 0x86

#define CODE_GENERAL 0x0
#define CODE_NAL 0xA

#define MAKE_STATUS_CODE(type,custom,status) \
    ((UINT32)(((UINT32)(type)<<30) | \
    ((UINT32)(OEM_INTEL)<<20) | \
    ((UINT32)(custom)<<16) | \
    ((UINT32)(status))))

#ifdef NAL_DRIVER
#define NalMakeCode(_Type,_Library,_Number,_Description) MAKE_STATUS_CODE(_Type, _Library, _Number)
#else

typedef UINT32 NAL_LINK_MODE;

#define NAL_MAX_CODES 1000

typedef struct _NAL_CODE_STRUCT
{
    BOOLEAN InUse;
    UINT32 Status;
    CHAR* Description;
} NAL_CODE_STRUCT;

extern NAL_CODE_STRUCT Global_CodeStruct[NAL_MAX_CODES];

NAL_STATUS
NalMakeCode(
    IN UINT32 Type,
    IN UINT32 Library,
    IN UINT32 Number,
    IN CHAR* Description
    );

CHAR*
NalGetStatusCodeDescription(
    IN NAL_STATUS Status
    );

#endif

#ifndef RC_INVOKED

#define NAL_SUCCESS 0
#define NAL_INVALID_PARAMETER 1

#define NAL_CODE_LIST \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_NOT_ENOUGH_SPACE, NAL_CODE_ERROR, CODE_NAL, 0x0002, "Not enough space") \
    NAL_MAKE_CODE(NAL_NOT_IMPLEMENTED, NAL_CODE_ERROR, CODE_NAL, 0x0003, "Not Implemented") \
    NAL_MAKE_CODE(NAL_TIMEOUT_ERROR, NAL_CODE_ERROR, CODE_NAL, 0x0004, "Timeout Error") \
    NAL_MAKE_CODE(NAL_NOT_ENABLED, NAL_CODE_ERROR, CODE_NAL, 0x0005, "Feature not enabled in HW") \
    NAL_MAKE_CODE(NAL_CONFIGURATION_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x0006, "Configuration failed") \
    NAL_MAKE_CODE(NAL_FEATURE_NOT_SUPPORTED, NAL_CODE_ERROR, CODE_NAL, 0x0007, "Feature is not supported in current mode") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_AQ_INITIALIZATION_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x0A00, "Admin Queue initialization failed") \
    NAL_MAKE_CODE(NAL_AQ_ALREADY_INITIALIZED, NAL_CODE_ERROR, CODE_NAL, 0x0A01, "Admin Queue is already initialized") \
    NAL_MAKE_CODE(NAL_AQ_COMMAND_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x0A02, "Admin Queue command failed") \
    NAL_MAKE_CODE(NAL_AQ_SEND_COMMAND_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x0A03, "Send Admin Queue command failed") \
    NAL_MAKE_CODE(NAL_AQ_COMMAND_TIMEOUT, NAL_CODE_ERROR, CODE_NAL, 0x0A04, "Admin Queue command timeout") \
    NAL_MAKE_CODE(NAL_AQ_MISMATCH_VERSION, NAL_CODE_ERROR, CODE_NAL, 0x0A05, "Admin Queue API version is not supported by this software") \
    NAL_MAKE_CODE(NAL_AQ_DEBUG_INIT_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x0AD0, "Tools Admin Queue initialization failed. Queue may be in use") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_INITIALIZATION_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x8001, "Initialization Failed") \
    NAL_MAKE_CODE(NAL_IO_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x8002, "IO Failure") \
    NAL_MAKE_CODE(NAL_MMAP_ADDRESS_IN_USE, NAL_CODE_ERROR, CODE_NAL, 0x8003, "Memory Map Address In Use") \
    NAL_MAKE_CODE(NAL_MMAP_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x8004, "Memory Mapping Failed") \
    NAL_MAKE_CODE(NAL_MMAP_ADDRESS_NOT_MAPPED, NAL_CODE_ERROR, CODE_NAL, 0x8005, "Memory Map Address Not Mapped") \
    NAL_MAKE_CODE(NAL_INVALID_VECTOR, NAL_CODE_ERROR, CODE_NAL, 0x8006, "Invalid IRQ Vector") \
    NAL_MAKE_CODE(NAL_VECTOR_INITIALIZATION_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x8007, "IRQ Vector Init Failed") \
    NAL_MAKE_CODE(NAL_SPINLOCK_FAILURE, NAL_CODE_ERROR, CODE_NAL, 0x8008, "Spinlock Failure") \
    NAL_MAKE_CODE(NAL_SECURITY_ACCESS_DENIED, NAL_CODE_ERROR, CODE_NAL, 0x8009, "Access Denied") \
    NAL_MAKE_CODE(NAL_DEBUGPRINT_NO_SUPPORT, NAL_CODE_ERROR, CODE_NAL, 0x800A, "No Debug Print Support") \
    NAL_MAKE_CODE(NAL_DEBUGPRINT_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x800B, "Debug Print Failed") \
    NAL_MAKE_CODE(NAL_TIMER_CALLBACK_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x800C, "Timer Callback Failed") \
    NAL_MAKE_CODE(NAL_MEMORY_BAR_INVALID, NAL_CODE_ERROR, CODE_NAL, 0x800E, "No PCI memory resources assigned by BIOS or OS!") \
    NAL_MAKE_CODE(NAL_INCORRECT_OS, NAL_CODE_ERROR, CODE_NAL, 0x800F, "Incorrect OS") \
    NAL_MAKE_CODE(NAL_NO_DEBUG_STACK_SPACE, NAL_CODE_ERROR, CODE_NAL, 0x8010, "Debug Stack Space Is Full") \
    NAL_MAKE_CODE(NAL_THREAD_CREATE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x8011, "Failed to Create Thread") \
    NAL_MAKE_CODE(NAL_INITIALIZATION_BASE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x8012, "Initialization Failed. Please unload device driver") \
    NAL_MAKE_CODE(NAL_INITIALIZATION_DLM_BASE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x8013, "Initialization in DLM Failed. Not capable device driver") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_INVALID_PCI_SLOT_ID, NAL_CODE_ERROR, CODE_NAL, 0x4002, "Invalid PCI Slot") \
    NAL_MAKE_CODE(NAL_PCICONFIG_NOT_AVAILABLE, NAL_CODE_ERROR, CODE_NAL, 0x4003, "PCI Config is not available") \
    NAL_MAKE_CODE(NAL_NOT_A_VALID_SLOT, NAL_CODE_ERROR, CODE_NAL, 0x4006, "Not a valid PCI slot") \
    NAL_MAKE_CODE(NAL_NOT_A_VALID_BUS, NAL_CODE_ERROR, CODE_NAL, 0x4007, "Invalid bus") \
    NAL_MAKE_CODE(NAL_PCI_CAPABILITY_NOT_FOUND, NAL_CODE_ERROR, CODE_NAL, 0x4008, "PCI Capability not found") \
    NAL_MAKE_CODE(NAL_IO_CALL_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x4009, "IO Driver Call failed") \
    NAL_MAKE_CODE(NAL_DMA_NOT_SUPPORTED, NAL_CODE_ERROR, CODE_NAL, 0x4010, "No usable DMA configuration") \
    NAL_MAKE_CODE(NAL_PCI_D3_STATE, NAL_CODE_ERROR, CODE_NAL, 0x4011, "PCI Device in D3 state") \
    NAL_MAKE_CODE(NAL_IOSF_ACCESS_ERROR, NAL_CODE_ERROR, CODE_NAL, 0x4012, "IOSF access failed") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_INVALID_ADAPTER_HANDLE, NAL_CODE_ERROR, CODE_NAL, 0x2001, "Adapter handle is invalid") \
    NAL_MAKE_CODE(NAL_ADAPTER_INITIALIZATION_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2002, "Adapter initialization failed") \
    NAL_MAKE_CODE(NAL_ADAPTER_START_REQUIRED, NAL_CODE_ERROR, CODE_NAL, 0x2003, "Adapter start required for this operation") \
    NAL_MAKE_CODE(NAL_ADAPTER_STOP_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2004, "Adapter stop failed") \
    NAL_MAKE_CODE(NAL_ADAPTER_RESET_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2005, "Adapter reset failed") \
    NAL_MAKE_CODE(NAL_INVALID_MAC_REGISTER, NAL_CODE_ERROR, CODE_NAL, 0x2006, "Invalid MAC register") \
    NAL_MAKE_CODE(NAL_INVALID_PHY_REGISTER, NAL_CODE_ERROR, CODE_NAL, 0x2007, "Invalid PHY register") \
    NAL_MAKE_CODE(NAL_NO_LINK, NAL_CODE_ERROR, CODE_NAL, 0x2008, "Adapter has no link") \
    NAL_MAKE_CODE(NAL_EEPROM_DOES_NOT_EXIST, NAL_CODE_ERROR, CODE_NAL, 0x2009, "Adapter has no EEPROM") \
    NAL_MAKE_CODE(NAL_EEPROM_BAD_INDEX, NAL_CODE_ERROR, CODE_NAL, 0x200A, "EEPROM index is bad or out of range") \
    NAL_MAKE_CODE(NAL_EEPROM_BAD_IMAGE, NAL_CODE_ERROR, CODE_NAL, 0x200B, "EEPROM image is bad") \
    NAL_MAKE_CODE(NAL_EEPROM_WRITE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x200C, "EEPROM write failure") \
    NAL_MAKE_CODE(NAL_FLASH_DOES_NOT_EXIST, NAL_CODE_ERROR, CODE_NAL, 0x200D, "Flash does not exist") \
    NAL_MAKE_CODE(NAL_FLASH_ID_UNKNOWN, NAL_CODE_SUCCESS, CODE_NAL, 0x200E,"Flash ID is unknown") \
    NAL_MAKE_CODE(NAL_FLASH_BAD_INDEX, NAL_CODE_ERROR, CODE_NAL, 0x200F, "Flash index is bad or our of range") \
    NAL_MAKE_CODE(NAL_FLASH_BAD_IMAGE, NAL_CODE_ERROR, CODE_NAL, 0x2010, "Flash image is bad") \
    NAL_MAKE_CODE(NAL_FLASH_WRITE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2011, "Flash write failed") \
    NAL_MAKE_CODE(NAL_FLASH_READ_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2011, "Flash read failed") \
    NAL_MAKE_CODE(NAL_ADAPTER_HANDLE_IN_USE, NAL_CODE_ERROR, CODE_NAL, 0x2012, "Adapter handle is in use") \
    NAL_MAKE_CODE(NAL_RESOURCE_ALLOCATION_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2013, "Resource allocation failed") \
    NAL_MAKE_CODE(NAL_RESOURCE_NOT_AVAILABLE, NAL_CODE_ERROR, CODE_NAL, 0x2014, "Resource is unavailable") \
    NAL_MAKE_CODE(NAL_CONNECTION_TO_DRIVER_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2015, "Connection to driver failed") \
    NAL_MAKE_CODE(NAL_DRIVER_HANDLE_INVALID, NAL_CODE_ERROR, CODE_NAL, 0x2016, "Invalid Driver Handle") \
    NAL_MAKE_CODE(NAL_DRIVER_IOCTL_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2017, "IOCTL to driver failed") \
    NAL_MAKE_CODE(NAL_IOCTL_INVALID_FUNCTION_ID, NAL_CODE_ERROR, CODE_NAL, 0x2018, "IOCTL to invalid function ID") \
    NAL_MAKE_CODE(NAL_HARDWARE_FAILURE, NAL_CODE_ERROR, CODE_NAL, 0x2019, "Hardware Failure") \
    NAL_MAKE_CODE(NAL_ADAPTER_IN_USE, NAL_CODE_ERROR, CODE_NAL, 0x201A, "Adapter is already in use") \
    NAL_MAKE_CODE(NAL_EEPROM_SIZE_INCORRECT, NAL_CODE_ERROR, CODE_NAL, 0x201B, "EEPROM size is incorrect") \
    NAL_MAKE_CODE(NAL_HOST_IF_COMMAND_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x201C, "Host interface command failure") \
    NAL_MAKE_CODE(NAL_WRITE_EEPROM_SIZE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x201D, "Writing of EEPROM size failed") \
    NAL_MAKE_CODE(NAL_NO_MODULE_VALIDITY_SIGNATURE, NAL_CODE_ERROR, CODE_NAL, 0x201E, "Module does not contain validity signature") \
    NAL_MAKE_CODE(NAL_WRONG_MODULE_FOR_DEVICE, NAL_CODE_ERROR, CODE_NAL, 0x201F, "This module does not support this device") \
    NAL_MAKE_CODE(NAL_DEVICE_DRIVER_UNLOAD_REQUIRED, NAL_CODE_ERROR, CODE_NAL, 0x2020, "OS Device driver must be unloaded for this operation") \
    NAL_MAKE_CODE(NAL_DEVICE_DRIVER_RELOAD_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2021, "The OS device driver could not be reloaded") \
    NAL_MAKE_CODE(NAL_PACKET_SIZE_TOO_LARGE, NAL_CODE_ERROR, CODE_NAL, 0x2022, "The packet size is too large for this adapter") \
    NAL_MAKE_CODE(NAL_NO_RECEIVE_PENDING, NAL_CODE_ERROR, CODE_NAL, 0x2023, "No receive is pending") \
    NAL_MAKE_CODE(NAL_TRANSMIT_TIMEOUT, NAL_CODE_ERROR, CODE_NAL, 0x2024, "Transmit packet timed out") \
    NAL_MAKE_CODE(NAL_ERASE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2025, "Flash could not be erased") \
    NAL_MAKE_CODE(NAL_ADAPTER_DOES_NOT_SUPPORT, NAL_CODE_ERROR, CODE_NAL, 0x2026, "The adapter does not support this feature") \
    NAL_MAKE_CODE(NAL_HEAD_WRITEBACK_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2027, "Head Writeback failed") \
    NAL_MAKE_CODE(NAL_ADAPTER_IN_USE_ISCSI, NAL_CODE_ERROR, CODE_NAL, 0x2028, "Adapter is in use for iSCSI and cannot be initialized") \
    NAL_MAKE_CODE(NAL_EEPROM_READ_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2029, "Failed to read EEPROM or EEPROM image.") \
    NAL_MAKE_CODE(NAL_EEPROM_CALCULATION_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x202A, "Failed to calculate Manageability CRC/Checksum.") \
    NAL_MAKE_CODE(NAL_EEPROM_ASF1_CRC_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x202B, "ASF1 CRC validation failed.") \
    NAL_MAKE_CODE(NAL_EEPROM_ASF2_CSUM_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x202C, "ASF2 Checksum validation failed.") \
    NAL_MAKE_CODE(NAL_EEPROM_ASF2_CRC_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x202D, "Failed to calculate Manageability CRC/Checksum.") \
    NAL_MAKE_CODE(NAL_RESOURCE_LESS_THAN_REQUESTED, NAL_CODE_ERROR, CODE_NAL, 0x202E, "Resource allocation succeeded but allocated less than requested.") \
    NAL_MAKE_CODE(NAL_REGISTER_CHECK_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x202F, "The register test for some value failed." ) \
    NAL_MAKE_CODE(NAL_TIMESYNC_NO_TIMESTAMP, NAL_CODE_ERROR, CODE_NAL, 0x2030, "No timestamp found") \
    NAL_MAKE_CODE(NAL_FLASH_IS_NOT_MAPPED, NAL_CODE_ERROR, CODE_NAL, 0x2031, "Flash is not mapped in the memory BAR") \
    NAL_MAKE_CODE(NAL_HMC_NOT_INITIALIZED, NAL_CODE_ERROR, CODE_NAL, 0x2032, "HMC is not initialized") \
    NAL_MAKE_CODE(NAL_HMC_PAGE_NOT_ALLOCATED, NAL_CODE_ERROR, CODE_NAL, 0x2033, "Requested HMC page is not allocated") \
    NAL_MAKE_CODE(NAL_HMC_PAGE_NOT_VALID, NAL_CODE_ERROR, CODE_NAL, 0x2034, "Requested HMC page is not marked valid") \
    NAL_MAKE_CODE(NAL_FLASH_REGION_PROTECTED, NAL_CODE_ERROR, CODE_NAL, 0x2035, "Flash region protected") \
    NAL_MAKE_CODE(NAL_FLASH_REGION_EMPTY, NAL_CODE_ERROR, CODE_NAL, 0x2036, "Flash region empty") \
    NAL_MAKE_CODE(NAL_EEPROM_MERGE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2037, "Cannot merge EEPROM images") \
    NAL_MAKE_CODE(NAL_EEPROM_POINTERS_CORRUPTED, NAL_CODE_ERROR, CODE_NAL, 0x2038, "Pointers in Shadow RAM are corrupted") \
    NAL_MAKE_CODE(NAL_FLASH_AUTHENTICATION_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2039, "FLASHimage authentication failed") \
    NAL_MAKE_CODE(NAL_FLASH_FW_AUTHENTICATION_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x203A, "Current firmware authentication failed - try performing full power cycle") \
    NAL_MAKE_CODE(NAL_FLASH_FW_AUTHENTICATION_TIMEOUT,NAL_CODE_ERROR, CODE_NAL, 0x203B, "Firmware authentication timeout") \
    NAL_MAKE_CODE(NAL_MPHY_READ_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x203C, "mPHY reading failed") \
    NAL_MAKE_CODE(NAL_MPHY_WRITE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x203D, "mPHY writing failed") \
    NAL_MAKE_CODE(NAL_EEPROM_RO_WORD_WRITE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x203E, "Attempt to write RO word failed") \
    NAL_MAKE_CODE(NAL_FLASH_DEVICE_TOO_SMALL, NAL_CODE_ERROR, CODE_NAL, 0x203F, "Flash device is too small for this image") \
    NAL_MAKE_CODE(NAL_ALTRAM_READ_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2040, "AltRAM read failed") \
    NAL_MAKE_CODE(NAL_ALTRAM_WRITE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2041, "AltRAM write failed") \
    NAL_MAKE_CODE(NAL_EEPROM_FW_CRC_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2042, "Failed to calculate Firmware CRC/Checksum.") \
    NAL_MAKE_CODE(NAL_EEPROM_FW_CSUM_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2043, "Firmware Checksum validation failed.") \
    NAL_MAKE_CODE(NAL_FLASH_IMAGE_SYNC_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2044, "Different instance of image update in progress, retry again later or try performing full power cycle") \
    NAL_MAKE_CODE(NAL_FLASH_BUSY, NAL_CODE_ERROR, CODE_NAL, 0x2045, "Flash is busy.") \
    NAL_MAKE_CODE(NAL_FLASH_READING, NAL_CODE_SUCCESS, CODE_NAL, 0x2046, "Flash reading process is active.") \
    NAL_MAKE_CODE(NAL_FLASH_WRITING, NAL_CODE_SUCCESS, CODE_NAL, 0x2047, "Flash writing process is active.") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_NVM_IMG_UPDATE_FAILED, NAL_CODE_ERROR, CODE_NAL, 0x2FFF, "NVM Image/Module update failed.") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_RSDP_TABLE_NOT_FOUND, NAL_CODE_ERROR, CODE_NAL, 0x1001, "RSDP BIOS Table was not found") \
    NAL_MAKE_CODE(NAL_ACPI_TABLE_NOT_FOUND, NAL_CODE_ERROR, CODE_NAL, 0x1002, "ACPI BIOS Table was not found") \
    NAL_MAKE_CODE(NAL_PCIE_TABLE_NOT_FOUND, NAL_CODE_ERROR, CODE_NAL, 0x1003, "PCIE BIOS Table was not found") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_QUEUE_NOT_DISABLED, NAL_CODE_ERROR, CODE_NAL, 0x1004, "Failed to disable the queue") \
    NAL_MAKE_CODE(NAL_QUEUE_NOT_ENABLED, NAL_CODE_ERROR, CODE_NAL, 0x1005, "Failed to enable the queue") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_PROTECTION_DOMAIN_MISMATCH, NAL_CODE_ERROR, CODE_NAL, 0x1007, "Protection Domain Mismatch") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_OTP_CANT_BE_UPDATED, NAL_CODE_ERROR, CODE_NAL, 0x1008, "OTP can't be updated") \
    NAL_MAKE_CODE(NAL_OTP_ACCESS_ERROR, NAL_CODE_ERROR, CODE_NAL, 0x1009, "OTP access failed.") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_SFP_EEPROM_ACCESS_ERROR, NAL_CODE_ERROR, CODE_NAL, 0x100A, "SFP EEPROM access failed.") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_ICSP_NOT_ENABLED, NAL_CODE_ERROR, CODE_NAL, 0x100B, "ICSP Protocol is not enabled.") \
    NAL_MAKE_CODE(NAL_ICSP_ID_UNKNOWN, NAL_CODE_ERROR, CODE_NAL, 0x100C, "Unknown Microcontroller Device ID.") \
                                                                                                                                                                    \
    NAL_MAKE_CODE(NAL_PHY_MODE_UNSUPPORTED, NAL_CODE_ERROR, CODE_NAL, 0x100D, "Current PHY mode is not supported.") \

#define NAL_MAKE_CODE(a,b,c,d,e) a = MAKE_STATUS_CODE(b, c, d),
enum { NAL_CODE_LIST };
#undef NAL_MAKE_CODE

#define NAL_PCISCANBUS_NOT_ENOUGH_SPACE NAL_NOT_ENOUGH_SPACE
#define NAL_NVM_IMG_UPDATE_FAILED_MASK 0x000000FF

#endif
#endif

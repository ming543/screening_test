/************************************************************************************
	File:             stdafx.h
	Description:      Build on Linux Compatible as Windows
	Company:          EverFine Industrial Co. Ltd.
	Author:           Tom.Hsu
	Modify Date:      2016/4/24
	Version:          1.00.01
*************************************************************************************/
#pragma once

#include "OsAL.h"

#if defined(__GNUC__)
	#include <inttypes.h>
	#include <sys/io.h>        // iopl, outb, inb, outl, inl
	#include <pthread.h>       // pthread_*
	#include <string.h>        // strncpy
	#include <fcntl.h>         // O_*
	#include <stdio.h>         // O_*
#endif

#include "resource.h"
#define _DBG_TEXT_                              1
#define _EKIOCTL_IMPORT_LIB_
#include "EKIOCTL.h"
#define _DMCI_DEF_EXPORT_
#include "DMCI.h"
#define _DDMI_DEF_EXPORT_
#include "DDMI.h"

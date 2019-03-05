/*******************************************************************************
* Copyright (c) 2012 Atmel Corporation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel microcontroller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/

#ifndef COMPILER_H
#define COMPILER_H

/*******************************************************************************
* File Name    : (compiler.h)
* Version      : (v1.0)
* Device(s)    : (ATmega328P)
* OS           : (if applicable)
* H/W Platform : (ATAB0101A-V1.0)
* Description  : (compiler initialisation)
*******************************************************************************/

/*******************************************************************************
* Included Header Files
*******************************************************************************/

/*******************************************************************************
* Macro (#define) Definitions
*******************************************************************************/

// Compiler is selected through a symbol defined in the build environment

#if defined(IAR_AVR_COMPILER)
#include "compiler_iar_avr.h"

#elif defined(AS6_GCC_AVR_COMPILER)
#include "compiler_as6_gcc_avr.h"

#else
#error "NO COMPILER SELECTED"
#endif

/*******************************************************************************
* History 
* : DD.MM.YYYY  Version  Description
* : 09.12.2014  1.0      Created initial structure
*
*******************************************************************************/

#endif

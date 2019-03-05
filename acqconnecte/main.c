/***********************************************************************************************************************
* Copyright (c) 2012 Atmel Corporation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
* following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
*    disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
*    following disclaimer in the documentation and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived from this software without specific 
*    prior written permission.
*
* 4. This software may only be redistributed and used in connection with an Atmel microcontroller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
* THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE EXPRESSLY AND 
* SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
* OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : (main.c)
* Version      : (v1.0)
* Device(s)    : (ATmega328P)
* OS           : (if applicable)
* H/W Platform : (ATAB0101A-V1.0)
* Description  : (main() function for SIGFOX application reading out temperature sensor and battery voltage)
***********************************************************************************************************************/

/***********************************************************************************************************************
* Included Header Files
***********************************************************************************************************************/

#include "STANDALONE328P_hal.h"
#include "application.h"
#include "compiler.h"

/***********************************************************************************************************************
* Global Variable Definitions
* - These can be accessed by functions outside this file
* - Note: Each global variable should be made "extern" in the associated header
* file
***********************************************************************************************************************/
FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE mystdin  = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
volatile FLAG_T timer2_event = FALSE;
volatile FLAG_T btn_event = FALSE;

/***********************************************************************************************************************
* Function Name: main()
* Description : For SIGFOX application reading out temperature sensor and battery voltage
* Arguments : none
* Return Value : none
***********************************************************************************************************************/
int main(void)
{
	unsigned char i;
	union valunion val;
	unsigned char twidata[2];
	
	// init io, led and sensor power
	at_init_io();
	at_set_led( TRUE );
	at_set_pwr( TRUE );
	
	// init general purpose timer 0
	// and timer 2 for sleep period
	at_init_timer0();
	at_init_timer2();
	
	// init ata8520 SIGFOX device
	rf_ata8520_init();
	
	// switch led off
	_delay_ms(200);
	at_set_led( FALSE );
	
	// Init TWI for temperature sensor
	twi_init();

	// init UART (38.4kbaud, 8bit, 1stop, no parity)
	usart_init();
	stdout = &mystdout;
	stdin  = &mystdin;

	printf("\r\n\r\n---------------- Xplained mini Demo (c)2014 Atmel V1.0.1 ----------------\r\n");
	printf("                    ATmega328P ATA8520-V%02X.%02X\r\n",rf.version[0],rf.version[1]);
	printf("                       Sensor Sigfox Transmitter\r\n");
	printf("---------------------------------------------------------------------------\r\n");
	printf("                    ID  = %02X %02X %02X %02X\r\n",rf.ID[3],rf.ID[2],rf.ID[1],rf.ID[0]);
	printf("                    PAC = ");
	for(i=0; i<8; i++)
	{
		printf("%2X ",rf.PAC[i]);
	}
	printf("\r\n---------------------------------------------------------------------------\r\n");
	printf("\r\nwait for Timer... ");
	
	// switch power off and enable interrupts
	_delay_ms(100);
	at_set_pwr( FALSE );
	EnableInterrupts();
	timer2_event = FALSE;
	btn_event = FALSE;

	while(1)
	{
		// enter sleep mode
		sleep_mode();
		
		// check for Timer2 or button event and read T Sensor
		if ((timer2_event == TRUE) || (btn_event == TRUE))
		{
			// switch led and sensor power on
	        at_set_pwr( TRUE );
			_delay_ms(35);
	        at_set_led( TRUE );
						
			// init ATA8520 SIGFOX device
			rf_ata8520_reinit();
			// initialize SPI for communication
			_delay_ms(1);
			at_enable_spi();
			
			// read temp. data from sensor
			if(twi_read(TSENS_ADDR, twidata, 2) == (unsigned char) TWI_RXDATA_NAK)
			{
				// convert 9-bit MSB aligned 2-complementary data into 16-bit int data
				val.u16    = 0;
				val.u8[1] = twidata[0];
				val.u8[0] = twidata[1];
				val.i16   /= 256;
				// clear buffer and copy data into right transmit order
				for(i=0; i<9; i++) rf.buffer[i]  = 0x00;
				// 8 bit temp value
				rf.buffer[9]  = val.u8[0];
				// read supply voltage from channel 0 in mV resolution with 1.1V reference
				val.u16 = (((long int) adc_read(0)) * 4070L) / 1024L;
				rf.buffer[10]  = val.u8[1];
				rf.buffer[11]  = val.u8[0];
				printf("U=%dmV  T=%d°C", val.i16, (int) rf.buffer[9]);
			}
			else
			{
				// clear buffer
				for(i=0; i<10; i++) rf.buffer[i]  = 0x00;
				// 16 bit voltage value = 99mV (error value)
				rf.buffer[10] = 0x00;
				rf.buffer[11] = 0x63;
				printf("T sensor error!");
			}
			
			// switch sensor power off
	        at_set_pwr( FALSE );
			
			// write TX buffer
			rf_ata8520_write_tx_buf( rf.buffer, 12 );
			// start tx mode
			rf_ata8520_start_tx( );
			// check for IRQ signal (RF Event, will take ~6s)
			do 
			{
				// blink LED to show activity
				at_set_led( FALSE );
				_delay_ms(200);
				at_set_led( TRUE );
				_delay_ms(100);

			} while (at_io_rd_chan(RF_EVPIN, RF_EVCHNL) != 0);
			// read RF event bytes
			rf_ata8520_get_status( rf.status );
			
		    // switch ATA8520 into OFF mode
		    rf_ata8520_set_OFFmode();
			
			// disable SPI
			at_disable_spi();
			
			printf(" (status= %02X %02X %02X)",rf.status[0],rf.status[1],rf.status[2]);
			printf("\r\nwait for Timer... ");

			// switch LED off
			at_set_led( FALSE );
			
			// check if button is released
			while(at_test_btn() == TRUE)
			{
				_delay_ms(100);
			}
			// clear events
			timer2_event = FALSE;
			btn_event = FALSE;
		}
	}
}

/***********************************************************************************************************************
* History 
* : DD.MM.YYYY  Version  Description
* : 09.12.2014  1.0      Created initial structure
*
***********************************************************************************************************************/



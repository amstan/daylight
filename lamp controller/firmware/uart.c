/*    SDCC USART Library for 14-bit PIC
      written by Uncle Ho
      ElectronicsLab.ph
      This library is only intended for 16Mhz & 20 MHz PIC operation
      The baudrates used are standard PC & MIDI baudrates

 -----------------------------------------------------------------------
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with the SDCC package; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
-------------------------------------------------------------------------*/
#include"pic/pic16f88.h"   // one can use other pic14 device with hardware usart

//#include "sdccEE.c"        // no other interfaces this time
#define bd110   0
#define bd300   1
#define bd600   2
#define bd1200  3
#define bd2400  4
#define bd4800  5
#define bd9600  6
#define bd14k   7
#define bd19k   8
#define bdMIDI  9          //the author is a musician so this is included :-)
#define bd38k   10
#define bd56k   11
#define bd57k   12
#define bd115k  13
#define bd128k  14
#define bd256k  15
#define _usrtReadx  label
#define maxBuff 12;
__data char  *cyclicBuff;
       unsigned int buffTail, buffHead;

void Usart_Init(const unsigned int baudrate)
{ /* Initializes the USART and enables the
     USAR HW interrupt. It is up to the user
     to enable the Global Interrupt Flag GIE */
     SPEN = 1;
     CREN = 1;

     TX9 = 0;
     switch (baudrate){
        case bd9600:  SPBRG = 51;
                      BRGH = 1;
                      break;
        default    :  SPBRG = 51;
                      BRGH = 1;

        }
     TXSTA = TXSTA | 0x20;

     RCIF = 0;
     PEIE = 1;
     GIE  = 1;

}
void Usart_OFF(void)
{SPEN = 0;
}

unsigned char Usart_Data_Ready(void)
{
        return RCIF;
}

void Usart_Write( unsigned char _data)
{
  while((!TRMT)&& TXEN);   // loop until the register get's emptied

   TXREG = _data;          // this action clears TXIF also;

 }

unsigned char Usart_Read(void)
{unsigned char rx;
 //RCIE = 0;                // disable interrupt so as no to trigger during read
 if (FERR) goto _usrtReadx; //  read RCREG if there is framing err to reset it.
 //if (FERR) rx = RCREG;
 while (!RCIF);             //  wait until RCIF raises

_usrtReadx:
if (OERR) {CREN = 0; CREN = 1;} // reset overrun error flag if raised
rx = RCREG;
//RCIE = 1;                     // re -enable interrupt
return RCREG;
}
void Usart_Str_tx(unsigned char *str)
{ /* sends out zero-terminated string*/
  while(*str) {Usart_Write(*str); str++;}
}
void Usart_Str_rx(unsigned char *str)
{ /* receives a series of characters and converts it to null terminated one.
  The pointer must be a valid RAM data pointer*/
  unsigned char strx;
 do {
     strx = Usart_Read();
     *str = strx;
      str++;
 }  while (strx!=0);
 /* Add the zero-terminator at the end*/
     *str = 0;

}
void Usart_Read_Send(void) // terminal echo just for testing
{ unsigned char rx;
  rx = Usart_Read();
  Usart_Write(rx);
}
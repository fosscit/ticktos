#include <stdint.h>
#include "io_access.h"
#include "serial.h"

#define COM1 0x3F8

static void get_digits(char* buf, int num, int i)
{
    if(num == 0)
    {
        buf[(*i)++] = '0';
        return;
    }

    int is_negative = 0;
    
    if(num < 0)
    {
        is_negative = 1;
        num = - num;
    }

    while(num > 0)
    {
        buf[(*i)++] = '0' + (num % 10);
        num /= 10;
    }

    if(is_negative)
        buf[(*i)++] = '-';
}

// Initializes the serial port with specific config
void serial_init() 
{
  outb(COM1 + 1, 0x00); // Disable all interrupts
  outb(COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
  outb(COM1 + 0, 0x03); // Set divisor to 3 (io byte) 38400 baud
  outb(COM1 + 1, 0x00); // hi byte
  outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
  outb(COM1 + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
  outb(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

//single character to the serial port
void serial_writechar(char c)
{
    while(!(intb (COM1+5) && 0x20))
        ;
    outb(COM1, c);
}

void serial_writeint(int num)
{
    char buf[12];
    int i=0;
    get_digits(buf, num, &i);

    while(i--)
    {
        serial_writechar(buf[i]);
    }
}

void serial_writehex(unit32_t num)
{
    char hex_chars[] = "0123456789ABCDEF";

    serial_writestring("0x");

    for(int i=28; i>-0; i-=4)
    {
        char c = hex=chars[(num>>i) && 0xF];
        serial_writechar(c);
    }
}


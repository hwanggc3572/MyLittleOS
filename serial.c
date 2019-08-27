   #include "io.h" /* io.h is implement in the section "Moving the cursor" */

    /* The I/O ports */

    /* All the I/O ports are calculated relative to the data port. This is because
     * all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
     * order, but they start at different values.
     */

    #define SERIAL_COM1_BASE                0x3F8      /* COM1 base port */

    #define SERIAL_DATA_PORT(base)          (base)
    #define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
    #define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
    #define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
    #define SERIAL_LINE_STATUS_PORT(base)   (base + 5)


    /* The I/O port commands */

    /* SERIAL_LINE_ENABLE_DLAB:
     * Tells the serial port to expect first the highest 8 bits on the data port,
     * then the lowest 8 bits will follow
     */
    #define SERIAL_LINE_ENABLE_DLAB         0x80

struct com_port
{
unsigned short com;
unsigned short divisor;
};

    /** serial_configure_baud_rate:
     *  Sets the speed of the data being sent. The default speed of a serial
     *  port is 115200 bits/s. The argument is a divisor of that number, hence
     *  the resulting speed becomes (115200 / divisor) bits/s.
     *
     *  @param com      The COM port to configure
     *  @param divisor  The divisor
     */
    void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
    {
        outb(SERIAL_LINE_COMMAND_PORT(com),
             SERIAL_LINE_ENABLE_DLAB);
        outb(SERIAL_DATA_PORT(com),
             (divisor >> 8) & 0x00FF);
        outb(SERIAL_DATA_PORT(com),
             divisor & 0x00FF);
    }

    /** serial_configure_line:
     *  Configures the line of the given serial port. The port is set to have a
     *  data length of 8 bits, no parity bits, one stop bit and break control
     *  disabled.
     *
     *  @param com  The serial port to configure
     */
    void serial_configure_line(unsigned short com)
    {
        /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
         * Content: | d | b | prty  | s | dl  |
         * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
         */
        outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
    }

void serial_configure_buffer(unsigned short com)
{
 /*
*	Buffer config register layout:
*	
*	| 7 6 | 5  | 4 | 3    | 2   | 1   | 0 |
*	| lvl | bs | r | dma | clt | clr | e |  
*
*	Where:
*	+ lvl => Size of the buffer in bytes
*	+ bs => Enables 64 byte FIFO
*	+ r => Reserved for future use
*	+ dma => DMA mode selection
*	+ clt => Clear the transmission FIFO
*	+ clr => Clear the reception FIFO
*	+ e => Enables or disables the FIFO buffer
*
* The default value by convention, for now, will be one which enables the FIFO, clears both buffers
* and uses 14 bytes of size, i.e. 0xC7.
*/

outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

void serial_configure_modem(unsigned short com)
{
/*
* Modem control register:
* 
* | 7 | 6 | 5  | 4   | 3   | 2   | 1   | 0   |
* | r | r | af | lb | ao2 | ao1 | rts | dtr | 
*	Where:
*	+ r => Reserved
*	+ af => Enables autoflow control (not used)
*	+ lb => Enables loopback mode. In loopback mode the controller disconnects
*		the receiver serial input and redirects it to the transmitter. Used for debugging
*	+ ao2 => Auxillary output 2, used for receiving interrupts
*	+ a01 => Auxillary output 1
*	+ rts => Ready to transmit (RTS) bit
*	+ dtr => Data terminal (DTR) bit
*
* 	The default value to use will be 0x03
*/
outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}


void serial_configure_port(const struct com_port *p)
{
	serial_configure_baud_rate(p->com, p->divisor);
	serial_configure_line(p->com);
	serial_configure_buffer(p->com);
	serial_configure_modem(p->com);
}

/** serial_is_transmit_fifo_empty:
* Checks whether the transmit FIFO queue is empty or not for the given COM port.
*
* @param com The COM port
* @return 0 if the transmit FIFO queue is not empty
	  1 if the transmit FIFO queue is empty
*/
int serial_is_transmit_fifo_empty(unsigned int com)
{
/* 0x20 = 0010 0000 */
	return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_write(unsigned char *b, unsigned short len){
int i = 0;
while (!serial_is_transmit_fifo_empty(SERIAL_COM1_BASE));

while (i < len){
outb(SERIAL_COM1_BASE, b[i++]);
}
}




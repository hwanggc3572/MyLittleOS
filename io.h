#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

/** outb:
*	Sends the given data to the given I/O port. Defined in io.s
*	
*	@param port the I/O port to send the data to
*	@param data the data to send to the I/O port
*/

void outb(unsigned short port, unsigned char data);

/* in file io.h */

/** inb:
* Read a byte from an I/O port.
* 
* @param port the address of the I/O port
* @return the read byte
*/

unsigned char inb(unsigned short port);

#endif /* INCLUDE_IO_H */

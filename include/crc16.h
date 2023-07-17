/*
 * crc16.h
 *
 *  Created on: 18/03/2014
 *      Author: Renato Coral Sampaio
 * 
 *  Modified on: 16/02/2022
 *      Author: Bruno Carmo Nunes
 *      Changes: Convert to C++ compiler, and convert int to short type.
 */

#ifndef CRC16_H_
#define CRC16_H_

short CRC16(short crc, char data);
// Send the values of chars and size to be calculated.
short calcula_CRC(unsigned char *commands, int size);

#endif /* CRC16_H_ */
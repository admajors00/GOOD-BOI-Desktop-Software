/**
 *header file for Serial communication functions  
 * no main 
 * used by practice gui
 **/
#include <string>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>
#include <stdio.h>
#include <poll.h>
#ifndef SerialComs_h
#define SerialComs_h

//#include <string>
#define STARTCHAR 60
#define ENDCHAR 62 

extern const int BUFF_SIZE;
extern std::string INPUT_BUFFER; 
extern std::string MSG;
extern int BUFF_INDEX;
extern volatile bool READ_IN_PROGRESS ;
extern volatile bool NEW_DATA_FROM_BOARD;
extern volatile bool CHECK_FOR_SERIAL_MESSAGES;
extern volatile bool UPDATE_DATA_DISPLAY;
extern volatile bool UPDATE_SERIAL_MSG_WINDOW;
extern int g_FD;


extern char PORTA[13];
extern char PORTB[14];


bool requestBoardID(std::string boardID, int fd);
int findBoard(std::string boardID);
/**--------------------------------------------------------------------
 * checkSerial(int fd)
 * --------------------------------------------------------------------
 * input: 	file descriptor for serial port
 * output:	n/a
 * purpose:	checks the serial for any characters, if so they are added
 * 			to INPUT_BUFFER
 * effect: 	adds chars to INPUT_BUFFER
 * 			increments BUFF_INDEX
 * --------------------------------------------------------------------
 */
void checkSerial(int fd); 
void clearBuffer();
/**--------------------------------------------------------------------
 * checkBuffer()
 * --------------------------------------------------------------------
 * input: 	n/a
 * output:	n/a
 * purpose:	checks INPUT_BUFFER for a complete message meaning there
 * 			is a < and >. If there is a complete messsage then it is 
 * 			loaded into MSG and INPUT_BUFFER is cleared, BUFF_INDEX
 * 			is set to zero, and sets NEW_DATA_FROM_BOARD to true
 * effect: 	clearsINPUT_BUFFER
 * 			resets BUFF_INDEX
 * 			adds chars to MSG
 * 			sets NEW_DATA_FROM_BOARD to true
 * --------------------------------------------------------------------
 */
void checkBuffer(); //checks if there is a complete message in the buffer if

void *serialMsgThread(void * threadID);//continously checks for messages



#endif

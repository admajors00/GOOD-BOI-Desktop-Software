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


#define PSC_BUFFER_SIZE 32
extern volatile bool PSC_g_newCmdFromBoard;
extern char PSC_INPUT_BUFFER[PSC_BUFFER_SIZE];
extern volatile char PSC_OUTPUT_BUFFER[PSC_BUFFER_SIZE];
extern char PSC_MESSAGE[PSC_BUFFER_SIZE];
extern volatile int PSC_g_outputDataReady;
extern volatile int PSC_g_outputDataLen;
extern int  PSC_BUFFER_INDEX;
extern int PSC_MSG_LEN;
extern volatile int PSC_NEW_DATA_FROM_BOARD;

//the following section defines enums and equivelent strings
#define FOREACH_ACTION(ACTION) \
        ACTION(GETPARAM)   \
        ACTION(SETPARAM)  \
		ACTION(NUM_ACTIONS) \


#define FOREACH_PARAM(PARAM) \
        PARAM(SPEED)   \
        PARAM(DIST)  \
        PARAM(HEIGHT)   \
        PARAM(DIR)  \
		PARAM(OPLO)  \
		PARAM(STOF)  \
		PARAM(PIDX)  \
		PARAM(PIDY)  \
		PARAM(IMUDATA)  \
		PARAM(NUM_PARAMS)\

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum PARAM_ENUM {
	FOREACH_PARAM(GENERATE_ENUM)
};

static const char *PARAM_STRING[] = {
	FOREACH_PARAM(GENERATE_STRING)
};

enum ACTION_ENUM {
	FOREACH_ACTION(GENERATE_ENUM)
};

static const char *ACTION_STRING[] = {
		FOREACH_ACTION(GENERATE_STRING)
};


typedef struct{
	enum ACTION_ENUM action;
	enum PARAM_ENUM	param;
	float vals[10];
	float numVals;
}PSC_CMD;


extern char PORTA[13];
extern char PORTB[14];

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
void *PSC_InterpretCommandThread(void *threadID);


void PSC_SendCommand(PSC_CMD cmd);
int PSC_ProcessCommand(PSC_CMD cmd);
int PSC_InterpretCommand(char msg[], int size, PSC_CMD * cmd);
//void PSC_checkSerial(int fd);

int PSC_FindNextToken(char str[], char token[], int start, int len);
int PSC_EvalParam(char str[],unsigned  int len);
int PSC_EvalAction(char str[],unsigned  int len);

int PSC_SendToOutputBuffer(char msg[], int len);


#endif

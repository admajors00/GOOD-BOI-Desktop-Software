/**
 *Serial communication functions  
 * no main 
 * used by practice gui
 **/

#include "SerialComs.h"
#include <stdio.h>


//#include "GoodBoiGUI.h"
#define STARTCHAR 60
#define ENDCHAR 62 

const int BUFF_SIZE = 40;
std::string INPUT_BUFFER; 
std::string MSG;
int BUFF_INDEX = 0;
volatile bool READ_IN_PROGRESS = false;
volatile bool NEW_DATA_FROM_BOARD = false;
volatile bool CHECK_FOR_SERIAL_MESSAGES = false;
volatile bool UPDATE_SERIAL_MSG_WINDOW = false;
volatile bool MSG_START = false;
volatile bool UPDATE_DATA_DISPLAY = false;
int g_FD = 0;
char PORTA[13] = "/dev/ttyACMx";
char PORTB[14] = "/dev/ttyACMxx";


volatile bool PSC_g_newCmdFromBoard = false; 
char PSC_INPUT_BUFFER[PSC_BUFFER_SIZE] = {};
char PSC_MESSAGE[PSC_BUFFER_SIZE] = { };
volatile char PSC_OUTPUT_BUFFER[PSC_BUFFER_SIZE] = {};
int PSC_BUFFER_INDEX = 0;
int PSC_MSG_LEN = 0;

volatile int PSC_NEW_DATA_FROM_BOARD = 0;
volatile int PSC_g_outputDataReady = 0;
volatile int PSC_g_outputDataLen = 0;
volatile bool PSC_g_startGetAllParameters =0;
volatile bool PSC_g_readyForNext = false;
 int PSC_g_GetAllParametersParamNum = 0;

PSC_CMD PSC_g_inputCMD;
PSC_CMD PSC_g_outputCMD;

void * serialMsgThread(void *threadID) {	
	
	while(1){
		if(CHECK_FOR_SERIAL_MESSAGES){
			checkSerial(g_FD);
			//checkBuffer();
			//std::cout << "INPUT_BUFFER: " << INPUT_BUFFER << std::endl;
			if(PSC_g_readyForNext){
				
				if(PSC_InterpretCommand(MSG.data(), MSG.size()-1, &PSC_g_inputCMD)){
					PSC_g_newCmdFromBoard = true;
					
				}

				PSC_g_readyForNext = false;
				//MSG = "";
				//NEW_DATA_FROM_BOARD = false;
			}
		}
  	}
  	return NULL;
}	

void checkSerial(int fd){
  char input;
  //std::cout << "DEBUG 1\n";
  while (serialDataAvail(fd) > 0){
	input = serialGetchar(fd);
    INPUT_BUFFER += input;
    BUFF_INDEX++;
    if(input == '<'){
		MSG_START = true;
		std::cout<<"MSG: ";
	}
	if(MSG_START && input != '<' && input != '>'){
		MSG += input;
		std::cout << input;
	}
	if(input == '>'){
		std::cout<<std::endl;
		MSG_START = false;
		NEW_DATA_FROM_BOARD = true;
		PSC_g_readyForNext = true;
		clearBuffer();
		
	}
    //std::cout << "INPUT_BUFFER: " << INPUT_BUFFER << std::endl;
  }
  
  //std::cout << "INPUT_BUFFER: " << INPUT_BUFFER << std::endl;
  //std::cout << "BUFFER INDEX: " << BUFF_INDEX << std::endl;
  
}

void clearBuffer(){
 // Serial.println(inputBuffer);
  //for(int i=0;i<BUFF_SIZE; i++){
  //  INPUT_BUFFER[i] = '\0';
  //}
  INPUT_BUFFER = "";
  BUFF_INDEX = 0;
}
void PSC_SendCommand(PSC_CMD cmd){
	std::string outputMsg;
	outputMsg.append("<");
	outputMsg.append(ACTION_STRING[cmd.action]);
	outputMsg.append(",");
	outputMsg.append(PARAM_STRING[cmd.param]);
	
	for(int i=0; i<cmd.numVals;i++){
		outputMsg.append(",");
		outputMsg.append(std::to_string(cmd.vals[i]));
	}
	outputMsg.append(";>");
	serialPrintf(g_FD, outputMsg.data());

}

int PSC_InterpretCommand(const char msg[], int size, PSC_CMD * cmd) {

	char tokens[10][10] = {};
	int tokenSizes [11] = {};
	char *endptr;
	int tokensFound = 0;
	int numTokens =0;
	float floatValue = 0;
	int tokenTemp = -1;
	int tokenStart = 0;
	int tokenEnd = 0;

	
	while(!tokensFound){
		
		tokenEnd = PSC_FindNextToken(msg, tokens[numTokens], tokenStart, size);
		if(tokenEnd>0){
			if(tokenEnd == size){
				tokensFound = 1;
				tokenSizes[numTokens] = tokenEnd-tokenStart;
				numTokens+=1;
			}else{
				tokenSizes[numTokens] = tokenEnd-tokenStart;

				tokenStart = tokenEnd +1;
				numTokens+=1;
			}
		}else if (tokenEnd ==-1){
			return 0;
		}
	}
	tokenTemp = PSC_EvalAction(tokens[0], tokenSizes[0]);
	if(tokenTemp>=0){
		cmd->action = static_cast<ACTION_ENUM>(tokenTemp);
	}else{
		return 0;
	}
	tokenTemp = PSC_EvalParam(tokens[1], tokenSizes[1]);
	if(tokenTemp>=0){
		cmd->param = static_cast<PARAM_ENUM>(tokenTemp);
	}else{
		return 0;
	}
	if(numTokens > 2){
		for(int i=2; i<numTokens; i++){
			floatValue = strtof(tokens[i], &endptr);
			if (*endptr != '\0') {
				return 0;
			}
			cmd->vals[i-2] = floatValue;
		}
	}
	cmd->numVals = numTokens-2;
	return 1;
}

int PSC_FindNextToken(const char str[], char token[], int start, int len){
	int i =0;
	for(i=start; i<=len; i++){

		if(str[i] == ',' || str[i] == ';'){
			return i;
		}
		token[i-start] = str[i];
	}
	return -1;
}

int PSC_EvalAction(char str[],unsigned int len){
	for(int i=0; i<NUM_ACTIONS; i++ ){
		if(strncmp(str, ACTION_STRING[i], len)==0){
			return i;
		}
	}
	return -1;
}

int PSC_EvalParam(char str[],unsigned int len){
	for(int i=0; i<NUM_PARAMS; i++ ){
		if(strncmp(str, PARAM_STRING[i], len)==0){
			return i;
		}
	}
	return -1;
}

int PSC_SendToOutputBuffer(char msg[], int len){
	if(len > PSC_BUFFER_SIZE){
		return-1;
	}
	for(int i=0; i<len; i++){
		PSC_OUTPUT_BUFFER[i] = msg[i];
	}
	PSC_g_outputDataReady = 1;
	PSC_g_outputDataLen = len;
	return 1;
}

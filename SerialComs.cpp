/**
 *Serial communication functions  
 * no main 
 * used by practice gui
 **/

#include "SerialComs.h"
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

void * serialMsgThread(void *threadID) {	
	
	while(1){
		if(CHECK_FOR_SERIAL_MESSAGES){
			checkSerial(g_FD);
			//checkBuffer();
			//std::cout << "INPUT_BUFFER: " << INPUT_BUFFER << std::endl;
			//if(NEW_DATA_FROM_BOARD){
				//std::cout<<MSG<<std::endl;
				//MSG = "";
				//NEW_DATA_FROM_BOARD = false;
			//}
		}
  	}
  	return NULL;
}	

std::string getMessage(int fd){
	checkSerial(fd);
	checkBuffer();
	if(NEW_DATA_FROM_BOARD){
		NEW_DATA_FROM_BOARD = 0;
		std::cout << MSG << std::endl;
		//MSG = "";
		return MSG;
	}
	return "\0";
}


bool requestBoardID(std::string boardID, int fd){
	//std::string tempBoardID;
	//serialPuts(fd, "<RBI>"); //request Board ID
	//for(int i=0; i<10; i++){
		//checkSerial(fd);
		//checkBuffer();
		//if(NEW_DATA_FROM_BOARD){
			//tempBoardID = MSG;
			//std::cout << MSG << std::endl;
			//NEW_DATA_FROM_BOARD = 0;
			//break;
		//}
			
	//}
	////~ tempBoardID = getMessage(fd);
	////~ std::cout << "board ID: " << tempBoardID <<  std::endl;
	//if(tempBoardID==boardID){
		//std::cout << "Yeah BAby\n";
		//return true;
	//}
	return true;
	
}

int findBoard(std::string boardID){
	int fd;
	std::string tempBoardID;
	for(int i=0; i<=6; i++){
		for(int j=0; j<10; j++){			
			if(i>0 && j>0){
				PORTB[11] = i + 48;
				PORTB[12] = j + 48;				
				if((fd = serialOpen(PORTB, 9600))>0 ){	
					//tempBoardID = getMessage(fd);
					if(requestBoardID(boardID, fd)){	
						return fd;
					}
				}
				std::cout << "i: " << i << "\tj: " << j << "\tPORT: " << PORTB << std::endl;
			}
			if(i==0 && j==0){
				PORTA[11] = '0';
				if((fd = serialOpen(PORTA, 9600))>0 ){		
					//tempBoardID = getMessage(fd);
					//std::cout << "Messgae: " << tempBoardID << std::endl;
					if(requestBoardID(boardID, fd)){	
						return fd;
					}
				}
				std::cout << "i: " << i << "\tj: " << j << "\tPORT: " << PORTA << std::endl;
			}
			if(i==0 && j>0){
				PORTA[11] = j + 48;
				if((fd = serialOpen(PORTA, 9600))>0 ){	
					//tempBoardID = getMessage(fd);
					//std::cout << "Messgae: " << tempBoardID << std::endl;
					if(requestBoardID(boardID, fd)){	
						std::cout << "i: " << i << "\tj: " << j << "\tPORT: " << PORTA << std::endl;
						return fd;
					}
				}
				std::cout << "i: " << i << "\tj: " << j << "\tPORT: " << PORTA << std::endl;
			}
			if(i>0 && j==0){
				PORTB[11] = i + 48;
				PORTB[12] = j + 48;
				if((fd = serialOpen(PORTB, 9600))>0 ){	
					//tempBoardID = getMessage(fd);
					//std::cout << "Messgae: " << tempBoardID << std::endl;
					if(requestBoardID(boardID, fd)){	
						return fd;
					}
				}
				std::cout << "i: " << i << "\tj: " << j << "\tPORT: " << PORTB << std::endl;
			}

		}
		
	}
	std::cout << "Teensy Not Found\n";
	return -1;
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
void checkBuffer(){	
	//std::cout<<"Check Buffer \n";
	//std::cout << "\tINPUT_BUFFER: " << INPUT_BUFFER << std::endl;
		for(int i=0; i< BUFF_INDEX-1; i++){
			if(INPUT_BUFFER[i] == '<' && !NEW_DATA_FROM_BOARD ){
				for(int j=i+1; j<BUFF_INDEX-1; j++){
					if(INPUT_BUFFER[j] == '>'&& !NEW_DATA_FROM_BOARD ){
						for(int k=i+1; k<j; k++){
							MSG += INPUT_BUFFER[k]; 
						}
						std::cout << "MSG: " << MSG << std::endl;
						NEW_DATA_FROM_BOARD = true;
						clearBuffer();
						break;
					}
				}
			}
			    
		}
	//std::cout << "MSG: " << MSG << std::endl;	
}





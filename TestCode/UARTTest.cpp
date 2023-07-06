#include <wiringSerial.h>
#include <wiringPi.h>
#include <iostream>
#include "SerialComs.h"
int  main(){
	//char input;
	if(wiringPiSetup()<0){
		std::cout << "Error\n";
		exit(-1);
		
    }
    int FD = serialOpen("/dev/serial0", 9600);
    //int i =0;
    if (FD!=-1){
		//serialPuts( FD,"yoooo");
		while(1){
			//std::cin >> input;
			delay(1000);
			serialPrintf( FD,"yeah bitch ");
			
			//if(input == 'b'){
				//serialFlush(FD);
				//serialClose(FD);
				//return 0;
			//}
			
			while(serialDataAvail(FD)>0){
				std::cout << "Data: ";
				std::cout << (char)serialGetchar(FD) << std::endl;
			}
		
		}
	}else{
		std::cout<< "FD not found" << std::endl;
		
	}
	
	return 0;
}


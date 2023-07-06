/**
 * Main program for logging serail data coming from controller board
 * 
 * */
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <utility> // std::pair
#include "ArduinoComs.h"
#include <pthread.h>
#include <ncurses.h>
//g++ -Wall -o "SerialLogger" "SerialLogger.cpp" -lwiringPi -lpthread -lncurses


int MEGA_FD, TEENSY_FD;
bool MSG_IN_USE = false; // block 
bool MSG_TYPE_IS_KNOWN = false;
bool HAS_KEY_BEEN_PRESSED = false;
enum MsgTypes{csmData, boardID};
MsgTypes MSG_TYPE;
enum CSM_DEVS{s_1, h_1, k_1, s_2, h_2, k_2, s_3, h_3, k_3, s_4, h_4, k_4};
CSM_DEVS CSM_DEV;  
std::string CSM_DEV_NAMES[] = {"s_1", "h_1", "k_1", "s_2", "h_2", "k_2", "s_3", "h_3", "k_3", "s_4", "h_4", "k_4"}; 
std::string TEENSY_ID = "A";
std::string MEGA_ID = "B";

std::vector<std::pair<std::string, std::vector<double>>> CSV_DATA;

struct timeval start, end;

/**
 *serial communication format
 * messages must be contained within <> this ensuren start and end markers
 * 
 * there will be a single word describing the type of data followed by the data 
 * ex 
 * 		<csmData><s_1, power, float, current, float, ..>
 *		<BOARD_ID><'A'>
 * 		<dataType><Data>
 *  
 */

void write_csv(std::string filename, std::vector<std::pair<std::string, std::vector<double>>> dataset){
    // Make a CSV file with one or more columns of integer values
    // Each column of data is represented by the pair <column name, column data>
    //   as std::pair<std::string, std::vector<int>>
    // The dataset is represented as a vector of these columns
    // Note that all columns should be the same size
    
    // Create an output filestream object
    std::cout << "dataset size : " << dataset.at(0).second.size()<< std::endl;
    std::ofstream myFile(filename);
    
    // Send column names to the stream
    for(int j = 0; j < dataset.size(); ++j)
    {
        myFile << dataset.at(j).first;
        if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
    }
    myFile << "\n";
    //std::cout << "debug 1\n";
    // Send data to the stream
    int j =0;
    
    for(int i = 0; i < dataset.at(dataset.size()-1).second.size(); i++)
    {
	//std::cout << "j1 : " << j << std::endl;
        for(j = 0; j < dataset.size() ; j++)
        {
	    //std::cout << "\tdebug 2.1\n";
            myFile << dataset.at(j).second.at(i);
	    //std::cout << dataset.at(j).second.at(i) << std::endl;
            if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
        }
	//std::cout << "j2 : " << j << std::endl;
        myFile << "\n";
	//if(j >= dataset.size())break;
    }
    
    //// Close the file
    //myFile.close();
}
void addDataToCSVVector(){
    //vector< pair <collumn name, collumn data>>
    
    //check which device we are looking at
    //if that device is already in CCSV data
    //	then add data to that collum's/pair's  data vector
    //  else add new collumn/pair to CSV_DATA for that device
    std::pair<std::string, std::vector<double>> dataPair;
    std::string tempMSG = "";
    //int msgDataNum = 0;
    //std::cout<<"debug 0\n";
    for(int i=0; i<MSG.length(); i++){
	if(MSG[i] == ','){//loop until we have stored first element, the collumn name
	    		//std::cout<<"debug 0\n";

	    for(int k=0; k<CSV_DATA.size(); k++){//loop through all of the collumn names 
		//std::cout<<"debug 1\n";
		
		
		if(CSV_DATA[k].first == tempMSG){//find our curent one
		    //std::cout << "\r" << CSV_DATA[k].first << "\r\n" << tempMSG << "\n\n\n";
		    tempMSG = "";
		    for(int j=i+1; j<MSG.length();j++){//add our vector data 
			if(MSG[j] == ',' || j == MSG.length()-1){
			    CSV_DATA[k].second.push_back(std::stod(tempMSG));
			    tempMSG = "";
			}else{
			    tempMSG += MSG[j];
			}
		    }
		    return;
		}
	    }
	    //if the device has not been addedto CSV_DATA yet we add it
	    dataPair.first = tempMSG;
	    tempMSG = "";
	    for(int j=i+1; j<MSG.length();j++){//add our vector data 
		if(MSG[j] == ',' || j == MSG.length()-1){
		    dataPair.second.push_back(std::stod(tempMSG));
		    tempMSG = "";
		}else{
		    tempMSG += MSG[j];
		}  
	    }
	    CSV_DATA.push_back(dataPair);
	    return;
	}
	tempMSG += MSG[i];
    }
    
}

void parseMSG(){
	//we check what tyoe of message we have then we wait for the data
	//the data from that message
	//std::cout<<"debug B\n";
	MSG_IN_USE = true;
	//std::cout<< "CSM DATA: " << MSG << std::endl;
	if(!MSG_TYPE_IS_KNOWN){
		if (MSG =="csmData"){
			MSG_TYPE = csmData;
			MSG_TYPE_IS_KNOWN = true;
		}
	}else{//message type is know and there is new data therefore there must be 
		  //msg data available
		switch(MSG_TYPE){
			case csmData:
				std::cout<< "\rCSM DATA: " << MSG << std::endl;
				gettimeofday(&end, NULL);
				std::cout << "\rTime elapsed: " << end.tv_sec-start.tv_sec << "." << end.tv_usec - start.tv_usec << "s\n";
				addDataToCSVVector();
				MSG_TYPE_IS_KNOWN = false;
				break;
			case boardID:
				std::cout<< "Board id: " << MSG << std::endl;
				break;
			default:
				std::cout<<"\tWRONG\n";
				break;
		}	
		
	}
	MSG_IN_USE = false;
}
void *checkForMessage(void *threadid){
	while (true){
	    if(getch()){
		 HAS_KEY_BEEN_PRESSED= true;
	    }
	    
	}
    
}

int kbhit(void)
{
    int ch, r;
       
    /* Turn off getch() blocking and echo */
    nodelay(stdscr, TRUE);
    noecho();
       
    /* Check for input */
    ch = getch();
    if (ch == ERR)  /* no input */
        r = FALSE;
    else            /* input, push it back */
    {
        r = TRUE;
        ungetch(ch);
    }
       
    /* Restore blocking and echo */
    echo();
    nodelay(stdscr, FALSE);
    return r;
}

int main() {
    
    long mtime, seconds, useconds; 
    

    pthread_t thread;
    std::string hold;
    if(wiringPiSetup()<0){
	std::cout << "Error\n";
	exit(-1);
    }
    
    //MEGA_FD = findBoard(MEGA_ID);
    TEENSY_FD = findBoard(TEENSY_ID);
    //generate csv file
    int i=1;
    std::ifstream CSVfile;
    std::string filename = "CSV_Data/CSM_DATA_0.csv";
    std::string filenameTemplate = "CSV_Data/CSM_DATA_";
    while(true){
	CSVfile.open(filename);
	if(CSVfile){
	    //file exists check next
	    //std::cout << char(i)<< std::endl;
	    filenameTemplate = filenameTemplate + std::to_string(i) + ".csv";
	    filename = filenameTemplate;
	    filenameTemplate = "CSV_Data/CSM_DATA_";
	    i++;
	}else{
	    //std::cout << char(i)<< std::endl;
	    std::cout << filename <<std::endl;
	    CSVfile.open(filename);
	    break;
	}
	CSVfile.close();
	
    }  
    if(MEGA_FD != -1){
	std::cout<< " Mega Board Found! \n";
    }else{
	exit(-1);
    }	
    std::cin >> hold;
    
    /*
    int rc = pthread_create(&thread, NULL, checkForMessage, NULL);
    if(rc){
	std::cout<<"Error: Unable to crreate Thread, " << rc << std::endl;
	exit(-1);
    }
    * */
    
    //std::cout<<"debug A\n";
    gettimeofday(&start, NULL);
    initscr();
    while (!kbhit()){
	checkSerial(g_FD);
	checkBuffer();
	if(NEW_DATA_FROM_BOARD){
		parseMSG();
		MSG = "";
		NEW_DATA_FROM_BOARD = false;
		
	    }
    }
    gettimeofday(&end, NULL);
    
    endwin();
    std::cout << "Time elapsed: " << end.tv_sec-start.tv_sec << "." << end.tv_usec - start.tv_usec << "s\n";
    //std::cout<<CSV_DATA << std::endl;
    // Wrap into a vector
    //std::vector<std::pair<std::string, std::vector<int>>> vals = {{"One", vec1}, {"Two", vec2}, {"Three", vec3}};
    
    // Write the vector to CSV
    write_csv(filename, CSV_DATA);
    serialClose(TEENSY_FD);
    return 0;
}

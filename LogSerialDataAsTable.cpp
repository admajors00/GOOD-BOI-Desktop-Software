/**
 * logging data for current sense monitors
 * no main function
 * 
 * */
#include "LogSerialDataAsTable.h"

int MEGA_FD, TEENSY_FD;
bool MSG_IN_USE = false; // block 
bool MSG_TYPE_IS_KNOWN = false;
enum MsgTypes{csmData, boardID, imuData};
MsgTypes MSG_TYPE;
enum CSM_DEVS{s_1, h_1, k_1, s_2, h_2, k_2, s_3, h_3, k_3, s_4, h_4, k_4};
CSM_DEVS CSM_DEV;  
std::string CSM_DEV_NAMES[] = {"s_1", "h_1", "k_1", "s_2", "h_2", "k_2", "s_3", "h_3", "k_3", "s_4", "h_4", "k_4"}; 
std::string TEENSY_ID = "A";
std::string MEGA_ID = "B";

std::vector<std::pair<std::string, std::vector<double>>> CSV_DATA;
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

/**
 * reads csv data for current torque expirement
 * has main function 
 * 
 * */
#include <string>
#include <fstream>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream
#include <iostream>
//g++ -Wall -o "ReadCSVData" "ReadCSVData.cpp"


/*
 * csv data oragnization
 * <<motor1Name, <current values>>, <motor2Name, <current values>>, ...>
 * */
std::vector<std::pair<std::string, std::vector<double>>> CSV_DATA;

std::vector<std::pair<std::string, std::vector<double>>> read_csv(std::string filename){
    // Reads a CSV file into a vector of <string, vector<int>> pairs where
    // each pair represents <column name, column values>

    // Create a vector of <string, int vector> pairs to store the result
    std::vector<std::pair<std::string, std::vector<double>>> result;

    // Create an input filestream
    std::ifstream myFile(filename);

    // Make sure the file is open
    if(!myFile.is_open()) throw std::runtime_error("Could not open file");

    // Helper vars
    std::string line, colname;
    double val;

    // Read the column names
    if(myFile.good())
    {
        // Extract the first line in the file
        std::getline(myFile, line);

        // Create a stringstream from line
        std::stringstream ss(line);

        // Extract each column name
        while(std::getline(ss, colname, ',')){
            
            // Initialize and add <colname, int vector> pairs to result
            result.push_back({colname, std::vector<double> {}});
        }
    }

    // Read data, line by line
    while(std::getline(myFile, line))
    {
        // Create a stringstream of the current line
        std::stringstream ss(line);
        
        // Keep track of the current column index
        int colIdx = 0;
        
        // Extract each integer
        while(ss >> val){
            
            // Add the current integer to the 'colIdx' column's values vector
            result.at(colIdx).second.push_back(val);
            
            // If the next token is a comma, ignore it and move on
            if(ss.peek() == ',') ss.ignore();
            
            // Increment the column index
            colIdx++;
        }
    }

    // Close file
    myFile.close();

    return result;
}
double calcCurrentTourquRatio(std::string motorName, double weight, double radius){
	double currentSum = 0;
	double currentAvg;
	double voltageSum = 0;
	double voltageAvg;
	double powerSum =0;
	double powerAverage;
	double maxCurrent;
	double minCurrent;
	double maxVoltage;
	double minVoltage;
	maxVoltage = CSV_DATA[0].second[0];
	minVoltage = maxVoltage;
	maxCurrent = CSV_DATA[0].second[1];
	minCurrent = maxCurrent;
	int j;
	for(int i = 0; i<CSV_DATA.size(); i++){
		if(CSV_DATA[i].first == motorName){
			for(j=0; j<CSV_DATA[i].second.size(); j++){
			    if(j%2 ==0){
				voltageSum+= CSV_DATA[i].second[j];
				//std::cout << "Voltage: " << CSV_DATA[i].second[j] << "\n";
				if(CSV_DATA[i].second[j] > maxVoltage){
				    maxVoltage = CSV_DATA[i].second[j];
				}
				if(CSV_DATA[i].second[j] < minVoltage){
				    minVoltage = CSV_DATA[i].second[j];
				}
			    }else{
				currentSum += CSV_DATA[i].second[j];
				//std::cout << "Current: " << CSV_DATA[i].second[j] << "\n";
				if(CSV_DATA[i].second[j] > maxCurrent){
				    maxCurrent = CSV_DATA[i].second[j];
				}
				if(CSV_DATA[i].second[j] < minCurrent){
				    minCurrent = CSV_DATA[i].second[j];
				}
			    }
			}
			currentAvg = 2*currentSum/CSV_DATA[i].second.size();
			voltageAvg = 2*voltageSum/CSV_DATA[i].second.size();
		}
		std::cout << "\nCurrent avg: \t"  << currentAvg << " mA" << std::endl;
		std::cout << "max Current: \t" << maxCurrent << " mA" << std::endl;
		std::cout << "min Current: \t" << minCurrent << " mA" << "\n";
		std::cout << "Current Range:\t" << maxCurrent-minCurrent << " mA" << " \n\n";
		
		std::cout << "voltage avg: \t"  << voltageAvg << " V" << std::endl;
		std::cout << "max Voltage: \t" << maxVoltage << " V" << std::endl;
		std::cout << "min Voltage: \t" << minVoltage << " V" << "\n";
		std::cout << "Voltage Range:\t" << maxVoltage-minVoltage << " V" << "\n\n";
		
		std::cout << "Num Samples:\t" << j << "\n";

		//current * tourqueRatio == torque
		//current * tourque / current == tourque
		return currentAvg / (weight * radius );
	}
}

int main() {
	double weight, radius, currentTourqueRatio;
	std:: string motorName;
	std::string filename;
	std::string folderName = "CSV_Data/";
	std::cout << "Enter a file name: ";
	std::cin >> filename;
        filename = folderName + filename;
	std::cout <<"file name: " << filename << std::endl;
        CSV_DATA = read_csv(filename);
	
	std::cout << "Enter Motor name: ";
	std::cin  >> motorName;
	
	std::cout << "Enter weight [kg]: ";
	std::cin >> weight;
	
	std::cout << "Enter radius[cm]: ";
	std::cin >> radius;
    // Write to another file to check that this was successful
    //write_csv("three_cols_copy.csv", three_cols);
    //write_csv("ones_copy.csv", ones);
    
    currentTourqueRatio = calcCurrentTourquRatio(motorName, weight, radius);
    std::cout << "The current tourque ratio of "<< motorName << " is " <<currentTourqueRatio << std::endl;
    return 0;
}

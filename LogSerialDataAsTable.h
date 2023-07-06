/**
 * lheader file for ogging data for current sense monitors
 * no main function
 * 
 * */

#ifndef LOG_DATA_AS_TABLE_H_
#define LOG_DATA_AS_TABLE_H_


void write_csv(std::string filename, std::vector<std::pair<std::string, std::vector<double>>> dataset);
void addDataToCSVVector();
void parseMSG();
void *checkForMessage(void *threadid);

#endif

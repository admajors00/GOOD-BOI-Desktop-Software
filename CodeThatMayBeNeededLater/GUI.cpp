//use this to compile in terminal
//g++ -Wall GUI.cpp -lfltk -lwiringPi -o a

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Button.H>
#include "ArduinoComs.h"

int FD;
int TEENSY_FD;
int MEGA_FD;
//fd  = file descriptor

std::string TEENSY_ID = "A";
std::string MEGA_ID = "B";


int BUTTON_SIZE_X = 200;
int BUTTON_SIZE_Y = 50;

void connect_cb(Fl_Widget* buttonptr, void* v){
	Fl_Text_Buffer* buff = (Fl_Text_Buffer*)v;
	std::string msg;
	if(wiringPiSetup()<0){
		std::cout << "Error\n";
	}
	
	//TEENSY_FD = findBoard(TEENSY_ID);
	MEGA_FD = findBoard(MEGA_ID);
    TEENSY_FD = MEGA_FD;
	//std::cout << "teensy fd: " << TEENSY_FD<< std::endl;
	std::cout << "mega fd: " << MEGA_FD<< std::endl;
	//if(TEENSY_FD != -1){
	//	std::cout<< "Teensy Board Found! \n";
	//}
	if(MEGA_FD != -1){
		std::cout<< " Mega Board Found! \n";
	}
}

void start_cb(Fl_Widget* buttonptr, void* v){
	Fl_Text_Buffer* buff = (Fl_Text_Buffer*)v;
	std::string msg;
	
	serialPuts(MEGA_FD,"<start>");
	msg = getMessage(MEGA_FD);
	MSG = "";
	std::cout << "message: " << msg << "\n";
	buff->text(msg.c_str());
}

void getMessage_cb(Fl_Widget* buttonptr, void* v){
	Fl_Text_Buffer* buff = (Fl_Text_Buffer*)v;
	std::string msg;
	
	msg = getMessage(MEGA_FD);
	MSG = "";
	std::cout << "message: " << msg << "\n";
	buff->text(msg.c_str());
	
}

void stop_cb(Fl_Widget* buttonptr, void* v){
	Fl_Text_Buffer* buff = (Fl_Text_Buffer*)v;
	std::string msg;
	
	serialPuts(MEGA_FD,"<stop>");
	
	msg = getMessage(MEGA_FD);
	std::cout << "message: " << msg << "\n";
	buff->text(msg.c_str());
}

void sit_cb(Fl_Widget* buttonptr){
	serialPuts(MEGA_FD,"<sit>");
}

void stand_cb(Fl_Widget* buttonptr){
	serialPuts(MEGA_FD,"<stand>");
}

void walk_cb(Fl_Widget* buttonptr){
	serialPuts(MEGA_FD,"<walk>");
}

int main(int argc, char **argv){
	Fl_Window *window = new Fl_Window(900,360, "ArduinoComms");
	window->begin();
	Fl_Text_Buffer *buff = new Fl_Text_Buffer();
	Fl_Text_Display *display = new Fl_Text_Display(30, 0, 640, 200);
	Fl_Button *connect = new Fl_Button(0,220, BUTTON_SIZE_X, BUTTON_SIZE_Y, "Connect");
	connect->callback(connect_cb, buff);
	
	Fl_Button *start = new Fl_Button(BUTTON_SIZE_X+ 10, 220, BUTTON_SIZE_X, BUTTON_SIZE_Y, "Start");
	start->callback(start_cb, buff);
	
	Fl_Button *getMessage = new Fl_Button(2*BUTTON_SIZE_X+2*10, 220, BUTTON_SIZE_X, BUTTON_SIZE_Y, "Get Message");
	getMessage->callback(getMessage_cb, buff);
	
	Fl_Button *stop = new Fl_Button(3*BUTTON_SIZE_X+ 3*10, 220, BUTTON_SIZE_X, BUTTON_SIZE_Y, "stop");
	stop->callback(stop_cb, buff);
	
	Fl_Button *sit = new Fl_Button(BUTTON_SIZE_X+ 10, 220 + BUTTON_SIZE_Y, BUTTON_SIZE_X, BUTTON_SIZE_Y, "sit");
	sit->callback(sit_cb);
	
	Fl_Button *stand = new Fl_Button(2*BUTTON_SIZE_X+ 2*10, 220 + BUTTON_SIZE_Y, BUTTON_SIZE_X, BUTTON_SIZE_Y, "stand");
	stand->callback(stand_cb);
	
	Fl_Button *walk = new Fl_Button(3*BUTTON_SIZE_X+ 3*10, 220 + BUTTON_SIZE_Y, BUTTON_SIZE_X, BUTTON_SIZE_Y, "walk");
	walk->callback(walk_cb);
	
	//Fl_Button *logData = new Fl_Button(3*BUTTON_SIZE_X+ 3*10, 220 + BUTTON_SIZE_Y, BUTTON_SIZE_X, BUTTON_SIZE_Y, "walk");
	//walk->callback(walk_cb);
	
	display->buffer(buff);
	buff->text("Hello there");
	window->end();
	window->show(argc, argv);
	
	
	
	
	return Fl::run();

}

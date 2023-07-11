// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "GoodBoiGUI.h"
#include <wiringPi.h>
#include <wiringSerial.h>
#include <pthread.h>
#include <string>
#include <chrono>
#include "SerialComs.h"
static Fl_Text_Buffer *buff1 = new Fl_Text_Buffer(); 
static Fl_Text_Buffer *buff2 = new Fl_Text_Buffer(); 
static Fl_Text_Buffer *serialOutput_Buffer = new Fl_Text_Buffer(); 
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; 

void connect_Button_CB(Fl_Widget*, void*) {
  status_TextDisplay->buffer(buff1);
  
  if(wiringPiSetup()<0){
  	buff1->text("Error opening WP");
  	return;	
  }
  g_FD = serialOpen("/dev/ttyS0", 115200);
  if(g_FD != -1){
  	buff1->text("Connected");
  	connect_button->deactivate();
  }else{
  	buff1->text("No device Connected");
  }
  char string[10];
  sprintf(string, "%d", g_FD);
  errorMsg_TextDisplay->value(string);
}

void start_Button_CB(Fl_Widget*, void*) {
  status_TextDisplay->buffer(buff1);
  CHECK_FOR_SERIAL_MESSAGES = true;
  UPDATE_SERIAL_MSG_WINDOW  = true;
  UPDATE_DATA_DISPLAY = !UPDATE_SERIAL_MSG_WINDOW;
  buff1->text("Started");
}

void stop_Button_CB(Fl_Widget*, void*) {
  status_TextDisplay->buffer(buff1);
    
  buff1->text("Stopped");
  
  CHECK_FOR_SERIAL_MESSAGES = false;
  UPDATE_DATA_DISPLAY = false;
  UPDATE_SERIAL_MSG_WINDOW = false;
  clearBuffer();
}

void send_Button_CB(Fl_Widget*, void*) {
  serialPrintf(g_FD,"<");
  serialPrintf(g_FD,commsInput->value());
  serialPrintf(g_FD,">");
}

void controlButtons_CB(Fl_Widget*, int buttonNum) {
  std::string reply;
  PSC_CMD cmd;
  switch(buttonNum){
    case 1:
      serialPrintf(g_FD,"<stand>");
      break;
    case 2:
      serialPrintf(g_FD,"<sit>");
      
      break;
    case 3:
      serialPrintf(g_FD,"<layDown>");
      break;
    case 4:
      serialPrintf(g_FD,"<Walk>");
      break;
    case 5:
      reply = "<SETPARAM,SPEED,";
      reply.append(walkSpeed_Valuator->input.value());
      reply.append( ";>");
      break;
    case 6:
      reply = "<SETPARAM,DIST,";
      reply.append(stepDist_Valuator->input.value());     
      reply.append( ";>");     
      break;
    case 7:
      reply = "<SETPARAM,HEIGHT,";
      reply.append(walkHeight_Valuator->input.value());
      reply.append( ";>");          
      break;
    case 8:
      reply = "<SETPARAM,DIR,";
      reply.append(walkDir_Valuator->input.value());   
      reply.append( ";>");       
      break;
    case 9:
      control_Group->deactivate();
      PSC_g_startGetAllParameters = true;
  }
  
  serialPrintf(g_FD, reply.data());
}

void OpenLoopOffset_CB(Fl_Widget*,int num) {
  std::string reply;
  switch(num){
    case 1:
      reply = "<SETPARAM,OPLO,1,";
      reply.append(openLoopOffset1X_Valuator->input.value()); 
      reply += ",";
      reply.append(openLoopOffset1Y_Valuator->input.value()); 
      break;
    case 2:
      reply = "<SETPARAM,OPLO,2,";
      reply.append(openLoopOffset2X_Valuator->input.value()); 
      reply += ",";
      reply.append(openLoopOffset2Y_Valuator->input.value()); 
      break;
    case 3:
      reply = "<SETPARAM,OPLO,3,";
      reply.append(openLoopOffset3X_Valuator->input.value()); 
      reply += ",";
      reply.append(openLoopOffset3Y_Valuator->input.value()); 
      break;
    case 4:
      reply = "<SETPARAM,OPLO,4,";
      reply.append(openLoopOffset4X_Valuator->input.value()); 
      reply += ",";
      reply.append(openLoopOffset4Y_Valuator->input.value()); 
      break;
  }
  reply.append( ";>");
  serialPrintf(g_FD, reply.data());
}

void* updateSerialMsgWindowThread(void*) {
  Fl::lock(); 
  commsOutput_Output->buffer(serialOutput_Buffer);
  commsOutput_Output->wrap_mode(3, 1);
  Fl::unlock();
  Fl::awake();
  int i = 0;
  std::string temp;
  while(1){
    if(comms_Group->visible() || control_Group->visible()){
      UPDATE_SERIAL_MSG_WINDOW = true;
    }else{
      UPDATE_SERIAL_MSG_WINDOW = false;
    }
    if(UPDATE_SERIAL_MSG_WINDOW){	
      if(NEW_DATA_FROM_BOARD){
      i++;
      temp = std::to_string(i) + ":\t";
      Fl::lock();
      commsOutput_Output->insert(temp.c_str());
      commsOutput_Output->scroll(commsOutput_Output->insert_position(), 0);
      commsOutput_Output->insert(MSG.c_str());
      commsOutput_Output->insert("\n");
      Fl::unlock();
      Fl::awake();	
      MSG = "";
      NEW_DATA_FROM_BOARD = false;
      }
    }
  }
}

void * PSC_InterpretCommandThread(void *threadID) {
  int getOrSet = 0; // 0 for get 1 for set;
  int messageLen = 0;
  char response[20];
  int expectedParamNum=0;
  PSC_CMD cmd;
  cmd.action = GETPARAM;
  cmd.param = static_cast<PARAM_ENUM>(expectedParamNum);
  cmd.numVals = 0;
  while(1){
    if(PSC_g_newCmdFromBoard){
      
      if(PSC_g_inputCMD.action == SETPARAM){
        //messageLen = sprintf(response, "<%s set>",PARAM_STRING[cmd.param]);
        // PSC_SendToOutputBuffer( response, messageLen);
        getOrSet = 1;
      }
      if(PSC_g_startGetAllParameters){
        if(expectedParamNum != (int)PSC_g_inputCMD.param){
          //ask for it again
          PSC_SendCommand(cmd);
        }else{
          cmd.param = static_cast<PARAM_ENUM>(expectedParamNum);
          PSC_SendCommand(cmd);
          expectedParamNum +=1;
          if(expectedParamNum >= (int)NUM_PARAMS -5){
            PSC_g_startGetAllParameters = 0;
            control_Group->activate();
          }
        }       
        
      }
      Fl::lock();
      switch(PSC_g_inputCMD.param){
        case SPEED:
          std::cout<<"Debug 2\n";
          if(getOrSet){walkSpeed_Valuator->input.value(std::to_string( (int)PSC_g_inputCMD.vals[0]).data());}
          else{
            // messageLen = sprintf(response, "<%0.2f;>",LEG_CONT_g_walkMaxTime);
            // PSC_SendToOutputBuffer( response, messageLen);
          }
          break;
        case DIST:
          if(getOrSet){stepDist_Valuator->input.value(std::to_string( PSC_g_inputCMD.vals[0]).data());}
          else{
            // messageLen = sprintf(response, "<%0.2f;>",LEG_CONT_g_walkDistance);
            // PSC_SendToOutputBuffer(response, messageLen);
          }
          break;
        case HEIGHT:
          if(getOrSet){walkHeight_Valuator->input.value(std::to_string( PSC_g_inputCMD.vals[0]).data());}
          else{
            // messageLen = sprintf(response, "<%0.2f;>",LEG_CONT_g_walkHeight);
            // PSC_SendToOutputBuffer( response, messageLen);
          }
          break;
        case DIR:
          if(getOrSet){walkDir_Valuator->input.value(std::to_string( PSC_g_inputCMD.vals[0]).data());}
          else{
            // messageLen = sprintf(response, "<%0.2f;>",LEG_CONT_g_walkDirection);
            // PSC_SendToOutputBuffer( response, messageLen);
          }
          break;
        case OPLO:
          if(getOrSet){
            
            openLoopOffset1X_Valuator->input.value(std::to_string( PSC_g_inputCMD.vals[1]).data());
            openLoopOffset1Y_Valuator->input.value(std::to_string( PSC_g_inputCMD.vals[2]).data());

          }else{
            // messageLen = sprintf(response, "<%0.2f,%0.2f;>",LEG_CONT_g_walkOpenLoopOffsets[(int)cmd.vals[0]].x, LEG_CONT_g_walkOpenLoopOffsets[(int)cmd.vals[0]].y);
            // PSC_SendToOutputBuffer( response, messageLen);
          }
          break;
        case STOF:
          if(getOrSet){
            // LEG_CONT_g_walkStartOffsets[(int)cmd.vals[0]].x = cmd.vals[1];
            // LEG_CONT_g_walkStartOffsets[(int)cmd.vals[0]].y = cmd.vals[2];
          }else{
            // messageLen = sprintf(response, "<%0.2f,%0.2f;>",LEG_CONT_g_walkStartOffsets[(int)cmd.vals[0]].x, LEG_CONT_g_walkStartOffsets[(int)cmd.vals[0]].y);
            // PSC_SendToOutputBuffer( response, messageLen);
          }
          break;
        case IMUDATA:
          if(!getOrSet){
            // messageLen = sprintf(response, "<%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f;>",
            // 						ADI_IMU_burstReadBufScaled[2],
            // 						ADI_IMU_burstReadBufScaled[3],
            // 						ADI_IMU_burstReadBufScaled[4],
            // 						ADI_IMU_burstReadBufScaled[5],
            // 						ADI_IMU_burstReadBufScaled[6],
            // 						ADI_IMU_burstReadBufScaled[7]);
            // PSC_SendToOutputBuffer( response, messageLen);
          }
          break;
        default:
          break;
          
      }
      Fl::unlock();
      Fl::awake();
      PSC_g_newCmdFromBoard = false;
    } 
  }
  return NULL;
}

void* updateDataDisplayThread(void*) {
  std::string temp;
      std::string::size_type sz;
      int newString = 1;
      int numIMUData = 0;
      bool readyForNext = true;//not waiting for response
      std::chrono::time_point<std::chrono::system_clock> start, current, end;
       std::chrono::duration<double> elapsed_seconds, refreshRate;
      start = std::chrono::system_clock::now();

      while(1){
      	if(!displayData_Group->visible()){
      		UPDATE_DATA_DISPLAY = false;
          readyForNext = true;	
      	}else{
      		UPDATE_DATA_DISPLAY = true;	
      	}
        if(UPDATE_DATA_DISPLAY ){//if update display is turned off while waiting for resposne  we will uave a problem
            //std::cout<<"debug 0\n";
            current = std::chrono::system_clock::now();
            elapsed_seconds = current-start;
            if(elapsed_seconds.count()>=.05 && readyForNext){
              readyForNext = false;
              serialPrintf(g_FD,"<");
              serialPrintf(g_FD,"imuDataRqst");
              serialPrintf(g_FD,">");
              start = std::chrono::system_clock::now();
              numIMUData++;
            }	
            //std::cout<<"debug 1\n";
            if(NEW_DATA_FROM_BOARD){
              std::string imuData[6];
              numIMUData = 0;
              temp = "";
              //std::cout<<"debug 2\n";
              std::cout<<"MSG: " << MSG;
              //std::cout<<"debug 3\n";
              for(int i=0;i< (int)MSG.size(); i++){
                  if(numIMUData>=6){
                    
                    break;		
                  }
                  if(newString){
                    temp[0] = MSG[i];
                    newString =0;
                  } 
                  if(MSG[i] == ',' && !newString){
                    newString = 1;
                    
                    imuData[numIMUData] = temp;
                    numIMUData++;
                    temp = "";
                    
                  }else{
                    temp += MSG[i];
                  }
                  
              }
              //std::cout<<"debug 1\n";
              //std::cout<<imuData<<std::endl;
              Fl::lock();
              xAcc_Value->value(std::stod(imuData[0])); 
              yAcc_Value->value(std::stod(imuData[1])); 
              zAcc_Value->value(std::stod(imuData[2])); 
              xGyro_Value->value(std::stod(imuData[3])); 
              yGyro_Value->value(std::stod(imuData[4])); 
              zGyro_Value->value(std::stod(imuData[5])); 
              Fl::unlock();
              Fl::awake();	
              MSG = "";
              NEW_DATA_FROM_BOARD = false;
              readyForNext = true;
              end = std::chrono::system_clock::now();
              refreshRate = start-end;
              //std::cout<< "Refresh period: " << refreshRate.count() <<std::endl;
            }
        }

        
    }
}

Fl_Group *control_Group=(Fl_Group *)0;

Fl_Button *downArrow_Button=(Fl_Button *)0;

Fl_Button *leftArrow_Button=(Fl_Button *)0;

Fl_Button *rightArrow_Button=(Fl_Button *)0;

Fl_Button *upArrow_Button=(Fl_Button *)0;

Fl_Button *stand_Button=(Fl_Button *)0;

Fl_Button *sit_Button=(Fl_Button *)0;

Fl_Button *layDown_Button=(Fl_Button *)0;

Fl_Button *walk_Button=(Fl_Button *)0;

Fl_Value_Input *stepDist_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *walkSpeed_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *walkHeight_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *walkDir_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *openLoopOffset1X_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *openLoopOffset1Y_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *openLoopOffset2X_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *openLoopOffset2Y_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *openLoopOffset3X_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *openLoopOffset3Y_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *openLoopOffset4X_Valuator=(Fl_Value_Input *)0;

Fl_Value_Input *openLoopOffset4Y_Valuator=(Fl_Value_Input *)0;

Fl_Button *getParams_Button=(Fl_Button *)0;

Fl_Group *logData_Group=(Fl_Group *)0;

Fl_Group *comms_Group=(Fl_Group *)0;

Fl_Input *commsInput=(Fl_Input *)0;

Fl_Button *send_Button=(Fl_Button *)0;

Fl_Text_Display *commsOutput_Output=(Fl_Text_Display *)0;

Fl_Group *displayData_Group=(Fl_Group *)0;

Fl_Value_Output *xAcc_Value=(Fl_Value_Output *)0;

Fl_Value_Output *yAcc_Value=(Fl_Value_Output *)0;

Fl_Value_Output *zAcc_Value=(Fl_Value_Output *)0;

Fl_Value_Output *xGyro_Value=(Fl_Value_Output *)0;

Fl_Value_Output *yGyro_Value=(Fl_Value_Output *)0;

Fl_Value_Output *zGyro_Value=(Fl_Value_Output *)0;

Fl_Output *errorMsg_TextDisplay=(Fl_Output *)0;

Fl_Button *connect_button=(Fl_Button *)0;

Fl_Text_Display *status_TextDisplay=(Fl_Text_Display *)0;

Fl_Button *start_Button=(Fl_Button *)0;

Fl_Button *stop_Button=(Fl_Button *)0;

Fl_Input *baudRate_Input=(Fl_Input *)0;

int main(int argc, char **argv) {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(945, 445, "G.O.O.D. B.O.I.");
    w = o; if (w) {/* empty */}
    o->box(FL_UP_BOX);
    o->color((Fl_Color)21);
    { Fl_Tabs* o = new Fl_Tabs(0, 30, 745, 375);
      o->box(FL_GLEAM_ROUND_DOWN_BOX);
      o->color((Fl_Color)19);
      o->selection_color(FL_DARK1);
      { control_Group = new Fl_Group(0, 60, 740, 345, "Control");
        { downArrow_Button = new Fl_Button(630, 175, 40, 40);
        } // Fl_Button* downArrow_Button
        { leftArrow_Button = new Fl_Button(670, 135, 40, 40);
        } // Fl_Button* leftArrow_Button
        { rightArrow_Button = new Fl_Button(590, 135, 40, 40);
        } // Fl_Button* rightArrow_Button
        { upArrow_Button = new Fl_Button(630, 95, 40, 40);
        } // Fl_Button* upArrow_Button
        { Fl_Group* o = new Fl_Group(20, 100, 70, 120);
          o->end();
        } // Fl_Group* o
        { stand_Button = new Fl_Button(20, 105, 70, 20, "Stand");
          stand_Button->callback((Fl_Callback*)controlButtons_CB, (void*)(1));
        } // Fl_Button* stand_Button
        { sit_Button = new Fl_Button(20, 135, 70, 20, "Sit");
          sit_Button->callback((Fl_Callback*)controlButtons_CB, (void*)(2));
        } // Fl_Button* sit_Button
        { layDown_Button = new Fl_Button(20, 165, 70, 20, "Lay Down");
          layDown_Button->callback((Fl_Callback*)controlButtons_CB, (void*)(3));
        } // Fl_Button* layDown_Button
        { walk_Button = new Fl_Button(20, 195, 70, 20, "Walk");
          walk_Button->callback((Fl_Callback*)controlButtons_CB, (void*)(4));
        } // Fl_Button* walk_Button
        { stepDist_Valuator = new Fl_Value_Input(225, 135, 50, 25, "Step Dist:");
          stepDist_Valuator->callback((Fl_Callback*)controlButtons_CB, (void*)(6));
        } // Fl_Value_Input* stepDist_Valuator
        { walkSpeed_Valuator = new Fl_Value_Input(225, 105, 50, 25, "Walk Speed: ");
          walkSpeed_Valuator->callback((Fl_Callback*)controlButtons_CB, (void*)(5));
        } // Fl_Value_Input* walkSpeed_Valuator
        { walkHeight_Valuator = new Fl_Value_Input(225, 165, 50, 25, "Walk Height: ");
          walkHeight_Valuator->callback((Fl_Callback*)controlButtons_CB, (void*)(7));
        } // Fl_Value_Input* walkHeight_Valuator
        { walkDir_Valuator = new Fl_Value_Input(225, 195, 50, 25, "Walk Direction: ");
          walkDir_Valuator->callback((Fl_Callback*)controlButtons_CB, (void*)(8));
        } // Fl_Value_Input* walkDir_Valuator
        { openLoopOffset1X_Valuator = new Fl_Value_Input(340, 100, 25, 25, "X: ");
          openLoopOffset1X_Valuator->callback((Fl_Callback*)OpenLoopOffset_CB, (void*)(1));
          openLoopOffset1X_Valuator->when(FL_WHEN_ENTER_KEY);
        } // Fl_Value_Input* openLoopOffset1X_Valuator
        { openLoopOffset1Y_Valuator = new Fl_Value_Input(387, 100, 25, 25, "Y: ");
          openLoopOffset1Y_Valuator->callback((Fl_Callback*)OpenLoopOffset_CB, (void*)(1));
          openLoopOffset1Y_Valuator->when(FL_WHEN_ENTER_KEY);
        } // Fl_Value_Input* openLoopOffset1Y_Valuator
        { openLoopOffset2X_Valuator = new Fl_Value_Input(475, 100, 25, 25, "X: ");
          openLoopOffset2X_Valuator->callback((Fl_Callback*)OpenLoopOffset_CB, (void*)(2));
          openLoopOffset2X_Valuator->when(FL_WHEN_ENTER_KEY);
        } // Fl_Value_Input* openLoopOffset2X_Valuator
        { openLoopOffset2Y_Valuator = new Fl_Value_Input(522, 100, 25, 25, "Y: ");
          openLoopOffset2Y_Valuator->callback((Fl_Callback*)OpenLoopOffset_CB, (void*)(2));
          openLoopOffset2Y_Valuator->when(FL_WHEN_ENTER_KEY);
        } // Fl_Value_Input* openLoopOffset2Y_Valuator
        { openLoopOffset3X_Valuator = new Fl_Value_Input(340, 185, 25, 25, "X: ");
          openLoopOffset3X_Valuator->callback((Fl_Callback*)OpenLoopOffset_CB, (void*)(3));
          openLoopOffset3X_Valuator->when(FL_WHEN_ENTER_KEY);
        } // Fl_Value_Input* openLoopOffset3X_Valuator
        { openLoopOffset3Y_Valuator = new Fl_Value_Input(387, 185, 25, 25, "Y: ");
          openLoopOffset3Y_Valuator->callback((Fl_Callback*)OpenLoopOffset_CB, (void*)(3));
          openLoopOffset3Y_Valuator->when(FL_WHEN_ENTER_KEY);
        } // Fl_Value_Input* openLoopOffset3Y_Valuator
        { openLoopOffset4X_Valuator = new Fl_Value_Input(475, 185, 25, 25, "X: ");
          openLoopOffset4X_Valuator->callback((Fl_Callback*)OpenLoopOffset_CB, (void*)(4));
          openLoopOffset4X_Valuator->when(FL_WHEN_ENTER_KEY);
        } // Fl_Value_Input* openLoopOffset4X_Valuator
        { openLoopOffset4Y_Valuator = new Fl_Value_Input(522, 185, 25, 25, "Y: ");
          openLoopOffset4Y_Valuator->callback((Fl_Callback*)OpenLoopOffset_CB, (void*)(4));
          openLoopOffset4Y_Valuator->when(FL_WHEN_ENTER_KEY);
        } // Fl_Value_Input* openLoopOffset4Y_Valuator
        { getParams_Button = new Fl_Button(15, 245, 70, 20, "Get Params");
          getParams_Button->callback((Fl_Callback*)controlButtons_CB, (void*)(9));
        } // Fl_Button* getParams_Button
        control_Group->end();
      } // Fl_Group* control_Group
      { logData_Group = new Fl_Group(0, 60, 735, 335, "Log Data");
        logData_Group->hide();
        { new Fl_File_Input(93, 82, 380, 28, "Folder Path:");
        } // Fl_File_Input* o
        { Fl_Tabs* o = new Fl_Tabs(10, 230, 705, 150);
          o->box(FL_GLEAM_THIN_DOWN_BOX);
          { Fl_Group* o = new Fl_Group(10, 280, 705, 100, "CSM");
            o->end();
            Fl_Group::current()->resizable(o);
          } // Fl_Group* o
          { Fl_Group* o = new Fl_Group(10, 260, 705, 115, "IMU");
            o->hide();
            o->end();
          } // Fl_Group* o
          o->end();
        } // Fl_Tabs* o
        { new Fl_Input(90, 115, 385, 25, "FIle Name:");
        } // Fl_Input* o
        logData_Group->end();
      } // Fl_Group* logData_Group
      { comms_Group = new Fl_Group(10, 60, 730, 335, "Comms");
        comms_Group->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        comms_Group->hide();
        { commsInput = new Fl_Input(70, 345, 565, 30, "input:");
        } // Fl_Input* commsInput
        { send_Button = new Fl_Button(655, 350, 50, 20, "Send");
          send_Button->callback((Fl_Callback*)send_Button_CB);
        } // Fl_Button* send_Button
        { commsOutput_Output = new Fl_Text_Display(75, 90, 630, 235, "Output: ");
          commsOutput_Output->color((Fl_Color)55);
          commsOutput_Output->selection_color((Fl_Color)55);
          commsOutput_Output->align(Fl_Align(FL_ALIGN_LEFT_TOP));
        } // Fl_Text_Display* commsOutput_Output
        comms_Group->end();
      } // Fl_Group* comms_Group
      { displayData_Group = new Fl_Group(0, 65, 740, 330, "Display Data");
        displayData_Group->hide();
        { xAcc_Value = new Fl_Value_Output(70, 90, 65, 25, "X Acc:");
        } // Fl_Value_Output* xAcc_Value
        { yAcc_Value = new Fl_Value_Output(70, 121, 65, 25, "Y Acc:");
        } // Fl_Value_Output* yAcc_Value
        { zAcc_Value = new Fl_Value_Output(70, 152, 65, 25, "Z Acc:");
        } // Fl_Value_Output* zAcc_Value
        { xGyro_Value = new Fl_Value_Output(70, 183, 65, 25, "X Gyro:");
        } // Fl_Value_Output* xGyro_Value
        { yGyro_Value = new Fl_Value_Output(70, 214, 65, 25, "Y Gyro:");
        } // Fl_Value_Output* yGyro_Value
        { zGyro_Value = new Fl_Value_Output(70, 245, 65, 25, "Z Gyro:");
        } // Fl_Value_Output* zGyro_Value
        displayData_Group->end();
      } // Fl_Group* displayData_Group
      o->end();
    } // Fl_Tabs* o
    { Fl_Group* o = new Fl_Group(0, 402, 740, 41);
      o->box(FL_GLEAM_THIN_DOWN_BOX);
      o->color(FL_DARK1);
      { errorMsg_TextDisplay = new Fl_Output(127, 411, 605, 25, "Error Messages:");
        errorMsg_TextDisplay->textcolor((Fl_Color)81);
      } // Fl_Output* errorMsg_TextDisplay
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(740, 60, 205, 385, "Connection");
      o->box(FL_GLEAM_DOWN_BOX);
      o->selection_color((Fl_Color)23);
      { connect_button = new Fl_Button(750, 140, 70, 20, "Connect");
        connect_button->callback((Fl_Callback*)connect_Button_CB);
      } // Fl_Button* connect_button
      { status_TextDisplay = new Fl_Text_Display(795, 95, 130, 30, "Status");
        status_TextDisplay->color(FL_GREEN);
        status_TextDisplay->selection_color((Fl_Color)9);
        status_TextDisplay->align(Fl_Align(FL_ALIGN_LEFT));
      } // Fl_Text_Display* status_TextDisplay
      { start_Button = new Fl_Button(835, 140, 70, 20, "Start");
        start_Button->callback((Fl_Callback*)start_Button_CB);
      } // Fl_Button* start_Button
      { stop_Button = new Fl_Button(835, 160, 70, 20, "Stop");
        stop_Button->callback((Fl_Callback*)stop_Button_CB, (void*)(2));
      } // Fl_Button* stop_Button
      { baudRate_Input = new Fl_Input(800, 190, 90, 25, "Baud: ");
      } // Fl_Input* baudRate_Input
      o->end();
    } // Fl_Group* o
    o->end();
  } // Fl_Double_Window* o
  Fl::lock();
    
    int rc1;
    pthread_t thread1, thread2, thread3, thread4;
    
    if((rc1=pthread_create(&thread1, NULL, serialMsgThread,(void*)1))){
    	printf("Thread creation failed: %d\n", rc1);
    }
    if((rc1=pthread_create(&thread2, NULL, updateSerialMsgWindowThread,(void*)1))){
      printf("Thread creation failed: %d\n", rc1);
    }
    if((rc1=pthread_create(&thread3, NULL, updateDataDisplayThread,(void*)1))){
      printf("Thread creation failed: %d\n", rc1);
    }
    if((rc1=pthread_create(&thread4, NULL, PSC_InterpretCommandThread,(void*)1))){
      printf("Thread creation failed: %d\n", rc1);
    }
  w->show(argc, argv);
  return Fl::run();
}

/*
Final Project: Arduino Clock
This program will include the LCD library. The wiring for this project consists of a liquid crystal display which is attached to the arduino and the breaboard with its regular wire placement. The circuit has three buttons to control the cursor, the characters and the alram on the display.
This program will read values from three different buttons and depending on the values read it will either change the placement of the cursor, change the seen values of the variables for time and date or visibly change the alarm mode from OFF to SET to ON. The program will play a sound from a piezo
when the set time for the alarm becomes true. The program will allow the user to change the variables due to the whole program calculating the time based on the seconds of the Arduino running. The whole program is based on the seconds that the Arduino has been on for.  
The clock is based on the standard that there are 30 days in a month.
Reference: Always counting digits from left to right. Threrefore setup in LCD is : hr1 hr2 : mn1 mn2 : sec1 sec2

Version 6
March 30. 2021
Written by: Alexis Fernandez
*/

#include <LiquidCrystal.h>                     //Importing the Liquid Crystal Library in order to use its functionalities.
LiquidCrystal LCD (4, 3, 11, 10, 9, 8);        //Intiliazing the liquid crystal library. Telling it what pins to use to communicate.

//CONSTANTS
const byte cursorButton = 7;       //Digital pin that the button for the cursor is connected to.              
const byte changeTimeButton = 6;   //Digital pin that the button to change the value of the characters (either time, weekday, month, date, or year) is connected to.
const byte alarmButton = 5;        //Digital pin that the button for the alarm is connected to.
const byte potPin = A2;            //Analog pin that the potentiometer is connected to.
const byte soundPin = 12;          //Digital pin that the piezo (buzzer) is connected to.

//VARIABLES

unsigned long previousSec2 = 0;              //Variable that is compared to the seconds (currentTime / 1000) that the Arduino has been on for. This variable is used to increase the seconds on the LCD as the seconds that the Arduino has been on for increase. Set as unsigned long in order to be able to compare to the the currentTime / 1000 for a long amount of time.

unsigned long currentTime;         //Variable for the the time that the Arduino has been running for. Obtained from millis(). Set as unsigned long in order for the variable to be able to hold the time that the Arduino has been on for for a long time (unsigned long can store a much larger amount of values than int and long).

int hr1 = 1;   //Variable for the first digit of the hours displayed in the LCD. Ranges from 0 to 2. 
int hr2 = 5;   //Variable for the second digit of the hours displayed in the LCD. Ranges from 0 to 9. When being changed by the program, the variable will run through two loops where it the value ranges from 0 to 9 and one last loop where the value ranges from 0 to 3. This is because in hours in a day (starting from hour 0) the seocnd digit of the hour goes up to 9 twice (hour 9 and hour 19) and then it goes up to 3 (hour 23)
int mn1 = 3;   //Variable for the first digit of the minutes displayed in the LCD. Ranges from 0 to 5 (because the minutes go up to 59 thererefore the first digit goes up to 5)
int mn2 = 1;   //Variable for the second digit of the minutes displayed in the LCD. Ranges from 0 to 9.
int sec1 = 2;  //Variable for the first digit of the seconds displayed in the LCD. Ranges from 0 to 5. (because seconds go from 0 to 59).
int sec2 = 5;  //Variable for the second digit of the seconds displayed in the LCD. Ranges from 0 to 9.

boolean cursorButtonState;     //State of the button that controls the cursor
int cursorButtonPresses = 0;   //Variable for the number of times that the cusor button is pressed
int MAPPEDcursorButtonPresses0;     //Mapped values from the times that the cursor button is pressed for the first line (line 0). Values are mapped from 1 to 16 to 0 to 15.
int MAPPEDcursorButtonPresses1;     //Mapped values from the times that the cursor button is being pressed for the second line (line 1).Values are mapped from 17 to 32 to 0 and 15.

boolean changeTimeButtonState;    //State of the button that changes the value of the different characters.
int changeTimeButtonPresses = 0;  //Variable for the number of times that the change value button is being pressed.
int MAPPEDchangeTimeButtonPresses;  //Variable for the number of times that the change time button is being pressed. Values are mapped from 1 to 10 to 0 to 9.
int MAPPEDchangeTimeButtonPressesMNTH; //Variable for the number of times that the change time button is being pressed when the cursor is in the place of the month. This is done separately because there are 12 months in a year therefore not allowing the use of the originally mapped values. Values are mapped from 1 to 12 to 0 to 11.

int wkday = 6;   // Variable for the weekday displayed on the LCD. Monday is zero, and Sunday is 6.
int mnth = 11;   //Variable for the mnth displayed on the LCD. January is 0, December is 11.
int date1 = 3;   //Variable for the first digit of the date displayed on the LCD. Goes from 0 to 3 because there is an average of 30 days in each month. 
int date2 = 0;   //Variable for the second digit of the date displayed on the LCD. This value is set to go up to 9 for three cycles (ones, tenths, twenties) and then only goes up to 0 for the last cycle. Reminder that clock work on a standard of 30 days in a month.
int yr1 = 2;     //Variable for the first digit of the year displayed on the LCD. Value can go from 0 to 9.
int yr2 = 0;     //Variable for the second digit of the year displayed on the LCD. Value can go from 0 to 9.

int alarm = false;    //Variable that holds if the alarm state is true or false (if it is set to on or off)
boolean alarmButtonState;  //Variable that holds the state of the alarm button.
int alarmButtonPresses = 0;  //Variable that holds the amount of times the alarm button is pressed.

int pitch;   //Variable that holds the pitch (frequency) that the piezo will play the sound at.
int potVal;  //Variable for the value read from the analog pin that the potentiometer to control the pitch is connnected to.

int ALARMhr1 = 0;  //Variable for the first digit of the hour that the alarm is set to.
int ALARMhr2 = 0;  //Variable for the second digit of the hour that the alarm is set to.
int ALARMmn1 = 0;  //Variable for the first digit of the minute that the alarm is set to.
int ALARMmn2 = 0;  //Variable for the second digit of the minute that the alarm is set to.
int ALARMsec1 = 0; //Variable for the first digit of the second that the alarm is set to.
int ALARMsec2 = 0; //Variable for the second digit of the second that the alarm is set to.


void setup() 
{
  Serial.begin(9600);                 //Opening a serial port for serial communication.

  pinMode(cursorButton, INPUT);       //Setting the cursor button pin as an input.
  pinMode(changeTimeButton, INPUT);   //Setting the change time button pin as an input.
  pinMode(alarmButton, INPUT);        //Setting the alarm button pin as an input.
  pinMode(soundPin, OUTPUT);          //Setting the sound pin that the piezo is connnected to as an input.

  LCD.begin(16,2);                    //Starting the LCD library and telling it how large the screen is (16 by 2 characters).
}

void loop() 
{
  cursorButtonState = digitalRead(cursorButton);          //Reading the input of the cursor button and storing it in the variable cursorButtonState.
  changeTimeButtonState = digitalRead(changeTimeButton);  //Reading the input of the change time button and storing it in the variable changeTimeButtonState.
  alarmButtonState = digitalRead(alarmButton);            //Reading the input of the alarm button and storing it in the variable alarmButtonState.      
  potVal = analogRead(potPin);                            //Reading the analog input of the potentiometer and storing it in the variable potVal.
  
  currentTime = millis();           //Storing the current time (in milliseconds) that the Arduino has been on for in the variable currentTime.

  
  if (currentTime / 1000 > previousSec2)  //If the currentTime divided by a thousand (seconds that the Arduino has been running for) is greater than the previousSec2 variable which is set to 0.
  {
    sec2 ++;           //Increase the ones place of the seconds. Basically increasing the second as each second passes form the Arduino running.     
    previousSec2 ++;   //Increase the variable in order for this process to repeat and keep increasing the seconds.

    if (sec2 > 9)      //If the second digit of the seconds is greater than 9, set the second digit of the seconds to zero and increase the first digit of the seconds by one.
    {
      sec2 = 0;       
      sec1 ++;
      if (sec1 > 5)    //If the first digit of seconds is greater than 5 (meaning reached 59 seconds), set the first digit back to zero and increase the ones of the minute by one.
      {
        sec1 = 0;
        mn2 ++;

        if(mn2 > 9)    //If the second digit of the minute is greater than 9, set the second digit back to zero and increase the first digit of the minutes
        {
          mn2 = 0;
          mn1++;
          if (mn1 > 5)   //If minute1 is greater than 5 (has reached 59 minutes), set min1 to zero and and increase the second digit of the hour.
          {
            mn1 = 0;
            hr2 ++;
                           //Now there are three possibilites:
            if (hr1 == 0)  //1. If hr 1 is zero and hr 2 is greater than 9, set hour2 to zero and increase hour1.  
            {
              if (hr2 > 9)
              {
                hr2 = 0;
                hr1 ++;
              }
            }

            if (hr1 == 1) //2. If hour1 is equal to one and hr2 is greater than 9, set hour 2 to zero and increase hour1.
            {
              if(hr2 > 9)
              {
                hr2 = 0;
                hr1 ++;
              }  
            }

            if (hr1 == 2)   //3. If hr1 is equal to 2 and hour2 is greater than 3, set hr2 and hr1 to zero, increase weekday and the ones of the date.
            {
              if(hr2 > 3)
              {
                hr2 = 0;
                hr1 = 0;
                wkday ++;          
                date2 ++;
                if (wkday > 6)  //If weekday is greater than 6 (referring to Sunday), set weekday to zero.
                {
                  wkday = 0;
                }               //4 other possibilites:
                if(date1 == 0)  //1. If date1 is equal to zero and date2 is greater than 9, increase the day (date1) and set date2 to zero.
                {
                  if(date2 > 9)
                  {
                  date1 ++;
                  date2 = 0;
                  }
                }
                if (date1 == 1)    //2. If date1 is equal to one and date2 is greater than 9, increase the day (date1) and set date2 to zero.
                {
                  if(date2 > 9)
                  {
                    date1 ++;
                    date2 = 0;
                  }
                }
                if (date1 == 2)   //3. If date1 is equal to two and date2 is greater than 9, increase the day (date1) and set date2 to zero.
                {
                  if (date2 > 9)
                  {
                    date1 ++;
                    date2 = 0;
                  }
                }
                if (date1 == 3)        //4. If date1 is equal to three and date2 is greater than 0, (reset back to the first of the month) date1 to zero and date2 to 1. Also increase month.
                {
                  if (date2 > 0)
                  {
                    date1 = 0;
                    date2 = 1;
                    mnth ++;
                    if(mnth > 11)    //If month is greater than 11, inncrese year and set month to zero.
                    {
                      mnth = 0;
                      yr2++;
                      if (yr2 > 9)    //If year2 is greater than 9, set year 2 to zero and increase yr1 
                      {
                        yr2 = 0;
                        yr1 ++;
                        if (yr1 > 9)  //If yr is greater than 9, set yr1 to zero.
                        {
                          yr1 = 0;
                        }
                      }
                    }
                  }
                } 
              }
            }      
          }
        }
      }
    }
  }

  //Above is just saying that if you increase seconds > increase minutes > increase hours > increase days and weekdays > increase months > increase years
  

 
  if (alarmButtonState == LOW && alarmButtonPresses != 1)  //If the alarm button is not being pressed and the number of times that the alarm  is pressed is not equal to one (meaning that alarm is not on the "SET" mode), print hours, mins and seconds onto LCD.
  {
    LCD.setCursor(0,0);
    LCD.print(hr1);
    LCD.setCursor(1,0);
    LCD.print(hr2);
    LCD.setCursor(2,0);
    LCD.print(":");
  
    LCD.setCursor(3,0);
    LCD.print(mn1);
    LCD.setCursor(4,0);
    LCD.print(mn2);
    LCD.setCursor(5,0);
    LCD.print(":");

    LCD.setCursor(6,0);
    LCD.print(sec1);
    LCD.setCursor(7,0);
    LCD.print(sec2);
    LCD.noCursor();            //Not displaying the cursor that is placed after seconds
  }


   LCD.setCursor(2,1);    //Printing the date
   LCD.print("/");

   LCD.setCursor(3,1);
   LCD.print(date1);

   LCD.setCursor(4,1); 
   LCD.print(date2);
 
   LCD.setCursor(5,1);
   LCD.print("/");

   LCD.setCursor(6,1);
   LCD.print(yr1);

   LCD.setCursor(7,1);
   LCD.print(yr2);


  if (cursorButtonState == HIGH)    //If the cursor button is being pressed, increase the amount of times it has been pressed.
  {
    cursorButtonPresses ++;  
    delay (250);                    //Small delay after button is pressed in order to obtain the value of one button press and not more than one (with one press). If the button was touched and no delay was set, more than one button presses would be recorded making it very innacurate
  }
  
  if (cursorButtonPresses > 0 && cursorButtonPresses < 17)                   // Mapping button presses on the first line from 1-16 to 0-15.
  {
    MAPPEDcursorButtonPresses0 = map(cursorButtonPresses, 1, 16, 0, 15);      //Mapping value of cursor button presses to value of characters on the LCD
    LCD.setCursor(MAPPEDcursorButtonPresses0, 0);  
    LCD.cursor();
  }

  if (cursorButtonPresses > 16 && cursorButtonPresses < 33)                     //Mapping cursor values for the second line.
  {
    MAPPEDcursorButtonPresses1 = map(cursorButtonPresses, 17, 32, 0, 15);       //Mapping value of cursor button presses to value of characters on the LCD
    LCD.setCursor(MAPPEDcursorButtonPresses1, 1);                               //Printing onto the second line
    LCD.cursor();
  }

  if (cursorButtonPresses > 32)     //If the cursor is pressed 33 times, the value of cursorButtonPresses will be reset back to zero. If cursorButtonPresses has a value of zero, the cursor has no placement therefore this means that when this happens, the cursor will dissapear from the screen until you press the cursorButton again.
  {                                //Allowing the cursor to dissapear is helpful in order for the screen to look visually appealing instead of having a cursor on the screen at all the times.
    cursorButtonPresses = 0;
  }
  



  
  
 
  if (changeTimeButtonState == HIGH)    //A value on the selected character will only display if changeTimeButton is pressed. If it is not pressed, no value will be displayed.
  {
    changeTimeButtonPresses ++;       //Increase the amount of presses
    delay (250);                     //Small delay after button is pressed in order to obtain the value of one button press and not more than one (with one press). If the button was touched and no delay was set, more than one button presses would be recorded making it very innacurate


    if (cursorButtonPresses > 16 && cursorButtonPresses < 19)  //If the cursor is in the month spot. Doing this because months need to be 12.
    {
      if (changeTimeButtonPresses > 0 && changeTimeButtonPresses < 13)      //Mapping values from 1 to 12 to 0 to 11.
      {
         MAPPEDchangeTimeButtonPressesMNTH = map(changeTimeButtonPresses, 1, 12, 0, 11);
         LCD.print(MAPPEDchangeTimeButtonPressesMNTH);
         LCD.noCursor(); 
      }
      
      else    //Else, displaying nothing.
      {
        changeTimeButtonPresses = 0;                         
        LCD.print(" ");      
        LCD.noCursor();
      }
    }

    else                    //If the cursor is not in the position of the months, then keep the values from 0 to 9                                               
    {
      if (changeTimeButtonPresses > 0 && changeTimeButtonPresses < 11)
      {
        MAPPEDchangeTimeButtonPresses = map(changeTimeButtonPresses, 1, 10, 0, 9);  //Mapping values to 0 to 9
   
          LCD.print(MAPPEDchangeTimeButtonPresses);  //After setting the position of the cursor depending on the amount of presses of the button, we will now write on the character that is selected depending on the amount of times the change time button is pressed.
          LCD.noCursor();                            //Eliminating the cursor that displays after the character
      
      }
      
      else          //If the change time button is presssed 11 times or value is zero, there will be no character displayed and the value of changeTimeButtonPresses will be reset to zero so next time you press the button, the character 0 will apear on the screen.
      {
        changeTimeButtonPresses = 0; 
      }
      
      if (changeTimeButtonPresses == 0)
      {
        LCD.print(" ");
        LCD.noCursor();    //Eliminating the cursor that displays after the character
      }
     }
  }


  if (cursorButtonState ==  HIGH)           //If the cursor is moved to the next character, the value to change the time resets back to zero (displaying nothing) and the character displayed in the new cursor spot can be now be changed with the changeTimeButton.
  {
    changeTimeButtonPresses = 0;
  }






//Restricting the values of the different varibles like , minute, hour, day, month, year:


if(cursorButtonPresses == 1 && changeTimeButtonPresses >= 1)  //If the cursor is in the postiion of the vairiable (in this case hr1)restrict the value depending on whether the alarm button is being pressed or not.
{
  
  if (alarmButtonPresses == 1)
  {
    ALARMhr1 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses >  2)    
    {
    changeTimeButtonPresses =  0;       
    }
  }
  
  if (alarmButtonState == LOW && alarmButtonPresses != 1)
  {
    hr1 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses >  2)   //Hours in military time do not go past 24 hrs    
    {
      changeTimeButtonPresses =  0;       
    }
  }
}




if(cursorButtonPresses == 2 && changeTimeButtonPresses >= 1)
{
  if(alarmButtonPresses == 1)
  {
    ALARMhr2 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses  >  9) 
    {
    changeTimeButtonPresses =  0;                              
    }
  }
  if (alarmButtonState == LOW && alarmButtonPresses != 1)
  {
    hr2 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses  >  9)  
    {
      changeTimeButtonPresses =  0;                              
    }
  }
}



if(cursorButtonPresses == 4 && changeTimeButtonPresses >= 1)
{
  if (alarmButtonPresses == 1)
  {
    ALARMmn1 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses  >  5)   
    {
    changeTimeButtonPresses =  0;
    }
  }
  if (alarmButtonState == LOW && alarmButtonPresses != 1)
  {
    mn1 = MAPPEDchangeTimeButtonPresses;
  
    if(changeTimeButtonPresses  >  5)   
    {
      changeTimeButtonPresses =  0;
    }
  }
}



if(cursorButtonPresses == 5 && changeTimeButtonPresses >= 1)
{
  if(alarmButtonPresses == 1)
  {
    ALARMmn2 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses  >  9) 
    {
    changeTimeButtonPresses =  0;                              
    }
  }
  if (alarmButtonState == LOW && alarmButtonPresses != 1)
  {
    mn2 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses  >  9) 
    {
      changeTimeButtonPresses =  0;                              
    }
  }
}




if(cursorButtonPresses == 7 && changeTimeButtonPresses >= 1)
{
  if(alarmButtonPresses == 1)
  {
    ALARMsec1 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses > 5)
    {
      changeTimeButtonPresses = 0;
    }
  }

  if (alarmButtonState == LOW && alarmButtonPresses != 1)
  {
    sec1 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses  >  5)   
    {
      changeTimeButtonPresses =  0;
    }
  } 
}

  


 if(cursorButtonPresses == 8 && changeTimeButtonPresses >= 1)
{
  if (alarmButtonPresses == 1)
  {
    ALARMsec2 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses  >  9)   
    {
      changeTimeButtonPresses =  0;
    }
  }
  if (alarmButtonState == LOW && alarmButtonPresses != 1)
  {
    sec2 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses  >  9)   
    {
      changeTimeButtonPresses =  0;
    }
  }
}


  LCD.setCursor(11,1);   //Setting the cursor to write the weekday
  
   switch(wkday) //If weekday is equal to any value below, it will print the corresponding weekday.
  {
    case 0:
    LCD.print("Mon");
    break;


    case 1:
    LCD.print("Tue");
    break;


    case 2:
    LCD.print("Wed");
    break;


    case 3:
    LCD.print("Thu");
    break;


    case 4:
    LCD.print("Fri");
    break;


    case 5:
    LCD.print("Sat");
    break;


    case 6:
    LCD.print("Sun");
    break;
  }



  if(cursorButtonPresses > 27 && cursorButtonPresses < 31 && changeTimeButtonPresses >= 1)  //Making it so you can place the cursor anywhere where the weekday is displayed in order to switch the day
  {
    wkday = MAPPEDchangeTimeButtonPresses;
    if (changeTimeButtonPresses > 7)
    {
     changeTimeButtonPresses = 0;
    }
  }


 



LCD.setCursor(0,1);   //Setting the cursor to write the month
  
   switch(mnth)         //Printing corresponding month according the the value of month
  {
    case 0:
    LCD.print("JA");
    break;


    case 1:
    LCD.print("FE");
    break;


    case 2:
    LCD.print("MR");
    break;


    case 3:
    LCD.print("AL");
    break;


    case 4:
    LCD.print("MA");
    break;


    case 5:
    LCD.print("JN");
    break;


    case 6:
    LCD.print("JL");
    break;


    case 7:
    LCD.print("AU");
    break;


    case 8:
    LCD.print("SE");
    break;


    case 9:
    LCD.print("OC");
    break;


    case 10:
    LCD.print("NO");
    break;


    case 11:
    LCD.print("DE");
    break;
  }


  if(cursorButtonPresses > 16 && cursorButtonPresses < 19 && changeTimeButtonPresses >= 1)  //Making it so you can swtich the month from any of the month's characters on the LCD.
  {
    mnth = MAPPEDchangeTimeButtonPressesMNTH;
    if (changeTimeButtonPresses > 12)
    {
      changeTimeButtonPresses = 0;
    }
  }



 
  if(cursorButtonPresses == 20 && changeTimeButtonPresses >=1)     //Depending on the cursor placement and if the change time button is pressed, can change the value of the selected character with the changeTimeButton. Same applies for below.
  {
    date1 = MAPPEDchangeTimeButtonPresses;
    if(changeTimeButtonPresses > 3)
    {
      changeTimeButtonPresses = 0;
    }
  }



  if(cursorButtonPresses == 21 && changeTimeButtonPresses >=1)
  {
    date2 = MAPPEDchangeTimeButtonPresses;
  }




  
  if(cursorButtonPresses == 23 && changeTimeButtonPresses >=1)
  {
    yr1 = MAPPEDchangeTimeButtonPresses;
  }



  if(cursorButtonPresses == 24 && changeTimeButtonPresses >=1)
  {
    yr2 = MAPPEDchangeTimeButtonPresses;
  }




  

   if (alarm == false)    //If the alarm is not set, print "OFF"
  {
    LCD.setCursor(13,0);
    LCD.print("OFF"); 
  }

  LCD.setCursor(10,0);   //Printing Al: for alarm
  LCD.print("Al:");
  LCD.noCursor();
  


  
  if (alarmButtonState == HIGH) //if alarm button is pressed
  {  
    
    alarmButtonPresses ++;   //Increase presses
    delay (250);
  
    
    if (alarmButtonPresses == 1)   //If equal to 1: Put alarm mode on SET and print hours mins and seconds to zero. These can then be changed separately from running time with the change time button   
    {
      alarm = true;
      LCD.setCursor(13,0);
      LCD.print("SET");
      LCD.noCursor();
        
  LCD.setCursor(0,0);
  LCD.print(ALARMhr1);
  LCD.setCursor(1,0);
  LCD.print(ALARMhr2);
  LCD.setCursor(2,0);
  LCD.print(":");
  
  LCD.setCursor(3,0);
  LCD.print(ALARMmn1);
  LCD.setCursor(4,0);    //Keep in mind seconds will keep running when settin the alarm
  LCD.print(ALARMmn2);
  LCD.setCursor(5,0);
  LCD.print(":");

  LCD.setCursor(6,0);
  LCD.print(ALARMsec1);
  LCD.setCursor(7,0);
  LCD.print(ALARMsec2);
  LCD.noCursor();

    }
  }


  pitch = map (potVal, 0, 1023, 100, 1000);    //Mapping the values from the potentiometer to acceptable values for the pitch of the sound from the piezo

  
  
    if (alarmButtonState == LOW && alarmButtonPresses == 2)  //If the alarm has been set, change mode to on (print ON)
    {
      
      alarm = true;
      LCD.setCursor(13,0);
      LCD.print("ON");
      LCD.setCursor(15,0);
      LCD.print(" ");

     
       if (hr1 == ALARMhr1 && hr2 == ALARMhr2 && mn1 == ALARMmn1 && mn2 == ALARMmn2 && sec1 == ALARMsec1 && sec2 == ALARMsec2)   //If the running time is the same as the set time for the alarm and if the alarm button is not being pressed, play the sound from the piezo.
       {
       if (alarmButtonState == LOW) //Alarm will go off until the alarm button is pushed
       {
         tone (soundPin, pitch);
       }
       
       else          //When the alarm button is pushed, there will be no sound produced and the alarm will stop, turning the mode to OFF. Also printing all of the running time variables (hours, mins and seconds)
       {
        noTone(soundPin);
        alarmButtonPresses = 0;
        alarm = false;
        LCD.setCursor(13,0);
        LCD.print("OFF");

        LCD.clear();
        LCD.setCursor(0,0);
        LCD.print(hr1);
        LCD.setCursor(1,0);
        LCD.print(hr2);
        LCD.setCursor(2,0);
        LCD.print(":");
  
        LCD.setCursor(3,0);
        LCD.print(mn1);
        LCD.setCursor(4,0);
        LCD.print(mn2);
        LCD.setCursor(5,0);
        LCD.print(":");

        LCD.setCursor(6,0);
        LCD.print(sec1);
        LCD.setCursor(7,0);
        LCD.print(sec2);
        LCD.noCursor();
       }
    } 
  }
    

   if (alarmButtonState == LOW && alarmButtonPresses > 2)        //If the alarm buttonState is low and the alarm button has been pressed more than two times, turn alarm mode to OFF not playing a sound.
  {
      noTone(soundPin);        //If the alarm is the off mode, no sound will be heard.
      alarmButtonPresses = 0;
      alarm = false;
      LCD.setCursor(13,0);
      LCD.print("OFF");
  }
}

  

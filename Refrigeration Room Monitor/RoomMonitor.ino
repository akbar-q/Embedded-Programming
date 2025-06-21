  //Including the I2C LCD Library to Drive the Display

  #include <LiquidCrystal_I2C.h>

  //Assign Adress 0x27 to the LCD address for a 16 char and 2 line display

  LiquidCrystal_I2C lcd(0x27,20,4);  

//---------------------------------------------------------------------------//

//Defining Global Variables For Time Calculation

  unsigned long initialTime;      //Start Point Variable for Calculation
  unsigned long finalTime;        //End Point Vartiable for Calculation
  unsigned long timeOpen;         //Duration of Door Being Open

  int secondsOpen;                //Duration Open in Seconds

//Defining Variables to Name Pin Numbers

  byte     LED = 13;
  byte  Buzzer = 10;
  
  byte Switch1 = 11;
  byte Switch2 = 12;
  
  byte    Test = 8;
  byte sysDis  = 4;
  byte Restart = 5;

  byte   Relay = 6;
  
//Defining Flags for the Code

  bool DoorStat;                  //Binary Status of the Door Flag
  bool ErrorStat;                 //Binary Error Condition Flag

  bool State1;                    //Binary State for Switch 1
  bool State2;                    //Binary State for Switch 2
  bool TestSw;                    //Binary State for Test Button

  bool disableState;              //Binary State for System Disable
  bool powerState;                //Binary State for Reset

//Overflow Prevention Threshold at 24days as milliseconds in UnsignedLong

unsigned long threshold = 24UL * (60UL * 60UL * 1000UL);

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

//Setup Function which will run Once at Boot

void setup() {

/*Defining the Function of the Pins as Output for the LED and Buzzer
 *and Input with the built-in pull up resistor for the Switches
 */

  pinMode(LED,           OUTPUT);  //5mm LED
  pinMode(Buzzer,        OUTPUT);  //Active Buzzer
  pinMode(Switch1, INPUT_PULLUP);  //Push Button1 / Limit Switch1
  pinMode(Switch2, INPUT_PULLUP);  //Push Button2 / Limit Switch2
  pinMode(Test,    INPUT_PULLUP);  //Push Button3
  pinMode(sysDis,  INPUT_PULLUP);  //Toggle Switch / SPST
  pinMode(Restart, INPUT_PULLUP);  //Push Button4

  pinMode(Relay,         OUTPUT);  //Setting Relay Trigger as Output

  lcd.init();                      //Running the Initialize LCD Function 
  lcd.backlight();                 //Running the Initialize Backlight Function

  digitalWrite(Relay, LOW);        //Disable Relay
}

//---------------------------------------------------------------------------//

//Function for Testing the Output Devices

    void test(){
      digitalWrite(LED,    HIGH);      //Turn ON The LED
      digitalWrite(Buzzer, HIGH);      //Turn ON the Buzzer

      lcd.setCursor(0,0);              //Setting the Cursor to Line 0 Char 0
      lcd.print("Outputs Test");       //Display the Author Location 
            
      lcd.setCursor(0,1);              //Setting the Cursoe to Line 1 Char 0
      lcd.print("by Akbar Q");         //Display the Name of the Coder
      
        delay(200);                    //Add a 200 milliSeconds Delay

      digitalWrite(LED,    LOW);       //Turn OFF LED         
      digitalWrite(Buzzer, LOW);       //Turn OFF Buzzer
    }

//Defining the Slow Beep Function and Toggle the Buzzer with a Delay

    void slowBeep(){                
          digitalWrite(Buzzer, HIGH);
            delay(50);
          digitalWrite (Buzzer, LOW);
            delay(850);
    }

    //Defining the Fast Beep Function and Toggle the Buzzer with a short Delay

    void fastBeep(){
          digitalWrite(Buzzer, HIGH);
            delay(20);
          digitalWrite (Buzzer, LOW);
            delay(350);
    }

    //Clear the Display

    void dispClear(){
      
      lcd.setCursor(0,0);               //Set Cursor to Line 0 Char 0
        lcd.print("                ");  //Write 16 Blank Chars
      lcd.setCursor(0,1);               //Set Cursor to Line 1 Char 0
        lcd.print("                ");  //Write 16 Blank Chars
    }

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

void loop() {
  
  initialTime = millis();         //Initial Time When the Loop Starts

  digitalWrite(LED, LOW);         //Keep LED OFF

  digitalWrite(Relay, LOW);       //Disable Relay

//---------------------------------------------------------------------------//

//Check for System Disable

  disableState = digitalRead(sysDis);

  while( disableState == 1){
    
      //Displaying System Status
      
      lcd.setCursor(0,0);               //Set Cursor to Line 0 Char 0
      lcd.print("System Disabled ");    //Display System Status
      lcd.setCursor(0,1);               //Set Cursor to Line 1 Char 0
      lcd.print("In/Out OFF      ");    //Display I/O Status

      //Checking Switch State to Break Loop
      
      disableState = digitalRead(sysDis);
    
  }

//---------------------------------------------------------------------------//

//Checking the State of the Switches and Storing Values

  State1 = digitalRead(Switch1);
  State2 = digitalRead(Switch2);

//---------------------------------------------------------------------------//

//Test Function Call
 TestSw = digitalRead(Test);
 
 while (TestSw == 0){
    test();
    TestSw = digitalRead(Test);
 }

//---------------------------------------------------------------------------//


//If Statement which Runs when both Switches are Closed
  
  if (State1 == 0 && State2 == 0){
    
    DoorStat  = 0;                     // Door Open Flag is made Flase
    ErrorStat = 0;                     //Door Error Flag is made False

      //Displaying System Status
      lcd.setCursor(0,0);              //Set Cursor to Line 0 Char 0
      lcd.print("System Normal  ");   //Display Normal Status
      lcd.setCursor(0,1);              //Set Cursor to Line 1 Char 0
      lcd.print("Door is Closed "); //Display Door Status

  }

  dispClear();                    //Clear Display

//While Loop that Runs when both Switches are Open

  while (State1 == 1 && State2 == 1){
     DoorStat  = 1;                 // Door Open Flag is made True
     ErrorStat = 0;                 //Door Error Flag is made Flase

      finalTime = millis();           //Final Time is Taken as Current Time

      //Total Time Open is Calculated from Initial LoopStart Time and Current Time
      timeOpen = finalTime - initialTime;

      //Calculating Seconds by Dividing by 1,000 as Unsigned Long Value
      secondsOpen = timeOpen / 1000UL;

      digitalWrite(LED, HIGH);        //Turning ON the LED

    //---------------------------------------------------------------------------//

        //Print Time and Status

          lcd.setCursor(0,0);             //Set Cursor to Line 0 Char 0
            lcd.print("Door is Open    ");  //Print Door Status

          lcd.setCursor(0,1);             //Set Cursor to Line 1 Char 0
            lcd.print(secondsOpen);         //Print Value of Seconds

          lcd.setCursor(8,1);             //Set Cursor to the Right Edge
            lcd.print(" Seconds");          //Print the Unit of Time

            if (secondsOpen > 30 && secondsOpen < 60)
              { slowBeep(); }

            if (secondsOpen > 60)
              { fastBeep(); }

              //Checking the State of the Switches to Break the Loop

              State1 = digitalRead(Switch1);
              State2 = digitalRead(Switch2);
    
  } //Both Switch While Loop

//---------------------------------------------------------------------------//

//If Statement which Runs when both Switches are Mismatched
  
  if (State1 != State2){
    delay(200);                           //Wait 200ms
       if (State1 != State2){             //Check if Mismatch Still Present
         DoorStat = 0;                    //Door Open Flag is made Flase
         ErrorStat = 1;                   //Door Error Flag is made False
 
           //Displaying System Status
          lcd.setCursor(0,0);             //Set Cursor to Line 0 Char 0
            lcd.print("System In Error ");  //Display System Status
          
          lcd.setCursor(0,1);             //Set Cursor to Line 1 Char 0
            lcd.print("Switch Mismatch ");  //Display Error Type Status

          slowBeep();                     //Run Buzzer
       }
  }

//---------------------------------------------------------------------------//

//Function to Prevent Time Vairable Overflow

//Check if the millis Time Count is Greater than Threshold

if (threshold < millis() && DoorStat == 0){
  delay(50);
    digitalWrite(Relay, HIGH);              //Trigger Relay
  delay(50);
  }

  powerState = digitalRead(Restart);        //Check State of Restart Button
                                
//If Restart Button is Pressed trigger Relay

  if (powerState == 0 && DoorStat == 0){
      delay(50);
        digitalWrite(Relay, HIGH);
        digitalWrite(LED,   HIGH);
      delay(50);
  }

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

} //Void Loop Close

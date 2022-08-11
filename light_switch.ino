/*
  ___       ___  ________  ___  ___  _________        ________  ___       __   ___  _________  ________  ___  ___
 |\  \     |\  \|\   ____\|\  \|\  \|\___   ___\     |\   ____\|\  \     |\  \|\  \|\___   ___\\   ____\|\  \|\  \
 \ \  \    \ \  \ \  \___|\ \  \\\  \|___ \  \_|     \ \  \___|\ \  \    \ \  \ \  \|___ \  \_\ \  \___|\ \  \\\  \
  \ \  \    \ \  \ \  \  __\ \   __  \   \ \  \       \ \_____  \ \  \  __\ \  \ \  \   \ \  \ \ \  \    \ \   __  \
   \ \  \____\ \  \ \  \|\  \ \  \ \  \   \ \  \       \|____|\  \ \  \|\__\_\  \ \  \   \ \  \ \ \  \____\ \  \ \  \
    \ \_______\ \__\ \_______\ \__\ \__\   \ \__\        ____\_\  \ \____________\ \__\   \ \__\ \ \_______\ \__\ \__\
     \|_______|\|__|\|_______|\|__|\|__|    \|__|       |\_________\|____________|\|__|    \|__|  \|_______|\|__|\|__|
                                                        \|_________|

*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SoftwareSerial.h>
//#include <IRremote.hpp>
#include <pitches.h>


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
//IRsend IR_SEND_PIN;


/***************Addons***************/
#define Servo_On       9        // Right Servo pin connect to pin 9
#define Buzz_Pin       13       // Active Buzzer pin connect to pin 13
#define trigPin        4        // Trigger Pin UDR Module connect to pin 4
#define echoPin        5        // Echo Pin UDR Module connect to pin 3


int duration = 500;  // 500 miliseconds

/***************Ultrasonic & Locking Variables***************/
const int MIN_DIST = 0;         // in CM
const int MAX_DIST = 15;        // in CM
long duration_scan = 0;         // duration of scanning variable
int  distance = 0;              // current distance in CM of an object in front of the sensor
bool switch_state = false;      // whether or not the lightswitch is on
bool flag_lock = false;         // whether or not the sensor_lock is enabled
bool remote_lock = false;       // whether or not the remote is enabled


/***************Serial Variables***************/
#define RXd1 0
SoftwareSerial espSerial(0, 1); // Emulate a listening Serial connection on pins 1(tx, rx)

String inputString = "";        // a String to hold incoming data
bool stringComplete = false;    // whether the string is complete


/***************Sensor Variables***************/
int  sensorAnalogPin = A0;      // Select the Arduino input pin to accept the Sound Sensor's analog output
int  sensorDigitalPin = 7;      // Select the Arduino input pin to accept the Sound Sensor's digital output
int  analogValue = 0;           // Define variable to store the analog value coming from the Sound Sensor
int  soundLevel = 0;            // Define variable to store the digital value coming from the Sound Sensor
int  Led13 = 13;                // Define LED port; this is the LED built in to the Arduino (labled L)


enum DN
{
  SWITCH_ON,                    // Light Switch On
  SWITCH_OFF,                   // Light Switch Off
  SWITCH_RESET,                 // Unlock Sensors
  SWITCH_LOCK,                  // Lock Sensors
  DISPLAY_OFF,                  // Turn Display Off
  DEF
} Tick_Num = DEF;


/**************Servo IR code***************/
Servo servo_On;   // Defines On Servo

// Change the position of the Servo briefly so as to toggle the lightswitch
void Lswitch_On()
{
  servo_On.write(125);
  delay(250);
  servo_On.write(90);
  lcd_LS_toggle("Lightswitch On");
}

void Lswitch_Off()
{
  servo_On.write(55);
  delay(250);
  servo_On.write(90);
  lcd_LS_toggle("Lightswitch Off");
}

void Lswitch_Reset()
{
  servo_On.write(90);
  lcd_display_off();
}

void Sensor_Lock()
{
  lcd_display_lock();
}

// Custom Characters for LCD
/************LCD Function Code**************/
byte lockchar[] = {
  0x00,
  0x0E,
  0x11,
  0x11,
  0x1F,
  0x1B,
  0x1B,
  0x1F
};

byte unlockchar[] = {
  B01110,
  B10001,
  B10000,
  B10000,
  B11111,
  B11011,
  B11011,
  B11111
};

// LCD messages
void lcd_LS_toggle(const char* message)
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print(message);
  delay(1500);
  lcd.off();
}

void lcd_display_off()
{
  lcd.createChar(1, unlockchar);
  lcd.begin(16, 2);
  lcd.setCursor(3, 0);
  lcd.print("Unlocking");
  lcd.setCursor(1, 1);
  lcd.print("Sensor Input ");
  lcd.write(byte(1));
  flag_lock = false;
  delay(2500);
  lcd.off();
}

void lcd_display_lock()
{
  
  lcd.createChar(0, lockchar);
  lcd.begin(16, 2);
  lcd.setCursor(4, 0);
  lcd.print("Locking");
  lcd.setCursor(1, 1);
  lcd.print("Sensor Input ");
  lcd.write(byte(0));
  flag_lock = true;
  delay(2500);
  lcd.off();
}
// To lazy to remove the buzzer code
/***********Active Buzzer Code**************/
void buzz_ON()   //open buzzer
{
  for (int i = 0; i < 100; i++)
  {
    digitalWrite(Buzz_Pin, LOW);
    delay(2);//wait for 1ms
    digitalWrite(Buzz_Pin, HIGH);
    delay(2);//wait for 1ms
  }
}

void buzz_OFF()  //close buzzer
{
  digitalWrite(Buzz_Pin, HIGH);
}

void alarm(int t = 2)
{
  buzz_ON();
  delay(t);
  buzz_OFF();
}

void on_alarm(int t = 2)
{
  buzz_ON();
  delay(t);
  buzz_OFF();
  delay(t * 50);
  buzz_ON();
  delay(t);
  buzz_OFF();
}


////////////////////////////// Whistle Patterns //////////////////////////////
int TooterTutor[] = {
  NOTE_G5, NOTE_E5};

int AndOrButt[] = {
  NOTE_C5, NOTE_G5, NOTE_E5};

int DieSpiderDie[] = {
    NOTE_G5, NOTE_G5, NOTE_E5};

int StompGoblin[] = {
    NOTE_E5, NOTE_C5, NOTE_G5};


////////////////////////////// Whistle Functions //////////////////////////////

void TooterTutor_whistle()
{
  for (int thisNote = 0; thisNote < 2; thisNote++) {
    // pin8 output the voice, every scale is 0.5 sencond
    tone(8, TooterTutor[thisNote], duration);
     
    // Output the voice after several milliseconds
    delay(525);
  }
}


void AndOrButt_whistle()
{
  for (int thisNote = 0; thisNote < 3; thisNote++) {
    // pin8 output the voice, every scale is 0.5 sencond
    tone(8, AndOrButt[thisNote], duration);
     
    // Output the voice after several milliseconds
    delay(525);
  }
}

void DieSpiderDie_whistle()
{
  for (int thisNote = 0; thisNote < 3; thisNote++) {
    // pin8 output the voice, every scale is 0.5 sencond
    tone(8, DieSpiderDie[thisNote], duration);
     
    // Output the voice after several milliseconds
    delay(525);
  }
}
void StompGoblin_whistle()
{
  for (int thisNote = 0; thisNote < 3; thisNote++) {
    // pin8 output the voice, every scale is 0.5 sencond
    tone(8, StompGoblin[thisNote], duration);
     
    // Output the voice after several milliseconds
    delay(315);
  }
}




/**************Ultrasonic Code***************/
void scan_Distance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);              //trigpin's output pulse
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration_scan = pulseIn(echoPin, HIGH);   //calibrate echo pin's pulse input
  distance = duration_scan * 0.034 / 2;     // distanceCm is half the duration multiplied by 0.034
  delay(25);                                //so you don't get overloaded by serial monitor outputs
}


/**************Sound Detection Code***************/

void scan_Sound()
{
  analogValue = analogRead(sensorAnalogPin);  // Read the value of the analog interface A0 assigned to soundLevel
  soundLevel = digitalRead(sensorDigitalPin); // Read the value of the digital interface 7 assigned to soundLevel
//  Serial.println(soundLevel); // Send the analog value to the serial transmit interface (Debugging Purposes)
  if (flag_lock == true)
  {
    return;
  }

  else if (soundLevel == HIGH && switch_state == false)  // When the Sound Sensor sends signal, via voltage present, light LED13 (L) and do normal procedure
  {
    digitalWrite(Led13, HIGH);
    switch_state = true;
    Tick_Num = SWITCH_ON;
  }
  else if (soundLevel == HIGH && switch_state == true)
  {
    digitalWrite(Led13, HIGH);
    switch_state = false;
    Tick_Num = SWITCH_OFF;
  }
  else
  {
    digitalWrite(Led13, LOW);
  }
}


/**************Hand Detection Code***************/
// Scans for anything within 15cm of the sensor, if there is something there, trigger the lightswitch mechanism
void do_Sensor_Tick()
{
  if (flag_lock == true)
  {
    return;
  }

  scan_Distance();
  if (distance < MAX_DIST && distance > MIN_DIST)
  {
    if (switch_state == false)
    {
      switch_state = true;
      Tick_Num = SWITCH_ON;
      while (distance < MAX_DIST && distance > MIN_DIST)
      {
        scan_Distance();
      }
    }
    else if (switch_state == true)
    {
      switch_state = false;
      Tick_Num = SWITCH_OFF;
      while (distance < MAX_DIST && distance > MIN_DIST)
      {
        scan_Distance();
      }
    }
  }
}


/**************Detect IR Code***************/
//void do_IR_Tick()
//{
//  if (!IR.decode(&IRresults))
//  {
//    return;
//  }
//  if (remote_lock)
//  {
//    return;
//  }
//  switch (IRresults.value)
//  {
//    case IR_TURN_ON:
//    {
//      Tick_Num = SWITCH_ON;
//      switch_state = true;
//      break;
//    }
//    case IR_TURN_OFF:
//    {
//      Tick_Num = SWITCH_OFF;
//      switch_state = false;
//      break;
//    }
//    case IR_UNLOCK:
//    {
//      Tick_Num = SWITCH_RESET;
//      flag_lock = false;
//      break;
//    }
//    case IR_LOCK:
//    {
//      Tick_Num = SWITCH_LOCK;
//      flag_lock = true;
//      break;
//    }
//    default:
//      break;
//  }
//
//  IRresults.value = 0;
//  IR.resume();
//}

void do_Switch_Tick()
{
  switch (Tick_Num)
  {
    case SWITCH_ON:    Lswitch_On(); on_alarm(); break;
    case SWITCH_OFF:   Lswitch_Off(); break;
    case SWITCH_RESET: Lswitch_Reset(); break;
    case SWITCH_LOCK:  Sensor_Lock(); break;
    default: break;
  }

  Tick_Num = DEF;
}


/**************Read Serial Data***************/
// When a serial event occurs, this will get called between when loop() is run.
void serialEvent()
{
  while (Serial.available() > 0)
  {
    // get the new byte:
    char inChar = (char)Serial.read();

    if (inChar != '\n')
    {
      // Append a single character onto the string.
      inputString += inChar;
    }
    else
    {
      stringComplete = true;
    }
  }
}



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// SETUP ///////////////////////////////////////
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv//


void setup()
{
  // Initialize Serial:
  Serial.begin(9600);
  espSerial.begin(9600);
  
  // Still a work in progress!
  // Define Infrared Send Pins:

//  #if defined(IR_SEND_PIN)
//    IrSender.begin(); // Start with IR_SEND_PIN as send pin and enable feedback LED at default feedback LED pin
//#else
//    IrSender.begin(3, ENABLE_LED_FEEDBACK); // Specify send pin and enable feedback LED at default feedback LED pin
//#endif
  

  // Custom LCD Characters
  lcd.createChar(0, lockchar);
  lcd.createChar(1, unlockchar);
  
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  // Ultrasonic Sensor
  pinMode(Buzz_Pin, OUTPUT);
  pinMode(Servo_On, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Sound Sensor
  pinMode(sensorDigitalPin,INPUT);
  pinMode(Led13,OUTPUT);

  // Servo
  servo_On.attach(9);
  servo_On.write(90);

}


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
/////////////////////////////////////// SETUP ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



// When Arduino Uno receives a command from ESP32, check what the command was.
// If it was one of the following, execute commands, otherwise do nothing.
void send_Command()
{
  if (inputString.length() == 0)
  {
    // If the string is empty, there's no point in
    // checking all the other cases OR setting it to "".
    return;
  }

  if (inputString.equalsIgnoreCase("ON"))
  {
    Tick_Num = SWITCH_ON;
    Serial.println("Successfully Turned the Lightswitch ON");
  }
  else if (inputString.equalsIgnoreCase("OFF"))
  {
    Tick_Num = SWITCH_OFF;
    Serial.println("Successfully Turned the Lightswitch OFF");
  }
  else if (inputString.equalsIgnoreCase("LOCK"))
  {
    Tick_Num = SWITCH_LOCK;
    Serial.println("Successfully Locked the Lightswitch");
  }
  else if (inputString.equalsIgnoreCase("UNLOCK"))
  {
    Tick_Num = SWITCH_RESET;
    Serial.println("Successfully Unlocked the Lightswitch");
  }
  else if (inputString.equalsIgnoreCase("LOCK REMOTE"))
  {
    remote_lock = true;
    Serial.println("Successfully Locked the Remote");
  }
  else if (inputString.equalsIgnoreCase("UNLOCK REMOTE"))
  {
    remote_lock = false;
    Serial.println("Successfully Unlocked the Remote");
  }
  else if (inputString.equalsIgnoreCase("HIDDEN LOCK"))
  {
    flag_lock = true;
    Serial.println("Successfully Locked the Sensors\nLCD Not Triggered");
  }
  else if (inputString.equalsIgnoreCase("HIDDEN UNLOCK"))
  {
    flag_lock = true;
    Serial.println("Successfully Unlocked the Sensors\nLCD Not Triggered");
  }
  else if (inputString.equalsIgnoreCase("LOCK ALL"))
  {
    flag_lock = true;
    remote_lock = true;
    Serial.println("Successfully Locked Everything\nLCD Not Triggered");
  }
  else if (inputString.equalsIgnoreCase("UNLOCK ALL"))
  {
    flag_lock = false;
    remote_lock = false;
    Serial.println("Successfully Unlocked Everything\nLCD Not Triggered");
  }
  else if (inputString.equalsIgnoreCase("TooterTutor"))
  {
    TooterTutor_whistle();
    Serial.println("Successfully Whistled for TooterTutor");
  }
  else if (inputString.equalsIgnoreCase("AndOrButt"))
  {
    AndOrButt_whistle();
    Serial.println("Successfully Whistled for AndOrButt");
  }
  else if (inputString.equalsIgnoreCase("DieSpiderDie"))
  {
    DieSpiderDie_whistle();
    Serial.println("Successfully Whistled for DieSpiderDie");
  }
  else if (inputString.equalsIgnoreCase("StompGoblin"))
  {
    StompGoblin_whistle();
    Serial.println("Successfully Whistled for StompGoblin");
  }
  // Still a work in progress!
  
//  else if (inputString.equalsIgnoreCase("stereo_power"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_cd"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_mute"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_aux"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_bt"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_fm"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_rr"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_play"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_ff"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_rpt"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_stop"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_prog"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_voldown"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_bass"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }
//  else if (inputString.equalsIgnoreCase("stereo_volup"))
//  {
//    uint16_t sAddress = 0x0;
//    uint8_t sCommand = 0xD;
//    uint8_t sRepeats = 0;
//    IrSender.sendNEC(sAddress, sCommand, sRepeats);
//  }

// Now that we've used the string, clear it so we can start fresh again.
  inputString = "";
}




void loop()
{
  scan_Distance();
  scan_Sound();
  do_Sensor_Tick();
  do_Switch_Tick();
  send_Command();
  serialEvent();
}

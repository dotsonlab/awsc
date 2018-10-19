#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include "RTClib.h"
RTC_DS3231 rtc;
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

//Real time clock dates incase you want weekends to act differently than weekdays
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//These are the digital pin numbers and what they turn on and off
int en1 = 2; //1 is fill
int step1 = 3;
int dir1 = 4;
int decantpump = 5; //decants SBR into Effluent Tanks
int fillpump = 6; //fills the SBR from the GW tank
int skimmer =7;
int air = 9; //Controls the air pump
int stir = 8;


//This is the analog pin for depth measurement
int depth = A0; //defines depth based off measurement coming from the sensor

//These are other variables set up for calculations
int x=0;
float starttime;
float realstarttime;
float begintime;
float tank;
float airtimer;

//This is the setup section that the code runs through once before doing the "loop" section over and over
void setup() {
  pinMode(en1, OUTPUT);
  pinMode(decantpump, OUTPUT);
  pinMode(step1, OUTPUT);
  pinMode(fillpump, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(skimmer, OUTPUT);
  pinMode(air, OUTPUT);
  pinMode(stir, OUTPUT);
  pinMode(depth, INPUT);
  digitalWrite(en1, HIGH);
  lcd.begin(16, 2);
   if (! rtc.begin()) {
     lcd.print("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    lcd.print("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  // Print a message to the LCD. We track how long it takes since
  // this library has been optimized a bit and we're proud of it :)
  lcd.print("start!");
  lcd.setBacklight(WHITE);
  delay(5000);
  lcd.clear();
  lcd.setCursor(0,1);
  measurevol();
  lcd.print(tank);
  measurevol();
  lcd.print(tank);
  measurevol();
  lcd.print(tank);
  measurevol();
}
uint8_t i=0;

//This function takes an average of tank volume readings to get a smoothed value when "measurevol();" is used
void measurevol(){
  int junk = analogRead(depth);
  float tank1 = analogRead(depth)*0.421569-28.4559;
  float tank2 = analogRead(depth)*0.421569-28.4559;//*0.438596-32.7018;
  float tank3 = analogRead(depth)*0.421569-28.4559;//*0.438596-32.7018;
  float tank4 = analogRead(depth)*0.421569-28.4559;//*0.438596-32.7018;
  float tank5 = analogRead(depth)*0.421569-28.4559;//*0.438596-32.7018;
  float tank6 = analogRead(depth)*0.421569-28.4559;//*0.438596-32.7018;
  float tank7 = analogRead(depth)*0.421569-28.4559;//*0.438596-32.7018;
  float tank8 = analogRead(depth)*0.421569-28.4559;//*0.438596-32.7018;
  tank = (tank1 + tank2 + tank3 + tank4 + tank5 + tank6 + tank7 + tank8)/8;
}



void fill(int lvl, int airadd){//check depth and fill until level is 25gal plus stir
  if (lvl<19){
    digitalWrite(stir, HIGH);//high turns that pin on (if it is set to output), low turns it off
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("FILL ");
  lcd.setBacklight(TEAL);
  measurevol();
  lcd.setCursor(0,1);
  lcd.print(tank);lcd.print("gal");
  DateTime now = rtc.now();
  if (airadd == 1){
    digitalWrite(air,HIGH);
  }
    while (tank<lvl){
      digitalWrite(fillpump, HIGH);
      measurevol();
      lcd.setCursor(0,1);
      lcd.print(tank);lcd.print("gal");
      delay(15000);
    }
    digitalWrite(fillpump, LOW);
    digitalWrite(stir, LOW);
    digitalWrite(air, LOW);
}

void periodicaeration(float ontime){//on for 15 min freq is 60 min
  ontime=ontime*60;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PeriodicAeration ");
  DateTime now = rtc.now();
  float tt = now.unixtime();
  float current=tt;
  int timedisplay = round((ontime-(current-tt))/60);
  while ((current-tt) < ontime){
    timedisplay = round((ontime-(current-tt))/60);
    measurevol();
    lcd.setCursor(0,1);
  lcd.print(tank);lcd.print("gal  ");
  lcd.print(timedisplay);
  lcd.print("min");
  delay(1000);
  digitalWrite(air, HIGH);
  now = rtc.now();
  current = now.unixtime();
  }
digitalWrite(air, LOW);
digitalWrite(stir, LOW);  
}
void aeration(float airduration, int lvl){//duration in min check depth and keep air on and pumps off for time period
  airduration=airduration*60*1000;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Aeration ");
  lcd.setBacklight(GREEN);
  measurevol();
  lcd.setCursor(0,1);
  lcd.print(tank);
  float starttime = millis();
  delay(100);
  while (((millis()-starttime)<airduration)&&(tank>lvl)){  //2 min
    digitalWrite(air, HIGH);
    int timedisplay = round((airduration-(millis()-starttime))/60/1000);
    measurevol();
    lcd.setCursor(0,1);
    lcd.print(tank);lcd.print("gal  ");
    lcd.print(timedisplay);lcd.print("min  ");
    delay(20000);}
    digitalWrite(air, LOW);
}
void thirtysecaeration(float airduration, int lvl){//duration in min check depth and keep air on and pumps off for time period
  airduration=airduration*60*1000;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("30Aeration ");
  lcd.setBacklight(GREEN);
  measurevol();
  lcd.setCursor(0,1);
  lcd.print(tank);
  float starttime = millis();
  delay(100);
  while (((millis()-starttime)<airduration)&&(tank>lvl)){  //2 min
    digitalWrite(air, HIGH);
    delay(30000);
    digitalWrite(air, LOW);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);

    int timedisplay = round((airduration-(millis()-starttime))/60/1000);
    measurevol();
    lcd.setCursor(0,1);
    lcd.print(tank);lcd.print("gal  ");
    lcd.print(timedisplay);lcd.print("min  ");
    //delay(20000);
    }
    digitalWrite(air, LOW);
}
void halftimeair(float airduration, int lvl){//duration in min check depth and keep air on and pumps off for time period
  airduration=airduration*60*1000;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("halftimeAir ");
  lcd.setBacklight(GREEN);
  measurevol();
  lcd.setCursor(0,1);
  lcd.print(tank);
  float starttime = millis();
  delay(100);
  while (((millis()-starttime)<airduration)&&(tank>lvl)){  //2 min
    digitalWrite(air, HIGH);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    digitalWrite(air, LOW);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);

    int timedisplay = round((airduration-(millis()-starttime))/60/1000);
    measurevol();
    lcd.setCursor(0,1);
    lcd.print(tank);lcd.print("gal  ");
    lcd.print(timedisplay);lcd.print("min  ");
    //delay(20000);
    }
    digitalWrite(air, LOW);
}
void oneonthreeoffair(float airduration, int lvl){//duration in min check depth and keep air on and pumps off for time period
  airduration=airduration*60*1000;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("halftimeAir ");
  lcd.setBacklight(GREEN);
  measurevol();
  lcd.setCursor(0,1);
  lcd.print(tank);
  float starttime = millis();
  delay(100);
  while (((millis()-starttime)<airduration)&&(tank>lvl)){  //2 min
    digitalWrite(air, HIGH);
    delay(30000);
    delay(30000);
    digitalWrite(air, LOW);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    delay(30000);
    int timedisplay = round((airduration-(millis()-starttime))/60/1000);
    measurevol();
    lcd.setCursor(0,1);
    lcd.print(tank);lcd.print("gal  ");
    lcd.print(timedisplay);lcd.print("min  ");
    //delay(20000);
    }
    digitalWrite(air, LOW);
}
void settle(float duration, int lvl){//everything off just timer
  duration=duration*60*1000;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Settle ");
  lcd.setBacklight(YELLOW);
  measurevol();
  lcd.setCursor(0,1);
  lcd.print(tank);lcd.print("gal  ");
  float starttime = millis();
  delay(100);
  while (((millis()-starttime)<duration)&&(tank>lvl)){
    int timedisplay = round((duration-(millis()-starttime))/60/1000);
    delay(10000);
    measurevol();
  lcd.setCursor(0,1);
  lcd.print(tank);lcd.print("gal  ");
  lcd.print(timedisplay);lcd.print("min  ");} //2 min
}

void decant(int level){ // empty through decant pump checking volume
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Decant ");
  lcd.setBacklight(RED);
  measurevol();
  lcd.setCursor(0,1);
  lcd.print(tank);
  float starttime = millis();
  digitalWrite(decantpump, HIGH);
  delay(2000);
  digitalWrite(decantpump, LOW);
  delay(1000);
  digitalWrite(decantpump, HIGH);
  delay(2000);
  digitalWrite(decantpump, LOW);
  delay(1000);
 while (tank>level){
    digitalWrite(decantpump, HIGH);
    measurevol();
    lcd.setCursor(0,1);
  lcd.print(tank);lcd.print("gal  ");
  delay(3000);}
    digitalWrite(decantpump, LOW);
}
void rest(float longt){ //everything off except maybe stir
  longt=longt*60*1000;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("REST ");
  lcd.setBacklight(VIOLET);
  measurevol();
  lcd.setCursor(0,1);
  lcd.print(tank);lcd.print("gal  ");
  float starttime = millis();
  delay(100);
   while ((millis()-starttime)<longt){
    float timedisplay = round((longt-(millis()-starttime))/60/1000);
     measurevol();
     lcd.setCursor(0,1);
  lcd.print(tank);lcd.print("gal  ");
  lcd.print(timedisplay);lcd.print("min  ");
    delay(1000);}
    digitalWrite(stir, LOW);
}
//This is a general stepper function that can be used for any stepper attached
void stepper(int num, int motor, int direct){
  if (motor == 1){
    digitalWrite(en1, LOW); 
    if (direct == 1){
      digitalWrite(dir1, HIGH);}//empty
      else if (direct ==2){
        digitalWrite(dir1, LOW);}// fill
    for (int i=0; i<num; i++){
      digitalWrite(step1, HIGH);
      delay(3);
      digitalWrite(step1, LOW);
      delay(3);
  }
  digitalWrite(en1, HIGH);
}
}

//calls stepper function above to remove sludge automatically
void sludgeremoval(){
  stepper(6400,1,2);//fills up tube. revolutions, 1st motor, "empty" dir
  stepper(2500,1,2);//empties(2500 steps equivalent to about 1 gal/wk). revolutions, 1st motor, "empty" dir
  stepper(6400,1,1);//empty tube back into sbr. revolutions, 1st motor, "fill" dir
}

////// This is what happens when resting //////
void weekend(){
  aeration(5, 4);
  rest(50);
  sludgeremoval();
}

///////// THIS IS THE SEQUENCE THAT MATTERS /////////
void normalsequence(){//10 hrs
  DateTime now = rtc.now();
  realstarttime = now.unixtime();
  fillplusair(21,1);
  now = rtc.now();
  delay(2000);
  thirtysecaeration(120, 19);
  delay(2000);
  oneonthreeoffair(60,19);
  delay(2000);
  halftimeair(60,19);
  delay(2000);
  aeration(60,19);
  delay(2000);
  settle(180, 19);
  delay(2000);
  decant(6);
  delay(2000);
}
//NOT USED//
//this was for a slow fillup to avoid bubbling over
void loading(int h){//takes 1 hr to fill before continuing
   DateTime now = rtc.now();
  begintime = now.unixtime();
   measurevol();
   fill(h,1);
   periodicaeration(15);
   now = rtc.now();
   float current = now.unixtime();
    while (current-begintime<3600){//1hr chill
    if (h<20){
    digitalWrite(stir, HIGH);
    }
    now = rtc.now();
    current = now.unixtime();
    delay(15000);
    lcd.setCursor(0,0);
    lcd.print("Loading Delay       ");
    int timedisplay = round((3600-(current-begintime))/60);
    measurevol();
    lcd.setCursor(0,1);
    lcd.print(tank);lcd.print("gal  ");
    lcd.print(timedisplay);
    lcd.print("min  ");
  }
  digitalWrite(stir, LOW);
}

//NOT USED//
//this was for a slow fillup to avoid bubbling over
void fillup(){
  loading(7);//12 times for 12 hours
  loading(9);
  loading(11);
  loading(13);
  loading(15);
  loading(16);
  loading(17);
  loading(18);
  loading(20);
  loading(21);
}


// put your main code here, to run repeatedly:
void loop() {
  DateTime now = rtc.now();
  //daysOfTheWeek[now.dayOfTheWeek()
  float current = now.unixtime();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  delay(100);
  lcd.setCursor(0,1);
  lcd.print(tank);
  while (x<1){//////////////If something gets off and you want to run a small part of the code put it in this while loop
   /////////////////Everything in here only runs once/////////////////////
   //sludgeremoval();
   //aeration(60,17);
   //settle(50, 19);//delay(1000);
   //decant(6);//loading();
    normalsequence();
    x++;}
  
    //this section checks what day it is then starts the sequence if time of day is between these hours
  if (now.dayOfTheWeek()== 1){//"Monday"
    if (now.hour()>5 && now.hour()<7){normalsequence();}}
  if (now.dayOfTheWeek()== 2){//"Tuesday"
    if (now.hour()>5 && now.hour()<7){normalsequence();}}
  if (now.dayOfTheWeek()== 3){//"Wednesday"
    if (now.hour()>5 && now.hour()<7){normalsequence();}}
  if (now.dayOfTheWeek()== 4){//"thursday"
    if (now.hour()>5 && now.hour()<7){normalsequence();}}
  if (now.dayOfTheWeek()== 5){//"Friday"
    if (now.hour()>5 && now.hour()<7){normalsequence();}}
  if (now.dayOfTheWeek()== 6){//"Saturday"
    if (now.hour()>5 && now.hour()<7){normalsequence();}}//fillplusair(21,1);thirtysecaeration(100,10);}}
  if (now.dayOfTheWeek()== 0){//"Sunday"
    if (now.hour()>5 && now.hour()<7){normalsequence();}}//weekend();}
  delay(20000);
  ////This weekend function is poorly named but means what happens before the sequence is triggered and after it is done
  weekend();}

/* in de comments staan op verschillende plekken "BT, debugging purpose"
 deze heb ik geplaatst zodat je makkelijk kan terug vinden welke regels ik heb omgeschreven zodat ik het met de bluetooth terminal kan testen
 de oude code staat er boven gecomment
*/

/* ------- Start Additional Libraries ------- */

#include <SD.h>                     // Write and Read Files: add library 
#include <SPI.h>
#include <RFID.h>
#include <Wire.h>
#include <SoftwareSerial.h>

/* ------- End Additional Libraries ------- */

/* ------- End General install & define ------- */     

/* ------- Start Bluetooth install & define ------- */

SoftwareSerial BT(10, 11);  // creates a "virtual" serial port/UART
                            // connect BT module TX to D10
                            // connect BT module RX to D11
                            // connect BT Vcc to 5V, GND to GND
String a;                   // stores incoming character from other device
int length;

/* ------- End Bluetooth install & define ------- */

/* ------- Start RFID install & define ------- */

/*
  edited by Velleman / Patrick De Coninck
  Read a card using a mfrc522 reader on your SPI interface
  Pin layout should be as follows (on Arduino Nano - Velleman VMA100):
  MOSI: Pin D51 / ICSP-4
  MISO: Pin D50 / ICSP-1
  SCK: Pin D52 / ISCP-3
  SS/SDA (MSS on Velleman VMA405) : Pin D53
  RST: Pin D9
  VCC: 3,3V (DO NOT USE 5V, VMA405 WILL BE DAMAGED IF YOU DO SO)
  GND: GND on Arduino UNO / Velleman VMA100
  IRQ: not used
*/


#define SS_PIN 53
#define RST_PIN 9

RFID rfid(SS_PIN, RST_PIN);
int serNum[5];

int cards[][5] = {                  // This integer should be the code of Your Mifare card / tag
  {7, 54, 97, 98, 50},              //kaart
  {170, 130, 218, 37, 215},         //badges 1 tm/11
  {43, 113, 79, 115, 102},
  {217, 73, 70, 115, 165},
  {124, 116, 85, 115, 46},
  {190, 152, 226, 195, 7},
  {228, 43, 242, 195, 254},
  {46, 142, 166, 131, 133},
  {162, 62, 169, 195, 246},
  {222, 18, 242, 195, 253},
  {182, 36, 83, 115, 178},
  {50, 140, 81, 115, 156}
 };

bool access = false;
int cardNumber;

/* ------- End RFID install & define ------- */

/* ------- Start Pin layout ------- */
const int LED = 35;         // Het LED'je is om aan te geven dat al 
                            // het water en siroop in het mengvat is 
                            // en zodra deze uitgaat wordt limo uitgegeven

const int pompw = 2;        // Dit zijn de pins die de relays van de pompjes           
const int pomps1 = 3;       // aansturen, w=water s=siroop en l=limonade a=afval
const int pomps2 = 4;
const int pomps3 = 12; 
const int pompl = 8; 
const int pompa = 7; 

const int troebelheid = A0;

const int ledw = 23;
const int leds1 = 25;
const int leds2 = 27;
const int leds3 = 29;
const int ledl = 31;
const int leda = 33;

// (0 = s1, 1 = s2, 2 = s3, 3 = reactievat, 4 = afvalvat)

int s1 = 22;
int s2 = 24;
int s3 = 26;
int limo = 28;
int afval = 30;
int water = 32;

int s1_sense = A4;
int s2_sense = A3;
int s3_sense = A2;
int limo_sense = A5;
int afval_sense = A6;
int water_sense = A7;


/* ------- End Pin Layout ------- */

/* ------- Start Global Variables ------- */
int WATER_SENSOR_DIG[] = {s1, s2, s3, limo,  water};
int WATER_SENSOR_ANA[] = {s1_sense, s2_sense, s3_sense, limo_sense, water_sense};
String WATER_SENSOR_NAME[] = {"s1", "s2", "s3", "limo", "afval",  "water"};
int sensorvalue;

String smaak1 = "";
String smaak2 = ""; 
String smaak3 = "";

// Belangrijk voor berekeningen:

const float grenssiroop = 0.5;    
const float grenswater = 0.5;
const float grenslimo = 9.5; 

const int delaya = 25000;             //badge kleiner dan 5,kleine beker
const int delayb = 30000;             //badge groter dan 5, grote beker
int gewenstedelay;

const int cs1 = 625;            // gewenste turbidity met siroop 1;
int i;
int pomps;
int huidige_check;
int pump_duration = 10000;      //10s voor siroop 90s voor water en 100s voor limo tappen
int Turbidity;
int tijd;
int hoogte;
String input;
int huidig_level;
int percentage;

// Arrays voor alle apparatten 
int pompv[] = {pomps1, pomps2, pomps3, pompw, pompl, pompa};
String sensorvnaam[] = {"sensors1", "sensors2", "sensors3", "sensorl"};
int cs[] = {cs1};
String smaak[] = {"smaak1", "smaak2", "smaak3"};    //nog naar kijken: smaak invulllen
int led[] = {leds1, leds2, leds3, ledw, ledl, leda};

/* ------- End Global Variables ------- */

/* ------- Start Equipment Setup ------- */

void setup()
{
  Wire.begin();                 //hier geen adress want dit is de arduino
  Serial.begin(9600);           //Baud rate: 9600
  
  SPI.begin();
  rfid.init();
  
  pinMode(WATER_SENSOR_ANA[0], INPUT);
  pinMode(WATER_SENSOR_ANA[1], INPUT);
  pinMode(WATER_SENSOR_ANA[2], INPUT);
  pinMode(WATER_SENSOR_ANA[3], INPUT);
  pinMode(WATER_SENSOR_ANA[4], INPUT);

  pinMode(WATER_SENSOR_DIG[0], OUTPUT);
  pinMode(WATER_SENSOR_DIG[1], OUTPUT);
  pinMode(WATER_SENSOR_DIG[2], OUTPUT);
  pinMode(WATER_SENSOR_DIG[3], OUTPUT);
  pinMode(WATER_SENSOR_DIG[4], OUTPUT);

  digitalWrite(WATER_SENSOR_DIG[0], HIGH);
  digitalWrite(WATER_SENSOR_DIG[1], HIGH);
  digitalWrite(WATER_SENSOR_DIG[2], HIGH);
  digitalWrite(WATER_SENSOR_DIG[3], HIGH);
  digitalWrite(WATER_SENSOR_DIG[4], HIGH);
  
  pinMode(LED, OUTPUT);

  pinMode(pompw, OUTPUT);
  pinMode(pomps1, OUTPUT);
  pinMode(pomps2, OUTPUT);
  pinMode(pomps3, OUTPUT);
  pinMode(pompl, OUTPUT);
  pinMode(pompa, OUTPUT);

  digitalWrite(pompw, LOW);
  digitalWrite(pomps1, LOW);
  digitalWrite(pomps2, LOW);
  digitalWrite(pomps3, LOW);
  digitalWrite(pompl, LOW);
  digitalWrite(pompa, LOW);
  
  pinMode(troebelheid, INPUT);

  pinMode(ledw, OUTPUT);
  pinMode(leds1, OUTPUT);
  pinMode(leds2, OUTPUT);
  pinMode(leds3, OUTPUT);
  pinMode(ledl, OUTPUT);
  pinMode(leda, OUTPUT);

  BT.begin(9600);
  BT.println("Hello from Arduino"); // Send test message to the connected Bluetooth device
  Serial.println("Start");
}

/* ------- End Equipment Setup ------- */

/* ------- Start Equipment Functions ------- */

  /* ------- Start Bluetooth Communication Functions ------- */
  
String hc06()                       
{
  a = BT.readString(); //BT, debugging purpose
  if (input != String(" "))
  {
    return a;
  }
}
  /* ------- End Bluetooth Communication Functions ------- */
  
  /* ------- Start Liquid Level Functions ------- */

int readsensor(int b)
{
  sensorvalue = analogRead(WATER_SENSOR_ANA[b]);
  Serial.print(WATER_SENSOR_ANA[b]);
  Serial.print(" - ");
  Serial.print(WATER_SENSOR_NAME[b]);
  Serial.print(": ");
  Serial.println(sensorvalue);
  delay(500);
  return sensorvalue;
 }
  /* ------- End Liquid Level Functions ------- */
  
  /* ------- Start Liquid Pump Functions ------- */
  
int isSyrupLow(int i)                // Check syrup level. i chooses the sensorv array (0 = s1, 1 = s2, 2 = s3, 3 = reactievat, 4 = afvalvat)
{
delay(500);                           // Even wachten zodat de sensor echt aan staat
huidig_level = readsensor(i);         
Serial.print("Is ");
Serial.print(smaak[i]); 
Serial.println(" low?");      

  if (huidig_level > 800)           //If syrup level drops below the minimum 
  {
    digitalWrite(led[i], HIGH);
    
    Serial.print(smaak[i]);           // Send alert to Serial Monitor
    Serial.print("_low");
    
    BT.print(smaak[i]);               // Send alert to Bluetooth Device
    BT.print("_low");
    
    return 0;                         // Return 0 if the level is low
  }
  else
  {
    digitalWrite(led[i], LOW);

    Serial.print(smaak[i]);           // Send alert to Serial Monitor
    Serial.print("_ok");
    
    return 1;                         // Return 1 if the level is sufficient
  }
}

int isWaterLow()                       // Check water level
{
  delay(500);
  huidig_level = readsensor(4);

  Serial.println("is water low?");          // BT, debugging purpose

  Serial.print("sensorw,");           // Send csv line to Serial Monitor
  Serial.println(huidig_level);
  
  BT.print("sensorw,");               // Send csv line to Bluetooth Device
  BT.println((String)huidig_level);

  delay(50);            
  
  
//  if (huidig_level < grenswater)      // If syrup level drops below the minimum 
if (huidig_level > 800)               //BT, debugging purpose
  {
    digitalWrite(led[i], HIGH);
    
    Serial.println("water_low");     // Send alert to Serial Monitor
    BT.println((String)"water_low");         // Send alert to Bluetooth Device
    
    return 0;                         // Return 0 if the level is low
  }
  else
  {
    digitalWrite(led[i], LOW);
    return 1;                          // Return 1 if the level is sufficient
  }
}

int isLimoFull()                        // Check if the lemonade vessel is full
{
  delay(50);
  huidig_level = readsensor(3);
  Serial.println("is limo full?");

  delay(50);
                  
  Serial.print("sensorl,");             // Send csv line to Serial Monitor
  Serial.println(huidig_level);
  
  BT.print("sensorl,");                 // Send csv line to Bluetooth Device
  BT.println((String)huidig_level);
  
  if (huidig_level > 800)         // If level in the mixing reactor exceeds the maximum 
  {
    digitalWrite(ledl, HIGH);
    Serial.println("limo_high");       // Send alert to Serial Monitor

    BT.println("limo_full");              // Send alert to Bluetooth Device 
    BT.println("use command afval or command limo");
        
    String bt_cmd = "";
    bt_cmd = hc06();
    while (bt_cmd == " ")                // Wait until a command is received from Bluetooth Device
    {
      bt_cmd = hc06();                  // bt_cmd blijven verversen
    }
    
    if (bt_cmd == "afval")               // Command is to dispose via trash
    {
      BT.println("let_know_when_stop");
      
      digitalWrite(pompa, HIGH);
      
      BT.println("pompa_high");
      Serial.println("pompa_high");
      
      while (bt_cmd != "stop")
      {
        bt_cmd = hc06();
      }
      
      digitalWrite(pompa, LOW);
      BT.println("pompa_low");
      
      Serial.println("pompa_low");
      
      BT.println("enough_space_for_limo");
      Serial.println("limo_ok");
      return 0;
    }
    else if (bt_cmd == "limo")                      // Command is to dispose via pouring a glass
    { 
      BT.println("5s_until_limo");       
      digitalWrite(LED, HIGH);                      // Geeft weer aan dat er een afgifte komt
      delay(5000);
      digitalWrite(LED, HIGH);
                                          //10x want 1 deel siroop, 9 delen water = 10 = 1 beker eenheid
                                          //hier nog naar kijken: staat 10 y voor 1 bekereenheid of willen we dit later nog aanpassen?
                                          //als 10y=bekereenheid moeten de andere pompen dan niet x*y zijn want we willen maar dan 1 beker/batch?
      for (int l = 0; l < 20; l++)
      {
        digitalWrite(pompl, HIGH);
        
        Serial.println("pompl_high");
        BT.println("pompl_high");
        
        delay(0.5*pump_duration); 
        
        digitalWrite(pompl, LOW);
        
        Serial.println("pompl_low");
        BT.println("pompl_low");
        
        delay(1000);
      }
      BT.println("limo_is_poured)");
      return 0;
    }
    else
    {
      return 1;
    }
  }
}
  
  /* ------- End Liquid Pump Functions ------- */


  /* ------- Start Concentration Function ------- */
  
int getConcentration()
{
  int Vout = analogRead(troebelheid);
  float Turbidity = map(Vout, 0, 800, 100, 0);        // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):

  Serial.println(Vout);
  Serial.print("turbiditysensor,");                 // Send csv line to Serial Monitor
  Serial.println(Turbidity);
  
  BT.print("sensor_turb,");                         // Send csv line to Bluetooth Device
  BT.print(Turbidity);
  
  return Turbidity;                                 // = percentage, waarbij 0% helder is en 100% volledig troebel
}
    /* ------- End Concentration Function ------- */
    
    /* ------- Start RFID Function ------- */

int getRFID() {
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      for (int x = 0; x < sizeof(cards); x++) {            
        for (int i = 0; i < sizeof(rfid.serNum); i++ ) {
          if (rfid.serNum[i] != cards[x][i]) {
            access = false;
            break;
          } 
          else
          {
            access = true;
            cardNumber = x;
          }
        }
        if (access) break;
      }
    }
    if (access) {                     // Valid card
      Serial.println("Allowed!");
      Serial.println(cardNumber);
      if (cardNumber < 5)
      {
        Serial.println("is klein");
        return delaya;
      }
      else
      {
        Serial.println("is groot");
        return delayb;
      }
      access = false;
    } else {                          // NON-Valid card
      Serial.println("Not allowed!");

    }
  }
  rfid.halt();
}
    /* ------- End RFID Function ------- */
    
/* ------- End Equipment Functions ------- */

/* ------- Start Arduino Loop Function ------- */

void loop() 
{
/* ------- Start Bluetooth-Pump Communication ------- */
  BT.println("kies de smaak");
  Serial.println("");
  Serial.println("Place your order");
  String bt_cmd = hc06();
  bt_cmd = hc06();  
  
  while (bt_cmd == "")                // Wait until a command is received from Bluetooth Device
  {
    bt_cmd = hc06();                  // bt_cmd blijven verversen
    delay(500);
    getConcentration();
  } 
  
  if(bt_cmd.indexOf("flush") != -1)
  {
      do 
      {
        bt_cmd = hc06();
        
        digitalWrite(pompa, HIGH);
        
        BT.println("pompa_high");
        Serial.println("pompa_high");
        
        delay(0.3 * pump_duration);               //tap_delay*10 want siroop:water = 1:10
        digitalWrite(pompa, LOW);
        
        BT.println("pompa_low");
        Serial.println("pompa_low");        //x * pump_syrup + y * pump_water = Volume
        delay(2000);   
      } while (bt_cmd != "stop_flush");
      
      digitalWrite(pompa, LOW);
      BT.println("pompa_low");
  }
  
  for (i = 0; i < 3; i += 1)       // i tussen 0, 1 en 2 varieren, de indexen van de 3 siropen in button en pompjes arrays
  {  
    if(bt_cmd.indexOf(smaak[i]) != -1)
    {
      Serial.print("new_order,");
      Serial.println(bt_cmd);
      
      Serial.print(smaak[i]);
      Serial.println(" is gekozen");

      Serial.print("With strength: ");
      String strength_str = bt_cmd.substring(bt_cmd.indexOf(",") + 1, bt_cmd.indexOf(",") + 5);
      float strength = strength_str.toFloat();
      int bar = (int) (strength * 10.0); 
      strength = (float) (bar / 10.0);  

      Serial.println(strength);
      Serial.println(i);
      
      BT.println(smaak[i]);
      BT.println(" is gekozen");
      
      if(isSyrupLow(i))                     // Check if there's enough syrup 'i'
      { 
        Serial.println("isSyrupLow(i) is done");
        BT.println("isSyrupLow(i) is done");
        
        if(isLimoFull())                    // Check if the mixing reactor is full and give choice to resolve if it's full
        { 
          Serial.println("isLimoFull() is done");
          BT.println("isLimoFul l() is done");
          
          if(isWaterLow())                  // Check if there's enough water to prepare the lemonade
          {
            Serial.println("isWaterLow() is done");
            BT.println("isWaterLow() is done");
            
            for (float w = 0.0; w < strength; w + 0.5)     // Tegen oververhitting pompen in porties
            {      
              Serial.println(strength);
              Serial.println(w);
              digitalWrite(pompv[i], HIGH);       // Wordt siroop 'i' gepompt
              
              BT.print("pompnr");
              BT.print(pompv[i]);
              BT.println("_high");

              Serial.print("pompnr");
              Serial.print(pompv[i]);
              Serial.println("_high");
              
              delay(0.5*pump_duration);
              digitalWrite(pompv[i], LOW);

              BT.print(pompv[i]);
              BT.println("_low");

              Serial.print("pompnr");
              Serial.print(pompv[i]);
              Serial.println("_low");
              w += 0.5;
              delay(2000);                  
            }
            
            delay(50);
            
            for (float w = 0.0; w <= 10-strength; w + 0.5)           // Tegen oververhitting pompen in porties
            {
              Serial.println(10-strength);
              Serial.println(w);
              digitalWrite(pompw, HIGH);
              
              BT.println("pompw_high");
              Serial.println("pompw_high");
              
              delay(pump_duration);               //tap_delay*10 want siroop:water = 1:10
              digitalWrite(pompw, LOW);
              
              BT.println("pompw_low");
              Serial.println("pompw_low");        //x * pump_syrup + y * pump_water = Volume
              w += 0.5;
              delay(2000);        
            }
      
            digitalWrite(LED, HIGH);              // Ledje v/d afgifte      
            delay(5000);                          // 5s de tijd (terwijl het LED'je brandt) om je beker onder de kraan te zetten
            digitalWrite(LED, LOW);
            
            int RFID_val = getRFID();
            Serial.println("RFID");
            Serial.println(RFID_val);
            
            
            while (RFID_val == 1)                    //wait for beker
            {
              RFID_val = getRFID();
            }
            
            digitalWrite(LED, LOW);
            gewenstedelay = 0;
            if (RFID_val == delaya)
            {
              gewenstedelay = delaya;
              Serial.println("gewenstedelay = delaya");
            }
            if (RFID_val == delayb)
            {
              gewenstedelay = delayb;
              Serial.println("gewenstedelay = delayb");
            }
            Serial.println(gewenstedelay);
            
            tijd = millis();
            Serial.println(tijd);
            Serial.println("in while RFID_val");    
            int w = 0;
            while (w < 10 && RFID_val != 1)
            {
              Serial.print("in cycle:");
              Serial.println(w);
              RFID_val = getRFID();
              Serial.println("RFID_val =");
              Serial.println(RFID_val);
              
              digitalWrite(pompl, HIGH);
              Serial.println("pompl, HIGH");
              
              RFID_val = getRFID();
              Serial.println("RFID_val =");
              Serial.println(RFID_val);
              
              delay(gewenstedelay/10);
              
              RFID_val = getRFID();
              Serial.println("RFID_val =");
              Serial.println(RFID_val);
              
              digitalWrite(pompl, LOW);
              Serial.println("pompl, LOW");
              
              delay(1000); //cool down
              
              RFID_val = getRFID();
              Serial.println("RFID_val =");
              Serial.println(RFID_val);
              Serial.println(millis());
              w++;
            }
              if (millis() - tijd >= gewenstedelay + 9000)
              { 
                Serial.println("break the loop");
                RFID_val = 1;
                
              }
       
              digitalWrite(pompl, LOW);       //v.g.m. zijn deze niet perse nodig?
              Serial.println("beker is gone/klaar met tappen");
//                    BT.println("beker is gone");
//            BT.println("uw limonade is getapt");
            
            digitalWrite(LED, HIGH);
            delay(5000);                          // Signaal dat de uitgifte is afgerond
            digitalWrite(LED, LOW);
          }
        }
      }
    }
  }
/* ------- End Bluetooth-Pump Communication ------- */
}

/* ------- End Arduino Loop Function ------- */

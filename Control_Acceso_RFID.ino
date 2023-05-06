


#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
int pos = 0;  

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

Servo myservo;

#if defined(ESP32)
  #define SS_PIN 5
  #define RST_PIN 4
  #define ledrojo 12
  #define ledverde 27
#endif

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
String DatoHex;
const String UserReg_1 = "11824D23";
const String UserReg_2 = "C1B39F18";
const String UserReg_3 = "7762C83B";
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void setup() 
{
  myservo.attach(13);
   // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  pinMode(ledverde, OUTPUT);
  pinMode(ledrojo, OUTPUT);
  
   Serial.begin(115200);
   SPI.begin(); // Init SPI bus
   rfid.PCD_Init(); // Init MFRC522
   Serial.println();
   Serial.print(F("Reader :"));
   rfid.PCD_DumpVersionToSerial();
   for (byte i = 0; i < 6; i++) {
     key.keyByte[i] = 0xFF;
   } 
   DatoHex = printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
   Serial.println();
   Serial.println();
   Serial.println("Iniciando el Programa");
}

void loop() 
{   
    lcd.setCursor(0,0);
    lcd.print("Favor de colocar");
    lcd.setCursor(0,1);
    lcd.print("su tarjeta RFID ");

     // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
     if ( ! rfid.PICC_IsNewCardPresent()){return;}
     
     // Verify if the NUID has been readed
     if ( ! rfid.PICC_ReadCardSerial()){return;}
     
     Serial.print(F("PICC type: "));
     MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
     Serial.println(rfid.PICC_GetTypeName(piccType));
     // Check is the PICC of Classic MIFARE type
     if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
     {
      lcd.clear(); 
      lcd.setCursor(0, 0);
      // print message
      lcd.print("Su Tarjeta no es del tipo MIFARE Classic.");
      delay(1000);
      // clears the display to print new message
      lcd.clear();
       
       return;
     }
     
     if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] )
     {
        lcd.clear(); 
       lcd.setCursor(0, 0);
      // print message
      lcd.print("Se ha detectado ");
      lcd.setCursor(0,1);
      lcd.print("tarjeta nueva");
      delay(1000);
      lcd.clear();
       // Store NUID into nuidPICC array
       for (byte i = 0; i < 4; i++) {nuidPICC[i] = rfid.uid.uidByte[i];}
    
       DatoHex = printHex(rfid.uid.uidByte, rfid.uid.size);
       Serial.print("Codigo Tarjeta: "); Serial.println(DatoHex);
       // set cursor to first column, first row
  lcd.clear();
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Codigo Tarjeta: ");
  lcd.setCursor(0,1);
  lcd.print(DatoHex);
  delay(1000);
  lcd.clear();     

       if(UserReg_1 == DatoHex)
       {
         lcd.setCursor(0, 0);
          // print message
      lcd.print("USUARIO 1");
      lcd.setCursor(0,1);
      lcd.print("Puede ingresar");     
          
          digitalWrite(ledverde, HIGH);
          abrir();
         
         digitalWrite(ledverde,LOW);
         lcd.clear(); 
       }
       else if(UserReg_2 == DatoHex)
       {
        lcd.setCursor(0, 0);
          // print message
      lcd.print("USUARIO 2");
      lcd.setCursor(0,1);
      lcd.print("Puede ingresar");
       
        digitalWrite(ledverde, HIGH);
         abrir();    
         
         digitalWrite(ledverde,LOW);
         lcd.clear();
       }
       else if(UserReg_3 == DatoHex)
       {
         Serial.println("USUARIO 3 - PUEDE INGRESAR");
        digitalWrite(ledverde, HIGH);
         delay(4000);
         digitalWrite(ledverde,LOW);
       }
       else
       {
        lcd.setCursor(0, 0);
          // print message
      lcd.print("NO REGISTRADO");
      lcd.setCursor(0,1);
      lcd.print("INGRESO DENEGADO");     
        digitalWrite(ledrojo, HIGH);
         delay(1000);
         digitalWrite(ledrojo,LOW);
         lcd.clear(); 
       }  
       
     }
     else 
     {
       lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("Tarjeta leida");
       lcd.setCursor(0,1);
      lcd.print("previamente");
      digitalWrite(ledrojo, HIGH);
         delay(1000);
         digitalWrite(ledrojo,LOW);
         lcd.clear();
     }
     // Halt PICC
     rfid.PICC_HaltA();
     // Stop encryption on PCD
     rfid.PCD_StopCrypto1();
}


String printHex(byte *buffer, byte bufferSize)
{  
   String DatoHexAux = "";
   for (byte i = 0; i < bufferSize; i++) 
   {
       if (buffer[i] < 0x10)
       {
        DatoHexAux = DatoHexAux + "0";
        DatoHexAux = DatoHexAux + String(buffer[i], HEX);  
       }
       else { DatoHexAux = DatoHexAux + String(buffer[i], HEX); }
   }
   
   for (int i = 0; i < DatoHexAux.length(); i++) {DatoHexAux[i] = toupper(DatoHexAux[i]);}
   return DatoHexAux;
}

void abrir (){
  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
  delay(3000);
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(20);                       // waits 15ms for the servo to reach the position
  }
}


//librerias

//Modulo RC522
#include <SPI.h>
#include <MFRC522.h>
//lcd I2C
#include <LiquidCrystal_I2C.h>
//Servo
#include <Servo.h>


// Variables con el numero de columnas y filas de la pantalla
int lcdColumns = 16;
int lcdRows = 2;

//Variable de la posicion origen servo
int pos = 0;  


//Objeto del servo
Servo myservo;

//Definicion de pines en el esp32
#if defined(ESP32)
  #define SS_PIN 5
  #define RST_PIN 4
  #define ledrojo 12
  #define ledverde 27
#endif

MFRC522 rfid(SS_PIN, RST_PIN); //Instancia del modulo rfid

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); //Instancia de la pantalla

MFRC522::MIFARE_Key key;

// Array donde se almacena el nuevo NUID
byte nuidPICC[4];

//Credenciales de las tarjetas autorizadas
String DatoHex;
const String UserReg_1 = "11824D23";
const String UserReg_2 = "C1B39F18";
const String UserReg_3 = "7762C83B";

//Inicio Setup
void setup() 
{
  //Definir el pin del servo 
  myservo.attach(13);
   // Inicio de pantalla 
  lcd.init();
  // Encender la retroiluminacion de la pantalla                      
  lcd.backlight();
  //Inidicar el modo de los pines de los leds
  pinMode(ledverde, OUTPUT);
  pinMode(ledrojo, OUTPUT);

    //Inicio terminal serial
   Serial.begin(115200);
   //Inicio bus SPI
   SPI.begin(); 
   //Inicio modulo MFRC522
   rfid.PCD_Init(); 

   // Show details of PCD - MFRC522 Card Reader details
   rfid.PCD_DumpVersionToSerial();
   for (byte i = 0; i < 6; i++) {
     key.keyByte[i] = 0xFF;
   } 
   DatoHex = printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  
}

void loop() 
{   
    //Mensaje en pantalla de Inicio
    lcd.setCursor(0,0);
    lcd.print("Favor de colocar");
    lcd.setCursor(0,1);
    lcd.print("su tarjeta RFID ");

     // Restablezca el bucle si no hay ninguna tarjeta nueva presente en el sensor/lector. 
     // Esto guarda todo el proceso cuando está inactivo
     if ( ! rfid.PICC_IsNewCardPresent()){return;}
     
     // Verifica si el NUID ya a sido leido
     if ( ! rfid.PICC_ReadCardSerial()){return;}
     

     MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
     
     // Verifica si el tipo de tarjeta es valido 
     if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
     {
       //Manda mensaje 
      lcd.clear(); 
      lcd.setCursor(0, 0);
      lcd.print("Tarjeta invalida");
      delay(1000);
      lcd.clear();
       
       return;
     }
     
     if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] )
     {
        lcd.clear(); 
       //Informamos que la tarjeta fue leida 
       lcd.setCursor(0, 0);
      
      lcd.print("Se ha detectado ");
      lcd.setCursor(0,1);
      lcd.print("tarjeta nueva");
      delay(1000);
      lcd.clear();
       // Almacenamos  el NUID en nuestro array
       for (byte i = 0; i < 4; i++) {nuidPICC[i] = rfid.uid.uidByte[i];}
    
       DatoHex = printHex(rfid.uid.uidByte, rfid.uid.size);
       
       
  lcd.clear();
  //Mostramos codigo de tarjeta
  lcd.setCursor(0, 0);
  lcd.print("Codigo Tarjeta: ");
  lcd.setCursor(0,1);
  lcd.print(DatoHex);
  delay(1000);
  lcd.clear();     
//Verificamos si el usuario esta registrado
       if(UserReg_1 == DatoHex)
       {
         // Mostramos mensaje al usuario
         lcd.setCursor(0, 0);
      lcd.print("USUARIO 1");
      lcd.setCursor(0,1);
      lcd.print("Puede ingresar");     
          //Encendemos el led y Abrimos el servo
          digitalWrite(ledverde, HIGH);
          abrir();
         
         digitalWrite(ledverde,LOW);
         lcd.clear(); 
       }
       else if(UserReg_2 == DatoHex)
       {
         // Mostramos mensaje al usuario
        lcd.setCursor(0, 0);
      lcd.print("USUARIO 2");
      lcd.setCursor(0,1);
      lcd.print("Puede ingresar");
       //Encendemos el led y Abrimos el servo
        digitalWrite(ledverde, HIGH);
         abrir();    
         
         digitalWrite(ledverde,LOW);
         lcd.clear();
       }
       else if(UserReg_3 == DatoHex)
       {
            // Mostramos mensaje al usuario
        lcd.setCursor(0, 0);
      lcd.print("USUARIO 3");
      lcd.setCursor(0,1);
      lcd.print("Puede ingresar");
       //Encendemos el led y Abrimos el servo
        digitalWrite(ledverde, HIGH);
         abrir();    
         
         digitalWrite(ledverde,LOW);
         lcd.clear();
       }
       else
       {
         //En caso de que no este registrado mandamos mensaje y encendemos led rojo
        lcd.setCursor(0, 0);
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
       //Si una tarjeta ya fue leida mostrara un mensaje al usuario y se enciende el led rojo
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
     // Detener PICC
     rfid.PICC_HaltA();
 
     //Detenemos la encrytacion
     rfid.PCD_StopCrypto1();
}

//Metodo para imprmir codigo de la tarjeta
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
//Metodo para abrir el servo
void abrir (){
  for (pos = 0; pos <= 90; pos += 1) { //va de 0 grados a 180 grados
    // se va en pasos de 1 grado
    myservo.write(pos);              // Le decimos al servo que vaya a la posición en variable 'pos'
    delay(20);                       // Tiempo para que se llegue a la posicion
  }
  delay(3000);
  for (pos = 90; pos >= 0; pos -= 1) { // va de 180 grados a 0 grados
    myservo.write(pos);              // Le decimos al servo que vaya a la posición en variable 'pos'
    delay(20);                       // Tiempo para que se llegue a la posicion
  }
}

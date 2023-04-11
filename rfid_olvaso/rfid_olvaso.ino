#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define KAPCSOLO 2
#define LED_ZOLD 3
#define LED_PIROS 4
#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);

bool betanito_mod = false;
bool olvaso_mod = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  bool result = mfrc522.PCD_PerformSelfTest();
  if (!result) 
  {
    Serial.println("Ellenőrizd a kapcsolást!");
  }
  pinMode(KAPCSOLO, INPUT);
  pinMode(LED_PIROS, OUTPUT);
  pinMode(LED_ZOLD, OUTPUT);
  digitalWrite(LED_PIROS, LOW);
  digitalWrite(LED_ZOLD, LOW);
}

void loop() 
{
  if (digitalRead(KAPCSOLO) == HIGH) 
  {
    kartya_betanit();
  } 
  else 
  {
    kartya_olvas();
  }
  delay(100);
}

void kartya_betanit()
{
  if(!betanito_mod)
  {
    betanito_mod = true;
    olvaso_mod = false;
    Serial.println("Betanító mód - Kártyára vár...");
  }
  
  if (!mfrc522.PICC_IsNewCardPresent())
  {
      return;
  }

  if (!mfrc522.PICC_ReadCardSerial())
  {
      return;
  }

  kartya_ment(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println("Rendben, kártya betanítva!");
  digitalWrite(LED_ZOLD, HIGH);
  digitalWrite(LED_PIROS, HIGH);
  delay(500);
  digitalWrite(LED_ZOLD, LOW);    
  digitalWrite(LED_PIROS, LOW);
  delay(500); 
  digitalWrite(LED_ZOLD, HIGH);
  digitalWrite(LED_PIROS, HIGH);
  delay(500);
  digitalWrite(LED_ZOLD, LOW);    
  digitalWrite(LED_PIROS, LOW);
}

void kartya_olvas()
{
  if(!olvaso_mod)
  {
    olvaso_mod = true;
    betanito_mod = false;
    Serial.println("Olvasó mód - Kártyára vár...");
  }
  
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  if (kartya_ellenoriz(mfrc522.uid.uidByte, mfrc522.uid.size))
  {
    Serial.println("Kártya elfogadva!");
    digitalWrite(LED_ZOLD, HIGH);
    delay(1000);
    digitalWrite(LED_ZOLD, LOW);    
  }
  else
  {
    Serial.println("Kártya elutasítva!");
    digitalWrite(LED_PIROS, HIGH);
    delay(1000);
    digitalWrite(LED_PIROS, LOW);   
  }
}

void kartya_ment(byte *data, byte size)
{
  for (int i = 0; i < size; i++)
  {
    EEPROM.write(i, data[i]);
  }
}

bool kartya_ellenoriz(byte *data, byte size)
{
  for (int i = 0; i < size; i++)
  {
     byte stored = EEPROM.read(i);
     if (stored != data[i])
     {
       return false;
     }
  }
  return true;
}

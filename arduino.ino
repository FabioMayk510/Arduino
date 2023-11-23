#include <Wire.h> // Biblioteca utilizada para fazer a comunicação com o I2C
#include <LiquidCrystal_I2C.h> // Biblioteca utilizada para fazer a comunicação com o display 20x4 
#include <SPI.h>
#include <MFRC522.h>

#define col 16 // Serve para definir o numero de colunas do display utilizado
#define lin  2 // Serve para definir o numero de linhas do display utilizado
#define ende  0x27 // Serve para definir o endereço do display.
#define   SS_PIN    10
#define   RST_PIN    9

const int pinoLed = 8;
const int sensorEsquerdo = 0; //sensor esq
const int sensorDireito = 3; //sensor dir
 
//Definicoes pinos Arduino ligados a entrada da Ponte H
int IN1 = 4;
int IN2 = 5;
int IN3 = 6;
int IN4 = 7;

int TRIG = 2;
int ECHO = 3;

int distancia;
char st[20];
bool atv = false;
bool acs = false;
bool pisc = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Cria instância com MFRC522
LiquidCrystal_I2C lcd(ende,col,lin); // Chamada da funcação LiquidCrystal para ser usada com o I2C

void rfid_func(); 
void p();
  
void setup()
{
  //Define os pinos como saida
 pinMode(IN1, OUTPUT);
 pinMode(IN2, OUTPUT);
 pinMode(IN3, OUTPUT);
 pinMode(IN4, OUTPUT);

 pinMode(sensorEsquerdo, INPUT);
 pinMode(sensorDireito, INPUT);
 Serial.begin(9600);

 pinMode(TRIG,OUTPUT);
 pinMode(ECHO,INPUT);

 pinMode(pinoLed, OUTPUT);

 SPI.begin();          // Inicia comunicação SPI bus
 mfrc522.PCD_Init();   // Inicia MFRC522

 lcd.init(); // Serve para iniciar a comunicação com o display já conectado
 lcd.backlight(); // Serve para ligar a luz do display
 lcd.clear(); // Serve para limpar a tela do display


}
  
void loop()
{
  // Serial.print(digitalRead(sensorEsquerdo));
  // Serial.print(" / ");
  // Serial.println(analogRead(sensorDireito));
  if(pisc == true){
    digitalWrite(pinoLed, LOW);
    delay(500);
    digitalWrite(pinoLed, HIGH);
  }
  rfid_func();
  p();
}

int sensor_morcego(int pinotrig,int pinoecho){
    digitalWrite(pinotrig,LOW);
    delayMicroseconds(2);
    digitalWrite(pinotrig,HIGH);
    delayMicroseconds(10);
    digitalWrite(pinotrig,LOW);
    return pulseIn(pinoecho,HIGH)/58;
} 

void p(){
  Serial.print(digitalRead(sensorEsquerdo));
  Serial.print(" / ");
  Serial.println(analogRead(sensorDireito));
  if(atv == true){
    distancia = sensor_morcego(TRIG, ECHO);
    // Serial.print("distancia: ");
    // Serial.println(distancia);
    if(distancia < 5){
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, HIGH);

      lcd.setCursor(1,0);
      lcd.print("LIXO ENCONTRADO");
      lcd.setCursor(5 ,1); //Coloca o cursor do display na coluna 1 e linha 2
      lcd.print("RETIRE");  // Comando de saida com a mensagem que deve aparecer na coluna 2 e linha 4
    } else {
      lcd.clear();
      if(digitalRead(sensorEsquerdo) == 0 && analogRead(sensorDireito) < 200){ 
        Serial.print("DIR/ESQ branco\n");
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
      }else if(digitalRead(sensorEsquerdo) == 0 && analogRead(sensorDireito) > 400){ 
          Serial.print("ESQ branco/DIR preto\n");
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, HIGH);
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, HIGH);
      } else if(digitalRead(sensorEsquerdo) ==  1 && analogRead(sensorDireito) < 200){
          Serial.print("ESQ preto/DIR branco\n");
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, HIGH);
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
      } else {
        Serial.print("DIR/ESQ preto\n");
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
      }
    }
  } else {
      
      digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
  }  
}

void rfid_func()                            //Função para identificação das tags RFID
{
      // -- verifica novas tags --
      if ( ! mfrc522.PICC_IsNewCardPresent()) 
      {
        return;
      }
      // seleciona um dos cartões
      if ( ! mfrc522.PICC_ReadCardSerial()) 
      {
        return;
      }
      
      // mostra UID na serial
      Serial.print("UID da tag :");
      String conteudo= "";
      byte letra;
      for (byte i = 0; i < mfrc522.uid.size; i++) 
      {
         Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
         Serial.print(mfrc522.uid.uidByte[i], HEX);
         conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
         conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      Serial.println();
      Serial.print("Mensagem : ");
      conteudo.toUpperCase();



      if (conteudo.substring(1) == "4D 51 D2 CF") 
      {
 	lcd.clear();
        atv = !atv;
        if(atv == true){
          lcd.print("Ligando...");
        } else {
          lcd.print("Desligando...");
        }
        delay(5000);
      }
     
      if (conteudo.substring(1) == "63 E0 8B 95") 
      {
	lcd.clear();
        lcd.print("ACESO");
        pisc = false;
        acs = !acs;
        if(acs == false){
          digitalWrite(pinoLed, HIGH);
        } else {
          digitalWrite(pinoLed, LOW);
        }
        delay(800);
      }

	if (conteudo.substring(1) == "17 95 85 A7") 
      {
        lcd.clear();
	lcd.print("PISCANDO!");
        acs = false;
        pisc = !pisc;
        delay(800);
        
      }
  lcd.clear();
  
}


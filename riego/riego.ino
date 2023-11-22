
/* Integrantes :  Maximo Olguin Muñoz y Jonatan Peredo

----------- PROYECTO RIEGO AUTOMATIZADO PARA MACETAS --------------------

El sistema de riego automatizado controla el encendido de una bomba de agua de acuerdo:
a la temperatura, humedad ambiente, humedad suelo y un sensor de intensidad de luz(ldr fotoresistor). 

En días lluviosos la bomba permanecera apagada mientras que en días con elevada temperatura se deberá encender.
En horas de máxima luz solar, no se debera regar.
Se regara solo cuando sea necesario.
Todo esto gracias al control de los sensores.

Lista de sensores usados en el proyecto: 
Sensor de humedad  HL-69
Mini Bomba De Agua Sumergible 3v 5v Dc Arduino
sensor de temperatura y humedad ambiente DTH11
Modulo Sensor De Luz Con Fotoresistor
arduino mega
*/


//sensor de temperatura y humedad ambiente DTH11
#include "DHT.h"   // Librería para el sensor de temperatura y humedad
#define DHTPIN 2    // Pin digital al que está conectado el sensor DHT
#define DHTTYPE DHT11   // Tipo de sensor DHT (DHT11 o DHT22)
DHT dht(DHTPIN, DHTTYPE);


// configuracion fotoresistores ldr
#define FOTORESISTOR_PIN             A1    
#define CONFIG_FOTORESISTOR          pinMode(FOTORESISTOR_PIN, INPUT)  
#define LEER_FOTORESISTOR            analogRead(FOTORESISTOR_PIN)


/*CONFIGURACION DE SENSOR DE HUMEDAD SUELO
Los valores obtenidos van desde 0 sumergido en agua, a 1023 en el aire (o en un suelo muy seco). 
Un suelo ligeramente húmero daría valores típicos de 600-700. Un suelo seco tendrá valores de 800-1023.
La salida digital dispara cuando el valor de humedad supera el umbral definido como predeterminado.
Por lo tanto, se obtendra una señal LOW cuando el suelo no está húmedo, y HIGH cuando la humedad supera el valor de predefinido.
*/
#define PIN_HUM_SUELO                  3
#define CONFIG_HUM                   pinMode(PIN_HUM_SUELO, INPUT)
#define LEER_HUMEDAD_SUELO                digitalRead(PIN_HUM_SUELO)

//CONFIGURACION BOMBA DE AGUA PARA EL RIEGO
#define PIN_RIEGO                    11
#define CONFIG_RIEGO                 pinMode(PIN_RIEGO, OUTPUT)
#define ACTUALIZAR_RIEGO(x)          digitalWrite(PIN_RIEGO, x)


//LED_TEST
#define INTERVALO_LED_TEST           250 //Function LedTest()
#define LED_TEST                     LED_BUILTIN
#define CONFIG_LED_TEST              pinMode(LED_TEST, OUTPUT)
#define ACTUALIZAR_LED_TEST(x)       digitalWrite(LED_TEST, x)


// max y min de valores de sensores
#define MAX_TEMP             38
#define MIN_TEMP             5

#define MAX_LUZ              100


//variables booleanas 
bool mucha_luz = 0; //ldr
bool frio = 0; //dth

bool estado_riego = 0; //define si esta prendido la bomba
bool humedad_suelo = 0; //estado del sensor de humedad


#define INTERVALO                     2000

void setup() {
  Serial.begin(9600);
  
  CONFIG_FOTORESISTOR;
  CONFIG_HUM;
  CONFIG_RIEGO;
  dht.begin(); 
  //ledtest
  CONFIG_LED_TEST;
  ACTUALIZAR_LED_TEST(0);
}

void loop(){
DTH11();
Riego();
Fotoresistor();
HumedadSuelo();
LedTest();
MostrarValores(); // esto no va 
}

void Riego(){  
  
  if(humedad_suelo == 1 && mucha_luz == 0 && frio == 0 ) estado_riego = 0;
  else estado_riego = 1;
  ACTUALIZAR_RIEGO(estado_riego);
}

void DTH11(){
  //dth11
  
  float temperatura = dht.readTemperature();
  if (isnan(temperatura)) return;  // si detecta algun error retorna
  
  if(temperatura < MIN_TEMP) frio = 1;
  else frio = 0;
}


void Fotoresistor(){
  // menor valor mas luz
  int dato = LEER_FOTORESISTOR;
  if(dato < MAX_LUZ) mucha_luz = 1;
  else mucha_luz = 0;
}

void HumedadSuelo(){
  //cambiar de estado boton, remplazo boton por falta de sensor de humedad
 static unsigned long antMillis = 0;     
  bool estado_actual = LEER_HUMEDAD_SUELO;
  bool estado_ant = 0;
  if (estado_actual == estado_ant) return;
    
  if (millis() - antMillis < INTERVALO) return;
  antMillis = millis();
  estado_ant = estado_actual;
  if(estado_ant) humedad_suelo = !humedad_suelo; 
}


//esto no va, es para correxion de errores
void MostrarValores(){
  static unsigned long anMillis=0;
  if(millis() - anMillis < INTERVALO) return;
  anMillis = millis();

  //dth11
  float humedad = dht.readHumidity();
  float temperatura = dht.readTemperature();
  if (isnan(humedad) || isnan(temperatura)) { // Verifica si la lectura del sensor fue exitosa
    Serial.println("Error al leer el sensor DHT11");
  } else {
    
    Serial.print("Humedad: ");
    Serial.print(humedad);
    Serial.print("%\t");
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println("°C");
    Serial.println(" ");
  } 
  //sensor humedad suelo
  if(humedad_suelo == 0) Serial.println("Se detecto humedad");
  else Serial.println("Se detecto suelo seco");

  // ldr fotoresistor
  int dato = LEER_FOTORESISTOR;
 
  Serial.println("Datos del LDR: ");
  Serial.println(dato);
  if(mucha_luz) Serial.println("Mucha Luz");
  else Serial.println("Poca Luz");
  Serial.println(" ");

  if(estado_riego = 0) Serial.println("RIEGO ANDANDO");
  else Serial.println("RIEGO APAGADO ");
  
}

void LedTest()
  {
  static int ledState= 0;
  static unsigned long antMillis = 0;        

  //Si no se cumple tal condicion, retorna
  if(millis() - antMillis < INTERVALO_LED_TEST) return;
  antMillis = millis();

  ledState = !ledState;
  ACTUALIZAR_LED_TEST(ledState);
}

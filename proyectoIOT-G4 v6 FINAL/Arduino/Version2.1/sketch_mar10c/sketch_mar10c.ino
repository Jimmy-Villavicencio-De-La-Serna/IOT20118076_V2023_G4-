#include <Adafruit_NeoPixel.h>
#include <DHT.h>
#include <DHT_U.h>
//#include <ppd42ns.h>

int Gas = 9;
int redLed = 7;
int partisensor = 2;
int partiLed = 4;
//int particulas = 1100;
int TRIG = 10;  //Pin del trigger en el puerto 10
int ECO = 11;   //Pin del eco en el puerto 11
int tiempo;
int dis;
String alarmaSeg;
String alarmaIn;
String alarmaPart;
String luz;
String temperatura;
String humedad;
const int analogInluz = A5; //Analog input pin that the pontetiometer is attached to
const int analogOutluz = 8; //Analog output pin that the LED is attached to
int sensorValue = 0;        //Value read from the pot
int outputValue = 0;        //Value output to the PWM (analog out)
char input;

#define DHTPIN  12
#define DHTTYPE DHT11
#define PIN     3     //Pin
#define N_LEDS  32    //Number of led in total strips

DHT_Unified dht(DHTPIN, DHTTYPE);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(9600);
  
  dht.begin();
  strip.begin();
  pinMode(Gas, INPUT);
  pinMode(TRIG, OUTPUT);  //Trigger como salida
  pinMode(ECO, INPUT);    //Echo como entrada
  pinMode(partiLed, OUTPUT);
  pinMode(partisensor, INPUT);

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
}

void loop() {
  //Sensor de luz
  sensorValue = analogRead(analogInluz);
  //Map it to the range of the analog out:
  outputValue = map(sensorValue, 0, 1023, 0, 255);
  analogWrite(analogOutluz, outputValue);
  if (outputValue <= 200){
    luz = "1";
  }
  else {
    luz = "0";
  }
  //Sensor de luz fin

  
  //Ultrasonido
  digitalWrite(TRIG, HIGH); //Emisión del pulso
  delay(1);                 //Duración
  digitalWrite(TRIG, LOW);  //del sensor
  tiempo = pulseIn(ECO, HIGH);  //Con función pulseIn se espera un pulso
  dis = 1 + tiempo / 58.2;  //Ecuación para convertir el valor de duracion en una distancia dada en cm
  
  if(dis <= 50){
    alarmaSeg = "1";
  }
  else {
    alarmaSeg = "0";
  }
  //Fin ultrasonido

  
  //Gas
  if(digitalRead(Gas) == HIGH){
    alarmaIn = "1";
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
  }
  else {
    alarmaIn = "0";
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
  }
  //Fin gas
  
  //Temperatura humedad
  //Humedad
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  humedad = event.relative_humidity;
  //Temperatura
  dht.temperature().getEvent(&event);
  temperatura = event.temperature;
  delay(10);
  //Fin temperatura humedad

  //Partículas finas
  int particulas = pulseIn(partisensor,LOW);
  float ratio = particulas / 1000.0; // calcular el ratio (en milisegundos)
  int concentration = ((1.1*pow(ratio,3)-3.8*pow(ratio,2))/5)+((1.1*pow(ratio,3)-3.8*pow(ratio,2))/500)-((1.1*pow(ratio,3)-3.8*pow(ratio,2))/35); // calcular la concentración de partículas en el aire (en pcs/0.01cf)

  if (concentration < 1000){
    alarmaPart = "1";
    digitalWrite(partiLed, HIGH);
  }
  else {
    alarmaPart = "0";
    digitalWrite(partiLed, LOW);
  }
  delay(10);
  //Fin particulas finas


  Serial.println(alarmaIn + ", " + alarmaSeg + ", " + luz + ", " + temperatura + ", " + humedad + ", " + alarmaPart);

  // Lo comente porque necesito 0 y 1 para hacerlo mas simple
  // el dashboard
  s
  /* 
  Serial.println("------------------------------------------------------------------------------------");
  Serial.println("                              Estado de los sensores");
  Serial.println();
  
  Serial.print("   Alarma de fuga de gas: ");
  if(alarmaIn == "0"){
    Serial.println("Apagada");
  }
  else {
    Serial.println("Encendida");
  }

  Serial.print("   Puerta: ");
  if(alarmaSeg == "0"){
    Serial.println("Cerrada");
  }
  else {
    Serial.println("Abierta");
  }
  
  Serial.print("   Foco: ");
  if(luz == "0"){
    Serial.println("Apagado");
  }
  else {
    Serial.println("Encendido");
  }

  Serial.println("   Temperatura: " + temperatura);
  Serial.println("   Humedad: " + humedad);

  // Esto estaba comentado
  // Serial.print("Ratio : ");
  // Serial.println(ratio);
  // Serial.print("Duracion : ");
  // Serial.println(particulas);
  
  Serial.print("   Alarma de Particulas finas por litro (pcs/L): ");
  if(alarmaPart == "0"){
    Serial.println("Apagada");
  }
  else {
    Serial.println("Encendida");
  }
  Serial.println("------------------------------------------------------------------------------------");
  Serial.println();
  Serial.println();
  Serial.println();
  */
  
  //Encender leds
  while (Serial.available() > 0){
    input = Serial.read();
    Serial.println(input);
    if(input == '0'){
      chase(strip.Color(0, 0, 0));  //Apagado
    }
    if(input == '1'){
      chase(strip.Color(0, 0, 255));  //Azul
    }
    if(input == '2'){
      chase(strip.Color(0, 255, 0));  //Verde
    }
    if(input == '3'){
      chase(strip.Color(255, 0, 0));  //Rojo
    }
    if(input == '4'){
      rainbow(30);
    }
  }
  delay(1000);
}


//Funciones para leds
static void chase(uint32_t c){
  for(uint16_t i = 33; i > 0; i--){
    strip.setPixelColor(i - 1, c);  //Draw new pixel
    strip.show();
    delay(30);
  }
}


void rainbow(uint8_t wait){
  uint16_t i, j;

  for(j = 0; j < 256; j++){
    for (i = 0; i < strip.numPixels(); i++){
      strip.setPixelColor(i, Wheel((i*1+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}


//Input a value 0 to 255 to get a color value
//The colours are a transition r-g-b back to r
uint32_t Wheel(byte WheelPos){
  if (WheelPos > 85){
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else {
    if (WheelPos <170){
      WheelPos -= 85;
      return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else {
      WheelPos -= 170;
      return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
  }
}

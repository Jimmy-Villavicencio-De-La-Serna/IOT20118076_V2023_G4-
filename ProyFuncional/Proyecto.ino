//Agregar librerias necesarias
#include <Adafruit_NeoPixel.h>
#include <DHT.h>               	//Para el sensor DHT
#include <DHT_U.h>             	//Para el sensor DHT

//Declaracion de variables globales
int Gas = 9;                		//Pin del sensor de gas
int redLed = 7;             		//Pin del led asociado al sensor de gas
int partisensor = 2;        		//Pin del sensor de particulas finas
int partiLed = 4;           		//Pin del led asociado a particulas finas
int TRIG = 10;             	 	//Pin del trigger en el puerto 10
int ECO = 11;               		//Pin del eco en el puerto 11
const int analogInluz = A5; 	//Pin analogico de entrada para el sensor de luz
const int analogOutluz = 8; 	//Pin analogico de salida para el sensor de luz

int sensorValue = 0;        //Valor obtenido del sensor de luz
int outputValue = 0;        //Valor que se muestra del sensor de luz
String luz;                 	//Variable para el uso del foco
int tiempo;                	//Variable para calcular el tiempo en el sensor de ultrasonido
int dis;                   	//Variable para calcular la distancia en el sensor de ultrasonido
String alarmaSeg;          	//Variable para la alarma del sensor de ultrasonido
String alarmaIn;            	//Variable para la alarma del sensor de gas
String alarmaPart;         	//Variable para la alarma del sensor de particulas finas
String temperatura;       	//Variable para calcular la temperatura en el dht
String humedad;           	//Variable para calcular el porcentaje de humedad en el dht
char input;               	//Variable para la entrada de datos por el monitor serial

//Definiciones de variables
#define DHTPIN  12
#define DHTTYPE DHT11
#define PIN     3     	//Pin
#define N_LEDS  32    	//Numero de leds

DHT_Unified dht(DHTPIN, DHTTYPE);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);


//SET UP    --- Codigo a ejecutarse una vez
void setup() {
  Serial.begin(9600);           //Puerto serial con velocidad de transmision de 9600 baudios
  
  dht.begin();                  		//Inicia el sensor dht
  strip.begin();                		//Inicia el strip
  pinMode(Gas, INPUT);          	//Establece como entrada el pin del sensor de gas
  pinMode(TRIG, OUTPUT);     	//Establece como salida el pin del trigger
  pinMode(ECO, INPUT);          	//Establece como entrada el pin del eco
  pinMode(partiLed, OUTPUT);    //Establece como salida el pin del led asociado al sensor de particulas finas
  pinMode(partisensor, INPUT);  //Establece como entrada el pin del sensor de particulas finas

  sensor_t sensor;                        		//Crea una variable sensor para el sensor dht
  dht.temperature().getSensor(&sensor);  //Para obtener la temperatura del sensor dht
  dht.humidity().getSensor(&sensor);      	//Para obtener la humedad del sensor dht
}



//LOOP    --- Codigo a ejecutarse continuamente
void loop() {
  //Sensor de luz --------------------------------------------------------------------------------------
  sensorValue = analogRead(analogInluz);            //Lectura del sensor de luz
  outputValue = map(sensorValue, 0, 1023, 0, 255);  //Mapea el valor de salida
  analogWrite(analogOutluz, outputValue);           //Asignarlo al rango de la salida analógica
  
  if (outputValue <= 200){    	//Si el valor de salida (en esta caso luz) es menor a 200
    luz = "1";                    		//Se activa el foco
  }
  else {                      		//Caso contrario
    luz = "0";                    		//Se mantiene apagado
  }

  
  //Ultrasonido ----------------------------------------------------------------------------------------
  digitalWrite(TRIG, HIGH);     	//Emisión del pulso
  delay(1);                     		//Retraso de 1 ms
  digitalWrite(TRIG, LOW);      	//Se deja de emitir el pulso
  tiempo = pulseIn(ECO, HIGH);  //Con función pulseIn se espera un pulso
  dis = 1 + tiempo / 58.2;      	//Ecuación para convertir el valor de duracion en una distancia dada en cm
  
  if(dis <= 50){              	//Si la distancia es menor a 50
    alarmaSeg = "1";              	//La alarma se activara
  }
  else {                      	//Caso contrario
    alarmaSeg = "0";             	//Se mantendra apagada
  }

  
  //Gas ------------------------------------------------------------------------------------------------
  if(digitalRead(Gas) == HIGH){//Si la lectura del gas es ALTA (haciendo referencia a fuga)
    alarmaIn = "1";                     		//Se activara la alarma
    digitalWrite(6, LOW);               	//Se apagara el led del pin 6
    digitalWrite(7, HIGH);              	//Se prendera el led del pin 7
  }
  else {                            		//Caso contrario
    alarmaIn = "0";                     //La alarma permanecera apagada
    digitalWrite(6, HIGH);              	//Se prendera el led del pin 6
    digitalWrite(7, LOW);               	//Se apagara el led del pin 7
  }
  
  
  //Temperatura humedad --------------------------------------------------------------------------------
  //Humedad
  sensors_event_t event;                	//Crea una variable sensor para el sensor dht
  dht.humidity().getEvent(&event);      	//Para obtener la humedad del sensor dht
  humedad = event.relative_humidity;    	//Se asigna ese valor a otra variable
  
  //Temperatura
  dht.temperature().getEvent(&event);   	//Para obtener la temperatura del sensor dht
  temperatura = event.temperature;      	//Se asigna ese valor a otra variable
  delay(10);                            		//Retraso de 10 ms

  
  //Partículas finas -----------------------------------------------------------------------------------
  int particulas = pulseIn(partisensor,LOW);      	//Obtiene la cantidad de particulas
  float ratio = particulas / 1000.0;              	//Calcula el ratio (en milisegundos)
  //Se convierte (en este caso se aproximo lo mejor posible)
  int concentration = ((1.1*pow(ratio,3)-3.8*pow(ratio,2))/5)+((1.1*pow(ratio,3)-3.8*pow(ratio,2))/500)-((1.1*pow(ratio,3)-3.8*pow(ratio,2))/35); // calcular la concentración de partículas en el aire (en pcs/0.01cf)

  if (concentration < 1000){   	//Si la concentracion es menor a 1000
    alarmaPart = "1";                         	//Se activara la alarma
    digitalWrite(partiLed, HIGH);             //Se prendera el led del pin partiLed definido previamente
  }
  else {                                  	//Caso contrario
    alarmaPart = "0";                         	//Se apagara la alarma
    digitalWrite(partiLed, LOW);              //Se apagara el led del pin partiLed definido previamente
  }
  delay(10);                              	//Retraso de 10 ms


  //Impresion en el monitor serial
  Serial.println(alarmaIn + ", " + alarmaSeg + ", " + luz + ", " + temperatura + ", " + humedad + ", " + alarmaPart);



  //Impresion que se realizaba previamente, pero tuvo que cambiarse para hacer mas sencilla la parte de Node-Red
  
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
  
  //Encender leds --------------------------------------------------------------------------------------
  while (Serial.available() > 0){	//Mientras la entrada del monitor serial sea mayor a 0
    input = Serial.read();                          	//Se leera el valor ingresado
    Serial.println(input);                          	//Se imprimira dicho valor
    if(input == '0'){                               	//Si el valor es 0
      chase(strip.Color(0, 0, 0));                      	//Estaran apagados
    }
    if(input == '1'){                               	//Si el valor es 1
      chase(strip.Color(0, 0, 255));                    	//Seran de color azul
    }
    if(input == '2'){                               	//Si el valor es 2
      chase(strip.Color(0, 255, 0));                    	//Seran de color verde
    }
    if(input == '3'){                               	//Si el valor es 3
      chase(strip.Color(255, 0, 0));                    	//Seran de color rojo
    }
    if(input == '4'){                               	//Si el valor es 4
      rainbow(30);                                      		//Seran de los colores del arcoiris
    }
  }
  delay(1000);                                      	//Retraso de 1 s
}



//Funciones para leds
static void chase(uint32_t c){
  for(uint16_t i = 33; i > 0; i--){
    strip.setPixelColor(i - 1, c);  //Dibuja un nuevo pixel
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

//Ingresa un valor de 0 a 255 para obtener un valor de color
//Los colores son una transición r-g-b de vuelta a r (rojo)
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

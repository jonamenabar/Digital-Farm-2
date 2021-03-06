


#include <Ultrasonido.h>      //libreria ultrasonido
#include <Encoder.h>         //Libreria encoders mecanicos
#include <DHT.h>            //cargamos la librería DHT (sensor humedad y temperatura
#include <LiquidCrystal.h>  //libreria pantalla
#include <Servo.h>          // libreria servo
#include <EEPROM.h>         //libreria memoria interna atme

//PANTALLA LCD
// Inicializo los pines de la pantalla de LCD
LiquidCrystal lcd(12, 11, 14, 15, 16, 17);

// ULTRASONIDO
int distanciaMaxima=100;                       //distancia maxima que toma el ultrasonido 
Ultrasonido ultrasonic(52,53,distanciaMaxima); // (Trig PIN,Echo PIN) maxdistancia


//SERVO
Servo myservo;                  // Creo un objeto para controlar el servo
                            
 int pos = 0;                   // creo una variable para establecer la posicion del servo 
 
// ENCODERS
Encoder encRight(18, 19);      // pines con interrupcion usados por el encoders de la derecha
Encoder encLeft(20, 21);       // pines con interruccion usados por el encoders de la izquierda
  
//MOTOR
  int IN3 = 5 ;        // Input3 conectada al pin 5
  int IN4 = 6 ;    // Input4 conectada al pin 6 
  int ENLeft = 7  ;    // ENB conectada al pin 7 de Arduino (con PWM)
  int IN1 = 2 ;    // Input3 conectada al pin 2
  int IN2 = 3 ;        // Input4 conectada al pin 3 
  int ENRight = 4 ;// ENB conectada al pin 4 de Arduino (con PWM)

//SENSOR DE TEMPERATURA
#define DHTPIN 13           //Seleccionamos el pin en el que se conectará el sensor
#define DHTTYPE DHT11       //Se selecciona el DHT11 (hay otros DHT)
DHT dht(DHTPIN, DHTTYPE);   //Se inicia una variable que será usada por Arduino para comunicarse con el sensor

//MEMORIA EEPROM
int direccion = 0;          //Se crea una variable con el valor de la posición de memoria en la que se va a almacenar el byte.

//PULSADORES
const int BotonComienzoPin = 22; //Pin al que esta conectado el boton de comienzo de ciclo
const int BotonSeriePin = 23;    //Pin al que esta conectado el boton de comienzo de transmicion


//*************************************** CONFIGURACION ***************************************************************************************
void setup() {

   //CONFIGURACION PUERTO SERIE 
   Serial.begin(9600);      //Se inicia la comunicación serial 
   
   //CONFIGURACION SENSOR HUMEDAD 
   dht.begin();             //Se inicia el sensor

  //CONFIGURACION LCD
  // Configuramos la cantidad de columnas y filas de la pantalla de LCD
  lcd.begin(16, 2);
  // Mostramos un mensaje en la pantalla LCD
  lcd.print("Digital farm 2");
  
  
  //CONFIGURACION DEL MOTOR
   pinMode (ENLeft, OUTPUT); 
   pinMode (IN3, OUTPUT);
   pinMode (IN4, OUTPUT);
   pinMode (ENRight, OUTPUT); 
   pinMode (IN1, OUTPUT);
   pinMode (IN2, OUTPUT);
   digitalWrite (IN1, LOW);
   digitalWrite (IN2, LOW);
   digitalWrite (IN3, LOW);
   digitalWrite (IN4, LOW);

  //CONFIGURACION SERVO
  myservo.attach(9);  // Indicamos en que pin se conectara el control del servo
  

//CONFIGURACION DE PULSADORES
  pinMode(BotonComienzoPin, INPUT);   //configuramos los pulsadores como entrada, 5v-->pulsador-->pinEntrada-->Resis(10k)-->0v (Pull-Down)
  pinMode(BotonSeriePin,INPUT);


}
//***************************************//VARIABLES//**************************************************************************************************************************


float humedad1;          
float temperatura1 ;        
int sensorValue;
float temperatura2=0;  
float humedad2;
float tx;
float distancia;
int contadorCycle=0;
float valorEEPROM;
int boton;
boolean flagCycle=false;
boolean flagSerie=false;
boolean flagEncoders=true;
boolean falgAcelerar=true;
int estadoBotonCycle;
int estadoBotonSerie;
long encoderRight;
long encoderLeft;
float distanciarecorrida;
float distanciaEncoders;
int difEncoders;


//**********************************//VARIABLES A MODIFICAR SEGUN EL FUNCIONAMIENTO//**********************************************************************************************

int distanciaTotal= -100;               //distancoia total recorrida utilizada para cuando retrocede negativa porque retrocede y los encoder cuantan negativamente (DEFAULT -100)
int distanciaRecorrida=20;              // distancia entre ciclos recorrida se modifica segun como esten los encoder y la relacion de vueltas (en Centimetros) Default 20
int distanciaULtrasonido=10;            //distancia a la cual detecta un obstaculo y tiene que actuar 
int velocidadMaximaMotorRight=255;      // valor del duty del PWM valor maximo 255 (100%)
int velocidadMaximaMotorLeft=255;       // valor del duty del PWM valor maximo 255 (100%)
int velocidadReduccionRight=0;          // calor del duty que se usa para reducier la velocidad en el caso de la correccion
int velocidadReduccionLeft=0;           // calor del duty que se usa para reducier la velocidad en el caso de la correccion
int diferenciaEncoders=2;               // a partir de esta diferencia entre los valores de los encodres empieza a realizar las correcciones 
int tiempoCorrecion=500;                //Tiempo de reduccion de velocidad cuando se acciona la correcion en milisegundos
int tiempoAceleracion=10;               // Tiempo que toma en pasar entre dos valores consecutivos del PWM (milisegundos)
int tiempoFrenado=10;                   // Tiempo que toma en pasar entre dos valores consecutivos del PWM (milisegundos)
int tiempoGiro=50000;                   //Default 50000


//***************************************//BUCLE PRINCIPAL//**********************************************************************************************************

void loop() {
  
  
botonCycle();
if(flagCycle==true){
 

  for(int i=0;i<5;i++){       //cantidad de ciclos o mediciones, por defaul 5
    
    avanzar();
    servoDown();
    delay(1000);
    measuringCycle();
    saveData();
    servoUp();
    lcdPrint();
    
 //Solo para debugear
  Serial.print("cycle:");
  Serial.println(contadorCycle);
  Serial.print(humedad1);
  Serial.print("; ");
  Serial.print(temperatura1);
  Serial.print("; ");
  Serial.print(humedad2);
 //   Serial.print("; ");
//    Serial.println(temperatura2);
  
  }
  contadorCycle=0;
 
  Serial.println("Finish cycle");
  flagCycle=false;
  retrocederGiro();                    //Puede Selecionar entre RetrocederGiro, el cual intentara girar 180 grados y avanzar nuevamente. o Retroceder, el cual volvera en marcha atras derecho.
 }
 
 botonSerie();
 if(flagSerie==true){
 //  Serial.println("Start Serie");
  txSerie();
  direccion=0;
  
  
 // Serial.println("Finish Serie");
 flagSerie=false;
 }
  

}







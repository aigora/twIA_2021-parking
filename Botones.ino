#include<Servo.h>
#define Echo 3
#define Trig 2
#define Vel_Son 34000.0
#define LED_rojo 13
#define LED_verde 12
#define boton_entrada 8// lo mismo que el de abajo
#define boton_salida 9//lo pones tu q no consigo usar mi placa
Servo servomotor;
float distancia, tiempo;
int Be, Bs;

void setup() {
  
  Serial.begin(9600);

  servomotor.attach(11);
  servomotor.write(0);
  pinMode(boton_entrada, INPUT);
  pinMode(boton_salida, INPUT); 
  pinMode (Echo, INPUT);
  pinMode (Trig, OUTPUT);
  pinMode (LED_rojo, OUTPUT);
  pinMode (LED_verde, OUTPUT);

}

void loop() {

  inicio();
  Be = digitalRead(boton_entrada);
  Bs = digitalRead(boton_salida);
  
  tiempo = pulseIn(Echo, HIGH);
  distancia = tiempo * 0.000001 * Vel_Son / 2.0;
  Serial.print(distancia);
  Serial.print(" cm\n");
  delay(500);
  
  if (Be==HIGH){
    delay(100);
    
    if(Be==HIGH){
    servomotor.write(90);
    digitalWrite(LED_rojo, LOW);
    digitalWrite(LED_verde, HIGH);
    delay (5000);}
  }
  
  if (Bs==HIGH){
    delay(100);
    
    if(Bs==HIGH){
    servomotor.write(90);
    digitalWrite(LED_rojo, LOW);
    digitalWrite(LED_verde, HIGH);
    delay(5000);}
  }
  
  if(distancia <= 7){
    servomotor.write(90);
    digitalWrite(LED_rojo, LOW);
    digitalWrite(LED_verde, HIGH);
  }
  
  if(distancia > 7){
    servomotor.write(0);
    digitalWrite(LED_rojo, HIGH);
    digitalWrite(LED_verde, LOW);
    delay(500);
  }
}

void inicio(){

  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
}

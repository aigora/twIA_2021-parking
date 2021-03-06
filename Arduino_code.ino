#include<Servo.h>
#define Echo 3
#define Trig 2
#define Vel_Son 34000.0
#define LED_rojo 13
#define LED_verde 12
#define boton_entrada 8
#define boton_salida 9

Servo servomotor;
float distancia, tiempo;
int Be, Bs, coches = 1, aforo;

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

  if (Serial.available() > 0)  // Si hay mensajes procedentes del PC  
        aforo = procesar_aforo();
  
  Be = digitalRead(boton_entrada);
  Bs = digitalRead(boton_salida);

  digitalWrite(LED_rojo, HIGH);
  digitalWrite(LED_verde, LOW);
  
  tiempo = pulseIn(Echo, HIGH);
  distancia = tiempo * 0.000001 * Vel_Son / 2.0;
  delay(500);
  
  if (Be==HIGH){
    delay(100);
    
    if(Be==HIGH){

      if(coches == aforo){
      //Serial.print("Aforo completo, por favor dé la vuelta\n");
      }
      
      else if(coches<=aforo){
        
       int numero;
       String cadena = Serial.readStringUntil('\n'); // Lee mensaje
       //Serial.print(cadena);
  
       String valor = Serial.readStringUntil('\n');  // Lee valor
       numero = valor.toInt();  // Transforma valor a entero  

       if(cadena.equals("Bienvenido")){
         Serial.println("Proceder_al_registro ");
         Serial.println(1);
       }

       delay(5000);

       cadena = Serial.readStringUntil('\n'); // Lee mensaje
       //Serial.print(cadena);
  
       valor = Serial.readStringUntil('\n');  // Lee valor
       numero = valor.toInt();  // Transforma valor a entero  
       
       if (cadena.equals("Registro_completado_e")){
         do{
          Serial.println("Coche_en_barrera ");
          Serial.println(distancia);
          servomotor.write(90);
          digitalWrite(LED_rojo, LOW);
          digitalWrite(LED_verde, HIGH);
          inicio();
          tiempo = pulseIn(Echo, HIGH);
          distancia = tiempo * 0.000001 * Vel_Son / 2.0;
          delay(500);
         }while(distancia <= 10);

        Serial.println("El_coche_ha_entrado ");
        Serial.println(10);

        //coches++;
      
        servomotor.write(0);
        digitalWrite(LED_rojo, HIGH);
        digitalWrite(LED_verde, LOW);
        delay(500);
       }
        
      }
    }
  }
  
  if (Bs==HIGH){
    delay(100);
    
    if(Bs==HIGH){
      
      if(coches > 0) {
        
        int numero;
        String cadena = Serial.readStringUntil('\n'); // Lee mensaje
        //Serial.print(cadena);
  
        String valor = Serial.readStringUntil('\n');  // Lee valor
        numero = valor.toInt();  // Transforma valor a entero  

        if(cadena.equals("Bienvenido")){
         Serial.println("Proceder_al_registro ");
         Serial.println(2);
        }

        delay(10000);

        cadena = Serial.readStringUntil('\n'); // Lee mensaje
        //Serial.print(cadena);
  
        valor = Serial.readStringUntil('\n');  // Lee valor
        numero = valor.toInt();  // Transforma valor a entero

        if (cadena.equals("Registro_completado_s")){
         do{
          Serial.println("Coche_en_barrera ");
          Serial.println(distancia);
          servomotor.write(90);
          digitalWrite(LED_rojo, LOW);
          digitalWrite(LED_verde, HIGH);
          inicio();
          tiempo = pulseIn(Echo, HIGH);
          distancia = tiempo * 0.000001 * Vel_Son / 2.0;
          delay(500);
         }while(distancia <= 10);

         Serial.println("El_coche_ha_salido ");
         Serial.println(11);
         
         //coches--;
        
         servomotor.write(0);
         digitalWrite(LED_rojo, HIGH);
         digitalWrite(LED_verde, LOW);
         delay(500);
       }
      }      
    }
  }
}

void inicio(void){

  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
}

int procesar_aforo(void){
  int numero;
  String cadena = Serial.readStringUntil('\n'); // Lee mensaje
  //Serial.print(cadena);
  
  String valor = Serial.readStringUntil('\n');  // Lee valor
  numero = valor.toInt();  // Transforma valor a entero

  if (cadena.equals("GET_AFORO_MAX")) // Entre las comillas se pone el texto del mensaje que se espera  
    {

      Serial.println("Aforo_fijado ");
      Serial.println(numero);
      return numero;
    }
}

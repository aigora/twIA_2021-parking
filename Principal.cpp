#define TARIFA 0.06
#define DIM 8
#define MAX_BUFFER 200
#define MAX_INTENTOS_READ 4
#define MS_ENTRE_INTENTOS 250
#define SI 1
#define NO 0

#include<time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <windows.h>
#include <string.h>
#include <conio.h>
#include "SerialClass/SerialClass.h"
#include <Windows.h>

typedef struct {
	int plaza;
	float tiempo;
	char matricula[DIM];
}COCHE;

void registro(char matricula[], int coches, char tiempo[26], char op);
void entrada(int coches, COCHE*, char tiempo[26], char op);
void salida(int coches, COCHE*, char tiempo[26], char op);
float tarifa(COCHE*, int coches);
void tiempo(char* t);

// Funciones prototipo
//int menu_principal(void);
void configura(void);

void Talk_with_Arduino(Serial* Arduino);
void Send_to_hw(Serial*, char*);
int Receive_from_hw(Serial* Arduino, char* BufferEntrada);
int Send_and_Receive(Serial* Arduino, const char* msg_out, int valor_out, char* msg_in, int* valor_in);
void monitorizar_aforo(Serial*);

int main() {
	char tiempo_actual[26];
	char o;

	Serial* Arduino;
	char puerto[] = "COM3"; //Puerto serie al que está conectado Arduino
	char BufferEntrada[MAX_BUFFER];
	int bytesReceive, numero_recibido;

	// Tareas de configuración y carga
	configura();
	Arduino = new Serial((char*)puerto);  // Establece la conexión con Arduino

	printf("Introduce el numero de plazas de tu parking: ");
	int n;
	scanf_s("%d", &n);

	bytesReceive = Send_and_Receive(Arduino, "GET_AFORO_MAX", n, BufferEntrada, &numero_recibido);
	if (bytesReceive == 0)
		printf("No se ha recibido respuesta al mensaje enviado\n");
	else
		printf("Mensaje recibido %s %d\n", BufferEntrada, numero_recibido);

	COCHE* plazas;
	plazas = (COCHE*)calloc(n, sizeof(COCHE));

	int  coches = 0;
	float precio = 0, importe_introducido = 0;

	do {
		tiempo(tiempo_actual);
		printf("\t\t\tBIENVENIDO, PRESIONE EL BOTON PARA CONTINUAR\n");

		do {
			bytesReceive = Send_and_Receive(Arduino, "Bienvenido", 1, BufferEntrada, &numero_recibido);
			if (bytesReceive != 0)
				printf("Mensaje recibido %s %d\n", BufferEntrada, numero_recibido);
				
		} while (numero_recibido != 1 && numero_recibido != 2);


		switch (numero_recibido) {

		case 1:
			o = 'E';
			entrada(coches, plazas, tiempo_actual, o);

			do {

				bytesReceive = Send_and_Receive(Arduino, "Registro_completado_e", 1, BufferEntrada, &numero_recibido);
				if (bytesReceive != 0)
					printf("Mensaje recibido %s %d\n", BufferEntrada, numero_recibido);

			} while (numero_recibido != 10);

			Sleep(2000);

			system("cls");

			coches++;
			break;


		case 2:

			if (coches == 0) {

				bytesReceive = Send_and_Receive(Arduino, "No_coches", 0, BufferEntrada, &numero_recibido);
				if (bytesReceive != 0)
					printf("Mensaje recibido %s %d\n", BufferEntrada, numero_recibido);

				Sleep(2000);
				system("cls");
			}

			if (coches > 0) {
				o = 'S';
				salida(coches, plazas, tiempo_actual, o);

				do {

					bytesReceive = Send_and_Receive(Arduino, "Registro_completado_s", 1, BufferEntrada, &numero_recibido);
					if (bytesReceive != 0)
						printf("Mensaje recibido %s %d\n", BufferEntrada, numero_recibido);

				} while (numero_recibido != 11);

				Sleep(2000);

				system("cls");
				coches--;
				break;
			}

		default:
			printf("Opcion incorrecta\n\n");
			Sleep(2000);

			system("cls");
			break;
		}

	} while (coches < n);

	system("cls");

	printf("\t\t\tAFORO COMPLETO, POR FAVOR DE LA VUELTA\n\n\n");
}

void entrada(int coches, COCHE* plazas, char tiempo_ac[26], char opc) {

	printf("Introduzca su matricula: ");
	getchar();
	gets_s(plazas[coches].matricula);

	plazas[coches].tiempo = clock();

	registro(plazas[coches].matricula, coches, tiempo_ac, opc);

	printf("\n\nOk, puede entrar\n\n");

}

void registro(char matricula_e[], int coches, char tiempo[26], char op) {

	FILE* pf;

	errno_t err; // Open for read (will fail if file "registro.txt" doesn't exist)

	err = fopen_s(&pf, "registro.txt", "a+");

	if (err == 0) {

		printf("\nEl archivo registro.txt esta abierto\n");

		fprintf_s(pf, "%s	%.19s	%c\n", matricula_e, tiempo, op);
	}

	else
		printf("\nEl archivo registro.txt NO está abierto\n");

	if (fclose(pf) == NULL)
		printf("\n Archivo cerrado correctamente\n");
	else
		printf("\n Error en el cierre del archivo\n");
}

void salida(int coches, COCHE* plazas, char tiempo_ac[26], char opc) {

	int i = 0;
	float importe_introducido = 0, importe_introducido2 = 0, precio;
	char matricula_sal[DIM];

	printf("Introduzca su matricula: ");
	gets_s(matricula_sal);

	do {  //comprobar si la matricula coincide con alguna del registro strcmp(matricula_sal, plazas[i].matricula) == 0

		if (strcmp(matricula_sal, plazas[i].matricula) == 0) {  //si coincide:

			precio = tarifa(plazas, coches);
			registro(plazas[i].matricula, coches, tiempo_ac, opc);
			printf("El importe asciende a %.2f EUROS, introduzcalos por favor: ", precio);
			scanf_s("%f", &importe_introducido);

			if ((importe_introducido + 0.009) < precio) {

				printf("\nLe falta introducir %.2f EUROS, introduzcalos por favor: ", (precio - importe_introducido));
				getchar();
				scanf_s(" %f", &importe_introducido2);
				importe_introducido += importe_introducido2;
			}

			if ((importe_introducido - 0.009) > precio) {
				printf("\nSe le han devuelto %.2f EUROS, recojalos por favor ", (importe_introducido - precio));
			}

			printf("\nBuen viaje\n");

		}

		i++;

	} while ((strcmp(matricula_sal, plazas[i].matricula) == 0) && i < coches);


	if (i > coches) {
		printf("Matricula introducida no encontrada.\n");
		Sleep(2000);

		system("cls");
	}

}

float tarifa(COCHE* plazas, int coches) {

	float precio, tiempo;

	tiempo = (float)(clock() - plazas[coches].tiempo) / 60000;

	precio = tiempo * TARIFA;

	return precio;
}

void configura(void)
{
	// Establece juego de caracteres castellano
	// Para que funcione hay que partir de un proyecto vacío
	// No utilice la plantilla Aplicación de consola C++
	setlocale(LC_ALL, "spanish");
}

// Ejemplo de función de intercambio de datos con Arduino
void Talk_with_Arduino(Serial* Arduino)
{
	//char BufferSalida[MAX_BUFFER];
	char BufferEntrada[MAX_BUFFER];
	int bytesReceive, numero_recibido;

	if (Arduino->IsConnected()) // Si hay conexión con Arduino 
	{

		// Para enviar un mensaje y obtener una respuesta se utiliza la función Send_and_Receive
		// El mensaje está formado por un texto y un entero
		// El mensaje que se recibe está formado también por un texto y un entero.
		// Parámetros de la función:
		// El primero es la referencia a Arduino
		// El segundo es el mensaje que se desea enviar
		// El tercero es un entero que complementa al mensaje que se desea enviar
		// El cuarto es el vector de char donde se recibe la respuesta
		// El quinto es la referencia donde se recibe el entero de la respuesta
		// IMPORTANTE: El mensaje de respuesta que emite Arduino  debe incluir un espacio en blanco separando respuesta de valor
		// La función devuelve un entero con los bytes recibidos. Si es cero no se ha recibido nada.

		bytesReceive = Send_and_Receive(Arduino, "GET_AFORO_MAX", -1, BufferEntrada, &numero_recibido);
		if (bytesReceive == 0)
			printf("No se ha recibido respuesta al mensaje enviado\n");
		else
			printf("Mensaje recibido %s %d\n", BufferEntrada, numero_recibido);
	}
	else
		printf("La comunicación con la plataforma hardware no es posible en este momento\n"); // Req 3
}

// Protocolo de intercambio mensajes entre Pc y platforma hardware
// Envío Mensaje valor
// Recibe Mensaje valor
		// IMPORTANTE: El mensaje de respuesta que emite Arduino  debe incluir un espacio en blanco separando respuesta de valor
// Retorna bytes de la respuesta (0 si no hay respuesta)
int Send_and_Receive(Serial* Arduino, const char* msg_out, int valor_out, char* msg_in, int* valor_in)
{
	char BufferSalida[MAX_BUFFER];
	char BufferEntrada[MAX_BUFFER];
	char* ptr;
	int bytesReceive;

	sprintf_s(BufferSalida, "%s\n%d\n", msg_out, valor_out);
	Send_to_hw(Arduino, BufferSalida);
	bytesReceive = Receive_from_hw(Arduino, BufferEntrada);
	if (bytesReceive != 0)
	{
		ptr = strpbrk(BufferEntrada, " ");
		if (ptr == NULL)
			*valor_in = -1;
		else
		{
			*valor_in = atoi(ptr);
			*ptr = '\0';
		}
		strcpy_s(msg_in, MAX_BUFFER, BufferEntrada);
	}
	return bytesReceive;
}


// Envía un mensaje a la plataforma hardware
void Send_to_hw(Serial* Arduino, char* BufferSalida)
{
	Arduino->WriteData(BufferSalida, strlen(BufferSalida));
}

//Recibe (si existe) un mensaje de la plataforma hardware
//Realiza MAX_INTENTOS_READ para evitar mensajes recortados
int Receive_from_hw(Serial* Arduino, char* BufferEntrada)
{
	int bytesRecibidos, bytesTotales = 0;
	int intentos_lectura = 0;
	char cadena[MAX_BUFFER];

	BufferEntrada[0] = '\0';
	while (intentos_lectura < MAX_INTENTOS_READ)
	{
		cadena[0] = '\0';
		bytesRecibidos = Arduino->ReadData(cadena, sizeof(char) * (MAX_BUFFER - 1));
		if (bytesRecibidos != -1)
		{
			cadena[bytesRecibidos] = '\0';
			strcat_s(BufferEntrada, MAX_BUFFER, cadena);
			bytesTotales += bytesRecibidos;
		}
		intentos_lectura++;
		Sleep(MS_ENTRE_INTENTOS);
	}
	return bytesTotales;
}
void tiempo(char* t) {
	struct tm newtime;
	__time64_t long_time;
	errno_t err;
	int i = 0;

	// Get time as 64-bit integer.
	_time64(&long_time);
	// Convert to local time.
	err = _localtime64_s(&newtime, &long_time);
	if (err)
	{
		printf("Invalid argument to _localtime64_s.");
		exit(1);
	}

	// Convert to an ASCII representation.
	err = asctime_s(t, 26, &newtime);
	if (err)
	{
		printf("Invalid argument to asctime_s.");
		exit(1);
	}
	printf("%.19s\n", t);
}
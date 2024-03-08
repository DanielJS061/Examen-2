/*
 * File:   main.c
 * Author: Juan Pérez
 *
 * Created on 29 de febrero de 2024, 09:09 AM
 */


#include <xc.h>
#include "config.h"
#define _XTAL_FREQ 20000000

void inicializar_ADC(void); // Inicializar los modulos ADC
int leer_ADC(int); // Recibir un valor de 0 a 1023 y seleccionamos en qué canal

int valor = 0;
int valor2 = 0;
int valor3 = 0;
int valor4 = 0;
int valor5 = 0;
int estado_bomba = 0;
int diferencia = 0;
int arreglo_display[] = {63,6,91,79,102,109,124,71,127,103,119,124,57,94,121,113};

void main(void) {
    TRISB = 0; // Salida de los led
    TRISD = 0b11110000; //D SALIDA
    inicializar_ADC();
    while(1){
        valor = leer_ADC(0);
        valor2 = leer_ADC(1);
        //valor3 = ((valor+valor2)/2)/102.3;
        valor3 = ((valor+valor2)/2)/10.23; // numero completo del 1 al 
        
        valor4 = valor3/10; 
        LATB = arreglo_display[valor4]; //mostrar l salida en binario de nuestra lectura en ADC
        LATDbits.LATD0 = 0;
        LATDbits.LATD1 = 1;
        __delay_ms(50);
        valor5 = valor3 - (valor4*10);
        LATB = arreglo_display[valor5]; //mostrar l salida en binario de nuestra lectura en ADC
        LATDbits.LATD0 = 1;
        LATDbits.LATD1 = 0;
        __delay_ms(50);
        //LATDbits.LATD3 = 1;
        //LATDbits.LATD2 = 1;
        if(PORTDbits.RD7 == 1) {
            estado_bomba = estado_bomba + 1;
            while(PORTDbits.RD7 == 1);
        }
        __delay_ms(20);
        if(estado_bomba == 1) {
            LATDbits.LATD3 = 1;
        }
        __delay_ms(10);
        if(estado_bomba == 2) {
            estado_bomba = 0;
            LATDbits.LATD3 = 0;
        }
        __delay_ms(10);
        if(valor3 <= 10 ){
            LATDbits.LATD3 = 0;
            estado_bomba = 0;
            LATDbits.LATD2 = 1;
        }
        diferencia = leer_ADC(0) - leer_ADC(1);
        if(diferencia < 0) {
            diferencia = -diferencia;
        }
        if(diferencia >= 100 ){
            LATDbits.LATD3 = 0;
            estado_bomba = 0;
            LATDbits.LATD2 = 1;
        }
        if(valor3 > 10) {
            LATDbits.LATD2 = 0;
        }
    }
   
    return;
}

void inicializar_ADC(void) {
    TRISA = 0xFF; // Todos son entradas
    ADCON1 = 0x0D; // VSS, VDD Y dos entradas analógicas A0 y A1
    ADCON2 = 0xB5; // justificado a la derecha con 16 TAD, y FOSC/16
    ADRESH = 0; // Limpiamos el registro
    ADRESL = 0; // Limpiamos el registro
}

int leer_ADC(int canal) {
    int digital = 0;
   
    // canal 0 es seleccionado 0000 y ADC está apagado
    ADCON0 = (ADCON0 & 0b11000011) | ((canal << 2) & 0b00111100);
   
    // habilitar el ADC y empezar la conversión
    ADCON0 |= ((1<<ADON) | (1 << GO));
   
    //Esperar para el término de la conversión
    while(ADCON0bits.GO_nDONE == 1);
   
    // Combinamos los 8 bits LSB y los 2 bits MSB
    digital = (ADRESH * 256) | (ADRESL);
   
    return digital;
}
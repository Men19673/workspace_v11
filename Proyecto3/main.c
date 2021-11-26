/**
 * Diego Alejandro Mendez
 * 19673
 * Proyecto3
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#define XTAL 16000000

/***************************************Definiciones************************************************************/
#define LED_PERIPH SYSCTL_PERIPH_GPIOF
#define LED_BASE GPIO_PORTF_BASE
#define RED_LED GPIO_PIN_1

#define Button_PERIPH SYSCTL_PERIPH_GPIOF
#define ButtonBase GPIO_PORTF_BASE
#define Button GPIO_PIN_4
#define ButtonInt GPIO_INT_PIN_4

/*************************************Declaracion de funciones ***********************************************/
void delayMs(uint32_t);
void semaforo(void);
void PortDIntHandler(void);
//void UART1IntHandler(void);


/**************************************Variables**************************************************************/
uint8_t Flag;
uint8_t mplex;
uint8_t parqueo1 = 49;
uint8_t parqueo2 = 49;
uint8_t parqueo3 = 49;
uint8_t parqueo4 = 49;
uint8_t inSerial;
char outSerial= 48;

uint8_t PB0;
uint8_t PB1;
uint8_t PB2;
uint8_t PB3;
void main(void)
{   //Reloj
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);  //40MHz con PLL
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);  // Se asigna reloj a puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);  // Se asigna reloj a puerto D
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);  // Se asigna reloj a puerto A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);  // Se asigna reloj a puerto B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);  // Se asigna reloj a puerto E
    //PORTF Y PORTD
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); // Establecer los LEDS portF
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_5 | GPIO_PIN_1 | GPIO_PIN_4); // Establecer los pines portB
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5); // Establecer los pines portE
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_5| GPIO_PIN_7); // Establecer los pines portA

    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //PORT D  PULLUP
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    //Configurar el WeakPullup

    //UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);    //Asignar reloj del UART1 y el GPIOC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinConfigure(GPIO_PC4_U1RX); //Activar el pin muxing para UART
    GPIOPinConfigure(GPIO_PC5_U1TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));
    /*UARTIntRegister(UART1_BASE, UART1IntHandler);
    UARTIntEnable(UART1_BASE, GPIO_PIN_4 | GPIO_PIN_5);*/

    //Interrupcion del PORTD
    GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_BOTH_EDGES);
    GPIOIntRegister(GPIO_PORTD_BASE, PortDIntHandler);  //FUNCION INT DE PORT D
    GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    //Activar interrupciones generales
    IntMasterEnable();

   /* SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));        //Esperar a que se inicialice
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);            //Configurar el timer para funcion periodica
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet()/2));   //Configurar el overload
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);            //Activar int del timer
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);   //Registro del interrupt
    IntEnable(INT_TIMER0A);                                     //Habilitar Int
    TimerEnable(TIMER0_BASE, TIMER_A);                          //Enable*/


      while(1){


          inSerial = UARTCharGet(UART1_BASE);

                        switch (inSerial){

                            case 65:    //Enviar parqueo 1 cuando recibe A
                                outSerial = parqueo1;
                                UARTCharPut(UART1_BASE,  outSerial);
                                break;

                            case 66:  //Enviar parqueo 2 cuando recibe B
                                outSerial = parqueo2;
                                UARTCharPut(UART1_BASE,  outSerial);
                                break;
                            case 67:  //Enviar parqueo 3 cuandor recibe C
                                outSerial = parqueo3;
                                UARTCharPut(UART1_BASE,  outSerial);
                                break;

                            case 68:  //Enviar parqueo 4 cuando recibe D
                                outSerial = parqueo4;
                                UARTCharPut(UART1_BASE,  outSerial);
                                break;
                                      }


          delayMs(1);
      }
}


void delayMs(uint32_t ui32Ms) {

    // 1 clock cycle = 1 / SysCtlClockGet() second
    // 1 SysCtlDelay = 3 clock cycle = 3 / SysCtlClockGet() second
    // 1 second = SysCtlClockGet() / 3
    // 0.001 second = 1 ms = SysCtlClockGet() / 3 / 1000

    SysCtlDelay(ui32Ms * (SysCtlClockGet() / 3 / 1000));
}

void PortDIntHandler(void){ //Pines PD0, PD1, PD2, PD3
    uint32_t PinInt=0;
    PinInt = GPIOIntStatus(GPIO_PORTD_BASE, true); //Revisar que pin hizo interrupcion

    //Realizar un antirrebote
    if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) == 0){
                      PB0=1;
                  }
    if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1) == 0){
                      PB1=1;
                  }
    if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2) == 0){
                      PB2=1;
                  }
    if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3) == 0){
                      PB3=1;
              }


    if( (PinInt & GPIO_INT_PIN_0) == GPIO_INT_PIN_0){
      //Then there was a pin0 interrupt
        if(PB0 == 1 && GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) ){
            switch(parqueo1){
                       case 48 :
                        parqueo1 = 49;
                        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_5, 0x20); //Rojo
                        break;
                       case 49 :
                        parqueo1 = 48;
                        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_5, 0x01); //Amamrillo
                        break;
                           }
            PB0 = 0;
            }
    }

    if( (PinInt & GPIO_INT_PIN_1) == GPIO_INT_PIN_1){
      //Then there was a pin1 interrupt
        if(PB1 == 1 && GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1)){
            switch(parqueo2){
                       case 48 :
                        parqueo2 = 49;
                        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_4, 0x02); // Amarillo
                        break;
                       case 49 :
                        parqueo2 = 48;
                        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_4, 0x10); //Rojo
                        break;

                           }
            PB1 = 0;
        }
    }
    if( (PinInt & GPIO_INT_PIN_2) == GPIO_INT_PIN_2){
         //Then there was a pin2 interrupt
           if(PB2 == 1 && GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2)){

                   switch(parqueo3){
                       case 48 :
                        parqueo3 = 49;
                        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0x20); //Amarillo
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
                        break;

                       case 49 :
                        parqueo3 = 48;
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x02);    //Rojo
                        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5, 0x00);
                        break;
                   }
                   PB2 = 0;
               }
       }

       if( (PinInt & GPIO_INT_PIN_3) == GPIO_INT_PIN_3){
         //Then there was a pin3 interrupt
           if(PB3 == 1 && GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3)){
               switch(parqueo4){
                      case 48 :
                       parqueo4 = 49;
                       GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7, 0x80); //Amarillo
                       break;

                      case 49 :
                       parqueo4 = 48;
                       GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7, 0x40); //Rojo
                       break;
               }
               PB3=0;
           }
       }
       GPIOIntClear(GPIO_PORTD_BASE, PinInt);
}




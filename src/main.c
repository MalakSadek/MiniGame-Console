/* 
 * File:   main.c
 * Author: Cristian Fatu
 *
 * Created on April 13, 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <sys/attribs.h>
#include "config.h"
 #include <plib.h>  
 #include <stddef.h>  

#include "mot.h"
#include "lcd.h"
#include "btn.h"
#include "ssd.h"
#include "led.h"
#include "uartjb.h"
#include "swt.h"
#include "rgbled.h"
#pragma config JTAGEN = OFF     
#pragma config FWDTEN = OFF      


/* ------------------------------------------------------------ */
/*						Configuration Bits		                */
/* ------------------------------------------------------------ */

    
// Device Config Bits in  DEVCFG1:	
#pragma config FNOSC =	FRCPLL
#pragma config FSOSCEN =	OFF
#pragma config POSCMOD =	XT
#pragma config OSCIOFNC =	ON
#pragma config FPBDIV =     DIV_2

// Device Config Bits in  DEVCFG2:	
#pragma config FPLLIDIV =	DIV_2
#pragma config FPLLMUL =	MUL_20
#pragma config FPLLODIV =	DIV_1

#define GetSystemClock()              (80000000ul)
#define XTAL                           80000000
#define GetPeripheralClock()          (GetSystemClock()/(1 << OSCCONbits.PBDIV))
#define DESIRED_BAUDRATE              (115200)      //The desired BaudRate

char c[1500];
int count = 0;
int timeleft = 500;
int score = 0;

/*This records whether the user chose to play the fun facts game by pressing 
 * the right button or the patterns game by pressing the left button and returns
 a code for the choice to be used by startGame to know which to start*/
int chooseGame() {
	    int gamechoice = 0;
	    while ((gamechoice != 1)&&(gamechoice !=2)) {
		LCD_WriteStringAtPos("Right = Fun Fact Quiz", 0, 0);
		LCD_WriteStringAtPos("Left = Patterns", 1, 0); 
		if (BTN_GetValue('L')) {
			 gamechoice = 1;
		}
		if (BTN_GetValue('R')) {
			gamechoice = 2;
		}
	}
	return gamechoice;
}
/*This displays the hashtag pattern on the LCD, as well as polling for 
 * the correct combination of switch states, it also lights the current
 question's corresponding LED and decreases the time left*/

void setPattern (int switches[5], int lcd[8], int index) {
		    while(((SWT_GetValue(0) != switches[0])||(SWT_GetValue(1) != switches[1])||(SWT_GetValue(2) != switches[2])||(SWT_GetValue(3) != switches[3])||(SWT_GetValue(4) != switches[4]))&&(timeleft > 1)) {
	    LCD_WriteStringAtPos("#",lcd[0],4);
	    LCD_WriteStringAtPos("#",lcd[1],6);
	    LCD_WriteStringAtPos("#",lcd[2],8);
	    LCD_WriteStringAtPos("#",lcd[3],10);
	    LCD_WriteStringAtPos("#",lcd[4],5);
	    LCD_WriteStringAtPos("#",lcd[5],7);
	    LCD_WriteStringAtPos("#",lcd[6],9);
	    LCD_WriteStringAtPos("#",lcd[7],11);
	    RGBLED_SetValue(255, 0, 0);
	    timeleft-=1;
	     SSD_WriteDigits(timeleft%10,(timeleft/10)%10,timeleft/100,0,0,0,0,0);
	    }

	        RGBLED_SetValue(0, 255, 0);
		LCD_DisplayClear();
		int i;
		for(i = 0; i <10000; i++);
			    LED_SetValue(index,1);
}

/*This sets the patterns in order and displays messages 
 * for winning the game or running out of time*/

void patternGameStart() {
	int score = 0;
	       LCD_DisplayClear();
	       int i;
		    LED_SetValue(7,1);	
		    
		while (timeleft > 1) {
			int switches[5] = {0,1,0,1,0};
			int lcd[8] = {0,0,0,0,1,1,1,1};
			setPattern(switches, lcd, 7);
			
			int switches2[5] = {1,0,1,1,0};
			int lcd2[8] = {1,0,0,1,0,1,0,0};
			setPattern(switches2, lcd2, 6);
			
			int switches3[5] = {0,1,0,0,0};
			int lcd3[8] = {1,0,1,0,1,1,1,1};
			setPattern(switches3, lcd3, 5);

			int switches4[5] = {1,1,1,1,0};
			int lcd4[8] = {1,1,0,0,1,1,0,0};
			setPattern(switches4, lcd4, 4);
			
			int switches5[5] = {1,0,0,0,1};
			int lcd5[8] = {0,1,1,1,1,0,1,0};
			setPattern(switches5, lcd5, 3);
			
			int switches6[5] = {0,1,0,0,1};
			int lcd6[8] = {0,1,1,0,1,0,1,1};
			setPattern(switches6, lcd6, 2);
			
			int switches7[5] = {1,0,0,0,0};
			int lcd7[8] = {0,1,1,1,1,1,1,0};
			setPattern(switches7, lcd7, 1);

			int switches8[5] = {1,0,0,1,1};
			int lcd8[8] = {1,1,0,1,1,0,1,0};
			setPattern(switches8, lcd8, 0);
		
		        LCD_DisplayClear();
			timeleft = 0;
			for(i = 0; i <100000; i++);
			LCD_WriteStringAtPos("Thank you for", 0, 0);
			LCD_WriteStringAtPos("playing!", 1, 0);
			SSD_WriteDigits(0,0,0,0,0,0,0,0); 
			RGBLED_SetValue(0, 255, 0);
    }
			if(timeleft == 1) {
			    RGBLED_SetValue(255, 0, 0);
			    LED_SetValue(0,0);
			    LED_SetValue(1,0);
			    LED_SetValue(2,0);
			    LED_SetValue(3,0);
			    LED_SetValue(4,0);
		        LED_SetValue(5,0);
		        LED_SetValue(6,0);
			    LED_SetValue(7,0);
			    SSD_WriteDigits(0,0,0,0,0,0,0,0); 
			    LCD_DisplayClear();
			    for(i = 0; i <10000; i++);
			    LCD_WriteStringAtPos("Time out,", 0, 0);
			    LCD_WriteStringAtPos("you lose!", 1, 0);
		    }
}

/*This is used to create facts for the fact game who's correct
 answer is yes and therefore do not need an explanation message*/
void positiveFact(char* msg1, char* msg2, int index) {
	int choice = 0;
	int i;
	for(i = 0; i <5000; i++);
	LCD_DisplayClear();
	LED_SetValue(index,1);
	while (!choice) {
	    	LCD_WriteStringAtPos(msg1, 0, 0);
	        LCD_WriteStringAtPos(msg2, 1, 0);
		
    if ( BTN_GetValue('U') == 1) {
	   choice = 1;
	   RGBLED_SetValue(0, 255, 0);
	   for(i = 0; i <10000; i++);
	   RGBLED_SetValue(0, 0, 0);
	   score += 1;
    }
    if ( BTN_GetValue('D') == 1) {
	  RGBLED_SetValue(255, 0, 0);
	  for(i = 0; i <10000; i++);
	   RGBLED_SetValue(0, 0, 0);
	  choice = 1;  
    }
		
    }
     for(i = 0; i <10000; i++);
     SSD_WriteDigits(score,0,0,0,0,0,0,0);
}

/*This is for facts who's correct answer is no, providing an explanation
 text once the user enters a choice*/
void negativeFact(char* msg1, char* msg2, char* msg3, char* msg4, int index) {
	int choice = 0; 
	int i;
	LCD_DisplayClear();
	LED_SetValue(index,1);
	while (!choice) {
	    	LCD_WriteStringAtPos(msg1, 0, 0);
	        LCD_WriteStringAtPos(msg2, 1, 0);
		
	if ( BTN_GetValue('U') == 1) {
	   LCD_DisplayClear();
	    for(i = 0; i <9000; i++);
	   LCD_WriteStringAtPos(msg3, 0, 0);
	   if (msg4 != NULL)
	      LCD_WriteStringAtPos(msg4, 1, 0);
	   RGBLED_SetValue(255, 0, 0);
	   for(i = 0; i <100000; i++);
	   RGBLED_SetValue(0, 0, 0);
	   choice = 1;

    }
	if ( BTN_GetValue('D') == 1) {
	    	  score += 1;
	   LCD_DisplayClear();
	    for(i = 0; i <9000; i++);
	   LCD_WriteStringAtPos(msg3, 0, 0);
	   	   if (msg4 != NULL)
		LCD_WriteStringAtPos(msg4, 1, 0);
	  RGBLED_SetValue(0, 255, 0);
	  for(i = 0; i <100000; i++);
	   RGBLED_SetValue(0, 0, 0);
	  choice = 1;  

    }
		
    }
     for(i = 0; i <10000; i++);
     SSD_WriteDigits(score,0,0,0,0,0,0,0);
}

/*Gives instructions for the fact game and then displays the facts in order*/
void factGameStart() {

	while ( BTN_GetValue('C') == 0) {
		int i;
		    for(i = 0; i <50000; i++);
		LCD_DisplayClear();
		LCD_WriteStringAtPos("Up-Yes, Down-No", 0, 0);
		LCD_WriteStringAtPos("Press C To Start", 1, 0); 
	}

	positiveFact("A flock of crows", "is called murder", 7);
	negativeFact("Polar bears can", "eat 68 penguins", "They can eat 86!", NULL, 6);
	positiveFact("Hippos sweat", "red when upset", 5);
	negativeFact("An eagle cannot", "carry a deer", "They can!", NULL, 4);
	positiveFact("Cherophobia is", "fear of fun", 3);
	negativeFact("A shrimp's heart", "is in its foot", "It's in their", "heads!", 2);
	positiveFact("Slugs have", "four noses", 1);
	negativeFact("Rabbits sweat", "5 liters daily", "Rabbits can't", "produce sweat!", 0);
  
		LCD_DisplayClear();
		int i;
		for(i = 0; i <10000; i++);
	    	LCD_WriteStringAtPos("Thank you for", 0, 0);
		LCD_WriteStringAtPos("playing!", 1, 0);	
}

/*Calls the appropriate function based upon the kind
 of game the user chose to play*/
void startGame(int gameChoice) {
	if (gameChoice == 1) {
		patternGameStart();
    }
	 if (gameChoice == 2) {
		 factGameStart();
	 }
}

void WriteString(const char *chr){
  while (*chr != NULL)
    {
      while (!UARTTransmitterIsReady(UART1))
        ;
      //UARTSendDataByte(UART1, *chr);
      UARTJB_PutChar(*chr);
      chr++;
      while (!UARTTransmissionHasCompleted(UART1))
        ;
    }
}
void PutCharacter(const char character){
  while (!UARTTransmitterIsReady(UART1))
    ;
 
  //UARTSendDataByte(UART1, character);
    UARTJB_PutChar(character);
  
  while (!UARTTransmissionHasCompleted(UART1))
    ;
}
void __ISR(_UART1_VECTOR, IPL2SOFT) IntUart1Handler(void){
  // Is this an RX interrupt?
  if (INTGetFlag(INT_SOURCE_UART_RX(UART1)))
    {
      // Echo what we just received.
      c[count++] = UARTGetDataByte(UART1);
 
      // Clear the RX interrupt Flag
      
      INTClearFlag(INT_SOURCE_UART_RX(UART1));
    }
 
}
void esp8266_mode(unsigned char mode) {
    WriteString("AT+CWMODE=");
    PutCharacter(mode + '0');
    WriteString("\r\n");
}
void esp8266_start(){
 WriteString("AT\r\n");

}
void esp8266_connect(unsigned char* ssid, unsigned char* pass) {
    PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
  WriteString("CWJAP=\"");
  WriteString(ssid);
  WriteString("\",\"");
  WriteString(pass);
  WriteString("\"\r\n");
}
void esp8266_enable_MUX() {
    PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
 WriteString("CIPMUX=1\r\n"); //Enable Multiple Connections
 }
void esp8266_create_server() {
    WriteString("AT+CIPSERVER=1,80\r\n"); //Enable Server on port 80
 }
void esp8266_connect_SMPT2GO(){
    int z;
    PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
    WriteString("CIPSTART=4,\"TCP\",\"mail.smtp2go.com\",2525\r\n");
      z = 30000000;
      while(z--);
      PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
    WriteString("CIPSEND=4,20\r\n");
       z = 100000;
      while(z--);
    WriteString("EHLO 192.168.1.123\r\n");
    z = 3000000;
      while(z--);
      PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
    WriteString("CIPSEND=4,12\r\n");
    z = 100000;
      while(z--);
}
void esp8266_login_mail(unsigned char* mail_ID, unsigned char* mail_Pas) {
    int z; 
    
       WriteString("AUTH LOGIN\r\n");
    z = 10000000;
      while(z--);
      
      int len = strlen(mail_ID);
     len+= 2;
     char l2 = len%10;
     char l1 = (len/10)%10;
   PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
    WriteString("CIPSEND=4,");
     if ((l1+'0')>'0')
     PutCharacter(l1+'0');
     PutCharacter(l2+'0');
    WriteString("\r\n");
     z = 100000;
      while(z--);
    
    WriteString(mail_ID);
    WriteString("\r\n");
     z = 10000000;
      while(z--);
    
    len = strlen(mail_Pas);
    len+= 2;
    l2 = len%10;
    l1 = (len/10)%10;
     PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
    WriteString("CIPSEND=4,");
    if ((l1+'0')>'0')
    PutCharacter(l1+'0');
    PutCharacter(l2+'0');
    WriteString("\r\n");
     z = 100000;
      while(z--);
    
    
   WriteString(mail_Pas);
    WriteString("\r\n");
     z = 10000000;
      while(z--);
}
void esp8266_start_mail()  {
        int z; 

    PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
    WriteString("CIPSEND=4,6\r\n");
    z = 100000;
      while(z--);
     WriteString("DATA\r\n");
    z = 10000000;
      while(z--);    
      
 }
void esp8266_mail_sendID(unsigned char* send_ID)  {
        int z; 
       
    int len = strlen(send_ID);
     len+= 14;
     char l2 = len%10;
     char l1 = (len/10)%10;
     
     PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
     WriteString("CIPSEND=4,");
     if ((l1+'0')>'0')
     PutCharacter(l1+'0');
     PutCharacter(l2+'0');
    WriteString("\r\n");
    z = 100000;
      while(z--);
    
    WriteString("MAIL FROM:<");
    WriteString(send_ID);
    WriteString(">\r\n");
    z = 10000000;
      while(z--);   
 } 
void esp8266_mail_recID(unsigned char* rec_ID)  {
     int z,len;
     len = strlen(rec_ID);
     len+= 12;
     char l2 = len%10;
     char l1 = (len/10)%10;
     
     PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
     WriteString("CIPSEND=4,");
     if ((l1+'0')>'0')
     PutCharacter(l1+'0');
     PutCharacter(l2+'0');
    WriteString("\r\n");
    z = 100000;
      while(z--);
    
    WriteString("RCPT To:<");
    WriteString(rec_ID);
    WriteString(">\r\n");
    z = 10000000;
      while(z--);
 } 
void esp8266_mail_subject(unsigned char* subject)  {
     int len,z;
     len = strlen(subject);
     len+= 10;
     char l2 = len%10;
     char l1 = (len/10)%10;
     PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
     WriteString("CIPSEND=4,");
     if ((l1+'0')>'0')
     PutCharacter(l1+'0');
     PutCharacter(l2+'0');
    WriteString("\r\n");
    z = 100000;
      while(z--);
    
    WriteString("Subject:");
    WriteString(subject);
    WriteString("\r\n");
    z = 10000000;
      while(z--);   
 } 
void esp8266_mail_body(unsigned char* body)  {
     int z,len;
     len = strlen(body);
     len+= 2;
     char l2 = len%10;
     char l1 = (len/10)%10;
     PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
     WriteString("CIPSEND=4,");
     if ((l1+'0')>'0')
     PutCharacter(l1+'0');
     PutCharacter(l2+'0');
    WriteString("\r\n");
    z = 100000;
      while(z--);
    
    WriteString(body);
    WriteString("\r\n");
    z = 10000000;
      while(z--);   
 } 
void esp8266_End_mail()  {
     int z;
     PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
    WriteString("CIPSEND=4,3\r\n");
    z = 100000;
      while(z--);
    WriteString(".\r\n");
    z = 10000000;
      while(z--);
 }
void esp8266_disconnect_server() {
     int z;
     PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
    WriteString("CIPSEND=4,6\r\n");
    z = 100000;
      while(z--);
    WriteString("QUIT\r\n");
    z = 10000000;
      while(z--);
 }
void esp8266_disconnect(void) {
    PutCharacter('A');
    PutCharacter('T');
    PutCharacter('+');
    WriteString("CWQAP\r\n");
 } 
 
void initialize_ESP8266(){
    int z;
    
  esp8266_start();
  z = 100000;
  while(z--);
  
  esp8266_mode(3);
  
  z = 100000;
  while(z--);
  
  esp8266_connect("Sweet Home","home2013");  
  //esp8266_connect("Mitcho","hibyebye");  
  //esp8266_connect("Mohamed's Note 5","himohamed");  
  z = 1000000;
  while(z--);
  
  esp8266_enable_MUX();
  z = 1000000;
  while(z--);
  
  esp8266_create_server();
  z = 1000000;
  while(z--);
}
void send_email_ESP8266(){
  esp8266_connect_SMPT2GO();
  esp8266_login_mail("bW9oYW1lZC1taW5hd2lAYXVjZWd5cHQuZWR1","bWljcm9jb250cm9sbGVycw==");
  esp8266_mail_sendID("mohamed-minawi@aucegypt.edu"); //The sender mail ID
  esp8266_mail_recID("mohamedsherif96@hotmail.com"); //The Receiver mail ID
  esp8266_start_mail();
  esp8266_mail_subject("Highscore2"); //Enter the subject of your mail
  esp8266_mail_body("Welcome to the Game"); //Enter the body of your mail       
  esp8266_End_mail();
  esp8266_disconnect_server();
  esp8266_disconnect();
    
}

int main(int argc, char** argv) 
{
//    Initialize needed peripherals (Buttons, LCD, LEDs, Seven segment display, RGB LED)
    BTN_Init();	
    LCD_Init();
    LED_Init();
    SSD_Init();
    RGBLED_Init();
    
//   // Prepare displays
    SSD_WriteDigits(0,0,0,0,0,0,0,0);
    LCD_DisplayClear();
    
// Start appropriate game based on user selection
    startGame(chooseGame());

    
      // UART 1 Configuration
  UARTConfigure(UART1, UART_ENABLE_PINS_TX_RX_ONLY);
  UARTSetFifoMode(UART1, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
  UARTSetLineControl(UART1, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
  int actual = UARTSetDataRate(UART1, GetPeripheralClock(), DESIRED_BAUDRATE);
  UARTEnable(UART1, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
 
  // Configure UART1 RX Interrupt
  INTEnable(INT_SOURCE_UART_RX(UART1), INT_ENABLED);
  INTSetVectorPriority(INT_VECTOR_UART(UART1), INT_PRIORITY_LEVEL_2);
  INTSetVectorSubPriority(INT_VECTOR_UART(UART1), INT_SUB_PRIORITY_LEVEL_0);
  
  // configure for multi-vectored mode
  INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
 
  // enable interrupts
  INTEnableInterrupts();
  
  // UART Mapping
  RPD11R = 3;
  U1RXR = 3;
  
  // Stop RGB Interrupts
  IEC0bits.T5IE = 0;
  T5CONbits.ON = 0; 
  
  initialize_ESP8266();
  send_email_ESP8266();
  // Let interrupt handler do the work
 // while (1);
    return (1);
}

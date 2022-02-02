#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <rangeLib.h>


/**
 * main.c
 */

#define COMMAND_LENGTH 100
char command[COMMAND_LENGTH];

int dst_int;
int dst_int1;
int dst_int2;
int dst_flt;
int dst_flt1;
int dst_flt2;

float tmp_flt;
float tmp_flt1;
float tmp_flt2;
float volts;
float volts1;
float volts2;
char dst_char[5];
char dst_char1[5];
char dst_char2[5];
char dst_flt_char[5];
char dst_flt_char1[5];
char dst_flt_char2[5];

char vt_chara0[5];
char vt_chara3[5];
char vt_chara4[5];

char volta0[] = "  A0: ";
char volta1[] = "  A1: ";
char volta2[] = "  A2: ";
char bars[] = " bars. ";
char dot[] = ".";
char zerro[] = "0";
char newline[] = " \r\n";


unsigned int adc[8];

volatile uint8_t counter;
volatile uint8_t k;

void mcu_init();
void uart_init();
void uart_write(char* str);
void uart_writen(char* data, int n);
void uart_writec(char data);

int main(void)
{

    mcu_init();
    uart_init();

    P1DIR |= 0x01;
    P1OUT &= 0x00;


    while(1)
    {
    ADC10CTL1 = INCH_7 + ADC10DIV_0 + CONSEQ_3 + SHS_0;
    ADC10CTL0 = SREF_0 + ADC10SHT_2 + MSC + ADC10ON; //ADC10IE
    ADC10AE0 = BIT7 + BIT6 + BIT5 + BIT4 + BIT3 + BIT0;
    ADC10DTC1 = 8;


    ADC10CTL0 &= ~ENC;
    while (ADC10CTL1 & BUSY);
    ADC10CTL0 |= ENC + ADC10SC;
    ADC10SA = (unsigned int)adc;

    int A0 = adc[7];
    int A1 = adc[4];
    int A2 = adc[3];

    float valA0 = path(A0, 0, 1023, 0.343, 70);
    float valA1 = path(A1, 0, 1023, 0.343, 70);
    float valA2 = path(A2, 0, 1023, 0.343, 70);

    //volts = (3*((A0*3.3)/1023)- 0.475)*10;
    dst_int = floor(valA0);
    tmp_flt = valA0 - dst_int;
    ltoa(dst_int, dst_char,10);

    //volts1 = (3*((A1*3.3)/1023)- 0.475)*10;
    dst_int1 = floor(valA1);
    tmp_flt1 = valA1 - dst_int1;
    ltoa(dst_int1, dst_char1,10);

    //volts2 = (3*((A2*3.3)/1023)- 0.475)*10;
    dst_int2 = floor(valA2);
    tmp_flt2 = valA2 - dst_int2;
    ltoa(dst_int2, dst_char2,10);


     if (tmp_flt < 0.01) {
        dst_flt = floor(tmp_flt * 1000);
        ltoa(dst_flt,dst_flt_char,10);
        //ser_output(volta0); ser_output(dst_char); ser_output(dot); ser_output(zerro); ser_output(dst_flt_char); ser_output(bars);
        }
        else if (tmp_flt < 0.1) {
            dst_flt = floor(tmp_flt * 100);
            ltoa(dst_flt,dst_flt_char,10);
            //ser_output(volta0); ser_output(dst_char); ser_output(dot); ser_output(zerro); ser_output(dst_flt_char); ser_output(bars);
        }
        else {
            dst_flt = floor(tmp_flt * 100);
            ltoa(dst_flt,dst_flt_char,10);
            //ser_output(volta0); ser_output(dst_char); ser_output(dot); ser_output(dst_flt_char); ser_output(bars);
        }

        if (tmp_flt1 < 0.01) {
            dst_flt1 = floor(tmp_flt1 * 1000);
            ltoa(dst_flt1,dst_flt_char1,10);
           // ser_output(volta1); ser_output(dst_char1); ser_output(dot); ser_output(zerro); ser_output(dst_flt_char1); ser_output(bars);
            }
        else if (tmp_flt1 < 0.1) {
                dst_flt1 = floor(tmp_flt1 * 100);
                ltoa(dst_flt1,dst_flt_char1,10);
              //  ser_output(volta1); ser_output(dst_char1); ser_output(dot); ser_output(zerro); ser_output(dst_flt_char1); ser_output(bars);
            }
        else {
                dst_flt1 = floor(tmp_flt1 * 100);
                ltoa(dst_flt1,dst_flt_char1,10);
               // ser_output(volta1); ser_output(dst_char1); ser_output(dot); ser_output(dst_flt_char1); ser_output(bars);
            }

        if (tmp_flt2 < 0.01) {
            dst_flt2 = floor(tmp_flt2 * 1000);
            ltoa(dst_flt2,dst_flt_char2,10);
            //ser_output(volta2); ser_output(dst_char2); ser_output(dot); ser_output(zerro); ser_output(dst_flt_char2); ser_output(bars);
            }
        else if (tmp_flt2 < 0.1) {
                dst_flt2 = floor(tmp_flt2 * 100);
                ltoa(dst_flt2,dst_flt_char2,10);
               // ser_output(volta2); ser_output(dst_char2); ser_output(dot); ser_output(zerro); ser_output(dst_flt_char2); ser_output(bars);
            }
        else {
                dst_flt2 = floor(tmp_flt2 * 100);
                ltoa(dst_flt2,dst_flt_char2,10);
                //ser_output(volta2); ser_output(dst_char2); ser_output(dot); ser_output(dst_flt_char2); ser_output(bars);
            }

        __delay_cycles(100000);

    }


}

void mcu_init() {

    WDTCTL = WDTPW +WDTHOLD;                  // Stop Watchdog Timer

    if (CALBC1_16MHZ==0xFF)                   // If calibration constant erased
    {
      while(1);                               // do not load, trap CPU!!
    }

    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;                   // Set range
    DCOCTL = CALDCO_16MHZ;                    // Set DCO step + modulation*/
}

void uart_init() {

    memset(command,'\0',COMMAND_LENGTH);// Reset command vector
    counter=0; // restart counter for a new command

    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD

    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 0x82;                            // 16MHz 9600
    UCA0BR1 = 0x06;                              // 16MHz 9600
    UCA0MCTL = UCBRS_6;                        // Modulation UCBRSx = 6
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

    //__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
    __enable_interrupt();
}

void uart_write(char* str) {
  int i;
  for(i = 0; str[i] != '\0'; i++) {
    while (!(IFG2 & UCA0TXIFG));    // TX buffer ready?
    UCA0TXBUF = str[i];
  }
}

void uart_writen(char* data, int n) {
  while(n--) {
    while (!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = *data++;
  }
}

void uart_writec(char data) {
  while (!(IFG2 & UCA0TXIFG));
  UCA0TXBUF = data;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(UCA0RXBUF != '\n')
    {

        command[counter] = UCA0RXBUF;

        counter = counter +1;

    }
    else if(UCA0RXBUF == '\n')
    {

        if (strcmp(command,"PRX")==0){ // Example Constant command
            uart_write("PRX sending...\n");
            P1OUT ^= BIT0; //test
        }
        else if(strcmp(command,"PR1")==0)
        {

            uart_write(volta0);
            uart_write(dst_char);
            uart_write(dot);
            //uart_write(zerro);
            uart_write(dst_flt_char);
            uart_write(bars);
            uart_write("\n");
        }
        else if(strcmp(command,"PR2")==0)
        {
            uart_write(volta1);
            uart_write(dst_char1);
            uart_write(dot);
            //uart_write(zerro);
            uart_write(dst_flt_char1);
            uart_write(bars);
            uart_write("\n");
        }
        else if(strcmp(command,"PR3")==0)
        {
            uart_write(volta2);
            uart_write(dst_char2);
            uart_write(dot);
            uart_write(zerro);
            uart_write(dst_flt_char2);
            uart_write(bars);
            uart_write("\n");

        }
        else if(strcmp(command,"PRALL")==0)
        {
            uart_write(volta0);
            uart_write(dst_char);
            uart_write(dot);
            //uart_write(zerro);
            uart_write(dst_flt_char);
            uart_write(bars   );


            uart_write(volta1);
            uart_write(dst_char1);
            uart_write(dot);
            //uart_write(zerro);
            uart_write(dst_flt_char1);
            uart_write(bars   );

            uart_write(volta2);
            uart_write(dst_char2);
            uart_write(dot);
            uart_write(zerro);
            uart_write(dst_flt_char2);
            uart_write(bars);
            uart_write("\n");

        }
        else if((strncmp(command,"CAL",3)==0) && (command[6]=='.') && (command[9]=='$')) // Example of variable command CAL+XX.YY$
        {
            if(command[3]=='+')
            {
                //Check XX integers
                //Check YY integers
                //Convert to double
                //Assign values to global variables
            }
            else if(command[3]=='-')
            {
                //Check XX integers
                //Check YY integers
                //Convert to double
                //Assign values to global variables
            }
            else
            {
                uart_write("CAL Sign Error...\n");
            }
            uart_write("CAL sending...\n");
        }
        else
        {
            memset(command,'\0',COMMAND_LENGTH); // Reset command vector
            counter=0; // restart counter for a new command
            uart_write("NOT ACK\n");
        }

        memset(command,'\0',COMMAND_LENGTH);// Reset command vector
        counter=0; // restart counter for a new command

    }
/*
    while (!(IFG2 & UCA0TXIFG));              // USCI_A0 TX buffer ready?
    UCA0TXBUF = UCA0RXBUF;                    // TX -> RXed character
*/

}


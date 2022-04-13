//Last configuration on 13.04.2022

#include <msp430.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <rangeLib.h>
#include <libGlobal.h>


int main(void)
{
    mcu_init();
    uart_init();
   
    P1DIR   |= 0x01;
    P1OUT   &= 0x01;

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


    if(senOneFlag == true)
    {
        for(int i = 0; i < count; i++)
        {
            int A0 = adc[7];
            __delay_cycles(25);
            raw += A0;
        }
        res = raw / count;
        pressure = res-offset;
        
        raw = 0;

        senOneFlag = false;
        
        if(unitFlag == 0)
        {
            float range = path(pressure, 171, 1036, 34.3, 200.);
            output = range;    
            unitAll = " kPa"; 
        }
        else if(unitFlag == 1)
        {
            float range = path(pressure, 171 , 1036, 0.343, 2.);
            output = range;
            unitAll = " bar";
        }
        else if (unitFlag == 2)
        {
            float range = path(pressure, 171 , 1036, 0.257, 1.5);
            output = range;
            unitAll = " kTorr";
        }
        else 
        {
            uart_write("No unit declared !");
        }


        char* sum_o = value_percent(output);

        //uart_write(volta0);
        uart_write(sum_o);
        uart_write(unitAll);
        uart_write("\n");

    } 

    if(senTwoFlag == true)
    {
        for(int i = 0; i < count; i++)
        {
            int A1 = adc[4];
            raw += A1;
        }
        res = raw / count;
        pressure = res-offset;
        
        raw = 0;

        senTwoFlag = false;

       if(unitFlag == 0)
        {
            float range = path(pressure, 171, 1036, 34.3, 200.);
            output = range;    
            unitAll = " kPa";
        }
        else if(unitFlag == 1)
        {
            float range = path(pressure, 171 , 1036, 0.343, 2.);
            output = range;
            unitAll = " bar";
        }
        else if (unitFlag == 2)
        {
            float range = path(pressure, 171 , 1036, 0.257, 1.5);
            output = range;
            unitAll = " kTorr";
        }
        else 
        {
            uart_write("No unit declared !");
        }

        char* sum_o = value_percent(output);

        //uart_write(volta1);
        uart_write(sum_o);
        uart_write(unitAll);
        uart_write("\n");
    }

    if(senThreeFlag == true)
    {
        for(int i = 0; i < count; i++)
        {
            int A2 = adc[3];
            raw += A2;
        }
        res = raw / count;
        pressure = res-offset;
        
        raw = 0;

        senThreeFlag = false;

        if(unitFlag == 0)
        {
            float range = path(pressure, 171, 1036, 34.3, 200.);
            output = range;    
            unitAll = " kPa";
        }
        else if(unitFlag == 1)
        {
            float range = path(pressure, 171 , 1036, 0.343, 2.);
            output = range;
            unitAll = " bar";
        }
        else if (unitFlag == 2)
        {
            float range = path(pressure, 171 , 1036, 0.257, 1.5);
            output = range;
            unitAll = " kTorr";
        }
        else 
        {
            uart_write("No unit declared !");
        }

        char* sum_o = value_percent(output);

        //uart_write(volta2);
        uart_write(sum_o);
        uart_write(unitAll);
        uart_write("\n");
    }
       
    if(rstOneFlag == true)
    {
        if( adc[7] != 0)
        {
            offset = adc[7];
        }
        else
        {
            offset = 0;
        }

        uart_write(rst);
        rstOneFlag = false;
    }

    if(rstTwoFlag == true)
    {
        if( adc[4] != 0)
        {
            offset = adc[4];
        }
        else
        {
            offset = 0;
        }

        uart_write(rst);
        rstTwoFlag = false;
    }

    if(rstThreeFlag == true)
    {
        if( adc[3] != 0)
        {
            offset = adc[3];
        }
        else
        {
            offset = 0;
        }

        uart_write(rst);
        rstThreeFlag = false;
    }

    }

}

void mcu_init() {

    WDTCTL = WDTPW +WDTHOLD;                  // Stop Watchdog Timer

    if (CALBC1_8MHZ==0xFF)                   // If calibration constant erased
    {
      while(1);                               // do not load, trap CPU!!
    }

    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                   // Set range
    DCOCTL = CALDCO_1MHZ;                    // Set DCO step + modulation*/
}

void uart_init() {

    memset(command,'\0',COMMAND_LENGTH);// Reset command vector
    counter=0; // restart counter for a new command

    unitFlag = 0;
    
    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD

    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 54;                            // 1MHz 19200
    UCA0BR1 = 0;                              // 1MHz 19200
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



char* value_percent(float a)
{
        char dst_char[7];
        char dst_flt_char[7];
    

        int dst_int = floor(a);
        float tmp_flt = a - dst_int;
        ltoa(dst_int, dst_char,10);

        if (tmp_flt < 0.01) 
        {
            int dst_flt = floor(tmp_flt * 10000);
            ltoa(dst_flt,dst_flt_char,10);
        }
        else if (tmp_flt < 0.1) 
        {
            int dst_flt = floor(tmp_flt * 1000);
            ltoa(dst_flt,dst_flt_char,10);
        }
        else 
        {
            int dst_flt = floor(tmp_flt * 1000);
            ltoa(dst_flt,dst_flt_char,10);             
        }

        strcat(dst_char, ".");
        char* allCh = dst_char; 
        strcat(allCh, dst_flt_char);
        char* sum = allCh;

        return sum;
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
            senOneFlag = true; 
        }
        else if(strcmp(command,"PR2")==0)
        {
            senTwoFlag = true;
        }
        else if(strcmp(command,"PR3")==0)
        {
            senThreeFlag = true;
        }
        else if(strcmp(command,"PRALL")==0)
        {
            uart_write(line);
            senOneFlag = true;
            senTwoFlag = true;
            senThreeFlag = true;
        }
        else if(strcmp(command,"RST1")==0)
        {
            rstOneFlag = true;
        }
        else if(strcmp(command,"RST2")==0)
        {
            rstTwoFlag = true;
        }
        else if(strcmp(command,"RST3")==0)
        {
            rstThreeFlag = true;
        }
        else if(strncmp(command,"UNIT",4) ==0)
        {
           if(command[4]=='P')
           {
                unitFlag = 0;
                uart_write("Unit pascal slected...");
                uart_write("\n");
           }
           else if(command[4]=='B')
           {
                unitFlag = 1;
                uart_write("Unit bar selected...");
                uart_write("\n");
           }
           else if(command[4]=='T')
           {
                unitFlag  = 2;
                uart_write("Unit tor selected...");
                uart_write("\n");
           }
           else
           {
               uart_write("error!");
               uart_write("\n");
           }
        }
        else
        {
            memset(command,'\0',COMMAND_LENGTH); // Reset command vector
            counter=0; // restart counter for a new command
            uart_write("-\n");
        }

        memset(command,'\0',COMMAND_LENGTH);// Reset command vector
        counter=0; // restart counter for a new command

    }

}

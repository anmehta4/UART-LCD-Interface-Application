/*
 * serial_debug.c
 *
 *      Author: Arnav Mehta
 */

#include "serial_debug.h"
#include "msp.h"

Circular_Buffer *Tx_Buffer;
volatile uint16_t Rx_Char_Count = 0;
volatile bool ALERT_STRING = 0;
volatile char Rx_String[80];


/*******************************************************************
 * Initializes the circular buffers used to transmit and receive data
 * from the serial debug interface. It will also initialize the UART
 * interface to enable interrupts.
 *******************************************************************/
void serial_debug_init_uart(void) {

    // Configure the IO pins used for the UART
    // Set 2-UART pins as secondary function
    P1->SEL0 |= BIT2 | BIT3;
    P1->SEL1 &= ~(BIT2 | BIT3);

    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; //Put EUSCI on reset
    //Remain EUSCI in reset and configure EUSCI clock source
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | EUSCI_B_CTLW0_SSEL__SMCLK;

    // Baud Rate calculation
    // 24000000/(16*115200) = 13.020833333
    // Fractional portion = 0.020833333
    // UCBRFx = int ((13.020833333 - 13)*16) = 0
    EUSCI_A0->BRW = 13;

    // Set the fractional portion of the baud rate and turn on oversampling
    EUSCI_A0->MCTLW = (0 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;

    // Enable EUSCI in UART mode
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;

    //Clear outstanding interrupts
    EUSCI_A0->IFG &= ~(EUSCI_A_IFG_RXIFG | EUSCI_A_IFG_TXIFG);

    //Enable Rx Interrupt
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;

    //Enable EUSCIA0 Interrupt
    NVIC_EnableIRQ(EUSCIA0_IRQn);

    // Prime the pump -- ADD CODE
    EUSCI_A0->TXBUF = 0;
}

/*******************************************************************
 * Initializes the EUSCI_A0 peripheral to be a UART with a baufd rate of
 * 115200.
 * Note: This function assumes that the SMCLK has been configured to run
 * at a clock cycle of 24MHz. Please change the __SYSTEM_CLOCK in system_msp432p401r.c
 * to the correct value of 24000000
 *******************************************************************/
void serial_debug_init(void)
{
    // Initialize the UART
    serial_debug_init_uart();

    // ADD CODE that initializes a circular buffer of size 80 and sets it
    // equal to Tx_Buffer
    Tx_Buffer = circular_buffer_init(80);

}

/*******************************************************************
 * Prints a string to the serial debug UART
 *******************************************************************/
void serial_debug_put_string(char* s) {

    while(*s != 0) {
        while(EUSCI_A0->STATW & EUSCI_A_STATW_BUSY) {};
        EUSCI_A0->TXBUF = *s;
        //Advance to the next character
        s++;
    }
}

//****************************************************************************
// This function is called from MicroLIB's stdio library.  By implementing
// this function, MicroLIB's putchar(), puts(), printf(), etc will now work.
// ****************************************************************************/
int fputc(int c, FILE* stream)
{
    // Busy wait while the circular buffer is full -- ADD CODE
    while(circular_buffer_full(Tx_Buffer)) {
        //do nothing
    }

    // globally disable interrupts
    __disable_irq();

    // add the character to the circular buffer  -- ADD CODE
    circular_buffer_add(Tx_Buffer, c);

    // globally enable interrupts
    __enable_irq();

    // Enable Tx Empty Interrupts  -- ADD CODE
    EUSCI_A0->IE |= EUSCI_A_IE_TXIE;

    return 0;
}


//****************************************************************************
// UART interrupt service routine
// ****************************************************************************/
void EUSCIA0_IRQHandler(void)
{
    char c;

    // Check for Rx interrupts
    if(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) {
        //Reading from the RXBUF automatically
        //Clears the Rx interrupt

        Rx_String[Rx_Char_Count] = EUSCI_A0->RXBUF;
        EUSCI_A0->TXBUF = Rx_String[Rx_Char_Count];
        if(Rx_String[Rx_Char_Count] ==0x0A || Rx_String[Rx_Char_Count] ==0x0D) {
            ALERT_STRING = 1;
        }
        Rx_Char_Count++;
    }

    // Check for Tx Interrupts
    if (EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)
    {
        //Check to see if the Tx circular buffer is empty
        if(circular_buffer_empty(Tx_Buffer))
        {
            // Disable Tx Interrupts -- ADD CODE
            EUSCI_A0->IE &= ~EUSCI_A_IE_TXIE;
        }
        else
        {
            // Get the next char from the circular buffer -- ADD CODE
            c = circular_buffer_remove(Tx_Buffer);

            // Transmit the character -- ADD CODE
            EUSCI_A0->TXBUF = c;
        }
    }
}





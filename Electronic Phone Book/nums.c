/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
� Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 1/27/2018
Author  : 
Company : 
Comments: 


Chip type               : ATmega32A
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

#include <mega32a.h>
#include <alcd.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>

#define ISDATAREADY (UCSRA)&(1<<RXC)

int alpha[10][5] = {
    {' ','0',0x00,0x00,0x00},
    {'.',',','?','!','1'},
    {'a','b','c','2',0x00},
    {'d','e','f','3',0x00},
    {'g','h','i','4',0x00},
    {'j','k','l','5',0x00},
    {'m','n','o','6',0x00},
    {'p','q','r','s','7'},
    {'t','u','v','8',0x00},
    {'w','x','y','z','9'},
};

char db[32][27];
int used = 0;
eeprom char saved_db[32][32]; //1KB EEPORM
char str[40];
   
char get_key(){
    PORTB=0xFF;
    PORTB.4 = 0;
    delay_ms(10);
    if ( !PINB.0 ) return 'D';
    if ( !PINB.1 ) return '#';
    if ( !PINB.2 ) return '0';
    if ( !PINB.3 ) return '*';
    PORTB.4 = 1;
    PORTB.5 = 0;
    delay_ms(10);
    if ( !PINB.0 ) return 'C';
    if ( !PINB.1 ) return '9';
    if ( !PINB.2 ) return '8';
    if ( !PINB.3 ) return '7';
    PORTB.5 = 1;
    PORTB.6 = 0;
    delay_ms(10);
    if ( !PINB.0 ) return 'B';
    if ( !PINB.1 ) return '6';
    if ( !PINB.2 ) return '5';
    if ( !PINB.3 ) return '4';
    PORTB.6 = 1;
    PORTB.7 = 0;
    delay_ms(10);
    if ( !PINB.0 ) return 'A';
    if ( !PINB.1 ) return '3';
    if ( !PINB.2 ) return '2';
    if ( !PINB.3 ) return '1';
    PORTB.7 = 1;
    return 0;
}

int write_alpha ( char name[17] ){
    int i, nowat=0, index=0;
    char ch, tch;
    while (1){
        ch = get_key();
        if ( ch == 'D' && index > 0 )
            return 1;
        if ( ch == 'D' && index == 0 )
            return 0;
        if ( ch == '*' && index > 0 ){
            lcd_gotoxy(index,1);
            lcd_putchar(' ');
            name[index]=' ';
            index--;
            while ( get_key() != 0x00 );
        }
        if ( ch >= '0' && ch <= '9' && index < 16 ){
            nowat=0;
            lcd_gotoxy(index,1);
            lcd_putchar(alpha[ch-'0'][nowat%5]);
            name[index]=alpha[ch-'0'][nowat%5];
            while ( get_key() != 0x00 );
            for ( i=0; i<30; i++ ){
                tch = get_key();
                if ( tch == 0x00 )
                    continue;
                else if ( tch != ch )
                    break;
                else if ( tch == ch ){
                    nowat++;
                    i=0;
                    while ( get_key() != 0x00 );
                }
                while ( alpha[ch-'0'][nowat%5] == 0x00 ) nowat++;
                lcd_gotoxy(index,1);
                lcd_putchar(alpha[ch-'0'][nowat%5]);
                name[index]=alpha[ch-'0'][nowat%5];
            }
            index++;
        }
        lcd_gotoxy(index,1);
        lcd_putchar('_');
        name[index]=0x00;
    }
    return 1;
}

int write_number ( char number[12] ){
    int index=0;
    char ch;
    while (1){
        ch = get_key();
        if ( ch == 'D' && index > 0 )
            return 1;
        if ( ch == 'D' && index == 0 )
            return 0;
        if ( ch == '*' && index > 0 ){
            lcd_gotoxy(index,1);
            lcd_putchar(' ');
            number[index]=' ';
            index--;
            while ( get_key() != 0x00 );
        }
        if ( ch >= '0' && ch <= '9' && index < 11 ){
            lcd_gotoxy(index,1);
            lcd_putchar(ch);
            number[index]=ch;
            index++;
            while ( get_key() != 0x00 );
        }
        lcd_gotoxy(index,1);
        lcd_putchar('_');
        number[index]=0x00;
    }
    return 1;
}

void save_to_eeprom(){
    int i,j;
    sprintf(str,"   Saving....   ");
    lcd_gotoxy(0,0);
    lcd_puts(str);
    lcd_gotoxy(9,1);
    lcd_putchar('%');
    for ( i=0; i<32; i++ )
        for ( j=0; j<27; j++ ){
            sprintf(str,"%3d",(((i*32)+j)*10)/101);
            lcd_gotoxy(6,1);
            lcd_puts(str);
            saved_db[i][j]=db[i][j];
        }
    delay_ms(1000);
    lcd_clear();
}

int load_from_eeprom(){
    int i,j;
    sprintf(str,"   Loading...   ");
    lcd_gotoxy(0,0);
    lcd_puts(str);
    lcd_gotoxy(9,1);
    lcd_putchar('%');
    for ( i=0; i<32; i++ ){
        for ( j=0; j<27; j++ ){
            sprintf(str,"%3d",(((i*32)+j)*10)/101);
            lcd_gotoxy(6,1);
            lcd_puts(str);
            if ( saved_db[i][j] == 0x00 || saved_db[i][j] == 0xFF )
                db[i][j]=0x00;
            else 
                db[i][j]=saved_db[i][j];
        }
        if ( db[i][0] == 0x00 ){
            lcd_clear();
            return i;
        }
    }
    delay_ms(1000);
    lcd_clear();
    return 32;
}

int view_contact( int index ){
    char curr_name[17];
    char curr_num[12];
    char ch;
    int i;
    if ( used < 1 ) return -1;
    if ( index >= used )
        index = used-1;
    curr_name[16] = 0x00;
    curr_num[11] = 0x00;
    lcd_clear();
    while (1){
        ch = get_key();
        if ( ch == 0x00 ) continue;
        if ( ch == 'D' )
            return index;
        if ( ch == 'A' ){
            while ( get_key() != 0x00 );
            return -1;
        }
        else if ( ch == '*' && index > 0 )
            index--;
        else if ( ch == '#' && index < used-1 )
            index++;
        else if ( ch == '*' && index == 0 )
            index=used-1;
        else if ( ch == '#' && index == used-1 )
            index=0;
        for ( i=0; i<11; i++ )
            curr_num[i] = db[index][i];
        for ( i=0; i<16; i++ )
            curr_name[i] = db[index][i+11];
        lcd_clear();
        sprintf(str,"%s",curr_name);
        lcd_gotoxy(0,0);
        lcd_puts(str);
        sprintf(str,"%16s",curr_num);
        lcd_gotoxy(0,1);
        lcd_puts(str);
        while ( get_key() != 0x00 );  
    }
    return -1;
}

void add_contact(){
    int i;
    char curr_name[17];
    char curr_num[12];
    if ( used > 31 ) return;
    lcd_clear();
    sprintf(str,"  Enter  name:  ");
    lcd_gotoxy(0,0);
    lcd_puts(str);
    if ( !write_alpha(curr_name) )
        return;
    while( get_key() != 0x00 );
    lcd_clear();
    sprintf(str," Enter  number: ");
    lcd_gotoxy(0,0);
    lcd_puts(str);
    if ( !write_number(curr_num) )
        return;
    while( get_key() != 0x00 );
    lcd_clear();
    for ( i=0; i<11; i++ )
        db[used][i] = curr_num[i];
    for ( i=0; i<16; i++ )
        db[used][i+11] = curr_name[i];
    used++;
    save_to_eeprom();
}

void delete_contact(){
    int i, j, to_delete;
    if ( used < 1 ) return;
    sprintf(str,"Choose to delete");
    lcd_gotoxy(0,0);
    lcd_puts(str);
    to_delete = view_contact(0);
    if ( to_delete < 0 )
        return;
    for ( i=0; i<27; i++ )
        db[to_delete][i] = 0x00;
    for ( i=to_delete; i<used-1; i++ )
        for ( j=0; j<27; j++ )
            db[i][j]=db[i+1][j];
    for ( i=0; i<27; i++ )
        db[used-1][i]= 0x00;
    lcd_clear();
    used--;
    save_to_eeprom();
}

void send_report(){
    int i, j;
    char curr_name[17];
    char curr_num[12];
    printf("Name,Phone\n");
    for ( j=0; j<used; j++ ){
        for ( i=0; i<11; i++ )
            curr_num[i] = db[j][i];
        for ( i=0; i<16; i++ )
            curr_name[i] = db[j][i+11];
        curr_num[11]=0x00;
        sprintf(str,"%s,%s",curr_name,curr_num);
        puts(str);
    }
}

void main(void){
// Declare your local variables here
char ch;
// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
DDRC=(1<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (1<<DDC1) | (1<<DDC0);
// State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=Out Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (1<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=0 Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0<<AS2;
TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 9600
UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
UBRRH=0x00;
UBRRL=0x33;

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
SFIOR=(0<<ACME);

// ADC initialization
// ADC disabled
ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

// SPI initialization
// SPI disabled
SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTC Bit 0
// RD - PORTC Bit 1
// EN - PORTC Bit 2
// D4 - PORTC Bit 4
// D5 - PORTC Bit 5
// D6 - PORTC Bit 6
// D7 - PORTC Bit 7
// Characters/line: 16
lcd_init(16);
    
    used=load_from_eeprom();
    
    while (1){
        lcd_gotoxy(0,0);
        lcd_puts("   Phone Book   ");
        lcd_gotoxy(0,1);
        lcd_puts("Shahidi  Adibnia");
        ch = get_key();
        if ( ch == 'A' )
            add_contact();
        else if ( ch == 'B' )
            view_contact(0);
        else if ( ch == 'C' )
            delete_contact();
        if ( ISDATAREADY ){
            scanf("%s",str);
            if ( !strcmp(str,"Report") )
                send_report();
            else
                printf("Unknown Command!\n");
        }
    }
    
}

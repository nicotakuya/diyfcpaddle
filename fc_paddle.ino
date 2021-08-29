// famicom paddle controller by takuya matsubara
// arduino(atmega328  5V  16MHz)

// connnector
// +--------------------------------+
// | (8) (7) (6) (5) (4) (3) (2) (1)|
//  |                              |
//   |(15)(14)(13)(12)(11)(10)( 9)|
//   +----------------------------+

// (1)GND : ground
// (7)P2_DAT:paddle data
// (9)P2_CLK:paddle clock
// (12)P/S : latch (low=latch / high=load)
// (13)P1_DAT :button data(low=on)
// (14)P1_CLK :button clock (not use)
// (15)VCC : 5V

// P/S
//   +--+
//   |  |  LATCH
// --+  +--------------------------------

// CLK
// ---------+--+--+--+--+--+--+--+-------
//          |  |  |  |  |  |  |  |    
//     

// P2DAT
//       +-----------------------+
//       |D7 D6 D5 D4 D3 D2 D1 D0|
// ------+                       +-------

//#define ADCLOCK  0 // clock 1/2 
//#define ADCLOCK  1 // clock 1/2 
#define ADCLOCK  2 // clock 1/4 
//#define ADCLOCK  3 // clock 1/8 
//#define ADCLOCK  4 // clock 1/16 
//#define ADCLOCK  5 // clock 1/32 
//#define ADCLOCK  6 // clock 1/64 
//#define ADCLOCK  7 // clock 1/128 

#define INVERTX 0 // INVERT

#define FC_PORT PORTC //
#define FC_PIN PINC   //
#define FC_DDR DDRC   //

#define P1CLK_BIT (1<<3)  // 
#define P1DAT_BIT (1<<2)  // 
#define P2CLK_BIT (1<<5)  // 
#define P2DAT_BIT (1<<4)  // 

#define BUTTON_DDR  DDRD   //
#define BUTTON_PORT PORTD  //
#define BUTTON_PIN PIND    //
#define BUTTON_BIT  (1<<2) //

#define LEDENABLE 1     // 
#define LED_BIT  (1<<0) // 
#define LED_PORT PORTB  // 
#define LED_DDR DDRB    // 

#define LATCH_BIT (1<<1)  //

//#define VALUE_MIN  98   // Arkanoid1
//#define VALUE_MAX  242  // Arkanoid1
//#define VALUE_MIN  80   // Arkanoid2
//#define VALUE_MAX  225  // Arkanoid2

volatile int adc0;  //  A/D count

//-------------------------------    
// ADC ISR 
ISR (ADC_vect) 
{
  int tempcnt;
  tempcnt = (int)ADCL; 
  tempcnt += ((int)ADCH) << 8;
  adc0 = tempcnt; // 10bit A/D count(0 - 1023)
}
     
//-------------------------------    
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  FC_DDR |= (P1DAT_BIT + P2DAT_BIT);    // FAMICOM <-- MULTITAP
  FC_PORT |= (P1DAT_BIT + P2DAT_BIT );  //OUTPUT
  FC_DDR &= ~(P1CLK_BIT + P2CLK_BIT);   // FAMICOM --> MULTITAP
  FC_PORT |= (P1CLK_BIT + P2CLK_BIT + LATCH_BIT);  // PULLUP

  BUTTON_DDR  &= ~BUTTON_BIT;
  BUTTON_PORT |= BUTTON_BIT;

#if LEDENABLE
  LED_DDR |= LED_BIT;
  LED_PORT &= ~LED_BIT;
#endif 

// AD init
// ADEN: ADC enable
// ADIF: ADC interrupt flag
// ADIE: ADC interrupt enable
  pinMode(A0, INPUT);
  ADCSRA = bit(ADEN) | bit(ADIE) | bit(ADIF) | ADCLOCK;
  ADCSRB = 0;
  ADMUX = bit(REFS0) | 0; //AD channel 0
  
  sei();
//  ADCSRA |= bit(ADSC);  //AD start
}

//-------------------------------    

void loop() {
  unsigned char bitcnt;
  unsigned char work;
  unsigned char senddata;
//  unsigned char loopcnt=0;// debug

   //無限ループ
  while(1){
    sei();
#if LEDENABLE
  LED_PORT |= LED_BIT;
#endif 
//    loopcnt = (loopcnt+1)% 60;// debug
//    if(loopcnt==0){                  // debug
//      Serial.print(senddata, HEX);   // debug
//      Serial.write('\n');            // debug
//    }                                // debug
    ADCSRA |= bit(ADSC);  //AD start
   delay(2);
    if((BUTTON_PIN & BUTTON_BIT)==0){
      FC_PORT &= ~P1DAT_BIT;  //ON
    }else{
      FC_PORT |= P1DAT_BIT;   //OFF
    }

#if INVERTX==0
    work = adc0 >> 2;  // convert 10bit --> 8bit
#else
    work = (1023 - adc0)>> 2;
#endif
//  バウスの上限／下限をチェック
//    if(work < VALUE_MIN) work = VALUE_MIN;  // min
//    if(work > VALUE_MAX) work = VALUE_MAX;  // max
    senddata = work;

    //--- アルカノイドIIでバウスがワープする現象の対策    
    if(senddata & 0x80){
      FC_PORT |= P2DAT_BIT;             // P2DAT=1
    }else{
      FC_PORT &= ~P2DAT_BIT;            // P2DAT=0
    }
#if LEDENABLE
  LED_PORT &= ~LED_BIT;
#endif 
    cli();

    while((FC_PIN & LATCH_BIT)==0);  //LOAD待ち(LATCH中)
    // while(digitalRead(PINPS)==LOW);

    while((FC_PIN & LATCH_BIT)!=0);  //LATCH待ち(LOADING)
    // while(digitalRead(PINPS)==HIGH);

    for(bitcnt=0; bitcnt<8; bitcnt++){
      while((FC_PIN & P2CLK_BIT)!=0);  // P2CLK=Low待ち
      while((FC_PIN & P2CLK_BIT)==0);  // P2CLK=High待ち
      senddata <<= 1;
      if(senddata & 0x80){
        FC_PORT |= P2DAT_BIT;             // P2DAT
      }else{
        FC_PORT &= ~P2DAT_BIT;            // P2DAT
      }
    }
//    FC_PORT |= P2DAT_BIT;             // P2DAT=high
    FC_PORT &= ~P2DAT_BIT;             // P2DAT=low
  }
}


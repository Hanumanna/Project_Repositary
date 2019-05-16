#include<stdint.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define SET_BIT(PORT,BIT) PORT|= (1<<BIT)
#define CLR_BIT(PORT,BIT) PORT&= ~(1<<BIT)
struct {
  
volatile unsigned int FLAG_ISR0:1;
  volatile unsigned int FLAG_ISR1:1;
}FLAG_BIT;
void output(int);
void output1(int);
int us_value;

typedef void (*funpointer)(void);
funpointer array_fp[3]={extint1,extint0};
int main()
  
{  
    SET_BIT(DDRD,PD7); //RED LED       
    SET_BIT(DDRD,PD6); //BUZZER
    SET_BIT(DDRD,PD5);//GREEN LED
    CLR_BIT(PORTD,PD7);        
    CLR_BIT(PORTD,PD6); 
    CLR_BIT(PORTD,PD5);
   SET_BIT(DDRB,PB0);  // blue led
    SET_BIT(DDRB,PB1);  //Red led
    SET_BIT(DDRB,PB2);  //Green led
   SET_BIT(DDRB,PB4); // motor
    SET_BIT(DDRB,PB3);  //Red led roll
    SET_BIT(DDRB,PB5); //red roll
  	array_fp[0]();
    array_fp[1]();
  while(1)
  {   
  if(FLAG_BIT.FLAG_ISR0==1)
  {
    
    if(FLAG_BIT.FLAG_ISR1==1){
      us_value=ultra_sonicread();
      output1(us_value);
      }
    else{
   lane_dep();
      roll_over();
        us_value=ultra_sonicread();
        output(us_value);
    }
   
    

  }
   else
   {
     CLR_BIT(PORTD,PD7);
     CLR_BIT(PORTD,PD6);
     CLR_BIT(PORTD,PD5);
     CLR_BIT(PORTB,PB0);
     CLR_BIT(PORTB,PB1);
     CLR_BIT(PORTB,PB2);
     CLR_BIT(PORTB,PB3);
     CLR_BIT(PORTB,PB4);
     CLR_BIT(PORTB,PB5);
   }
  
  }
  
}
void extint1()
{
  EICRA|=(1<<ISC10);
    EICRA&=~(1<<ISC11);
    EIMSK|=(1<<INT1);
  sei();
}
void extint0()
{
  EICRA|=(1<<ISC00);
    EICRA&=~(1<<ISC01);
    EIMSK|=(1<<INT0);
  sei();
}

ISR(INT1_vect)
            {
   			cli();
            FLAG_BIT.FLAG_ISR1=!FLAG_BIT.FLAG_ISR1;
            sei(); 
            }
ISR(INT0_vect)
            {
   			cli();
            FLAG_BIT.FLAG_ISR0=!FLAG_BIT.FLAG_ISR0;
            sei(); 
            }


void lane_dep()
{
 int adc1_value=adc1();
  
              
   if((adc1_value<300 || adc1_value>500))
  {
    
      SET_BIT(PORTD,PD7);
      SET_BIT(PORTD,PD6);
      CLR_BIT(PORTD,PD5);
   }
  else
  {
      CLR_BIT(PORTD,PD7);
      CLR_BIT(PORTD,PD6);
      SET_BIT(PORTD,PD5);
  }
}

void roll_over()
{
  int adc2_value=adc2();
    if(adc2_value>500)
    {
     SET_BIT(PORTB,PB3);
      int adc3_value=adc3();
    
    if(adc3_value>500)
    {
      SET_BIT(PORTB,PB5);
       SET_BIT(PORTB,PB4);
      _delay_ms(1000);
      CLR_BIT(PORTB,PB4);
    }
      else
      {
        CLR_BIT(PORTB,PB5);
      }
    }
  else
  {
        CLR_BIT(PORTB,PB3);
        CLR_BIT(PORTB,PB5);             
  }
}

int ultra_sonicread()
{
  
  SET_BIT(DDRD,PD4);
  SET_BIT(PORTD,PD4);
  _delay_ms(5);
  CLR_BIT(PORTD,PD4);
  CLR_BIT(DDRD,PD4);
  int time_duration = pulseIn(PD4,HIGH);
  float distance=0.343*time_duration*0.5*0.1;
  return(distance);
}

void output(int us_value)
{
 if(us_value<=40)
       {
           CLR_BIT(PORTB,PB2); //GREEN
           SET_BIT(PORTB,PB1); //RED
           CLR_BIT(PORTB,PB0); //BLUE
           SET_BIT(PORTB,PB4); // MOTOR
       }
       else if(us_value>40 && us_value<=60) //check if distance is in between 40 to 60 cm
        {
            CLR_BIT(PORTB,PB2); //GREEN
            CLR_BIT(PORTB,PB1); //RED
            SET_BIT(PORTB,PB0); //BLUE
            CLR_BIT(PORTB,PB4); // MOTOR
         }
         else
        {
            SET_BIT(PORTB,PB2); //GREEN
            CLR_BIT(PORTB,PB1); //RED
            CLR_BIT(PORTB,PB0); //BLUE
            CLR_BIT(PORTB,PB4); // MOTOR
        }
 
}
void output1(int us_value)
{
 if(us_value>15 && us_value<65)
 {
     CLR_BIT(PORTD,PD7);
      CLR_BIT(PORTD,PD6);
      SET_BIT(PORTD,PD5);
 }
  else
  {
      SET_BIT(PORTD,PD7); //red
      SET_BIT(PORTD,PD6);//buzzer
      CLR_BIT(PORTD,PD5);//green
  }
}
int adc1()
{
  ADMUX&=0x00;
  ADMUX|=(1<<REFS0);
  ADCSRA|=(1<<ADEN);
  ADCSRA|=(1<<ADSC);
while(ADCSRA & (1<<ADSC));
  return ADC;
}
int adc2()
{
  ADMUX&=0x00;
  ADMUX|=(1<<MUX0);
  ADMUX|=(1<<REFS0);
  ADCSRA|=(1<<ADEN);
  ADCSRA|=(1<<ADSC);
  ADC=0;
while(ADCSRA & (1<<ADSC));
  return ADC;
}
int adc3()
{
  ADMUX&=0x00;
   ADMUX|=(1<<MUX1);
  ADMUX|=(1<<REFS0);
  ADCSRA|=(1<<ADEN);
  ADCSRA|=(1<<ADSC);
  ADC=0;
while(ADCSRA & (1<<ADSC));
  return ADC;
}
  

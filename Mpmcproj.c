#include <reg52.h>
#include <intrins.h>

/* Define value to be loaded in timer for PWM period of 20 milli second */
#define PWM_Period 0xB7FE

sbit Servo_Motor_Pin = P3^0;

unsigned int ON_Period, OFF_Period, DutyCycle;

void dElay(unsigned int count)
{
    int i,j;
    for(i=0; i<count; i++)
			for(j=0; j<112; j++);
}

void Timer_init()
{
	TMOD = 0x01;		/* Timer0 mode1 */
	TH0 = (PWM_Period >> 8);/* 20ms timer value */
	TL0 = PWM_Period;
	TR0 = 1;		/* Start timer0 */
}

/* Timer0 interrupt service routine (ISR) */
void Timer0_ISR() interrupt 1	
{
	Servo_Motor_Pin = !Servo_Motor_Pin;
	if(Servo_Motor_Pin)
	{
		TH0 = (ON_Period >> 8);
		TL0 = ON_Period;
	}	
	else
	{
		TH0 = (OFF_Period >> 8);
		TL0 = OFF_Period;
	}	
			
}

/* Calculate ON & OFF period from duty cycle */
void Set_DutyCycle_To(float duty_cycle)
{
	float period = 65535 - PWM_Period;
	ON_Period = ((period/100.0) * duty_cycle);
	OFF_Period = (period - ON_Period);	
	ON_Period = 65535 - ON_Period;	
	OFF_Period = 65535 - OFF_Period;
}

sbit r1 = P1^0;
sbit r2 = P1^1;
sbit r3 = P1^2;
sbit r4 = P1^3;
sbit c1 = P1^4;
sbit c2 = P1^5;
sbit c3 = P1^6;
// 4 x 3 keypad connection

sbit rs = P0^0;
sbit rw = P0^1;
sbit en = P0^2;
sfr lcd = 0xa0; // address of port 2
// lcd connection

void lcdcmd(unsigned char);
void lcddat(unsigned char);
void delay();
void delay1();

void lcddisplay(unsigned char *q);
unsigned char pin[] = {"55555"};
unsigned char Epin[5];

char keypad();
void check();

void main()
{
	 EA  = 1;		/* Enable global interrupt */
   ET0 = 1;    /* Enable timer0 interrupt */
   Timer_init();	
	
	P2=0X00;
	lcdcmd(0x38); // 5x7 matrix
	delay();
	lcdcmd(0x01); // clear display
	delay();
	lcdcmd(0x06); // cursor blinking
	delay();
	lcdcmd(0x0c); // display on
	delay();
	lcdcmd(0x81); 
	delay();
	
  while (1)
    {
        unsigned int i = 0;
        lcdcmd(0x80); //decimal value: 128
        lcddisplay("ENTER PIN NUMBER");
        delay();
        lcdcmd(0xc0); //decimal value: 192
        while (pin[i] != '\0')
        {
            Epin[i] = keypad();
            delay();
            i++;
        }
        check();
    }	
		
}

char keypad()
{
	int x=0;
	while(x==0)
	{
		r1=0; r2=1; r3=1; r4=1;
		if(c1==0)
		{
			lcddat('1');
			delay();
			delay1();
			x=1;
			return '1';
		}
		if(c2==0)
		{
			lcddat('2');
			delay();
			delay1();
			x=1;
			return '2';
		}
		if(c3==0)
		{
			lcddat('3');
			delay();
			delay1();
			x=1;
			return '3';
		}
		// row 1 scan complete
		r1=1; r2=0; r3=1; r4=1;
		if(c1==0)
		{
			lcddat('4');
			
			delay();
			delay1();
			x=1;
			return '4';
		}
		if(c2==0)
		{
			lcddat('5');
			
			delay();
			delay1();
			x=1;
			return '5';
		}
		if(c3==0)
		{
			lcddat('6');
			
			delay();
			delay1();
			x=1;
			return '6';
		}
 		
		// row 2 scan complete
		r1=1; r2=1; r3=0; r4=1;
		if(c1==0)
		{
			lcddat('*');
			
			delay();
			delay1();
			x=1;
			return '*';
		}
		if(c2==0)
		{
			lcddat('8');
			
			delay();
			delay1();
			x=1;
			return '8';
		}
		if(c3==0)
		{
			lcddat('9');
			
			delay();
			delay1();
			x=1;
			return '9';
		}
		
		// row 3 scan complete
    r1=1; r2=1; r3=1; r4=0;
		if(c1==0)
		{
			lcddat('7');
			delay();
			delay1();
			x=1;
			return '7';
		}
		if(c2==0)
		{
			lcddat('0');
			
			delay();
			delay1();
			x=1;
			return '0';
		}
		if(c3==0)
		{
			lcddat('#');
			delay();
			delay1();
			x=1;
			return '#';
		}
 				
	}
	
}

void lcddisplay(unsigned char *q)
{
    int k;
    for (k = 0; q[k] != '\0'; k++)
    {
        lcddat(q[k]);
    }
    delay();
}


void lcdcmd(unsigned char val)
{
P2=val;
rs=0;
rw=0;
en=1;
delay();
en=0;
	
}
void lcddat(unsigned char dat)
{
	P2=dat;
	rs=1;
	rw=0;
	en=1;
	delay();
	en=0;	
}

void delay()
{
int i;
for(i=0; i<10000; i++);
}

void delay1()
{
	unsigned int j,k;
	for(j=0;j<200;j++)
	 for(k=0;k<1000;k++);
	
}

void check()
{
	if (pin[0] == Epin[0] && pin[1] == Epin[1] && pin[2] == Epin[2] && pin[3] == Epin[3] && pin[4] == Epin[4])
    {
        delay();
        lcdcmd(0x01); //decimal value: 1
        lcdcmd(0x81); //decimal value: 129
        // show pin is correct
        lcddisplay("PIN CORRECT");
        delay();
        // door motor will run
        //motorpin1 = 1;
        //motorpin2 = 0;
			 
				Set_DutyCycle_To(0.7);/* 0.54ms(2.7%) of 20ms(100%) period */
				dElay(8000);
				Set_DutyCycle_To(9);/* 1.4ms(7%) of 20ms(100%) period */
				dElay(2000);
			
        lcdcmd(0xc1); //decimal value: 193
        // show the door is unlocked
        lcddisplay("DOOR OPENED");
        delay();
        //motorpin1 = 1;
        //motorpin2 = 0;
        lcdcmd(0x01); //decimal value: 1
				dElay(5000);
				Set_DutyCycle_To(12);/* 2.4ms(12%) of 20ms(100%) period */
    }
    else
    {
        lcdcmd(0x01); //decimal value: 1
        lcdcmd(0x80); //decimal value: 128
        lcddisplay("WRONG PIN");
        delay();
        lcdcmd(0x01); //decimal value: 1
    }
		
	
}
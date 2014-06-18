/**
*	@file
*
*		@author sushil kumar meena (sushilm.iitb@gmail.com)
*		@author mudit malpani (cooldudemd.malpani@gmail.com)
*		@author palak dalal (palak.dalal@gmail.com)
*		@author hemant noval (novalhemant@gmail.com)
*
*	Description:
*		This file contains white line following code. This file is to be compiled and fed to the bot. It also contains zigbee communication module.
*/
	
#include <math.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define FCPU 11059200ul
#define RS 0
#define RW 1
#define EN 2
#define lcd_port PORTC
#define sbit(reg,bit)	reg |= (1<<bit)
#define cbit(reg,bit)	reg &= ~(1<<bit)


unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char sharp, sharp2, sharp3, distance, adc_reading;
unsigned int value, value2, value3;
float BATT_Voltage, BATT_V;


void lcd_reset_4bit();
void lcd_init();
void lcd_wr_command(unsigned char);
void lcd_wr_char(char);
void lcd_home();
void lcd_cursor(char, char);
void lcd_print(char, char, unsigned int, int);
void lcd_string(char*);

/** Various global variables needed **/
unsigned int temp;
unsigned int unit;
unsigned int tens;
unsigned int hundred;
unsigned int thousand;
unsigned int million;
unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder 
unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning
unsigned char Front_Sharp_Sensor=0;
unsigned char Front_IR_Sensor=0;
int i,count,curTurningLane,skip_lanes=1;
int right_skip=0;
int move=0;
int vel=130;
int counter=0;
int prevState = 0;


// F - forward , R - right , L - left and B - back
typedef enum {
    F = 0, L = 1, B = 2, R = 3
} turn; // turnActual = [TurnOrig + (4-direc)]%4;

typedef struct state {
    turn direc;
    int nodeNum;
} botState;


struct direction{
float distance;
int direction;
int node_name;
};

//
turn t[10],turningLane[10],skipLanes[10];

/*****Function to Reset LCD*****/
void lcd_set_4bit()
{
	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x20;				//Sending 2 to initialise LCD 4-bit mode
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	
}

/*****Function to Initialize LCD*****/
void lcd_init()
{
	_delay_ms(1);

	lcd_wr_command(0x28);			//LCD 4-bit mode and 2 lines.
	lcd_wr_command(0x01);
	lcd_wr_command(0x06);
	lcd_wr_command(0x0E);
	lcd_wr_command(0x80);
}

	 
/*****Function to Write Command on LCD*****/
void lcd_wr_command(unsigned char cmd)
{
	unsigned char temp;
	temp = cmd;
	temp = temp & 0xF0;
	lcd_port &= 0x0F;
	lcd_port |= temp;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
	
	cmd = cmd & 0x0F;
	cmd = cmd<<4;
	lcd_port &= 0x0F;
	lcd_port |= cmd;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}

/*****Function to Write Data on LCD*****/
void lcd_wr_char(char letter)
{
	char temp;
	temp = letter;
	temp = (temp & 0xF0);
	lcd_port &= 0x0F;
	lcd_port |= temp;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);

	letter = letter & 0x0F;
	letter = letter<<4;
	lcd_port &= 0x0F;
	lcd_port |= letter;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}


void lcd_home()
{
	lcd_wr_command(0x80);
}


/*****Function to Print String on LCD*****/
void lcd_string(char *str)
{
	while(*str != '\0')
	{
		lcd_wr_char(*str);
		str++;
	}
}

/*** Position the LCD cursor at "row", "column". ***/

void lcd_cursor (char row, char column)
{
	switch (row) {
		case 1: lcd_wr_command (0x80 + column - 1); break;
		case 2: lcd_wr_command (0xc0 + column - 1); break;
		case 3: lcd_wr_command (0x94 + column - 1); break;
		case 4: lcd_wr_command (0xd4 + column - 1); break;
		default: break;
	}
}

/***** Function To Print Any input value upto the desired digit on LCD *****/
void lcd_print (char row, char coloumn, unsigned int value, int digits)
{
	unsigned char flag=0;
	if(row==0||coloumn==0)
	{
		lcd_home();
	}
	else
	{
		lcd_cursor(row,coloumn);
	}
	if(digits==5 || flag==1)
	{
		million=value/10000+48;
		lcd_wr_char(million);
		flag=1;
	}
	if(digits==4 || flag==1)
	{
		temp = value/1000;
		thousand = temp%10 + 48;
		lcd_wr_char(thousand);
		flag=1;
	}
	if(digits==3 || flag==1)
	{
		temp = value/100;
		hundred = temp%10 + 48;
		lcd_wr_char(hundred);
		flag=1;
	}
	if(digits==2 || flag==1)
	{
		temp = value/10;
		tens = temp%10 + 48;
		lcd_wr_char(tens);
		flag=1;
	}
	if(digits==1 || flag==1)
	{
		unit = value%10 + 48;
		lcd_wr_char(unit);
	}
	if(digits>5)
	{
		lcd_wr_char('E');
	}
	
}
		

/*** To configure the lcd ports **/
void LCD_port_config( void){

	DDRC = DDRC | 0xF7;
	PORTC = PORTC & 0x80;
}


/*** To configure the pins **/
void adc_pin_config(void){
	
	DDRF = 0x00;
	PORTF = 0x00;
	DDRK = 0x00;
	PORTK = 0x00;
}


/**Function to configure INT4 (PORTE 4) pin as input for the left position encoder**/
void left_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x10; //Enable internal pullup for PORTE 4 pin
}

/**Function to configure INT5 (PORTE 5) pin as input for the right position encoder**/
void right_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x20; //Enable internal pullup for PORTE 4 pin
}

void left_position_encoder_interrupt_init (void) //Interrupt 4 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
 EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
 sei();   // Enables the global interrupt 
}

void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
 EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
 sei();   // Enables the global interrupt 
}


/**ISR for right position encoder*/
ISR(INT5_vect)  
{
 ShaftCountRight++;  //increment right shaft position count
}


/**SR for left position encoder**/
ISR(INT4_vect)
{
 ShaftCountLeft++;  //increment left shaft position count
}


void motion_pin_config() {
	DDRA = DDRA | 0x0F;
	PORTA = PORTA & 0xF0; 
	DDRL = DDRL | 0x18;
	PORTL = PORTL | 0x18;

}


void port_init(){

	LCD_port_config();
	adc_pin_config();
	motion_pin_config();
	
 left_encoder_pin_config(); //left encoder pin config
 right_encoder_pin_config(); //right encoder pin config	
}


void adc_init(){

	ADCSRA = 0x00;
	ADCSRB = 0x00;
	ADMUX = 0x20;
	ACSR = 0x80;
	ADCSRA = 0x86;
}

/** Timer 5 initialised in PWM mode for velocity control
	Prescale:64
	PWM 8bit fast, TOP=0x00FF
	Timer Frequency:674.988Hz
	*/
void timer5_init()
{

	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionalit to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}


/**Function used for setting motor's direction*/
void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 			// removing upper nibbel as it is not needed
 PortARestore = PORTA; 			// reading the PORTA's original status
 PortARestore &= 0xF0; 			// setting lower direction nibbel to 0
 PortARestore |= Direction; 	// adding lower nibbel for direction command and restoring the PORTA status
 PORTA = PortARestore; 			// setting the command to the port
}

/**Move both wheel forward*/
void forward (void) //both wheels forward
{
  motion_set(0x06);
}

/**Move both wheel backward*/
void back (void) //both wheels backward
{
  motion_set(0x09);
}

/**Move the bot to left*/
void left (void) //Left wheel backward, Right wheel forward
{
  motion_set(0x05);
}

/**Move the bot to right*/
void right (void) //Left wheel forward, Right wheel backward
{
  motion_set(0x0A);
}

/**Soft left turn*/
void soft_left (void) //Left wheel stationary, Right wheel forward
{
 motion_set(0x04);
}

/**Soft right turn*/
void soft_right (void) //Left wheel forward, Right wheel is stationary
{
 motion_set(0x02);
}

/**soft left turn*/
void soft_left_2 (void) //Left wheel backward, right wheel stationary
{
 motion_set(0x01);
}

void soft_right_2 (void) //Left wheel stationary, Right wheel backward
{
 motion_set(0x08);
}

/**Stop the bot*/
void stop (void)
{
  motion_set(0x00);
}

/**Function used for turning robot by specified degrees
	args: Degrees
	Action: move the bot by given angle
*/
void angle_rotate(unsigned int Degrees)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
 ShaftCountRight = 0; 
 ShaftCountLeft = 0; 

 while (1)
 {
 	lcd_print(1, 1, ShaftCountLeft, 3);
 	lcd_print(1, 5, ShaftCountRight, 3);
	lcd_print(1, 9, ReqdShaftCountInt, 3);
  if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
  break;
 }
 stop(); //Stop action
}

/**Function used for moving robot forward by specified distance
	args: DistanceInMM
	Action: Moves the bot by given distance
*/

void linear_distance_mm(unsigned int DistanceInMM)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
  
 ShaftCountRight = 0;
 while(1)
 {
 	lcd_print(1, 1, ShaftCountLeft, 3);
 	lcd_print(1, 5, ShaftCountRight, 3);
	lcd_print(1, 9, ReqdShaftCountInt, 3);
  if(ShaftCountRight > ReqdShaftCountInt)
  {
  	break;
  }
 } 
 stop(); //Stop action
}

/**Function to move bot by DistanceInMM*/
void forward_mm(unsigned int DistanceInMM)
{
 forward();
 linear_distance_mm(DistanceInMM);
}

/**Function to move bot backward by DistanceInMM*/
void back_mm(unsigned int DistanceInMM)
{
 back();
 linear_distance_mm(DistanceInMM);
}

/**Function to move the bot left by given Degrees**/
void left_degrees(unsigned int Degrees) 
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 left(); //Turn left
 angle_rotate(Degrees);
}


/**Function to move the bot right by given Degrees**/
void right_degrees(unsigned int Degrees)
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 right(); //Turn right
 angle_rotate(Degrees);
}

/**Function to move the bot left softly by given Degrees**/
void soft_left_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_left(); //Turn soft left
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

/**Function to move the bot right softly by given Degrees**/
void soft_right_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_right();  //Turn soft right
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

/**Function to move the bot left softly by given Degrees**/
void soft_left_2_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_left_2(); //Turn reverse soft left
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

/**Function to move the bot right softly by given Degrees**/
void soft_right_2_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_right_2();  //Turn reverse soft right
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}


/** Function to do ADC conversion**/
unsigned char ADC_Conversion(unsigned char Ch){

	unsigned char a;

	if(Ch>7){
		ADCSRB = 0x08;
	}

	Ch = Ch & 0x07;
	ADMUX = 0x20 | Ch;
	ADCSRA = ADCSRA | 0x40;

	while((ADCSRA & 0x10) == 0);

	a = ADCH;
	ADCSRB = 0x00;
	return a;
}

void print_sensor(char row, char column, unsigned char channel){
	
	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, column, ADC_Value, 3);
}

/**To set wheel velocities*/
void velocity(unsigned char left_motion, unsigned char right_motion) {
	if (left_motion > 255) left_motion = 255;
	if (right_motion > 255) right_motion = 255;
	OCR5AL = (unsigned char)left_motion;
	OCR5BL = (unsigned char)right_motion;
}	

/**To calculate sensor values*/
unsigned int Sharp_GP2D12_estimation(unsigned char adc_reading){

	float distance;
	distance = (int)(10.00*(2799.6* (1/pow(adc_reading, 1.1546))));
	int distanceInt = (int) distance;

	if(distance>800){
		distanceInt = 800;
	}

	return distanceInt;
}


unsigned int Sharp_GP2D120_estimation(unsigned char adc_reading){

	float distance;
	distance = (int) (10.00*((1.00/ ((0.001240875*(float) adc_reading) + 0.005)) - 0.42));


	int distanceInt = (int) distance;

	if(distance>400){
		distanceInt = 400;
	}

	return distanceInt;
}

/** Init the devides **/
void init_devices(void){

cli(); //disable all interrupts
	port_init();
	left_position_encoder_interrupt_init();
	right_position_encoder_interrupt_init();
	adc_init();
	timer5_init();
sei(); //re-enable interrupts

}

/**Function to make buzzer on**/
void buzzer_on (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore | 0x08;
 PORTC = port_restore;
}

/**Function to make buzzer off**/
void buzzer_off (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore & 0xF7;
 PORTC = port_restore;
}




void uart0_init(void){
  UCSR0B = 0x00; //disable while setting baud rate
  UCSR0A = 0x00;
  UCSR0C = 0x06;
  UBRR0L = 0x47; //set baud rate lo
  UBRR0H = 0x00; //set baud rate hi
  UCSR0B = 0x98;
}

/**
Interrupt Routine for recieving signal from zigbee.
First the data recieved is converted into integer and stored in variable binVal.
The data is sent in this form -> dir*50 + lane*6 + skipLane. Sending data in this form is convenient for handling.
So the direction,skipLane and lane (lanes to be skipped before turning) are extracted and stored in the appropraite arrays.
Also when 'a' is sent thorugh zigbee module which will be sent e.g when it sees a red light thus instructing the bot to stop.
Similarly 'b' is sent to make the bot start moving.
Speed of the bot after seeing the speed limit sign boards is communicated through the characters 'c','d' and 'e'.
*/

SIGNAL(SIG_USART0_RECV) 		// ISR for receive complete interrupt
{
	unsigned char data = UDR0;
	
	int binVal = (int) data;

	int dir = binVal/50;
	binVal = (binVal-dir*50);
	int lane = binVal/6;
	binVal = (binVal-lane*6);
	int skipLane = binVal;

// write the values of direction  , turning lanes and skip lanes in the appropriate arrays
	t[counter] = dir;
	turningLane[counter] = lane;
	skipLanes[counter] = skipLane;

	lcd_print(1, 1, dir, 3);
	lcd_print(1, 5, lane, 3);
	lcd_print(1, 9, skipLane, 3);
	lcd_print(2, 1, counter, 3);
	lcd_print(2, 5, binVal, 3);
	counter++;
	int x;
	//detect speed of bot
	if(data=='a') {
		move=0;
		x = 0;
    }
	else if(data=='b') {
		move=1;
		x=1;
	}
	
	if(data=='c') {
		vel = 45;
		x=30;
	}
	else if(data=='d'){
		vel = 90;
		x=60;
	}
	else if(data=='e'){
		vel = 120;
		x=90;
	}

	//a=````3;

	lcd_print(1, 1, (int)x, 3);
}


/**
The function which is called each time when the bot wants to follow a straight line.
It reads the values of the various sennsors like sharp and IR sensor ( for sensing things in front of it to avoid collision),
and also the values of the three light sensors used in line following.
If the bot sees anything in front of it stops until the thing moves out of its way.
If it deviates a littlebit left while following  a line it shifts right and vice versa.

It also maintains a previous state which is used in the recovery system in case all the three sensors are on black.
It will remember the last time where it came from and will take the actin which it took that time to come to the white line in case it comes completely on the black.

When it comes to a junction that is at the big white sqaure ,all its sensors are on white so it will just move straight until atleast one sensor comes on the black.
Also it looks at the three values here : 
			1. Direction -> IN which direction it has to go after this junction i.e left, right or forward.
			2. TurningLanes -> Before turning how many junctions it has to skip since it has to follow the lane system and 
			also all raods have different number of lanes.
			3. SkipLanes -> After turning how many junctions it has to skip so as to leave the entire junction and enter the
			actual road.

*/
void lineFollow(turn t)
{

	unsigned char left_line, centre_line, right_line;

// For adaptive cruise control
	Front_Sharp_Sensor = ADC_Conversion(11);
	Front_IR_Sensor = ADC_Conversion(6);
	//Get sensor values for white line
	left_line = ADC_Conversion(3);
	centre_line = ADC_Conversion(2);
	right_line = ADC_Conversion(1);

	//output them on lcd
	lcd_print(1, 1, left_line, 3);
	lcd_print(1, 5, centre_line, 3);
	lcd_print(1, 9, curTurningLane, 3);
	lcd_print(2, 1, count, 3);
	lcd_print(2, 5, t, 3);
	lcd_print(2, 9, skip_lanes, 3);
		
		// if detects something in front of it then it will stop
	if(Front_Sharp_Sensor>0x82 || Front_IR_Sensor<0xF0){
		//flag2=1;
		stop();
		buzzer_on();			
	}
			
	else if (centre_line > 0x10 && left_line > 0x18 && right_line < 0x18) {
		prevState=1;
		forward();velocity(vel, vel/2);buzzer_off(); //move a little right
	}
	else if (centre_line > 0x10 && right_line > 0x18 && left_line < 0x18) {
		prevState=2;
		forward();velocity(vel/2,  vel); buzzer_off();// move a little left
	}
	else if (centre_line > 0x10 && right_line > 0x18 && left_line > 0x18) {
		forward();velocity(0,0);buzzer_off(); //stop, got lost
		if(prevState==1)// move based on previous state
		{
		forward();
		velocity( vel, vel/2);
		}
		else if(prevState==2){
		forward();velocity(vel/2, vel);
		}
	}
	else if (centre_line < 0x10 && right_line < 0x10 && left_line < 0x10) {
		prevState=0;
		stop();
		_delay_ms(1000);
		forward();
			// stop, intersection detected!
			count++; // for counting lanes, i have got one lane
			
	    	lcd_print(1, 5, centre_line, 3);
     		lcd_print(1, 9, curTurningLane, 3);
	    	lcd_print(2, 1, count, 3);
			lcd_print(2, 5, t, 3);
			lcd_print(2, 9, skip_lanes, 3);
			//_delay_ms(1000);
			if(count==curTurningLane && right_skip==0) {
			// if this is the right lane where we have to turn (or move ahead)    
				if(t==L)
				{
				// skip past the junction by just moving straight
				while(centre_line < 0x10 && right_line < 0x10 && left_line < 0x10){
						//left_line = ADC_Conversion(3);
						centre_line = ADC_Conversion(2);
						right_line = ADC_Conversion(1);
						lcd_print(1, 1, left_line, 3);
	    				lcd_print(1, 5, centre_line, 3);
	    				lcd_print(1, 9, curTurningLane, 3);
						lcd_print(2, 1, count, 3);
						lcd_print(2, 5, t, 3);
						lcd_print(2, 9, skip_lanes, 3);
						forward();
						velocity(vel,vel);				
				}
				//linear_distance_mm(10);
				left_degrees(90);// take a hard left turn
				}
				else if(t==R)
				{
				
					right_skip = 1;
					while(centre_line < 0x10 && right_line < 0x10 && left_line < 0x10){
						left_line = ADC_Conversion(3);
						centre_line = ADC_Conversion(2);
						right_line = ADC_Conversion(1);
						//lcd_print(1, 1, left_line, 3);
	    				lcd_print(1, 5, centre_line, 3);
	    				lcd_print(1, 9, curTurningLane, 3);
						lcd_print(2, 1, count, 3);
			    		lcd_print(2, 5, t, 3);
				    	lcd_print(2, 9, skip_lanes, 3);	
						forward();
						velocity(vel,vel);
					}
				//linear_distance_mm(15);
					right_degrees(90);
					count=0;
					// after turning right it has to skip some lanes which are part of that junction only 
					curTurningLane=skip_lanes;
					while(1){
					if(count==curTurningLane) {
						right_skip = 0;
						break;
					}
					lineFollow(F);
					}
				}
				else if(t==F){
					while(centre_line < 0x10 && right_line < 0x10 && left_line < 0x10){
						left_line = ADC_Conversion(3);
						centre_line = ADC_Conversion(2);
						right_line = ADC_Conversion(1);
					//	lcd_print(1, 1, left_line, 3);
	    				lcd_print(1, 5, centre_line, 3);
	    				lcd_print(1, 9, curTurningLane, 3);
	    				lcd_print(2, 1, count, 3);
						lcd_print(2, 5, t, 3);
						lcd_print(2, 9, skip_lanes, 3);
						forward();
						velocity(vel,vel);
					}
				}
				lcd_print(2, 9, right_line, 3);
				if(right_skip != 1) i++;
				count=0;				
			}
			else{// if lane to skip then move forward along the big square and cross the square
				while(centre_line < 0x10 && right_line < 0x10 && left_line < 0x10){
					left_line = ADC_Conversion(3);
					centre_line = ADC_Conversion(2);
					right_line = ADC_Conversion(1);
				//	lcd_print(1, 1, left_line, 3);
	    			lcd_print(1, 5, centre_line, 3);
	    			lcd_print(1, 9, curTurningLane, 3);
	    			lcd_print(2, 1, count, 3);
					lcd_print(2, 5, t, 3);
					lcd_print(2, 9, skip_lanes, 3);
					forward();
					velocity(vel,vel);
				}
			}
	//	}
		forward();
		velocity(vel,vel);
	}
	else if (centre_line < 0x12) {
		forward();velocity(vel, vel); buzzer_off();// on the line, continue following it
	}
	_delay_ms(50);
}

/**
It extracts the three values of direction , turningLanes and skipLanes which were stored after recieving these values thorugh zigbee module from the central(GPS) server and calls the lineFollow function which will use these values to move forward.
*/
int main(){

	init_devices();

	lcd_set_4bit();
	lcd_init();
	uart0_init();
	lcd_string("IR6 IR7 IR8");
	_delay_ms(3000);
		

	PORTA = 0x06;
	velocity(vel,vel);
	count=0;
	i=0;
	/*turningLane[0] = 2;
	turningLane[1] = 2;
	turningLane[2] = 2;
	t[0] = 0;
	t[1] = 0;
	t[2] = 3;
	skipLanes[0] = 0;
	skipLanes[1] = 0;
	skipLanes[2] = 3;*/


	while(1){
//move = 1;
	if(move==1){
		curTurningLane = turningLane[i];
		turn curTurn = t[i];
		skip_lanes = skipLanes[i];
		//lcd_print(2, 1, vel, 3);
		lineFollow(curTurn);
	}
	else {
		stop();
		// not recieved the command to move from the zigbee module
		}
	}
}

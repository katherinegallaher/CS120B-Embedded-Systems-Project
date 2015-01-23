/*    Partner 1 Name & E-mail: Katherine Gallaher & kgall005@ucr.edu
 *    Lab Section: 23
 *    Assignment: Lab #10 Custom Lab
 *    Exercise Description: A rhythm game, arrows travel up the LCD screen and you input the correct combination of arrows on the SNES 
 *    
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 */


// up_go[] = {1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0};
// down_go[] {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0};
// left_go[] {0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// right_go[]{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1};
//output[] = {0,0,0,4,0,0,0,1,0,0,4,0,0,0,3,0,0,0,0,1,0,0,0,0,2,0,0,0,6,0,0,0,0,9,0,0,0,0,15,0,0,0,2,0,0,0,0,4,0,0,0,6,0,2,0,0,1,0,0,9,0,0,0,10,0,0,0,6,0,0,0,5,0,0,0,0,4,0,0,0,1,0,0,0,0,2,0,0,0,0,10,0,0,0,0,0,0,0,2};

//1 = left, 2 = right, 3 = down, 4 = up, 5 = left,right, 6 = left,down, 7 = left,up, 8 = right,down, 9 = right,up, 10 = down,up, 11 = left,right,down, 12 = right,down,up
//13 = left,down,up, 14 = left,right,up, 15 = all, 0 = none

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>//for the delays
#include "io.c"
#include "scheduler.h"
#include "timer.h"
#include "bit.h"//includes getbit and setbit

//global vars
int lose =0;
unsigned short button_presses=0x00;
unsigned char start = 0;
unsigned char currentpattern =0;
int score = 10;
//end global

//UNSIGNED SHORT, 16 BITS for snes
unsigned short SNES() {
	
	unsigned short pushed = 0x00;//will hold the bits that tell you which buttons were pushed
	PORTA = SetBit(PINA, 7, 1);
	_delay_us(12);
	PORTA = SetBit(PINA, 7, 0);
	_delay_us(6);
	
	for (unsigned char i = 0; i < 16; i++) {
		if(!GetBit(PINA,5)){
			pushed = SetBit(pushed, i, 1);
		}
		else{
			pushed = SetBit(pushed, i, 0);
		}
		PORTA = SetBit(PINA, 6, 1);
		_delay_ms(6);
		PORTA = SetBit(PINA, 6, 0);
		_delay_ms(6);
	}
	
	return pushed;
}

enum SM9_States {SM9_init, SM9_wait, SM9_started};//start screen
int SMTick9(int state){//transitions
	switch(state){
		case SM9_init:
		state = SM9_wait;
		break;
		case SM9_wait:
		if(GetBit(button_presses,3)){
			start = 1;
			LCD_ClearScreen();
			state = SM9_started;
		}
		else{
			state = SM9_wait;
		}
		break;
		case SM9_started:
		state = SM9_started;
		break;
		default:
		state = SM9_init;
		break;
	}
	switch(state){
		case SM9_init:
		LCD_DisplayString(6, "Rhythm");
		LCD_DisplayString(20,"press start");
		break;
		default:
		break;
	}
	return state;
}

enum SM1_States {SM1_start, SM1_init, SM1_LED, SM1_bottom, SM1_top,SM1_done};//up arrow
int SMTick1(int state){//
	static unsigned char up_go[] = {1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0};
	static unsigned count = 0;
	
	switch(state){
		case SM1_start:
			if(start){
				state = SM1_init;
			}
			else{
				state = SM1_start;
			}
			break;
		case SM1_init:
			count++;
			if(count >= 100 || lose == 1){
				state = SM1_done;
			}
			else if(up_go[count] == 1){
				state =SM1_LED;
			}
			else{
				state = SM1_init;
			}
			break;
		case SM1_LED:
			if(lose){
				state = SM1_done;
			}
			count++;
			state = SM1_bottom;
			break;
		case SM1_bottom:
			if(lose){
				state = SM1_done;
			}
			count++;
			state = SM1_top;
			break;
		case SM1_top:
			if(lose){
				state = SM1_done;
			}
			count++;
			LCD_Cursor(1);
			LCD_WriteData(0x20);//clears the space
			state = SM1_init;
			break;
		case SM1_done:
			state = SM1_done;
			break;
		default:
			state = SM1_start;
			break;
	}
	switch(state){
		case SM1_start:
			count =0;
			break;
		case SM1_init:
			break;
		case SM1_LED:
			PORTC = SetBit(PORTC,0 , 1);
			break;
		case SM1_bottom:
			PORTC = SetBit(PORTC,0 , 0);
			LCD_Cursor(17);
			LCD_WriteData(0x00);
			break;
		case SM1_top:
			LCD_Cursor(17);
			LCD_WriteData(0x20);//clears the space
			LCD_Cursor(1);
			LCD_WriteData(0x00);
			break;
		default:
			break;
	}
	return state;
}

enum SM2_States {SM2_start, SM2_init, SM2_LED, SM2_bottom, SM2_top, SM2_done};//down arrow
int SMTick2(int state){//transitions
	static unsigned char down_go[] = {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0};
	static unsigned count = 0;
	
	switch(state){
		case SM2_start:
			if(start){
				state = SM2_init;
			}
			else{
				state = SM2_start;
			}
			break;
		case SM2_init:
			count++;
			if(count >= 100 || lose ==1){
				state = SM2_done;
			}
			else if(down_go[count] == 1){
				state =SM2_LED;
			}
			else{
				state = SM2_init;
			}
			break;
		case SM2_LED:
			if(lose){
				state = SM2_done;
			}
			count++;
			state = SM2_bottom;
			break;
		case SM2_bottom:
			if(lose){
				state = SM2_done;
			}
			count++;
			state = SM2_top;
			break;
		case SM2_top:
			if(lose){
				state = SM2_done;
			}
			count++;
			LCD_Cursor(2);
			LCD_WriteData(0x20);//clears the space
			state = SM2_init;
			break;
		case SM2_done:
			state = SM2_done;
			break;
		default:
			state = SM2_start;
			break;
	}
	switch(state){
		case SM2_start:
			count =0;
			break;
		case SM2_init:
			break;
		case SM2_LED:
			PORTC = SetBit(PORTC,1 , 1);
			break;
		case SM2_bottom:
			PORTC = SetBit(PORTC,1 , 0);
			LCD_Cursor(18);
			LCD_WriteData(0x01);
			break;
		case SM2_top:
			LCD_Cursor(18);
			LCD_WriteData(0x20);//clears the space
			LCD_Cursor(2);
			LCD_WriteData(0x01);
			break;
		default:
			break;
	}
	return state;
}

enum SM3_States {SM3_start, SM3_init, SM3_LED, SM3_bottom, SM3_top, SM3_done};//left arrow
int SMTick3(int state){//transitions
	static unsigned char left_go[] = {0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	static unsigned count = 0;
	
	switch(state){
		case SM3_start:
			if(start){
				state = SM3_init;
			}
			else{
				state = SM3_start;
			}
			break;
		case SM3_init:
			count++;
			if(count >= 100 || lose ==1){
				state = SM3_done;
			}
			else if(left_go[count] == 1){
				state =SM3_LED;
			}
			else{
				state = SM3_init;
			}
			break;
		case SM3_LED:
			if(lose){
				state = SM3_done;
			}
			count++;
			state = SM3_bottom;
			break;
		case SM3_bottom:
			if(lose){
				state = SM3_done;
			}
			count++;
			state = SM3_top;
			break;
		case SM3_top:
			if(lose){
				state = SM3_done;
			}
			count++;
			LCD_Cursor(3);
			LCD_WriteData(0x20);//clears the space
			state = SM3_init;
			break;
		case SM3_done:
			state = SM3_done;
			break;
		default:
			state = SM3_start;
			break;
	}
	switch(state){
		case SM3_start:
			count =0;
			break;
			case SM3_init:
			break;
		case SM3_LED:
			LCD_Cursor(3);
			LCD_WriteData(0x20);//clears the space
			PORTC = SetBit(PORTC,2 , 1);
			break;
		case SM3_bottom:
			PORTC = SetBit(PORTC,2 , 0);
			LCD_Cursor(19);
			LCD_WriteData(0x02);
			break;
		case SM3_top:
			LCD_Cursor(19);
			LCD_WriteData(0x20);//clears the space
			LCD_Cursor(3);
			LCD_WriteData(0x02);
			break;
		default:
			break;
	}
	return state;
}

enum SM4_States {SM4_start, SM4_init, SM4_LED, SM4_bottom, SM4_top, SM4_done};//right arrow
int SMTick4(int state){//transitions
	static unsigned char right_go[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1};
	static unsigned count = 0;
	
	switch(state){
		case SM4_start:
			if(start){
				state = SM4_init;
			}
			else{
				state = SM4_start;
			}
			break;
		case SM4_init:
			count++;
			if(count >= 100 || lose ==1){
				state = SM4_done;
			}
			else if(right_go[count] == 1){
				state =SM4_LED;
			}
			else{
				state = SM4_init;
			}
			break;
		case SM4_LED:
			if(lose){
				state = SM4_done;
			}
			count++;
			state = SM4_bottom;
			break;
		case SM4_bottom:
			if(lose){
				state = SM4_done;
			}
			count++;
			state = SM4_top;
			break;
		case SM4_top:
			if(lose){
				state = SM4_done;
			}
			count++;
			LCD_Cursor(4);
			LCD_WriteData(0x20);//clears the space
			state = SM4_init;
			break;
		case SM4_done:
			state = SM4_done;
			break;
		default:
			state = SM4_start;
			break;
	}
	switch(state){
		case SM4_start:
			count =0;
			break;
		case SM4_init:
			break;
		case SM4_LED:
			LCD_Cursor(4);
			LCD_WriteData(0x20);//clears the space
			PORTC = SetBit(PORTC,3 , 1);
			break;
		case SM4_bottom:
			PORTC = SetBit(PORTC,3 , 0);
			LCD_Cursor(20);
			LCD_WriteData(0x03);
			break;
		case SM4_top:
			LCD_Cursor(20);
			LCD_WriteData(0x20);//clears the space
			LCD_Cursor(4);
			LCD_WriteData(0x03);
			break;
		default:
			break;
	}
	return state;
}

enum SM5_States {SM5_start, SM5_init, SM5_countdown, SM5_done};//clock and final screen display
int SMTick5(int state){//transitions
	static unsigned char i=0;
	static unsigned char j=0;
	static unsigned char count=0;
	static unsigned char counter =0;
	
	switch(state){
		case SM5_start:
			if(start){
				state = SM5_init;
			}
			else{
				state = SM5_start;
			}
			break;
		case SM5_init:
			LCD_Cursor(15);
			LCD_WriteData(5 + '0');
			state = SM5_countdown;
			break;
		case SM5_countdown:
			if(counter >=60 || lose == 1){
				if(!lose){
					LCD_ClearScreen();
					LCD_DisplayString(1, "Level Complete!");
					if(score == 56){
						LCD_DisplayString(17,"Perfect Score:");
						showScore(31);
					}
					else{
						LCD_DisplayString(18,"Final Score:");
						showScore(30);
					}
					
				}
				else{
					LCD_ClearScreen();
					LCD_DisplayString(1, "Game Over!");
					LCD_DisplayString(17, "You lost.");
				}
				state = SM5_done;
			}
			else{
				i--;
				count++;
				counter++;
				state = SM5_countdown;
			}
			break;
		case SM5_done:
			state = SM5_done;
			break;
		default:
			state = SM5_start;
			break;
	}
	switch(state){
		case SM5_init:
			counter = 0;
			i = 9;
			j = 5;
			count = 0;
			break;
		case SM5_countdown:
			if(counter <60){
				if(i==0){
					LCD_Cursor(16);
					LCD_WriteData(i + '0');
					i=10;
				}
				else{
					LCD_Cursor(16);
					LCD_WriteData(i + '0');
				}
				if(count > 9){
					j--;
					LCD_Cursor(15);
					LCD_WriteData(j + '0');
					count = 0;
				}
			}
			else{
				counter = 60;
			}
			break;
		case SM5_done:
			break;
		default:
			break;
	}
	return state;
}

enum SM6_States {SM6_first, SM6_init, SM6_done};//puts the current pattern in the currentpattern variable to check with the inputs
//1 = left, 2 = right, 3 = down, 4 = up, 5 = left,right, 6 = left,down, 7 = left,up, 8 = right,down, 9 = right,up, 10 = down,up, 11 = left,right,down, 12 = right,down,up
//13 = left,down,up, 14 = left,right,up, 15 = all, 0 = none
int SMTick6(int state){//transitions
	static unsigned char count = 0;
	static unsigned char output[] = {0,0,0,4,0,0,1,0,0,4,0,0,0,3,0,0,0,0,1,0,0,0,0,2,0,0,0,6,0,0,0,0,9,0,0,0,0,15,0,0,0,2,0,0,0,0,4,0,0,0,6,0,2,0,0,1,0,0,9,0,0,0,10,0,0,0,6,0,0,0,5,0,0,0,0,4,0,0,0,1,0,0,0,0,2,0,0,0,0,10,0,0,0,0,0,0,0,0,2,0};
	switch(state){
		case SM6_first:
			if(start)
			{
				state = SM6_init;
			}
			else{
				state = SM6_first;
			}
			break;
		case SM6_init:
			count++;
			if(count >= 100 || lose == 1){
				state = SM6_done;
			}
			else{
				state = SM6_init;
			}
			break;
		case SM6_done:
			state = SM6_done;
			break;
		default:
			state = SM6_first;
			break;
	}
	switch(state){
		case SM6_first:
			count =0;
			break;
		case SM6_init:
			currentpattern = output[count];
			break;
		default:
			break;
	}
	return state;
}

enum SM7_States {SM7_read};//checks to see what buttons are pressed
int SMTick7(int state){//transitions
	switch(state){
		case SM7_read:
			state = SM7_read;
			break;
		default:
			state = SM7_read;
			break;
	}
	switch(state){
		case SM7_read:
			button_presses = SNES();
			break;
		default:
			break;
	}
	return state;
}

enum SM8_States {SM8_init, SM8_checkbuttons, SM8_pointup, SM8_pointdown, SM8_done};//determines whether you get a point or not
int SMTick8(int state){//transitions
	static unsigned char pastpattern =0;
	static unsigned char hit=0;
	static unsigned char count =0;
	static unsigned char miss =0;
	switch(state){
		case SM8_init:
			if(start){
				LCD_DisplayString(10,"Time:60");
				LCD_DisplayString(25, "Score:10");
				count=0;
				state = SM8_checkbuttons;
			}
			else{
				state = SM8_init;
			}
			break;
		case SM8_checkbuttons:
			if(hit == 1){
				score+= 2;
				state = SM8_pointup;
			}
			else if(miss == 1 && count >= 10){
				score -= 3;
				state = SM8_pointdown;
			}
			else{
				
				state = SM8_checkbuttons;
			}
			break;
		case SM8_pointup:
			if(currentpattern != pastpattern){
				count =0;
				state = SM8_checkbuttons;
			}
			else{
				state = SM8_pointup;
			}
			break;
		case SM8_pointdown:
			if(lose){
				state = SM8_done;
			}
			if(currentpattern != pastpattern){
				count =0;
				state = SM8_checkbuttons;
			}
			else{
				state = SM8_pointdown;
			}
			break;
		case SM8_done:
			state = SM8_done;
			break;
		default:
			state = SM8_init;
			break;
	}
	switch(state){
		case SM8_init:
			pastpattern =0;
			miss =0;
			count=0;
			hit=0;
			break;
		case SM8_checkbuttons:
			pastpattern = currentpattern;
			if(currentpattern != 0 ){
				count++;
				if((currentpattern == 1 && GetBit(button_presses,6)) ||
				(currentpattern == 2 && GetBit(button_presses,7)) ||
				(currentpattern == 3 && GetBit(button_presses,5)) ||
				(currentpattern == 4 && GetBit(button_presses,4)) ||
				(currentpattern == 5 && GetBit(button_presses,6) && GetBit(button_presses,7)) ||
				(currentpattern == 6 && GetBit(button_presses,6) && GetBit(button_presses,5)) ||
				(currentpattern == 7 && GetBit(button_presses,4) && GetBit(button_presses,6)) ||
				(currentpattern == 8 && GetBit(button_presses,5) && GetBit(button_presses,7)) ||
				(currentpattern == 9 && GetBit(button_presses,4) && GetBit(button_presses,7)) ||
				(currentpattern == 10 && GetBit(button_presses,4) && GetBit(button_presses,5)) ||
				(currentpattern == 11 && GetBit(button_presses,6) && GetBit(button_presses,5) && GetBit(button_presses,7)) ||
				(currentpattern == 12 && GetBit(button_presses,4) && GetBit(button_presses,5) && GetBit(button_presses,7)) ||
				(currentpattern == 13 && GetBit(button_presses,6) && GetBit(button_presses,5) && GetBit(button_presses,4)) ||
				(currentpattern == 14 && GetBit(button_presses,6) && GetBit(button_presses,4) && GetBit(button_presses,7)) ||
				(currentpattern == 15 && GetBit(button_presses,6) && GetBit(button_presses,5) && GetBit(button_presses,7) &&  GetBit(button_presses,4)) ){
					miss = 0;
					hit = 1;
				}
				else{
					miss =1;
				}
			}
			else{
				count=0;
			}
			break;
		case SM8_pointup:
			showScore(31);
			hit =0;
			break;
			case SM8_pointdown:
			if(score <0){
				lose =1;
			}
			showScore(31);
			miss =0;
			break;
		default:
			break;
	}
	return state;
}



void showScore(unsigned char line)
{
	int tens = 0;
	int ones = 0;
	if(score > 9)
	{
		tens = score/10;
		ones = score % 10;
	}
	else{
		ones = score;
	}
	LCD_Cursor(line);
	LCD_WriteData(tens + '0');
	
	LCD_Cursor(line+1);
	LCD_WriteData(ones + '0');
}

void createSymbol(unsigned char save, unsigned char *ptr)
{
	LCD_WriteCommand(save);//starts at 0x40, then 0x48.....
	for(unsigned char i=0; i<8; i++){
		LCD_WriteData(ptr[i]);
	}
	LCD_WriteCommand(0x80);
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0xCF; PORTA = 0x30; //snes + lcd output pins 0 &1
	DDRD = 0xFF; PORTD = 0x00; //lcd ouput
	DDRC = 0xFF; PORTC = 0x00; //led ouput
	
	LCD_init();
	
	unsigned char up_pattern[] = {0x04, 0x0e, 0x1f, 0x15, 0x04, 0x04, 0x04, 0x04};
	unsigned char down_pattern[] = {0x04, 0x04, 0x04, 0x04, 0x15, 0x1f, 0x0e, 0x04};
	unsigned char left_pattern[] = {0x00, 0x04, 0x0c, 0x1f, 0x0c, 0x04, 0x00, 0x0};
	unsigned char right_pattern[] = {0, 0x04, 0x06, 0x1f, 0x06, 0x04, 0, 0};

	createSymbol(0x40, up_pattern);
	createSymbol(0x48, down_pattern);
	createSymbol(0x50, left_pattern);
	createSymbol(0x58 ,right_pattern);
	
	unsigned long int SMTick1_periodcalc = 600;
	unsigned long int SMTick2_periodcalc = 600;
	unsigned long int SMTick3_periodcalc = 600;
	unsigned long int SMTick4_periodcalc = 600;
	unsigned long int SMTick5_periodcalc = 1000;
	unsigned long int SMTick6_periodcalc = 600;
	unsigned long int SMTick7_periodcalc = 60;
	unsigned long int SMTick8_periodcalc = 60;
	unsigned long int SMTick9_periodcalc = 100;

	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_periodcalc, SMTick2_periodcalc);
	tmpGCD = findGCD(tmpGCD, SMTick3_periodcalc);
	tmpGCD = findGCD(tmpGCD, SMTick4_periodcalc);
	tmpGCD = findGCD(tmpGCD, SMTick5_periodcalc);
	tmpGCD = findGCD(tmpGCD, SMTick6_periodcalc);
	tmpGCD = findGCD(tmpGCD, SMTick7_periodcalc);
	tmpGCD = findGCD(tmpGCD, SMTick8_periodcalc);
	tmpGCD = findGCD(tmpGCD, SMTick9_periodcalc);
	
	unsigned long int GCD = tmpGCD;
	
	unsigned long int SMTick1_period = SMTick1_periodcalc/GCD;
	unsigned long int SMTick2_period = SMTick2_periodcalc/GCD;
	unsigned long int SMTick3_period = SMTick3_periodcalc/GCD;
	unsigned long int SMTick4_period = SMTick4_periodcalc/GCD;
	unsigned long int SMTick5_period = SMTick5_periodcalc/GCD;
	unsigned long int SMTick6_period = SMTick6_periodcalc/GCD;
	unsigned long int SMTick7_period = SMTick7_periodcalc/GCD;
	unsigned long int SMTick8_period = SMTick8_periodcalc/GCD;
	unsigned long int SMTick9_period = SMTick9_periodcalc/GCD;
	
	TimerSet(GCD);
	TimerOn();
	
	static task task1, task2, task3, task4, task5, task6, task7, task8, task9;
	task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6, &task7, &task8, &task9};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	task1.state = -1;
	task1.period = SMTick1_period;
	task1.elapsedTime = SMTick1_period;
	task1.TickFct = &SMTick1;
	
	task2.state = -1;
	task2.period = SMTick2_period;
	task2.elapsedTime = SMTick2_period;
	task2.TickFct = &SMTick2;
	
	task3.state = -1;
	task3.period = SMTick3_period;
	task3.elapsedTime = SMTick3_period;
	task3.TickFct = &SMTick3;
	
	task4.state = -1;
	task4.period = SMTick4_period;
	task4.elapsedTime = SMTick4_period;
	task4.TickFct = &SMTick4;
	
	task5.state = -1;
	task5.period = SMTick5_period;
	task5.elapsedTime = SMTick5_period;
	task5.TickFct = &SMTick5;
	
	task6.state = -1;
	task6.period = SMTick6_period;
	task6.elapsedTime = SMTick6_period;
	task6.TickFct = &SMTick6;
	
	task7.state = -1;
	task7.period = SMTick7_period;
	task7.elapsedTime = SMTick7_period;
	task7.TickFct = &SMTick7;
	
	task8.state = -1;
	task8.period = SMTick8_period;
	task8.elapsedTime = SMTick8_period;
	task8.TickFct = &SMTick8;
	
	task9.state = -1;
	task9.period = SMTick9_period;
	task9.elapsedTime = SMTick9_period;
	task9.TickFct = &SMTick9;
	
	unsigned short i;
	
	while(1) {
		for(i=0;i<numTasks;i++){
			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime=0;
			}
			tasks[i]->elapsedTime+=1;
		}
		while(!TimerFlag);
		TimerFlag=0;
		
	}
}
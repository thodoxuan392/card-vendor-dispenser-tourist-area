/*
 * lcd.c
 *
 *  Created on: May 14, 2023
 *      Author: xuanthodo
 */


#include "main.h"
#include "Device/lcd.h"
#include "Hal/gpio.h"

/**
 * Section: define Macros and constants
 */

//basic commands
#define LCD_FUNCTIONSET 		0x30u
#define LCD_DISPLAYCONTROL 		0x08u
#define LCD_DISPLAYCLEAR 		0x01u
#define LCD_RETURNHOME			0x02u
#define LCD_ENTRYMODE	 		0x04u
#define LCD_SETDRAMADDRESS 		0x80u // or-ed with address

//extended commands
#define LCD_FUNCTIONEXTEND 					0x24u
#define LCD_FUNCTIONSCROLLORADDRESSSELECT 	0x02u
#define LCD_DISPLAYCONTROLEXTEND 			0x0Cu
#define LCD_SETGRAPHICADDRESS 				0x80u // or-ed with address

//FunctionSet features
#define LCD_INTERFACE_8BITS 0x10u
#define LCD_INTERFACE_4BITS 0x00u
#define LCD_GRAPHIC_ON      0x02u // only for extended
#define LCD_GRAPHIC_OFF     0x00u // only for extended

//DisplayControl features
#define LCD_DISPLAY_ON 		0x04u
#define LCD_DISPLAY_OFF		0x00u
#define LCD_CURSOR_ON		0x02u
#define LCD_CURSOR_OFF		0x00u
#define LCD_BLINK_ON		0x01u
#define LCD_BLINK_OFF		0x00u

//EntryMode features
#define LCD_CURSOR_MOVE_RIGHT 0x02u
#define LCD_CURSOR_MOVE_LEFT  0x00u
#define LCD_DISPLAY_SHIFT	  0x01u
#define LCD_DISPLAY_NO_SHIFT  0x00u


// data types

enum SCREEN_SECTION{
  SCREEN_COMPLETE,
  SCREEN_TOP_HALF,
  SCREEN_BOTTOM_HALF,
  SCREEN_QUADRANT_1,
  SCREEN_QUADRANT_2,
  SCREEN_QUADRANT_3,
  SCREEN_QUADRANT_4
};

enum {
	LCD_RS,
	LCD_RW,
	LCD_ENABLE,
	LCD_D0,
	LCD_D1,
	LCD_D2,
	LCD_D3,
	LCD_D4,
	LCD_D5,
	LCD_D6,
	LCD_D7
};

static GPIO_info_t gpio_table[] = {
		[LCD_RS] = 		{GPIOB, { GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}},
		[LCD_RW] = 		{GPIOB,{ GPIO_PIN_14, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_ENABLE] = 	{GPIOB,{ GPIO_PIN_15, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D0] = 		{GPIOD,{ GPIO_PIN_8, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D1] = 		{GPIOD,{ GPIO_PIN_9, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D2] = 		{GPIOD,{ GPIO_PIN_10, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D3] = 		{GPIOD,{ GPIO_PIN_11, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D4] = 		{GPIOD,{ GPIO_PIN_12, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D5] = 		{GPIOD,{ GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D6] = 		{GPIOD,{ GPIO_PIN_14, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
		[LCD_D7] = 		{GPIOD,{ GPIO_PIN_15, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH }},
};

static const uint8_t offset_line[] = { 0x80u, 0x90u, 0x88u, 0x98u };
static uint8_t _display_control;
static uint8_t _display_function;

static bool LCD_begin();
static bool LCD_delay_us(size_t us);
static bool LCD_delay_ms(size_t ms);
static bool LCD_send_cmd(uint8_t cmd);
static bool LCD_send_data(uint8_t data);


bool LCD_init(){
	// Init GPIO
	int nb_io = sizeof(gpio_table)/sizeof(GPIO_info_t);
	for (uint8_t var = 0; var < nb_io; ++var) {
		HAL_GPIO_Init(gpio_table[var].port, &gpio_table[var].init_info);
	}
	// Enable
	HAL_GPIO_WritePin(gpio_table[LCD_ENABLE].port, gpio_table[LCD_ENABLE].init_info.Pin, SET);
	LCD_begin();
	LCD_text_mode();
	LCD_clear();
	return true;
}



void LCD_clear(){
	LCD_send_cmd(LCD_DISPLAYCLEAR);
}

void LCD_home(){
	LCD_send_cmd(LCD_RETURNHOME);
}

void LCD_display(){
	_display_control |= (LCD_DISPLAY_ON);
	LCD_send_cmd( LCD_DISPLAYCONTROL | _display_control );
}

void LCD_no_display(){
	_display_control &= ~(LCD_DISPLAY_ON);
	LCD_send_cmd( LCD_DISPLAYCONTROL | _display_control );
}
void LCD_blink(){
	_display_control |= (LCD_BLINK_ON);
	LCD_send_cmd( LCD_DISPLAYCONTROL | _display_control );
}
void LCD_no_blink(){
	_display_control &= ~(LCD_BLINK_ON);
	LCD_send_cmd( LCD_DISPLAYCONTROL | _display_control );
}
void LCD_set_cursor(uint8_t row, uint8_t col){
	if( row >= 4u)
		row = 3u;

	if( col >= 8u )
		col = 7u;

	LCD_send_cmd( LCD_SETDRAMADDRESS | (offset_line[row] + col ) );
}

void LCD_text_mode(){
	  //Function Set
	  LCD_send_cmd( LCD_FUNCTIONSET | LCD_INTERFACE_8BITS );
	  //Wait time > 100uS
	  LCD_delay_us(150);

	  //Function Set
	  LCD_send_cmd( LCD_FUNCTIONSET | LCD_INTERFACE_8BITS );
	  //Wait time > 37uS
	  LCD_delay_us(60);

	  //Display ON/OFF  Control
	  LCD_send_cmd( LCD_DISPLAYCONTROL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF );
	  //Wait time > 100uS
	  LCD_delay_us(150);

	  LCD_send_cmd( LCD_RETURNHOME );
	  //Wait time > 10mS
	  LCD_delay_ms(20);

	  LCD_send_cmd( LCD_ENTRYMODE| LCD_CURSOR_MOVE_RIGHT | LCD_DISPLAY_NO_SHIFT );

	  LCD_delay_ms(50);
}
void LCD_display_str(char * str){
	size_t str_len = strlen(str);
	for (int var = 0; var < str_len; ++var) {
		LCD_send_data(str[var]);
	}
}

static bool LCD_begin(){
	_display_control = LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF;
	_display_function = LCD_INTERFACE_8BITS;


	LCD_send_cmd( LCD_FUNCTIONEXTEND | _display_function | LCD_GRAPHIC_OFF );
	LCD_delay_ms(1);
	LCD_send_cmd( LCD_FUNCTIONSET | _display_function );
	LCD_delay_ms(1);

	LCD_send_cmd( LCD_DISPLAYCLEAR );
	LCD_delay_ms(1);

	LCD_send_cmd( LCD_ENTRYMODE | LCD_CURSOR_MOVE_RIGHT | LCD_DISPLAY_NO_SHIFT );
	LCD_delay_ms(1);

	LCD_send_cmd( LCD_DISPLAYCONTROL | _display_control );

	LCD_delay_us(80);
	LCD_delay_us(80);

	LCD_send_cmd( LCD_FUNCTIONEXTEND | _display_function | LCD_GRAPHIC_OFF );
	LCD_delay_ms(1);
	LCD_send_cmd( LCD_FUNCTIONSCROLLORADDRESSSELECT );
	LCD_delay_ms(1);

	//Clear screen
	for( uint8_t i = 0; i < 32; i++ ){
	   LCD_send_cmd( LCD_SETGRAPHICADDRESS | i ); // y
	   LCD_delay_ms(1);
	   LCD_send_cmd( LCD_SETGRAPHICADDRESS | 0 ); // x
	   LCD_delay_ms(1);

	   for( uint8_t j = 0; j < 32; j++ ){
		LCD_delay_ms(1);
		LCD_send_data( 0u );
		}
	}
}

static bool LCD_send_cmd(uint8_t cmd){
	HAL_GPIO_WritePin(gpio_table[LCD_RS].port, gpio_table[LCD_RS].init_info.Pin, RESET);
	HAL_GPIO_WritePin(gpio_table[LCD_RW].port, gpio_table[LCD_RW].init_info.Pin, RESET);
	HAL_GPIO_WritePin(gpio_table[LCD_D7].port, gpio_table[LCD_D7].init_info.Pin, (cmd >> 7) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D6].port, gpio_table[LCD_D6].init_info.Pin, (cmd >> 6) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D5].port, gpio_table[LCD_D5].init_info.Pin, (cmd >> 5) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D4].port, gpio_table[LCD_D4].init_info.Pin, (cmd >> 4) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D3].port, gpio_table[LCD_D3].init_info.Pin, (cmd >> 3) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D2].port, gpio_table[LCD_D2].init_info.Pin, (cmd >> 2) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D1].port, gpio_table[LCD_D1].init_info.Pin, (cmd >> 1) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D0].port, gpio_table[LCD_D0].init_info.Pin, (cmd) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_RW].port, gpio_table[LCD_RW].init_info.Pin, SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(gpio_table[LCD_RW].port, gpio_table[LCD_RW].init_info.Pin, RESET);
}

static bool LCD_send_data(uint8_t data){
	HAL_GPIO_WritePin(gpio_table[LCD_RS].port, gpio_table[LCD_RS].init_info.Pin, SET);
	HAL_GPIO_WritePin(gpio_table[LCD_RW].port, gpio_table[LCD_RW].init_info.Pin, RESET);
	HAL_GPIO_WritePin(gpio_table[LCD_D7].port, gpio_table[LCD_D7].init_info.Pin, (data >> 7) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D6].port, gpio_table[LCD_D6].init_info.Pin, (data >> 6) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D5].port, gpio_table[LCD_D5].init_info.Pin, (data >> 5) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D4].port, gpio_table[LCD_D4].init_info.Pin, (data >> 4) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D3].port, gpio_table[LCD_D3].init_info.Pin, (data >> 3) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D2].port, gpio_table[LCD_D2].init_info.Pin, (data >> 2) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D1].port, gpio_table[LCD_D1].init_info.Pin, (data >> 1) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_D0].port, gpio_table[LCD_D0].init_info.Pin, (data) & 0x01);
	HAL_GPIO_WritePin(gpio_table[LCD_RW].port, gpio_table[LCD_RW].init_info.Pin, SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(gpio_table[LCD_RW].port, gpio_table[LCD_RW].init_info.Pin, RESET);
}

static bool LCD_delay_us(size_t us){
	while(us--);
}

static bool LCD_delay_ms(size_t ms){
	HAL_Delay(ms);
}

bool LCD_test(){
	LCD_display_str("Hello");
}

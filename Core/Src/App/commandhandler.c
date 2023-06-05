/*
 * commandhandler.c
 *
 *  Created on: Jun 3, 2023
 *      Author: xuanthodo
 */


#include "App/commandhandler.h"
#include "App/mqtt.h"

enum {
	COMMANDHANDLE_IDLE,
	COMMANDHANDLE_EXECUTE
};

static uint8_t state = COMMANDHANDLE_IDLE;
static MQTT_message_t message;

static void COMMANDHANDLER_idle();
static void COMMANDHANDLER_execute();

bool COMMANDHANDLER_init(){

}

bool COMMANDHANDLER_run(){
	switch (state) {
		case COMMANDHANDLE_IDLE:
			COMMANDHANDLER_idle();
			break;
		case COMMANDHANDLE_EXECUTE:
			COMMANDHANDLER_execute();
			break;
		default:
			break;
	}
}

static void COMMANDHANDLER_idle(){
	if(MQTT_receive_message(&message)){
		state = COMMANDHANDLE_EXECUTE;
	}
}

static void COMMANDHANDLER_execute(){

}

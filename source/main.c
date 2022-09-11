#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "semphr.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"

/* App */
#include "queue.h"
/*-----------------------------------------------------------*/
 int systimeTime =0;
 float cpu_load =0;
 int task_button_1_in_time =0  , task_button_1_out_time =0  ,task_button_1_Total_time =0;
 int task_button_2_in_time =0 , task_button_2_out_time =0 ,task_button_2_Total_time=0 ;
 int task_Periodic_Transmitter_in_time  =0  , task_Periodic_Transmitter_out_time =0   ,task_Periodic_Transmitter_Total_time=0   ;
 int task_Uart_Receiver_in_time=0   , task_Uart_Receiver_out_time=0   , task_Uart_Receiver_Total_time=0 ;
 int task_Load_1_Simulation_in_time=0   ,task_Load_1_Simulation_out_time=0  ,task_Load_1_Simulation_Total_time=0  ;
 int task_Load_2_Simulation_in_time=0 , task_Load_2_Simulation_out_time =0,task_Load_2_Simulation_Total_time=0 ;

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

// Constant to specify max number of characters in a message
#define MESSAGE_LEN 25

// Max Queue size
#define MESSAGE_QUEUE_LEN 10

// Type definitions
typedef struct
{
	char message[MESSAGE_LEN + 1]; 
	uint8_t message_len; 
} Message_st;


/* Task Handlers */
TaskHandle_t Button_1_task_handler = NULL, Button_2_task_handler = NULL;
TaskHandle_t Periodic_Transmitter_task_handler = NULL, Consumer_task_handler = NULL;
TaskHandle_t Load_1_Simulation_task_handler = NULL, Load_2_Simulation_task_handler = NULL;

/* hooks */

void vApplicationTickHook( void ){
	GPIO_write(PORT_0, PIN0, PIN_IS_HIGH);
	GPIO_write(PORT_0, PIN0, PIN_IS_LOW);
}

/* Prototypes for functions implement tasks */
void Button_1_Monitor_Imp(void *pvParameters);
void Button_2_Monitor_Imp(void *pvParameters);
void Periodic_Transmitter_Imp(void *pvParameters);
void Uart_Receiver_consumer_Imp(void *pvParameters);
void Load_1_Simulation_Imp(void *pvParameters);
void Load_2_Simulation_Imp(void *pvParameters);

/* interprocess comm */
QueueHandle_t message_q;


static void prvSetupHardware( void );


int main( void )
{
	// Messages
	Message_st button1_message, button2_message, string_message;
	
	// Create Message Queue
	message_q = xQueueCreate(MESSAGE_QUEUE_LEN, sizeof(Message_st *));

	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	
	
	/* Create Tasks here*/
	xTaskPeriodicCreate(Button_1_Monitor_Imp, // Function that implements the task.
              "Button1 Task", // Text name for the task.
              100, // Stack size in words, not bytes.
              (void *) &button1_message, // Parameter passed into the task.
              1, // Priority at which the task is created.
              &Button_1_task_handler,
							50); // Used to pass out the created task's handle.
			
	xTaskPeriodicCreate(Button_2_Monitor_Imp, // Function that implements the task.
              "Button2 Task", // Text name for the task.
              100, // Stack size in words, not bytes.
              (void *) &button2_message, // Parameter passed into the task.
              1, // Priority at which the task is created.
              &Button_2_task_handler,
							50							); // Used to pass out the created task's handle.
	
			
	xTaskPeriodicCreate(Periodic_Transmitter_Imp, // Function that implements the task.
              "String Task", // Text name for the task.
              100, // Stack size in words, not bytes.
              (void *) &string_message, // Parameter passed into the task.
              1, // Priority at which the task is created.
              &Periodic_Transmitter_task_handler, 
							100); // Used to pass out the created task's handle.
							
	
	// Consumer Task					
	xTaskPeriodicCreate(Uart_Receiver_consumer_Imp, // Function that implements the task.
              "Consumer Task", // Text name for the task.
              100, // Stack size in words, not bytes.
              (void *) NULL, // Parameter passed into the task.
              1, // Priority at which the task is created.
              &Consumer_task_handler,
							20); // Used to pass out the created task's handle.

	// Load_1 Simulation Task					
	xTaskPeriodicCreate(Load_1_Simulation_Imp, // Function that implements the task.
              "Load_1 Simulation Task	", // Text name for the task.
              100, // Stack size in words, not bytes.
              (void *) NULL, // Parameter passed into the task.
              1, // Priority at which the task is created.
              &Load_1_Simulation_task_handler,
							10); // Used to pass out the created task's handle.
							
	// Load_2 Simulation Task			
	xTaskPeriodicCreate(Load_2_Simulation_Imp, // Function that implements the task.
              "Load_2 Simulation Task	", // Text name for the task.
              100, // Stack size in words, not bytes.
              (void *) NULL, // Parameter passed into the task.
              1, // Priority at which the task is created.
              &Load_2_Simulation_task_handler,
							100); // Used to pass out the created task's handle.
                	
	vTaskStartScheduler();

	for( ;; );
}


void Timer1Reset(){
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

static void configTimer1(){
	T1PR=1000; //high speed time(20khz) while system tick is 1khz
	T1TCR |=0x1;
	
}
static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();

	configTimer1();
	
	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}


void Button_1_Monitor_Imp(void *task_message)
{
	
	TickType_t xLastWakeTime=xTaskGetTickCount();
	// Task Message
	Message_st *button1_message_ptr = (Message_st *)task_message;
	
	// Button States
	pinState_t button1_prev_state = PIN_IS_LOW;
	pinState_t button1_curr_state = button1_prev_state;
	vTaskSetApplicationTaskTag(NULL, (void *) 1);
	for(; ;)
	{
		
		
		// Detect state transition (rising/falling edge detection)
		if(button1_curr_state != button1_prev_state)
		{
			if(button1_curr_state == PIN_IS_HIGH) // prev is LOW -> rising edge
			{
				// Add message body to the packet
				strcpy(button1_message_ptr->message, "Button1 Rising Edge\n");
				
				// Add message length to the packet
				button1_message_ptr->message_len = (uint8_t) strlen(button1_message_ptr->message);
			}
			else // prev is HIGH, curr is LOW -> falling edge
			{
				// Add message body to the packet
				strcpy(button1_message_ptr->message, "Button1 Falling Edge\n");
				
				// Add message length to the packet
				button1_message_ptr->message_len = (uint8_t) strlen(button1_message_ptr->message);
			}
			
			// Send message to queue
			xQueueSend(message_q,(void *) &button1_message_ptr, portMAX_DELAY);
		}
		
		// Save current state
		button1_prev_state = button1_curr_state;
		
		vTaskDelayUntil(&xLastWakeTime,50);
		
		// Read pin state
		button1_curr_state = GPIO_read(PORT_0, PIN7);
		
	}
}

void Button_2_Monitor_Imp(void *task_message)
{
	TickType_t xLastWakeTime=xTaskGetTickCount();
	
	// Task Message
	Message_st *button2_message_ptr = (Message_st *)task_message;
	
	// Button States
	pinState_t button2_prev_state = PIN_IS_LOW;
	pinState_t button2_curr_state = button2_prev_state;
	vTaskSetApplicationTaskTag(NULL, (void *) 2);
	for(; ;)
	{
		
		
		// Detect state transition (rising/falling edge detection)
		if(button2_curr_state != button2_prev_state)
		{
			if(button2_curr_state == PIN_IS_HIGH) // prev is LOW -> rising edge
			{
				// Add message body to the packet
				strcpy(button2_message_ptr->message, "Button2 Rising Edge\n");
				
				// Add message length to the packet
				button2_message_ptr->message_len = strlen(button2_message_ptr->message);
			}
			else // prev is HIGH, curr is LOW -> falling edge
			{
				// Add message body to the packet
				strcpy(button2_message_ptr->message, "Button2 Falling Edge\n");
				
				// Add message length to the packet
				button2_message_ptr->message_len = strlen(button2_message_ptr->message);
			}
			
		// Send message to queue
			xQueueSend(message_q, (void *) &button2_message_ptr, portMAX_DELAY);
		}
		
		// Save current state
		button2_prev_state = button2_curr_state;
		

		vTaskDelayUntil(&xLastWakeTime,50);
		
		// Read pin states
		button2_curr_state = GPIO_read(PORT_0, PIN8);
		
	}
}

void Periodic_Transmitter_Imp(void *task_message)
{
	
	TickType_t xLastWakeTime=xTaskGetTickCount();
	// Task Message
	Message_st *string_message_ptr = (Message_st *)task_message;
	
	// Add message body to the packet
	strcpy(string_message_ptr->message, "Periodic Message\n");
	
	// Add message length to the packet
	string_message_ptr->message_len = (uint8_t) strlen(string_message_ptr->message);
	vTaskSetApplicationTaskTag(NULL, (void *) 3);
	for(; ;)
	{
// Send message to queue
		xQueueSend(message_q, (void *) &string_message_ptr, portMAX_DELAY);
		
		// Run every 100 ms
		vTaskDelayUntil(&xLastWakeTime,100);
	}
}

void Uart_Receiver_consumer_Imp(void *task_parameters)
{
	TickType_t xLastWakeTime=xTaskGetTickCount();
	
	// Task Message
	Message_st *consumer_message_ptr;
	vTaskSetApplicationTaskTag(NULL, (void *) 4);
	for(; ;)
	{
		// Receive message from queue
		xQueueReceive(message_q, (void *) &consumer_message_ptr, portMAX_DELAY);
			
		// Printing message
		vSerialPutString((const signed char *)consumer_message_ptr->message, consumer_message_ptr->message_len);
		
		vTaskDelayUntil(&xLastWakeTime,20);
	}
}


void Load_1_Simulation_Imp(void *pvParameters){
	
	TickType_t xLastWakeTime=xTaskGetTickCount();
	int i;
	vTaskSetApplicationTaskTag(NULL, (void *) 5);
	for(; ;){
		
		for(i=0;i<33333;i++){ 
			i=i;
		}
		vTaskDelayUntil(&xLastWakeTime,10);
	}
}


void Load_2_Simulation_Imp(void *pvParameters){
	
	TickType_t xLastWakeTime=xTaskGetTickCount();
	int i;
	
	vTaskSetApplicationTaskTag(NULL, (void *) 6);
	for(; ;){
		
		for(i=0;i<80000;i++){
			i=i;
		}
		
		vTaskDelayUntil(&xLastWakeTime,100);
	}
	
}



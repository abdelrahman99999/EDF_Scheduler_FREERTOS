

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <lpc21xx.h>
#include "GPIO.h" //for trace hooks


 
 /////////////
 #define configUSE_EDF_SCHEDULER 1
 #define initIDLEPeriod  200
 /////////////
 
#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			1
#define configCPU_CLOCK_HZ			( ( unsigned long ) 60000000 )	/* =12.0MHz xtal multiplied by 5 using the PLL. */
#define configTICK_RATE_HZ			( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES		( 4 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 90 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) 13 * 1024 )
#define configMAX_TASK_NAME_LEN		( 8 )
#define configUSE_TRACE_FACILITY	1
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
////////////////////
#define configUSE_TIME_SLICING		0
///////////////////
#define configUSE_APPLICATION_TASK_TAG 1
////////////////
#define configUSE_MUTEXES	0
/////////////

#define configQUEUE_REGISTRY_SIZE 	0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1


/* Trace Hooks */ 

extern int systimeTime;
extern float cpu_load;
extern int task_button_1_in_time   , task_button_1_out_time  ,task_button_1_Total_time ;
extern int task_button_2_in_time  , task_button_2_out_time  ,task_button_2_Total_time ;
extern int task_Periodic_Transmitter_in_time    , task_Periodic_Transmitter_out_time    ,task_Periodic_Transmitter_Total_time   ;
extern int task_Uart_Receiver_in_time   , task_Uart_Receiver_out_time   , task_Uart_Receiver_Total_time ;
extern int task_Load_1_Simulation_in_time   ,task_Load_1_Simulation_out_time  ,task_Load_1_Simulation_Total_time  ;
extern int task_Load_2_Simulation_in_time , task_Load_2_Simulation_out_time ,task_Load_2_Simulation_Total_time ;




#define traceTASK_SWITCHED_OUT() do\
                                   {\
																		 if((int)pxCurrentTCB -> pxTaskTag ==1)\
																		 {\
																			 GPIO_write(PORT_0 , PIN1 , PIN_IS_LOW);\
																			 task_button_1_out_time = T1TC; \
																			 task_button_1_Total_time += (task_button_1_out_time -task_button_1_in_time );\
																		 }\
																		 else if ((int)pxCurrentTCB -> pxTaskTag == 2)\
																		 {\
																			 GPIO_write(PORT_0 , PIN2 , PIN_IS_LOW);\
																			 task_button_2_out_time = T1TC; \
																			 task_button_2_Total_time += (task_button_2_out_time -task_button_2_in_time );\
																		 }\
																		 else if ((int)pxCurrentTCB -> pxTaskTag == 3)\
																		 {\
																			 GPIO_write(PORT_0 , PIN3 , PIN_IS_LOW);\
																			  task_Periodic_Transmitter_out_time = T1TC; \
																			 task_Periodic_Transmitter_Total_time += (task_Periodic_Transmitter_out_time - task_Periodic_Transmitter_in_time );\
																		 }\
																		 else if ((int)pxCurrentTCB -> pxTaskTag == 4)\
																		 {\
																			 GPIO_write(PORT_0 , PIN4 , PIN_IS_LOW);\
																			  task_Uart_Receiver_out_time = T1TC; \
																			 task_Uart_Receiver_Total_time += (task_Uart_Receiver_out_time - task_Uart_Receiver_in_time );\
																		 }\
																		 else if ((int)pxCurrentTCB -> pxTaskTag == 5)\
																		 {\
																			 GPIO_write(PORT_0 , PIN5 , PIN_IS_LOW);\
																			  task_Load_1_Simulation_out_time = T1TC; \
																			task_Load_1_Simulation_Total_time += (task_Load_1_Simulation_out_time -task_Load_1_Simulation_in_time );\
																		 }\
																		 else if ((int)pxCurrentTCB -> pxTaskTag == 6)\
																		 {\
																			 GPIO_write(PORT_0 , PIN6 , PIN_IS_LOW);\
																			  task_Load_2_Simulation_out_time = T1TC; \
																			 task_Load_2_Simulation_Total_time += (task_Load_2_Simulation_out_time -task_Load_2_Simulation_in_time );\
																		 }\
																		 systimeTime =T1TC;\
	cpu_load =(((task_button_1_Total_time + task_button_2_Total_time +task_Periodic_Transmitter_Total_time+ task_Uart_Receiver_Total_time + task_Load_1_Simulation_Total_time + task_Load_2_Simulation_Total_time) *100 ) / (float)systimeTime);\
																	 }while(0)
#define traceTASK_SWITCHED_IN() do\
                                   {\
																		 if((int)pxCurrentTCB -> pxTaskTag ==1)\
																		 {\
																			 GPIO_write(PORT_0 , PIN1 , PIN_IS_HIGH);\
																			 task_button_1_in_time = T1TC; \
																		 }\
																		 else if ((int)pxCurrentTCB -> pxTaskTag == 2)\
																		 {\
																			 GPIO_write(PORT_0 , PIN2 , PIN_IS_HIGH);\
																			 task_button_2_in_time = T1TC; \
																		 }\
																		 else if ((int)pxCurrentTCB -> pxTaskTag == 3)\
																		 {\
																			 GPIO_write(PORT_0 , PIN3 , PIN_IS_HIGH);\
																			  task_Periodic_Transmitter_in_time = T1TC; \
																		 }\
																		 else if ((int)pxCurrentTCB -> pxTaskTag == 4)\
																		 {\
																			 GPIO_write(PORT_0 , PIN4 , PIN_IS_HIGH);\
																			  task_Uart_Receiver_in_time = T1TC; \
																		 }\
																		 else if ((int)pxCurrentTCB -> pxTaskTag == 5)\
																		 {\
																			 GPIO_write(PORT_0 , PIN5 , PIN_IS_HIGH);\
																			  task_Load_1_Simulation_in_time = T1TC; \
																		 }\
																		 else if ((int)pxCurrentTCB -> pxTaskTag == 6)\
																		 {\
																			 GPIO_write(PORT_0 , PIN6 , PIN_IS_HIGH);\
																			  task_Load_2_Simulation_in_time = T1TC; \
																		 }\
																	 }while(0)


#endif /* FREERTOS_CONFIG_H */

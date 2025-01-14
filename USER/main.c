//通用
#include "sys.h"
#include "delay.h"
#include "usart.h"
//硬件设备
#include "Lcd_Driver.h"
#include "GUI.h"
#include "Picture.h"
#include "led.h"
//操作系统
#include "FreeRTOS.h"
#include "task.h"
 
//任务优先级(起始任务，必要)
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);


//任务优先级
#define LCD_TASK_PRIO		1
//任务堆栈大小	
#define LCD_STK_SIZE 		50  
//任务句柄
TaskHandle_t LCD0Task_Handler;
//任务函数
void LCD_task(void *pvParameters);


//任务优先级
#define LED1_TASK_PRIO		3
//任务堆栈大小	
#define LED1_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void led1_task(void *pvParameters);


 int main(void)
 {	 
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
		delay_init();	    				//延时函数初始化	  
		uart_init(115200);					//初始化串口
		LED_Init();	//初始化LED
		Lcd_Init(); //初始化LCD
		 
		//创建开始任务
			xTaskCreate((TaskFunction_t )start_task,            //任务函数
									(const char*    )"start_task",          //任务名称
									(uint16_t       )START_STK_SIZE,        //任务堆栈大小
									(void*          )NULL,                  //传递给任务函数的参数
									(UBaseType_t    )START_TASK_PRIO,       //任务优先级
									(TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
			vTaskStartScheduler();          //开启任务调度		 
 }

 
 void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建LCD任务
    xTaskCreate((TaskFunction_t )LCD_task,     	
                (const char*    )"LCD_task",   	
                (uint16_t       )LCD_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LCD_TASK_PRIO,	
                (TaskHandle_t*  )&LCD0Task_Handler);  
   
    //创建LED1任务
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);       					
           
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

void LCD_task(void *pvParameters)
{
	LCD_LED_SET;
	Lcd_Clear(WHITE);
	while(1)
	{
		Gui_DrawFont_GBK16(1, 100, BLACK, WHITE, "你好,世界");
		Gui_DrawFont_GBK16(1, 50, BLACK, WHITE, "A");
		Gui_DrawFont_GBK16(1, 1, BLACK, WHITE, "B");
	}
}


//LED1任务函数
void led1_task(void *pvParameters)
{
    while(1)
    {
        LED1=0;
        vTaskDelay(200);
        LED1=1;
        vTaskDelay(800);
    }
}

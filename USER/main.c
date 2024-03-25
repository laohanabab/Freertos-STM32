//ͨ��
#include "sys.h"
#include "delay.h"
#include "usart.h"
//Ӳ���豸
#include "Lcd_Driver.h"
#include "GUI.h"
#include "Picture.h"
#include "led.h"
//����ϵͳ
#include "FreeRTOS.h"
#include "task.h"
 
//�������ȼ�(��ʼ���񣬱�Ҫ)
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);


//�������ȼ�
#define LCD_TASK_PRIO		1
//�����ջ��С	
#define LCD_STK_SIZE 		50  
//������
TaskHandle_t LCD0Task_Handler;
//������
void LCD_task(void *pvParameters);


//�������ȼ�
#define LED1_TASK_PRIO		3
//�����ջ��С	
#define LED1_STK_SIZE 		50  
//������
TaskHandle_t LED1Task_Handler;
//������
void led1_task(void *pvParameters);


 int main(void)
 {	 
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
		delay_init();	    				//��ʱ������ʼ��	  
		uart_init(115200);					//��ʼ������
		LED_Init();	//��ʼ��LED
		Lcd_Init(); //��ʼ��LCD
		 
		//������ʼ����
			xTaskCreate((TaskFunction_t )start_task,            //������
									(const char*    )"start_task",          //��������
									(uint16_t       )START_STK_SIZE,        //�����ջ��С
									(void*          )NULL,                  //���ݸ��������Ĳ���
									(UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
									(TaskHandle_t*  )&StartTask_Handler);   //������              
			vTaskStartScheduler();          //�����������		 
 }

 
 void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����LCD����
    xTaskCreate((TaskFunction_t )LCD_task,     	
                (const char*    )"LCD_task",   	
                (uint16_t       )LCD_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LCD_TASK_PRIO,	
                (TaskHandle_t*  )&LCD0Task_Handler);  
   
    //����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);       					
           
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

void LCD_task(void *pvParameters)
{
	LCD_LED_SET;
	Lcd_Clear(WHITE);
	while(1)
	{
		Gui_DrawFont_GBK16(1, 100, BLACK, WHITE, "���,����");
		Gui_DrawFont_GBK16(1, 50, BLACK, WHITE, "A");
		Gui_DrawFont_GBK16(1, 1, BLACK, WHITE, "B");
	}
}


//LED1������
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

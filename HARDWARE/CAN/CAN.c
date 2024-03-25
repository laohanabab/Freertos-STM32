#include "stm32f10x.h"                  // Device header

#define CAN_ID 0x1234
void CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	
	//CAN_RX PA11
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	//CAN_TX PA12
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	CAN_InitStructure.CAN_TTCM=DISABLE;            //��ʱ�䴥��ͨ��ģʽ��������ʱ������
	CAN_InitStructure.CAN_ABOM=DISABLE;            //����Զ����߹���������˳�����״̬���
	CAN_InitStructure.CAN_AWUM=DISABLE;						 //˯��ģʽͨ���������
	CAN_InitStructure.CAN_NART=ENABLE;						 //��ֹ�����Զ����ͣ�CAN����ֻ�ܱ�����һ�Σ����ܷ��ͽ�����
	CAN_InitStructure.CAN_RFLM=DISABLE;						 //���Ĳ�������������FIFO�ı���δ����������һ���յ��ı��ĻḲ��ԭ�б���
	CAN_InitStructure.CAN_TXFP=DISABLE;						 //���ȼ��ɱ��ı�ʶ���������������ɷ��������˳�����
	CAN_InitStructure.CAN_Mode=mode;						 //ģʽ���ã�0-��ͨģʽ��1-�ػ�ģʽ
	
	CAN_InitStructure.CAN_SJW=tsjw;       //����ͬ����Ծ���
	CAN_InitStructure.CAN_BS1=tbs1;				//tbs1
	CAN_InitStructure.CAN_BS2=tbs2;				//tbs2
	CAN_InitStructure.CAN_Prescaler=brp;	//��Ƶϵ��
	
	CAN_Init(CAN1,&CAN_InitStructure);
	
//	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);  //FIFO��Ϣ�Һ��ж�����
//	
//	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     //
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;            //
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
}

void Filtration(void)
{
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
 
	CAN_FilterInitStructure.CAN_FilterNumber=0;	                        //������0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList; 	    //����λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//32λ��
	
	CAN_FilterInitStructure.CAN_FilterIdHigh=((CAN_ID<<5))&0xffff;	    //ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((0x00<<5))&0xffff;    //32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;  //������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                //���������
 
	CAN_FilterInit(&CAN_FilterInitStructure);			      

}

u8 Can_Send_Msg(u8*msg,u8 sID,u8 len)
{
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=CAN_ID;//��׼��ʶ�� CAN_ID
	TxMessage.ExtId=CAN_ID;//��չ��ʶ�� CAN_ID
	TxMessage.IDE=CAN_Id_Standard;//��׼֡
	TxMessage.RTR=CAN_RTR_Data; //����֡
	TxMessage.DLC=len;
	for(i=0;i<len;i++)
	{
		TxMessage.Data[i]=msg[i];
	}
	mbox=CAN_Transmit(CAN1,&TxMessage);
	i=0;
	
	//�ȴ����ͽ���
	while((CAN_TransmitStatus(CAN1,mbox)==CAN_TxStatus_Failed)&&(i<0xFFF)) i++;
	if(i>=0xFFF)
		return 1;
	return 0;
}

u8 Can_Receive_Msg(u8*buf)
{
	u32 i;
	CanRxMsg RxMessage;
	if(CAN_MessagePending(CAN1,CAN_FIFO0)==0) return 0; //û�н��յ����ݣ�ֱ���˳�
	CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
	for(i=0;i<12;i++)
	buf[i]=RxMessage.Data[i];
	return RxMessage.DLC;
}
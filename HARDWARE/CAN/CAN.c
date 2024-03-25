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
	
	CAN_InitStructure.CAN_TTCM=DISABLE;            //非时间触发通信模式，与生成时间戳相关
	CAN_InitStructure.CAN_ABOM=DISABLE;            //软件自动离线管理，与如何退出离线状态相关
	CAN_InitStructure.CAN_AWUM=DISABLE;						 //睡眠模式通过软件唤醒
	CAN_InitStructure.CAN_NART=ENABLE;						 //禁止报文自动传送，CAN报文只能被发送一次，不管发送结果如何
	CAN_InitStructure.CAN_RFLM=DISABLE;						 //报文不锁定，当接受FIFO的报文未被读出，下一个收到的报文会覆盖原有报文
	CAN_InitStructure.CAN_TXFP=DISABLE;						 //优先级由报文标识符决定，而不是由发送请求的顺序决定
	CAN_InitStructure.CAN_Mode=mode;						 //模式设置：0-普通模式；1-回环模式
	
	CAN_InitStructure.CAN_SJW=tsjw;       //重新同步跳跃宽度
	CAN_InitStructure.CAN_BS1=tbs1;				//tbs1
	CAN_InitStructure.CAN_BS2=tbs2;				//tbs2
	CAN_InitStructure.CAN_Prescaler=brp;	//分频系数
	
	CAN_Init(CAN1,&CAN_InitStructure);
	
//	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);  //FIFO消息挂号中断允许
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
 
	CAN_FilterInitStructure.CAN_FilterNumber=0;	                        //过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList; 	    //屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//32位宽
	
	CAN_FilterInitStructure.CAN_FilterIdHigh=((CAN_ID<<5))&0xffff;	    //ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=((0x00<<5))&0xffff;    //32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;  //过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                //激活过滤器
 
	CAN_FilterInit(&CAN_FilterInitStructure);			      

}

u8 Can_Send_Msg(u8*msg,u8 sID,u8 len)
{
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=CAN_ID;//标准标识符 CAN_ID
	TxMessage.ExtId=CAN_ID;//扩展标识符 CAN_ID
	TxMessage.IDE=CAN_Id_Standard;//标准帧
	TxMessage.RTR=CAN_RTR_Data; //数据帧
	TxMessage.DLC=len;
	for(i=0;i<len;i++)
	{
		TxMessage.Data[i]=msg[i];
	}
	mbox=CAN_Transmit(CAN1,&TxMessage);
	i=0;
	
	//等待发送结束
	while((CAN_TransmitStatus(CAN1,mbox)==CAN_TxStatus_Failed)&&(i<0xFFF)) i++;
	if(i>=0xFFF)
		return 1;
	return 0;
}

u8 Can_Receive_Msg(u8*buf)
{
	u32 i;
	CanRxMsg RxMessage;
	if(CAN_MessagePending(CAN1,CAN_FIFO0)==0) return 0; //没有接收到数据，直接退出
	CAN_Receive(CAN1,CAN_FIFO0,&RxMessage);
	for(i=0;i<12;i++)
	buf[i]=RxMessage.Data[i];
	return RxMessage.DLC;
}
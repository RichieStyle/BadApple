/**
  ******************************************************************************
  * @project Bad Apple
  * @author  Richie Li
  * @version V1.0.0
  * @date    2020.04.06
  * @brief   Main funcation entrance, use C99 mode at least.
  ******************************************************************************
  */
#include "bsp.h"
#include "oled.h"

/* 串口1，波特率230400。
   数据通信格式：[0]帧头0xEE, [1~1024]数据段, [1025]帧尾0xED
   正确接收回复上位机1，接收错误回复0,具体参见bsp_usart.c
   OLED屏幕根据视频中的位置摆放即可，或者重新改写OLED_ShowPicture函数的解码方式
   使用C99标准
 */

extern uint8_t Rcvbuf[15][1024],RxFrame,TxFrame;

int main(void)
{
	BSP_Init();
	OLED_Init();
	OLED_Clear();
	LED0 = 1;
	
	while(1)
	{
		if(RxFrame != TxFrame)
		{
			OLED_ShowPicture(0,0,128,8,Rcvbuf[TxFrame]);
			if(++TxFrame >= 16)	TxFrame = 0;
		}
	}
}

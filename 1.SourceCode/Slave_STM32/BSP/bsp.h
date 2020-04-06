#ifndef __BSP_H
#define __BSP_H
#include "bsp_sys.h"
#include "bsp_delay.h"
#include "bsp_usart.h"

#define SUPPORT_OS	0	//1,支持os


#define LED0 PCout(13)

void LED_Init(void);//初始化
void BSP_Init(void);
		 				    
#endif

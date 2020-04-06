#ifndef __DELAY_H
#define __DELAY_H 			   
#include "bsp.h" 

void delay_init(void);
void delay_us(u32 nus);

#if SUPPORT_OS
void delay_ms(u32 nms);
void delay_xms(u32 nms);
#else
void delay_ms(u16 nms);
#endif

#endif






























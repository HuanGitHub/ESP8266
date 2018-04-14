#include "IO.h"
#include "include.h"
os_timer_t Time_led;
char flag=0;
int Close_led()
{/*{{{*/
	gpio_output_set(BIT12,0,BIT12,0);
	gpio_output_set(0,BIT13,BIT13,0);
	return 0;
}/*}}}*/
int Open_led()
{/*{{{*/
	gpio_output_set(BIT12,0,BIT12,0);
	gpio_output_set(BIT13,0,BIT13,0);
	return 0;
}/*}}}*/
void Init_led()
{/*{{{*/
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U,FUNC_GPIO14);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,FUNC_GPIO12);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U,FUNC_GPIO13);	
	gpio_output_set(0,BIT14,BIT14,0);
	gpio_output_set(BIT12,0,BIT12,0);
	gpio_output_set(0,BIT13,BIT13,0);
	Bit12 = 1; Bit13 = 0; Bit14 = 0;
}/*}}}*/

void GPIO_12_Off()
{
	gpio_output_set(BIT12,0,BIT12,0);
	
}
void GPIO_13_Off()
{
	gpio_output_set(BIT13,0,BIT13,0);
	
}
void GPIO_14_Off()
{
	gpio_output_set(BIT14,0,BIT14,0);
	
}
void GPIO_12_No()
{
	gpio_output_set(0,BIT12,BIT12,0);
	
}
void GPIO_13_No()
{
	gpio_output_set(0,BIT13,BIT13,0);
}
void GPIO_14_No()
{
	
	gpio_output_set(0,BIT14,BIT14,0);
}





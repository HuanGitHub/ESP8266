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
	gpio_output_set(0,BIT12,BIT12,0);
	gpio_output_set(BIT13,0,BIT13,0);
	return 0;
}/*}}}*/
void Init_led()
{/*{{{*/
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,FUNC_GPIO12);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U,FUNC_GPIO13);	
}/*}}}*/
void Led_test1()
{/*{{{*/
	if(flag==0)
	{
		Close_led();
		flag=1;
	}else{
			Open_led();
			flag=0;	
	}
}/*}}}*/
void Led_Gpio12_test()
{/*{{{*/
	
	Init_led();
		os_timer_disarm(&Time_led);
		os_timer_setfn(&Time_led,Led_test1,NULL);
		os_timer_arm(&Time_led,1000,1);
}/*}}}*/

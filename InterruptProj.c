#include <stdio.h>
#include <sys/irqinfo.h>
#include <sys/proxy.h>
#include <sys/kernel.h>
#include <sys/sched.h>
#include <conio.h>
#include <sys/seginfo.h>
#include <sys/name.h>

#include "c3vs_rg.h"
#include "txcri.h"


#pragma off (check_stack);
pid_t far handler(){
   return( proxy );
  }
 #pragma on (check_stack);

void main(){
	int id=0,i=0;
	unsigned short data,flag;
	
	setprio(0,20); //приоритет процесса
	Init();
	
	baseA=qnx_segment_overlay(0xA0000,0x20000);		// Открытие сегмента
    cpA=MK_FP(baseA,0);								// Указатель char
	cpAshort = (unsigned short __far *)cpA;			// Указатель short
    qnx_name_attach(0,"/potok");					// Регистрируем имя процесса
	
	baseB=qnx_segment_overlay(0x0B0000,0x8000);
	cpB=(char __far *)MK_FP(baseB,0);
	cpBshort=(unsigned short __far *)cpB;
	
	if((proxy = qnx_proxy_attach( 0, 0, 0, -1 ) )== -1){
		printf( " Unable to attach proxy. " );
        return;
	}
	if((id = qnx_hint_attach( 11, &handler, FP_SEG(&counter))) == -1 ){ //цепляем 11 прерывание
		printf( "Unable to attach interrupt." );
        return;
	}
	
//_____установка регистров
	data=RV_VME_BASE_ADD_RAM_DATA;
	zzw(RV_VME_BASE_ADD_RAM,data); //устанавливаем источник прерывания IRQ6
	
	data=0x3;
	zzw(RV_VME_CNTR_EXT_INT,data); //устанавливаем разряды признак и SYSFAIL в 1

	StatusRestart();
	


	
//_____вывод 
	printf("Waiting interrupt from Vulkan...\n");
	
	for( i = 0 ; i < 1 ; ++i ) {
		
	 //_____отправляем прерывание на Вулкан
	  printf("Send interrupt on Vulkan...\n");
	  data=0;
	  zzw(RV_VME_INT,data);
	  data=0x1;
	  zzw(RV_VME_INT,data);

	//_____ждем ответного прерывания от Вулкана
      Receive( proxy, 0, 0 );
	  
	//_____ обработка
	 printf("statusID ->%d\n",inp(STATUS_ID)) ; 
     printf( "check_proxy = %d\n",i );

	  StatusRestart(); 
    }
	
	for(i=0;i<5;i++){
		ccw(0x3060+(i<<1),flag); // читаем записанные вулканом регистры +0x60
		printf("xuinya_%d -> %x\n",i,flag) ;
	}
	
	
    qnx_hint_detach( id );
}
  

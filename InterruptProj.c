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
	unsigned short data;
	
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
	if((id = qnx_hint_attach( 11, &handler, FP_SEG(&counter))) == -1 ){
		printf( "Unable to attach interrupt." );
        return;
	}
	
//_____установка регистров
	data=RV_VME_BASE_ADD_RAM_DATA;
	zzw(RV_VME_BASE_ADD_RAM,data); //устанавлием источник прерывания IRQ6
	
	data=0x3;
	zzw(RV_VME_CNTR_EXT_INT,data); //устанавлием разряды признак и SYSFAIL в 1

	StatusRestart();
	
//_____вывод 
	printf("Start...\n");
	
	for( i = 0 ; i < 10 ; ++i ) {
	
      Receive( proxy, 0, 0 );
	  
	  printf("statusID ->%d\n",inp(STATUS_ID)) ; 
      printf( "check_proxy = %d\n",i );
	  
	  StatusRestart();
    }
	
    qnx_hint_detach( id );
}
  

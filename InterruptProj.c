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
   ++counter;
   return( proxy );

  }
 #pragma on (check_stack);

void main(){
	
}
  

////////////////////////// txcri.h /////////////////////////////////
//==================================================================
//      Программа начальной инициализации базового резидентного
//				ПМО вычислительного модуля  на базе микроЭВМ "КРЕДО-3VS".
//==================================================================

void StatusRestart(){
	outpw(Index_VIC,VICR6);
	outp(Data_VIC,(VICR_RESTART ^ 0x80)); //выключили прерывание на VIC (старший разряд в 1)
	
	outpw(Index_VIC,VICR6);
	outp(Data_VIC,(VICR_RESTART));  //включили прерывание на VIC (старший разряд в 0)
	
	inp(STATUS_ID);
	outp(STATUS_ID ,0xAA);
}



void Init(){

//******************************************************************
//       НАСТРАИВАЕМ LIM и VME - окна, VME - страницы
//******************************************************************

// 1-я VME страница  Supervisor data A24   0A.0000h - 0A.FFFFh
		outpw( Index_FPGA, WAR0 );
		outpw( Data_FPGA, 0x00A2 );
		outpw( Index_FPGA, MAR0 );
		outpw( Data_FPGA, 0xE00D );

// 2-я VME страница  Supervisor data A24   0B.8000h - 0B.FFFFh
		outpw( Index_FPGA, WAR1 );
		outpw( Data_FPGA, 0x00BA );
		outpw( Index_FPGA, MAR1 );
		outpw( Data_FPGA, 0xE005 );

// 4-я VME страница  Supervisor data A16   0B.0000h - 0B.7FFFh
		outpw( Index_FPGA, WAR3 );
		outpw( Data_FPGA, 0x00B2 );
		outpw( Index_FPGA, MAR3 );
		outpw( Data_FPGA, 0xA005 );

// запрещаем nvram
		outpw( Index_R400EX, PCS1 );
		outpw( Data_R400EX, 0 );

//******************************************************************
//       НАСТРАИВАЕМ VIC
//******************************************************************

// Первичная настройка прерываний по VME
		outpw( Index_VIC, 0x001B );
		outp( Data_VIC, 0x0FE );

// РАЗМАСКИРОВАНИЕ IRQ11 в SLAVE контроллере
        b=inp(0x0A1);
        outp(0x0A1,b&0x0F7);

// РАЗРЕШЕНИЕ ПРЕРЫВАНИЯ В ПРОЦЕССОРНОМ МОДУЛЕ
        outp(STATUS_ID,0x0AA);

//инициализируем VIC
//INI_VIC:
// обнуление регистра немаскируемых прерываний - NSR FPGA1
		outpw( Index_FPGA, NSR );
		outpw( Data_FPGA, 0 );

//устанавливаем в SYSRST 0-й бит - инициализация VICa и сбрасываем остальные
		outp( Index_RTC, SYSRST|0x80 );
		outp( Data_RTC, 1 );

/*   //разрешение работы WDT  Значение SYCOR(индекс 01h) в AX.
		outpw( Index_FPGA, SYCOR );
		w=inpw(Data_FPGA);
		outpw( Data_FPGA, w|0x8000);  //Разрешение WatchDog.
*/
// читаем номер версии м/с VIC
		outpw( Index_VIC, ICR5 );
		inp(Data_VIC);    //сохраняем зн-е номера версии м/с VICa

// запрещаем WDT
		outpw( Index_FPGA, SSR );
		w=inpw(Data_FPGA);             //сброс RESET по WatchDog
		w=w^0x8000;	                   //Сброс схем контроля
		outpw( Data_FPGA, w|3 );       //питающих напряжений.
		outpw( Index_FPGA, SYCOR );
		w=inpw(Data_FPGA);
		outpw( Data_FPGA, w&0x7FFF );  //Запрет WatchDog.

// сбрасываем в SYSRST 0-й бит - инициализация VICa
		b=inp(SYSRST);             //?????
		outp( Index_RTC, b|0x80);  //?????
		b=inp(Data_RTC);           //?????
		outp( Data_RTC, b&0xFE);   //?????

VIC_RST:
// считываем значение ICR6
		outpw( Index_VIC, ICR6 );
		b=inp(Data_VIC);
		if(b&0x80) goto	VIC_RST;  //VIC не закончил инициацию

// настраиваем регистр ICR (регистрация ошибок на шине VME
// по сигналу LBERR) биты 4 и 3 - 10
		outpw( Index_VIC, ICR );
		b=inp(Data_VIC); b=b&0xF7;
		outp( Data_VIC, b|0x10 );

// clear BESR
		outpw( Index_VIC, BESR );
		outp( Data_VIC, 0 );

//обнуление регистра немаскируемых прерываний - NSR FPGA1
		outpw( Index_FPGA, NSR );
		outp( Data_FPGA, 0 );

//настраиваем регистр LBTR - PAS -4T,  DS - 2T,   PAS - PAS - 4T
		outpw( Index_VIC, LBTR );
		outp( Data_VIC, 0x72 );

//настраиваем регистр TTR  - включаем арбитраж времени на шине
//(16 mkc) запрещаем арбитраж на лок шине
		outpw( Index_VIC, TTR );
		outp( Data_VIC, 0x3E );

//запрещаем выработку сигнила SYSFAIL VICу
		outpw( Index_VIC, ICR7 );
		b=inp(Data_VIC);
		outp( Data_VIC, b|0x80 );

//******************************************************************
//       РАЗРЕШАЕМ LIM, VME окно, VME страницы
//******************************************************************

//Значение SYCOR(индекс 01h) в AX.
		outpw( Index_FPGA, SYCOR );
		outpw( Data_FPGA, 0x0080 );  //0000h   //Запрет: WatchDog
	 								//Резрешам кеш-ПЗУ
	 								//ОЗУ - 41h, 42h, 43h, 44h, 45h

//******************************************************************
//       ПРОВЕРЯЕМ СОСТОЯНИЕ VIC RESET
//******************************************************************
//дожидаемся завершения RESET
//считываем зн-е регистра POWER_UP
VIC_IN_RST:
		outpw( Index_FPGA, POWER_UP );
		w=inpw(Data_FPGA);
		if((w&0x0400)!=0) goto VIC_IN_RST;

//clear BESR
		outpw( Index_VIC, BESR );
		outp( Data_VIC, 0 );

//обнуление взведенных битов ненорм регистра немаскируемых
//прерываний - NSR FPGA
		outpw( Index_FPGA, NSR );
		outpw( Data_FPGA, 0 );

//******************************************************************
//       ПРОВОДИМ ИНИЦИАЛИЗАЦИЮ БЛОКОВ НА ШИНЕ VME
//******************************************************************

INIT_BL:
//блоки прошли инициализацию (SYSFAIL не вырабатывают)
		outpw( Index_VIC, EGICR );
		b=inp(Data_VIC); b=b&8;
		if(b==8) goto INIT_BL;  //блоки не закончили инициацию

}// init

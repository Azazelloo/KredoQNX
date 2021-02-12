////////////////////////// txcri.h /////////////////////////////////
//==================================================================
//      �ணࠬ�� ��砫쭮� ���樠����樨 �������� १����⭮��
//				��� ���᫨⥫쭮�� �����  �� ���� ���஝�� "�����-3VS".
//==================================================================

void StatusRestart(){
	outpw(Index_VIC,VICR6);
	outp(Data_VIC,(VICR_RESTART ^ 0x80)); //�몫�稫� ���뢠��� �� VIC (���訩 ࠧ�� � 1)
	
	outpw(Index_VIC,VICR6);
	outp(Data_VIC,(VICR_RESTART));  //����稫� ���뢠��� �� VIC (���訩 ࠧ�� � 0)
	
	inp(STATUS_ID);
	outp(STATUS_ID ,0xAA);
}



void Init(){

//******************************************************************
//       ����������� LIM � VME - ����, VME - ��࠭���
//******************************************************************

// 1-� VME ��࠭��  Supervisor data A24   0A.0000h - 0A.FFFFh
		outpw( Index_FPGA, WAR0 );
		outpw( Data_FPGA, 0x00A2 );
		outpw( Index_FPGA, MAR0 );
		outpw( Data_FPGA, 0xE00D );

// 2-� VME ��࠭��  Supervisor data A24   0B.8000h - 0B.FFFFh
		outpw( Index_FPGA, WAR1 );
		outpw( Data_FPGA, 0x00BA );
		outpw( Index_FPGA, MAR1 );
		outpw( Data_FPGA, 0xE005 );

// 4-� VME ��࠭��  Supervisor data A16   0B.0000h - 0B.7FFFh
		outpw( Index_FPGA, WAR3 );
		outpw( Data_FPGA, 0x00B2 );
		outpw( Index_FPGA, MAR3 );
		outpw( Data_FPGA, 0xA005 );

// ����頥� nvram
		outpw( Index_R400EX, PCS1 );
		outpw( Data_R400EX, 0 );

//******************************************************************
//       ����������� VIC
//******************************************************************

// ��ࢨ筠� ����ன�� ���뢠��� �� VME
		outpw( Index_VIC, 0x001B );
		outp( Data_VIC, 0x0FE );

// ��������������� IRQ11 � SLAVE ����஫���
        b=inp(0x0A1);
        outp(0x0A1,b&0x0F7);

// ���������� ���������� � ������������ ������
        outp(STATUS_ID,0x0AA);

//���樠�����㥬 VIC
//INI_VIC:
// ���㫥��� ॣ���� ����᪨�㥬�� ���뢠��� - NSR FPGA1
		outpw( Index_FPGA, NSR );
		outpw( Data_FPGA, 0 );

//��⠭�������� � SYSRST 0-� ��� - ���樠������ VICa � ���뢠�� ��⠫��
		outp( Index_RTC, SYSRST|0x80 );
		outp( Data_RTC, 1 );

/*   //ࠧ�襭�� ࠡ��� WDT  ���祭�� SYCOR(������ 01h) � AX.
		outpw( Index_FPGA, SYCOR );
		w=inpw(Data_FPGA);
		outpw( Data_FPGA, w|0x8000);  //����襭�� WatchDog.
*/
// �⠥� ����� ���ᨨ �/� VIC
		outpw( Index_VIC, ICR5 );
		inp(Data_VIC);    //��࠭塞 ��-� ����� ���ᨨ �/� VICa

// ����頥� WDT
		outpw( Index_FPGA, SSR );
		w=inpw(Data_FPGA);             //��� RESET �� WatchDog
		w=w^0x8000;	                   //���� �奬 ����஫�
		outpw( Data_FPGA, w|3 );       //������ ����殮���.
		outpw( Index_FPGA, SYCOR );
		w=inpw(Data_FPGA);
		outpw( Data_FPGA, w&0x7FFF );  //����� WatchDog.

// ���뢠�� � SYSRST 0-� ��� - ���樠������ VICa
		b=inp(SYSRST);             //?????
		outp( Index_RTC, b|0x80);  //?????
		b=inp(Data_RTC);           //?????
		outp( Data_RTC, b&0xFE);   //?????

VIC_RST:
// ���뢠�� ���祭�� ICR6
		outpw( Index_VIC, ICR6 );
		b=inp(Data_VIC);
		if(b&0x80) goto	VIC_RST;  //VIC �� �����稫 ���樠��

// ����ࠨ���� ॣ���� ICR (ॣ������ �訡�� �� 設� VME
// �� ᨣ���� LBERR) ���� 4 � 3 - 10
		outpw( Index_VIC, ICR );
		b=inp(Data_VIC); b=b&0xF7;
		outp( Data_VIC, b|0x10 );

// clear BESR
		outpw( Index_VIC, BESR );
		outp( Data_VIC, 0 );

//���㫥��� ॣ���� ����᪨�㥬�� ���뢠��� - NSR FPGA1
		outpw( Index_FPGA, NSR );
		outp( Data_FPGA, 0 );

//����ࠨ���� ॣ���� LBTR - PAS -4T,  DS - 2T,   PAS - PAS - 4T
		outpw( Index_VIC, LBTR );
		outp( Data_VIC, 0x72 );

//����ࠨ���� ॣ���� TTR  - ����砥� �ࡨ�ࠦ �६��� �� 設�
//(16 mkc) ����頥� �ࡨ�ࠦ �� ��� 設�
		outpw( Index_VIC, TTR );
		outp( Data_VIC, 0x3E );

//����頥� ��ࠡ��� ᨣ���� SYSFAIL VIC�
		outpw( Index_VIC, ICR7 );
		b=inp(Data_VIC);
		outp( Data_VIC, b|0x80 );

//******************************************************************
//       ��������� LIM, VME ����, VME ��࠭���
//******************************************************************

//���祭�� SYCOR(������ 01h) � AX.
		outpw( Index_FPGA, SYCOR );
		outpw( Data_FPGA, 0x0080 );  //0000h   //�����: WatchDog
	 								//����蠬 ���-���
	 								//��� - 41h, 42h, 43h, 44h, 45h

//******************************************************************
//       ��������� ��������� VIC RESET
//******************************************************************
//���������� �����襭�� RESET
//���뢠�� ��-� ॣ���� POWER_UP
VIC_IN_RST:
		outpw( Index_FPGA, POWER_UP );
		w=inpw(Data_FPGA);
		if((w&0x0400)!=0) goto VIC_IN_RST;

//clear BESR
		outpw( Index_VIC, BESR );
		outp( Data_VIC, 0 );

//���㫥��� ���������� ��⮢ ����� ॣ���� ����᪨�㥬��
//���뢠��� - NSR FPGA
		outpw( Index_FPGA, NSR );
		outpw( Data_FPGA, 0 );

//******************************************************************
//       �������� ������������� ������ �� ���� VME
//******************************************************************

INIT_BL:
//����� ��諨 ���樠������ (SYSFAIL �� ��ࠡ��뢠��)
		outpw( Index_VIC, EGICR );
		b=inp(Data_VIC); b=b&8;
		if(b==8) goto INIT_BL;  //����� �� �����稫� ���樠��

}// init

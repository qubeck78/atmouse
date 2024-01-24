#include "config.h"


uchar ps2_mouse_packet[8];
ushort ps2_mouse_packet_counter;

/*
int uart_init(uchar baudrate)
{
	// Set the baud rate 
	UBRRH=0;
	UBRRL=baudrate;
	

	// Enable UART receiver and transmitter 
	UCSRB=(1<<RXEN)|(1<<TXEN);

	// set to 8 data bits, 1 stop bit 
	UCSRC = (1 << UCSZ1) | (1 << UCSZ0);
	return 0;
}

// Read and write functions 

int uart_putc(uchar data)
{
  // Wait for empty transmit buffer
  while (!(UCSRA & (1 << UDRE)));

  // Start transmittion 
  UDR = data;
  return 0;
}

void print(uchar *txt)
{
	uchar c;
	while(c=*txt++)
	{
		uart_putc(c);
	}
	return;
}


void print_b(uchar d)
{
	uchar c;
	
	c=d/100;
	d=d%100;
	uart_putc(c+'0');
	c=d/10;
	d=d%10;
	uart_putc(c+'0');
	uart_putc(d+'0');
	return;
}
*/

int ps2mouse_wait_for_clock_rising()
{

//	  __
// __/
//	counter_3=10;	//mouse
	PIND;
	
	//any level
	do{
//		if (counter_3==0) return 1;
	}while((PIND&PS2_MOUSECLK)!=0);

	PIND;
	
	//low level, wait for high
//	counter_3=10;
	do{
//		if (counter_3==0) return 1;
	}while((PIND&PS2_MOUSECLK)==0);
	//rising edge
	
	return 0;
}

int ps2mouse_wait_for_clock_falling()
{

// __
//   \__
	PIND;

	//any level
//	counter_3=10;
	do{
//		if (counter_3==0) return 1;		
	}while((PIND&PS2_MOUSECLK)==0);
	//high level, wait for low
//	counter_3=10;
	PIND;

	do{
//		if (counter_3==0) return 1;
	}while((PIND&PS2_MOUSECLK)!=0);
	//falling edge
	
	return 0;
}

int ps2mouse_get_answer(uchar *packet)
{
	volatile ulong j;
	
	ulong ps2mouse_data_in;
	uchar ps2mouse_packet;
	ushort ps2mouse_fsm_state;
	
	ps2mouse_fsm_state=0;
	
	do
	{
		ps2mouse_wait_for_clock_rising();

//		for(j=0;j<10;j++);
		PIND;
		ps2mouse_data_in=PIND&PS2_MOUSEDATA;
		
		switch(ps2mouse_fsm_state)
		{
			case 0:
				//start bit
				
				ps2mouse_packet=0;
				if (ps2mouse_data_in==0) 
				{
					ps2mouse_fsm_state=1;
				}else{

				}
					
				break;
			case 1:
				//bit0
				if (ps2mouse_data_in) 
				{
					ps2mouse_packet|=1;
				}
				ps2mouse_fsm_state=2;
				break;
			case 2:
				//bit1
				if (ps2mouse_data_in) 
				{
					ps2mouse_packet|=2;
				}
				ps2mouse_fsm_state=3;
				break;
			case 3:
				//bit2
				if (ps2mouse_data_in) 
				{
					ps2mouse_packet|=4;
				}
				ps2mouse_fsm_state=4;
				break;
			case 4:
				//bit3
				if (ps2mouse_data_in) 
				{
					ps2mouse_packet|=8;
				}
				ps2mouse_fsm_state=5;
				break;
			case 5:
				//bit4
				if (ps2mouse_data_in) 
				{
					ps2mouse_packet|=16;
				}
				ps2mouse_fsm_state=6;
			break;
			case 6:
				//bit5
				if (ps2mouse_data_in) 
				{
					ps2mouse_packet|=32;
				}
				ps2mouse_fsm_state=7;
				break;
			case 7:
				//bit6
				if (ps2mouse_data_in) 
				{
					ps2mouse_packet|=64;
				}
				ps2mouse_fsm_state=8;
				break;
			case 8:
				//bit7
				if (ps2mouse_data_in) 
				{
					ps2mouse_packet|=128;
				}
				ps2mouse_fsm_state=9;
				break;
			
			case 9:
				//parity
				ps2mouse_fsm_state=10;
				break;
			
			case 10:
				//stop bit
				*packet=ps2mouse_packet;
				return 0;
				
				break;
			
			default:
				ps2mouse_fsm_state=0;
		}
	}while(1);
}

int ps2mouse_send_command(uchar cmd,ushort response_bytes_no)
{
	int i,rv;
	volatile ulong j;
	
	uchar rb;
	ushort parity;
	ulong rbit;
		
	parity=1;
	
	PORTD|=PS2_MOUSEDATA|PS2_MOUSECLK;
	PS2_MOUSEDATA_OUT();
	PS2_MOUSECLK_OUT();
	
	for(j=0;j<50;j++);
	PORTD&=PS2_MOUSECLK^0xff; //low clock
	for(j=0;j<50;j++);
	PORTD&=PS2_MOUSEDATA^0xff;	//low data
	for(j=0;j<50;j++);	
	PORTD|=PS2_MOUSECLK;	//release clock
	PS2_MOUSECLK_IN();
	
	
	ps2mouse_wait_for_clock_falling();
	if (cmd&1)
	{
		parity^=1;
		PORTD|=PS2_MOUSEDATA;
	}else{
		PORTD&=PS2_MOUSEDATA^0xff;
	}
	ps2mouse_wait_for_clock_falling();	
	if (cmd&2)
	{
		parity^=1;
		PORTD|=PS2_MOUSEDATA;
	}else{
		PORTD&=PS2_MOUSEDATA^0xff;
	}
	ps2mouse_wait_for_clock_falling();	
	if (cmd&4)
	{
		parity^=1;
		PORTD|=PS2_MOUSEDATA;
	}else{
		PORTD&=PS2_MOUSEDATA^0xff;
	}
	ps2mouse_wait_for_clock_falling();	
	if (cmd&8)
	{
		parity^=1;
		PORTD|=PS2_MOUSEDATA;
	}else{
		PORTD&=PS2_MOUSEDATA^0xff;
	}
	ps2mouse_wait_for_clock_falling();	
	if (cmd&16)
	{
		parity^=1;
		PORTD|=PS2_MOUSEDATA;
	}else{
		PORTD&=PS2_MOUSEDATA^0xff;
	}
	ps2mouse_wait_for_clock_falling();	
	if (cmd&32)
	{
		parity^=1;
		PORTD|=PS2_MOUSEDATA;
	}else{
		PORTD&=PS2_MOUSEDATA^0xff;
	}
	ps2mouse_wait_for_clock_falling();	
	if (cmd&64)
	{
		parity^=1;
		PORTD|=PS2_MOUSEDATA;
	}else{
		PORTD&=PS2_MOUSEDATA^0xff;
	}
	ps2mouse_wait_for_clock_falling();	
	if (cmd&128)
	{
		parity^=1;
		PORTD|=PS2_MOUSEDATA;
	}else{
		PORTD&=PS2_MOUSEDATA^0xff;
	}
	ps2mouse_wait_for_clock_falling();	
	//parity
	if (parity)
	{
		PORTD|=PS2_MOUSEDATA;
	}else{
		PORTD&=PS2_MOUSEDATA^0xff;
	}
	ps2mouse_wait_for_clock_falling();	
	//stop
	PORTD|=PS2_MOUSEDATA;
	//release data
	PS2_MOUSEDATA_IN();

	ps2mouse_wait_for_clock_falling();	
	ps2mouse_wait_for_clock_rising();	
	//ack
	rbit=PIND&PS2_MOUSEDATA;
	
	if (rbit!=0) return -1;
	
	
	for(i=0;i<response_bytes_no;i++)
	{
		rv=ps2mouse_get_answer(&rb);
		if (rv==0)
		{
			ps2_mouse_packet[i]=rb;
			ps2_mouse_packet_counter=i+1;
		}else{
			return rv;
		}
	}
	return 0;
}

void ps2_mouse_init()
{
	
	ps2_mouse_packet_counter=0;
	
	ps2mouse_send_command(0xFF,2);	//reset mouse
	
	ps2mouse_send_command(0xE8,1);	//set resolution
	ps2mouse_send_command(0x01,1);	//2cnt/mm
		
	return;
}

void ami_mouse_init()
{
	//PD6 -> RMB
	//PB0 -> LMB
	
	//PB1 -> HQP
	//PB2 -> VQP
	//PB3 -> HP
	//PB4 -> VP
	
	
	DDRD|=AMI_RMB;
	DDRB|=AMI_LMB|AMI_HQP|AMI_VQP|AMI_HP|AMI_VP;
	PORTD|=AMI_RMB;
	PORTB|=AMI_LMB;
	PORTB&=(AMI_HQP|AMI_VQP|AMI_HP|AMI_VP)^0xff;
	
	return;
}

int main()
{
	int i;
	uchar rm;
	volatile long int j;
		
/*	uart_init(51);	//9600 @8000000
	print("PS2mAmi\r\n");
*/
	ps2_mouse_init();

//	print("Init ok\r\n");
	
	ami_mouse_init();
	do{
		ps2mouse_send_command(0xeb,4);
		
		if (ps2_mouse_packet[1]&1)
		{
			PORTB&=AMI_LMB^0xff;
		}else{
			PORTB|=AMI_LMB;		
		}
		if (ps2_mouse_packet[1]&2)
		{
			PORTD&=AMI_RMB^0xff;
		}else{
			PORTD|=AMI_RMB;		
		}

		
		if (ps2_mouse_packet[1]&32)
		{
			rm=ps2_mouse_packet[3]^0xff;
			rm++;
			for(i=0;i<rm;i++)
			{
				PORTB|=AMI_VP;
				for(j=0;j<10;j++);
				PORTB|=AMI_VQP;
				for(j=0;j<10;j++);
				PORTB&=AMI_VP^0xff;
				for(j=0;j<10;j++);
				PORTB&=AMI_VQP^0xff;
				for(j=0;j<10;j++);				
			}
		}else{
			for(i=0;i<ps2_mouse_packet[3];i++)
			{
				PORTB|=AMI_VQP;
				for(j=0;j<10;j++);
				PORTB|=AMI_VP;
				for(j=0;j<10;j++);
				PORTB&=AMI_VQP^0xff;
				for(j=0;j<10;j++);
				PORTB&=AMI_VP^0xff;
				for(j=0;j<10;j++);	
			}
		}


		if (ps2_mouse_packet[1]&16)
		{
			rm=ps2_mouse_packet[2]^0xff;
			rm++;
			for(i=0;i<rm;i++)
			{
				PORTB|=AMI_HQP;
				for(j=0;j<10;j++);
				PORTB|=AMI_HP;
				for(j=0;j<10;j++);
				PORTB&=AMI_HQP^0xff;
				for(j=0;j<10;j++);
				PORTB&=AMI_HP^0xff;
				for(j=0;j<10;j++);				
			}
		}else{
			//right movement
			for(i=0;i<ps2_mouse_packet[2];i++)
			{
				PORTB|=AMI_HP;
				for(j=0;j<10;j++);
				PORTB|=AMI_HQP;
				for(j=0;j<10;j++);
				PORTB&=AMI_HP^0xff;
				for(j=0;j<10;j++);
				PORTB&=AMI_HQP^0xff;
				for(j=0;j<10;j++);	
			}
		}
	}while(1);
	
}
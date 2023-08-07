#include<reg52.h>

#define uchar unsigned char
#define uint unsigned int

uchar code Tab0[]=		//定义数码管数组，没有小数点
{
	0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f
};
uchar code Tab1[]=		//定义数码管数组，有小数点
{	
	0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef
};

sbit K1=P3^2;			//模式选择键，本程序三种模式，分别是时间显示、秒表、时间设置
sbit K2=P3^3;			//设置时间时加一/开始（暂停）秒表按键
sbit K3=P3^4;			//切换设置位/清零秒表按键

void Delay(uchar x);		//延时函数
void Display0(); 		//时间显示函数
void Display1(); 		//秒表显示函数
void Display2();		//时间设置显示函数
void Init(); 			//中断初始化函数
void Mode_K1();			//模式切换函数
void KEY_MiaoBiao();	//检测秒表操作按键
void KEY_Time_Set();	 //检测时间设置按键


uchar Hours=23,Minutes=56,seconds=55;		//分别定义三个变量表示时、分、秒
uchar Time0_bit=0;			//用于定时器0的溢出标志
bit Set_Time=0;				//设置时 闪烁时长的标志位
bit Set0=0;					//设置时间标志，确定是设置‘分’还设置‘时’
uchar K1_bit=0;		  		//用于模式切换的标志
uchar Miao_Biao0,Miao_Biao1,Miao_Biao2,Miao_Biao3,Miao_Biao4;		//用于秒表的变量

void main()
{
	Init();										//中断初始化
	while(1)									//死循环
		{
			Mode_K1();							//模式扫描
			switch(K1_bit)		 				//模式选择
				{
					case 0:
						{
							Display0();			//显示时间								
							break;
						}
					case 1:
						{
							Display1();	 		//显示秒表
							KEY_MiaoBiao();		//扫描秒表操作
							break;
						}
					case 2:
							{
								Display2();		//设置时间时的显示程序，可闪烁定位
								KEY_Time_Set();	//扫描按键
								break;
							}
				}
		}
}

void KEY_Time_Set()	   							//设置时间时的按键扫描函数
{
	if(K1_bit==2)	  							//判断是否为时间设置模式
		{
			if(K2==0)		 					//K2有按下（下同）
				{
					Delay(10);	   				//延时消抖（下同）
					if(K2==0)				 	//再次检测是否为误按
						{
							if(Set0==0)			//检测是设置‘时’还是分，Set0为0时设置‘分’，Set0为1时设置‘时’
								{
									Minutes++; 	//‘分’加1
									if(Minutes==60)	//当'分'加到60时，重新赋值0
										Minutes=0;
								}
							else
								{
									Hours++;  	//‘时’加1
									if(Hours==24)	//当'时'加到24时，重新赋值0
										Hours=0;
								}
							while(!K2);			//按键松开检测，消除松开按键时的抖动干扰
						}
				}
			if(K3==0)
				{
					Delay(10);
					if(K3==0)
						{
							Set0=~Set0;			//K3每按一次Set0取反，以确定是设置‘时’还是设置‘分’，Set0为0时设置‘分’，Set0为1时设置‘时’
							while(!K3);
						}	
				}
		}
}

void KEY_MiaoBiao()								//检测秒表按键操作
{
	if(K1_bit==1)								//判断是否为秒表模式
		{
			if(K2==0)
				{
					Delay(10);
					if(K2==0)
						{
							TR1=~TR1;			//K2每按一次TR1取反，暂停或开始定时器1，达到暂停或开始秒表的目的
							while(!K2);
						}
				}
			if(K3==0)
				{
					Delay(10);
					if(K3==0)	 				//当K3按下时秒表所有数据清零，并停止定时器1
						{
							TR1=0;	 			//停止定时器1
							Miao_Biao0=0; 		//清零数据
							Miao_Biao1=0;
							Miao_Biao2=0;
							Miao_Biao3=0;
							Miao_Biao4=0;
							while(!K3);
						}
				}
		}
}

void Mode_K1()									//模式选择键，本程序三种模式，分别是时间显示、秒表、时间设置
{
	if(K1==0)
		{
			Delay(10);
			if(K1==0)
				{
					K1_bit++;
					if(K1_bit==3)
						{
							K1_bit=0;
						}
					while(!K1);
/***********************************************************************
*************当K1_bit为0时显示时钟，为1时进入秒表，*********************
*******************为2时进入时间设置模式********************************
***********************************************************************/
				}
		}
}
	

void Time1() interrupt 3  			//定时器1函数
{
	TH1=(65536-50000)/256;			//重装初值
	TL1=(65536-50000)%256;

	Miao_Biao0++;
	if(Miao_Biao0==2)				//以下为秒表数据处理
		{
			Miao_Biao0=0;
			Miao_Biao1++;	 		//Miao_Biao1每加1次为100ms，
			if(Miao_Biao1==10)
				{
					Miao_Biao1=0;
					Miao_Biao2++;
					if(Miao_Biao2==60)
						{
							Miao_Biao2=0;
							Miao_Biao3++;
							if(Miao_Biao3==60)
								{
									Miao_Biao3=0;
									Miao_Biao4++;
									if(Miao_Biao4==10)
										Miao_Biao4=0;
								}
						}
				}
		}
}


void Time0() interrupt 1 					//定时器0函数
{
	TH0=(65536-50000)/256;	   				//重装初值
	TL0=(65536-50000)%256;
	Time0_bit++;
	if((Time0_bit%10)==0)					//每溢出10次Time0_bit取反一次
		Set_Time=~Set_Time;	  				//0.5秒闪烁一次

	if(Time0_bit==20)						//以下为时间数据处理
		{
			Time0_bit=0;
			seconds++;
			if(seconds==60)
				{
					seconds=0;
					Minutes++;
					if(Minutes==60)
						{
							Minutes=0;
							Hours++;
							if(Hours==24)
								{
									Hours=0;
								}
						}
				}

		}
}

void Init()	 						//中断初始化函数
{
	EA=1;		 					//开总中断
	TMOD=0X11;			  			//定时器工作方式选择，定时器0和定时器1都选择第1种工作方式
	TH0=(65536-50000)/256;			//定时器0装初值，定时50ms
	TL0=(65536-50000)%256;
	ET0=1;							//开定时器0开关
	TR0=1;			   				//开定时器0小开关

	TH1=(65536-50000)/256;			//定时器1装初值，定时50ms
	TL1=(65536-50000)%256;
	ET1=1;				  			//开定时器1开关
	TR1=0;							//关定时器1小开关
}

void Display2()							//时间设置时的显示函数	
{
	if(Set0==0)							//判断是否为设置‘时’还是‘分’
		{
			P0=Tab0[seconds%10];		//显示秒的个位
			P1=0xdf;					//段选
			Delay(10);					//延时
			P0=0X00;					//消隐
		
			P0=Tab0[seconds/10];		//显示秒的十位
			P1=0xef;					//段选
			Delay(10);					//延时
			P0=0X00;					//消隐
			
			if(Set_Time)				//这个if语句实现分钟以0.5秒的频率闪烁
				{
					P0=Tab1[Minutes%10];		//显示分的个位，需要加上小数点做分隔符
					P1=0xf7;					//段选
					Delay(10);					//延时
					P0=0X00;					//消隐

					P0=Tab0[Minutes/10];		//显示分的十位
					P1=0xfb;					//段选
					Delay(10);					//延时
					P0=0X00;					//消隐
				}
			else
				{
					P0=0x00;		//显示分的个位，需要加上小数点做分隔符
					P1=0xf7;					//段选
					Delay(10);					//延时
					P0=0X00;					//消隐
		
					P0=0x00;		//显示分的十位
					P1=0xfb;					//段选
					Delay(10);					//延时
					P0=0X00;					//消隐
				}
			
			P0=Tab1[Hours%10];			//显示时的个位，需要加上小数点做分隔符
			P1=0xfd;					//段选
			Delay(10);					//延时
			P0=0X00;					//消隐
		
			P0=Tab0[Hours/10];			//显示时的十位
			P1=0xfe;					//段选
			Delay(10);					//延时
			P0=0X00;					//消隐
		}


	else			//‘时’钟闪烁
		{
			P0=Tab0[seconds%10];		//显示秒的个位
			P1=0xdf;					//段选
			Delay(10);					//延时
			P0=0X00;					//消隐
		
			P0=Tab0[seconds/10];		//显示秒的十位
			P1=0xef;					//段选
			Delay(10);					//延时
			P0=0X00;					//消隐
			
			P0=Tab1[Minutes%10];		//显示分的个位，需要加上小数点做分隔符
			P1=0xf7;					//段选
			Delay(10);					//延时
			P0=0X00;					//消隐

			P0=Tab0[Minutes/10];		//显示分的十位
			P1=0xfb;					//段选
			Delay(10);					//延时
			P0=0X00;					//消隐
			
			if(Set_Time)				//这个if语句实现‘时’钟以0.5秒的频率闪烁
				{
					P0=Tab1[Hours%10];			//显示时的个位，需要加上小数点做分隔符
					P1=0xfd;					//段选
					Delay(10);					//延时
					P0=0X00;					//消隐
				
					P0=Tab0[Hours/10];			//显示时的十位
					P1=0xfe;					//段选
					Delay(10);					//延时
					P0=0X00;					//消隐
				}
			else
				{
					P0=0x00;			//显示时的个位，需要加上小数点做分隔符
					P1=0xFF;					//段选
					Delay(10);					//延时
					P0=0X00;					//消隐
				
					P0=0X00;			//显示时的十位
					P1=0xFF;					//段选
					Delay(10);					//延时
					P0=0X00;					//消隐				
				}		
		}
}

void Display1()			//显示秒表
{
	P0=Tab0[Miao_Biao1%10];		//显示1/10秒的个位
	P1=0xdf;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab1[Miao_Biao2%10];		//显示秒的个位，需要加上小数点做分隔符
	P1=0xef;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab0[Miao_Biao2/10];		//显示秒的十位
	P1=0xf7;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab1[Miao_Biao3%10];		//显示分的个位，需要加上小数点做分隔符
	P1=0xfb;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab0[Miao_Biao3/10];		//显示分的十位
	P1=0xfd;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab1[Miao_Biao4%10];		//显示时的个位，需要加上小数点做分隔符
	P1=0xfe;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐
}

void Display0()			//显示时钟
{
	P0=Tab0[seconds%10];		//显示秒的个位
	P1=0xdf;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab0[seconds/10];		//显示秒的十位
	P1=0xef;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab1[Minutes%10];		//显示分的个位，需要加上小数点做分隔符
	P1=0xf7;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab0[Minutes/10];		//显示分的十位
	P1=0xfb;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab1[Hours%10];			//显示时的个位，需要加上小数点做分隔符
	P1=0xfd;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐

	P0=Tab0[Hours/10];			//显示时的十位
	P1=0xfe;					//段选
	Delay(10);					//延时
	P0=0X00;					//消隐
}


void Delay(uchar x)		  		//延时
{
	uchar i,j;
	for(i=x;i>0;i--)
		for(j=110;j>0;j--);
}


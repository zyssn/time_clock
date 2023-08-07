bit K1_bit=0;

void K_1()
{
	if(K1==0)
		{
			Delay(10);
			if(K1==0)
				{
					Delay(500);
					if(K1==0)
						{
							while(!K1);
							K1_bit=1;		//时间设置模式
						}
					else
						K1_bit=0;			//进入秒表模式
				}
		}
}

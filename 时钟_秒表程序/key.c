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
							K1_bit=1;		//ʱ������ģʽ
						}
					else
						K1_bit=0;			//�������ģʽ
				}
		}
}

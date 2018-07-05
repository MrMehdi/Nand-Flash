//============================================================================
// Name        : NandFlash.c
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Simple function in C, Ansi-style
//============================================================================

#define GpioAdd 0x40013000      //д�Ĵ�����ַ
#define OE 0x40013010           //����Ĵ�����ַ
#define EXT 0x40013020          //���Ĵ�����ַ
#define READADD 10              //����ַƫ����
typedef unsigned short U16;
typedef unsigned char U8;
typedef volatile U16* RP16;
void delay(unsigned int);
int Page_Program(U8,U8,U8,U8,U8*);
int Block_Erase(U8,U8);
U8 Page_Read(U8,U8,U8,U8,U8*);
int main(void)
{
     U8 col_add1,col_add2,row_add1,row_add2;
     int i,j;
     i=0;
     U8 wdata[2048];
     U8 rdata;
	 //ѡ���512�飬32ҳ��0�ֽڴ��ĵ�ַ0x4010000
     col_add1 = 0x00;
     col_add2 = 0x00;
     row_add1 = 0x10;
     row_add2 = 0x40;
	 //����2KB���ݣ�0~255ѭ��
     j=0;
     for(i=0;i<2048;i++)
     {
		wdata[i] = j;
		j++;
		if (j>255){
			j=0;
		}
     }
	 Block_Erase(row_add1,row_add2);
	 Page_Program(col_add1,col_add2,row_add1,row_add2,wdata);
	 Page_Read(col_add1,col_add2,row_add1,row_add2,&rdata);
	 return 0;
}
//�����,ֻҪд�е�ַ
int Block_Erase(U8 A12_A19,U8 A20_A27)
{

	*(RP16)(OE) = 0x3ffb;		//��oe�Ĵ�����40013010����1��ʹGPIO���������ģʽ

	//���ÿ����źţ�д����
	*(RP16)(GpioAdd) = 0x0015; //��GPIO��dr�Ĵ�����40013000��д������
	*(RP16)(GpioAdd) = 0x1815;			//60h�͸�IO������
	delay(10);						//�����������Ҫ17ns�����ȫ����Ϊ10us
	*(RP16)(GpioAdd) = 0x181d;		//WE���ߣ���WE�������أ�������浽nf������������
	delay(10);


	*(RP16)(GpioAdd) = 0x1816;		//���ÿ����źţ�д��ַ

	//д����ַ����Ϊ������������Ϊ��λ��

	//д�е�ַ1
	*(RP16)(GpioAdd) = (A12_A19<<6)|0x16;
	delay(10);
	*(RP16)(GpioAdd) = (A12_A19<<6)|0x1e;
	delay(10);

	//д�е�ַ2
	*(RP16)(GpioAdd) = (A20_A27<<6)|0x16;
	delay(10);
	*(RP16)(GpioAdd) = (A20_A27<<6)|0x1e;
	delay(10);

	*(RP16)(GpioAdd) = 0x0015;				//����WE,����CLE
	*(RP16)(GpioAdd) = 0x3415;				//��������d0h,��ʼ����
	delay(10);
	*(RP16)(GpioAdd) = 0x341c;				//����WE,����CLE
	delay(10);

	while(((*(RP16)(EXT)) & 0x0004) != 0x0004);	//ͨ��R_B�ź����ж��Ƿ�������

	//д���״̬����
	*(RP16)(GpioAdd) = 0x0415;
	*(RP16)(GpioAdd) = 0x1c15;				//70h�͸�IO
	delay(10);

	*(RP16)(GpioAdd) = 0x1c1d;				//WE���ߣ���WE�������أ�������浽nf������������
	*(RP16)(GpioAdd) = 0x1c1c;				//CLE����
	delay(5);

	*(RP16)(OE) = 0x003b;			//��ʱ��ģ��IO�ڲ��ֵ�GPIO��oe��0��ʹ�乤��������ģʽ

	*(RP16)(GpioAdd) = 0x1c0c;				//RE����,��RE���½��ؽ�Read Status�Ĵ��������������IO�ܽ���
	delay(10);								//�ȴ�����״̬

	if(((*(RP16)(EXT))& 0x0040) == 0x0000)  //���IO0�Ƿ�Ϊ0����Ϊ0��������ɹ�;
	{
		*(RP16)(GpioAdd) = 0x001c;				//��RE����
		return 0;
	}
	else
		return -1;

}

//ҳ���
int Page_Program(U8 A0_A7,U8 A8_A11,U8 A12_A19,U8 A20_A27,U8 *wdata)
{
	int i;
	*(RP16)(OE) = 0x3ffb;			//R_B��0Ϊ����ģʽ�������Ϊ���ģʽ

	//���ÿ����źţ�д����
	*(RP16)(GpioAdd) = 0x0015;
	//��WE�ź����������ź�
	*(RP16)(GpioAdd) = 0x2015;				//80h�͸�IO,����
	delay(10);								//�����������Ҫ17ns�����ȫ����Ϊ10us
	*(RP16)(GpioAdd) = 0x201d;				//WE���ߣ���WE�������أ�������浽nf������������
	delay(10);

	//д��ҳ�ڵ�ַ��2048���ֽ��еĵڼ����ֽ�
	//д�е�ַ1
	*(RP16)(GpioAdd) = 0x2016;				//���ÿ����źţ�д��ַ WE����
	*(RP16)(GpioAdd) = (A0_A7<<6)|0x16;		//��8λ�����ַ
	delay(10);								//�͵�ƽ����ʱ��
	*(RP16)(GpioAdd) = (A0_A7<<6)|0x1e;		//WE���ߣ���WE�������أ���ַ�����浽nf�ĵ�ַ������
	delay(10);								//�ߵ�ƽ����ʱ��

	//д�е�ַ2
	*(RP16)(GpioAdd) = (A0_A7<<6)|0x16;		//WE����
	*(RP16)(GpioAdd) = (A8_A11<<6)|0x16;	//д���е�ַ2
	delay(10);
	*(RP16)(GpioAdd) = (A8_A11<<6)|0x1e;	//WE���������������ַ
	delay(10);

	//д�е�ַ1
	*(RP16)(GpioAdd) = (A8_A11<<6)|0x16;
	*(RP16)(GpioAdd) = (A12_A19<<6)|0x16;
	delay(10);
	*(RP16)(GpioAdd) = (A12_A19<<6)|0x1e;
	delay(10);

	//д�е�ַ2
	*(RP16)(GpioAdd) = (A12_A19<<6)|0x16;
	*(RP16)(GpioAdd) = (A20_A27<<6)|0x16;
	delay(10);
	*(RP16)(GpioAdd) = (A20_A27<<6)|0x1e;	//WE����
	delay(10);

	//����ͨ��I/Oд��NF�����ݼĴ���
	*(RP16)(GpioAdd) = (A20_A27<<6)|0x14;	//WE��ALE��CLE���ͽ������ݴ���
											//����ͨ��I/Oд��NF�����ݼĴ���
	for(i=0;i<2048;i++)
	{
		*(RP16)(GpioAdd) = (wdata[i]<<6)|0x14;
		delay(10);
		*(RP16)(GpioAdd) = (wdata[i]<<6)|0x1c;		//WE���ߣ�������WE�������ر����浽nf��������������
		delay(10);
	}

	//д������10h
	*(RP16)(GpioAdd) = wdata[i]<<6|0x15;		   //WE���ͣ�CLE����
	*(RP16)(GpioAdd) = 0x0415;				//10h�͸�IO,���ݴ����ݼĴ���д������
	delay(10);
	*(RP16)(GpioAdd) = 0x041d;				//WE���ߣ���WE�������أ�������浽nf������������
	delay(10);                              //twb�ӳ�ʱ��
	while(((*(RP16)(EXT)) & (0x0004)) != 0x0004);	//ͨ��R_B�ź����ж��Ƿ�д�����

	//д���״̬����
	*(RP16)(GpioAdd) = 0x1c15;				//WE���ͣ�CLE����,70h�͸�IO
	delay(10);

	*(RP16)(GpioAdd) = 0x1c1d;				//WE���ߣ���WE�������أ�������浽nf������������
	*(RP16)(GpioAdd) = 0x1c1c;				//CLE����
	delay(5);								//tCLR+tREA�ĵȴ�ʱ��

	*(RP16)(OE) = 0x003b;					//��ʱ��ģ��IO�ڲ��ֵ�GPIO��oe��0��ʹ�乤��������ģʽ

	*(RP16)(GpioAdd) = 0x1c0c;				//RE����,��RE���½��ؽ�Read Status�Ĵ��������������IO�ܽ���
	delay(10);								//�ȴ�����״̬

	if((*(RP16)(EXT) & (0x0040))== 0x0000)
	{
		*(RP16)(GpioAdd) = 0x001c;          //��RE����
		return 0;							//���IO0�Ƿ�Ϊ0����Ϊ0����ҳ��̳ɹ�;
	}
	else
		return -1;
}

//��ҳ��
U8 Page_Read(U8 A0_A7,U8 A8_A11,U8 A12_A19,U8 A20_A27, U8 * rdata)
{
	*(RP16)(OE) = 0x3ffb;			//oe��1�����ģʽ oe��0������ģʽ(R_B)

	//���ÿ����źţ�д����
	*(RP16)(GpioAdd) = 0x0015;              //���ÿ����ź�
	*(RP16)(GpioAdd) = 0x0015;				//00h�͸�IO,����
	delay(10);								//�����������Ҫ17ns�����ȫ����Ϊ10us
	*(RP16)(GpioAdd) = 0x001d;				//WE���ߣ���WE�������أ�������浽nf������������
	delay(10);

	//д�е�ַ1
	*(RP16)(GpioAdd) = 0x0016;				//���ÿ����źţ�д��ַ WE����
	*(RP16)(GpioAdd) = ((A0_A7+READADD)<<6)|0x16;		//��8λ�����ַ+ƫ������ַ
	delay(10);								//�͵�ƽ����ʱ��
	*(RP16)(GpioAdd) = ((A0_A7+READADD)<<6)|0x1e;		//WE���ߣ���WE�������أ���ַ�����浽nf�ĵ�ַ������
	delay(10);								//�ߵ�ƽ����ʱ��

	//д�е�ַ2
	*(RP16)(GpioAdd) = ((A0_A7+READADD)<<6)|0x16;
	*(RP16)(GpioAdd) = (A8_A11<<6)|0x16;
	delay(10);
	*(RP16)(GpioAdd) = (A8_A11<<6)|0x1e;
	delay(10);

	//д�е�ַ1
	*(RP16)(GpioAdd) = (A8_A11<<6)|0x16;
	*(RP16)(GpioAdd) = (A12_A19<<6)|0x16;
	delay(10);
	*(RP16)(GpioAdd) = (A12_A19<<6)|0x1e;
	delay(10);

	//д�е�ַ2
	*(RP16)(GpioAdd) = (A12_A19<<6)|0x16;
	*(RP16)(GpioAdd) = (A20_A27<<6)|0x16;
	delay(10);
	*(RP16)(GpioAdd) = (A20_A27<<6)|0x1e;	//WE����
	delay(10);


	*(RP16)(GpioAdd) = 0x0015;
	*(RP16)(GpioAdd) = 0x0c15;				//��������30h
	delay(10);
	*(RP16)(GpioAdd) = 0x0c1c;				//����ALE��CLE������WE
											//���ݴӴ洢�崫�͵����ݼĴ�����
	while(((*(RP16)(GpioAdd)) & (0x0004)) != 0x0004);	//ͨ��R_B�ź����ж��Ƿ����Ƿ����
	delay(10);

	*(RP16)(OE) = 0x003b;			//IOȫ���������ģʽ���ڶ�ȡ���ݼĴ����е�����
	*(RP16)(GpioAdd) = 0x000c;				//RE����
	delay(10);
	*rdata = *(RP16)(EXT)>>6;
	*(RP16)(GpioAdd) = 0x001c;				//RE���ߣ���������
	delay(10);

	*(RP16)(GpioAdd) = 0x003c;				//CE���ߣ�ȡ��Ƭѡ������
	return 0;
}

//��ʱ������������Ϊ50M����j=1ʱ�պ��ӳ�1us
void delay(unsigned int j)
{
	unsigned int a,b,c;
		for(a=j;a>0;a--)
			for(b=0;b<1;b++)
				for(c=0;c<50;c++);
}

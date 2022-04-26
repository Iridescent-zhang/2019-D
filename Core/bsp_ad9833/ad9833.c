#include "ad9833.h"

void AD9833_Write(unsigned int TxData)
{

        unsigned char i;

        SCK_1();

        HAL_Delay(2);

        FSYNC_1();

         HAL_Delay(2);

        FSYNC_0();

         HAL_Delay(2);

        for(i = 0; i < 16; i++)
        {

                if (TxData & 0x8000)

                        DAT_1();

                else

                        DAT_0();

                 HAL_Delay(2);

                SCK_0();

                 HAL_Delay(2);
                SCK_1();
                TxData <<= 1;
        }

        FSYNC_1();
}



void AD9833_WaveSeting(float Freq,unsigned int Freq_SFR,unsigned int WaveMode,unsigned int Phase )
{

                int frequence_LSB,frequence_MSB,Phs_data;

                float   frequence_mid,frequence_DATA;

                long int frequence_hex;

                /*********************************����Ƶ�ʵ�16����ֵ***********************************/

                frequence_mid=268435456/25;//�ʺ�25M����

                //���ʱ��Ƶ�ʲ�Ϊ25MHZ���޸ĸô���Ƶ��ֵ����λMHz ��AD9833���֧��25MHz

                frequence_DATA=Freq;

                frequence_DATA=frequence_DATA/1000000;

                frequence_DATA=frequence_DATA*frequence_mid;

                frequence_hex=frequence_DATA;  //���frequence_hex��ֵ��32λ��һ���ܴ�����֣���Ҫ��ֳ�����14λ���д���

                frequence_LSB=frequence_hex; //frequence_hex��16λ�͸�frequence_LSB

                frequence_LSB=frequence_LSB&0x3fff;//ȥ�������λ��16λ����ȥ����λ������14λ

                frequence_MSB=frequence_hex>>14; //frequence_hex��16λ�͸�frequence_HSB

                frequence_MSB=frequence_MSB&0x3fff;//ȥ�������λ��16λ����ȥ����λ������14λ

                Phs_data=Phase|0xC000;        //��λֵ

                AD9833_Write(0x0100); //��λAD9833,��RESETλΪ1

                AD9833_Write(0x2100); //ѡ������һ��д�룬B28λ��RESETλΪ1

                if(Freq_SFR==0)                                  //���������õ�����Ƶ�ʼĴ���0
                {

                         frequence_LSB=frequence_LSB|0x4000;

                         frequence_MSB=frequence_MSB|0x4000;

                         //ʹ��Ƶ�ʼĴ���0�������

                        AD9833_Write(frequence_LSB); //L14��ѡ��Ƶ�ʼĴ���0�ĵ�14λ��������

                        AD9833_Write(frequence_MSB); //H14 Ƶ�ʼĴ����ĸ�14λ��������

                        AD9833_Write(Phs_data);        //������λ

                        //AD9833_Write(0x2000); /**����FSELECTλΪ0��оƬ���빤��״̬,Ƶ�ʼĴ���0�������**/

            }

                if(Freq_SFR==1)                                //���������õ�����Ƶ�ʼĴ���1

                {

                         frequence_LSB=frequence_LSB|0x8000;

                         frequence_MSB=frequence_MSB|0x8000;

                        //ʹ��Ƶ�ʼĴ���1�������

                        AD9833_Write(frequence_LSB); //L14��ѡ��Ƶ�ʼĴ���1�ĵ�14λ����

                        AD9833_Write(frequence_MSB); //H14 Ƶ�ʼĴ���1Ϊ

                        AD9833_Write(Phs_data);        //������λ

                        //AD9833_Write(0x2800); /**����FSELECTλΪ0������FSELECTλΪ1����ʹ��Ƶ�ʼĴ���1��ֵ��оƬ���빤��״̬,Ƶ�ʼĴ���1�������**/

                }

                if(WaveMode==TRI_WAVE) //������ǲ�����
                         AD9833_Write(0x2002);

                if(WaveMode==SQU_WAVE)        //�����������

                        AD9833_Write(0x2028);

                if(WaveMode==SIN_WAVE)        //������Ҳ���

                        AD9833_Write(0x2000);

}

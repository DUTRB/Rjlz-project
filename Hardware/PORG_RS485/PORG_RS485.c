#include "PORG_RS485.h"
#include "gd32f350r_eval.h"
//#include "gd32f3x0_it.h"
#include "gd32e23x_it.h"
#include "data.h"
#include "24cxx.h"

//���ÿ���λ
RS485_BIT_STRUCT RS485_C;

uchar 		esadd										 ;//���յ��ĵ�ַ��
uchar 		esdata									 ;//���յ���������
uchar     R_DATA[TR_MAX_COUNTER]   ;//RS485�ջ�����
uchar     T_DATA[TR_MAX_COUNTER]   ;//RS485��������
uchar     T_counter                ;//RS485������������
uchar     R_counter                ;//RS485������������
uchar     TR_JISHU                 ;//RS485�շ�������
uchar     rs_485_contror           ;//rs485ͨѶ���������

uchar     net_control_data				 ;//���������

union  GLQ_TEST
{
    unsigned long   ADATA;
    uint8_t  adata[4];
};



//;-------------------------------------------------------------------
//;RS485_INI_DATA();//RS485���ݳ�ʼ���ӳ���
//;-------------------------------------------------------------------
void RS485_init_data()
{
    uchar i=0;
    for(i=0; i<TR_MAX_COUNTER; i++)     R_DATA[i]=0; //RS485�շ�������
    RS485_NEED = 0      ;
    T_counter = 0       ;//RS485������������
    R_counter = 0       ;//RS485������������
    TR_JISHU = 0        ;//RS485�շ�������
    rs_485_contror = 0  ;//RS485ͨѶ���������
    RS485_RX						;//RS485����״̬
    RS485_RX_Statue = 0 ;//RS485����Ҫ�÷���״̬
}


//;-------------------------------------------------------------------
//out_rs485();    //rs485�������ݴ����ӳ���
//����ֵ����
//������i ���η��������ֽ�����
void out_rs485(uchar i)
{
    uchar y = 0;
    RS485_TX;      				//������
    //T_counter = i;						//���÷�������
    //TR_JISHU = 0;							//�����Ѿ����͵��ֽ���
    for(y=0; y < i; ++y)
    {
        usart_data_transmit(USART0,(uint8_t)T_DATA[y]);
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));//�ȴ����ݷ������
    }
    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));//�ȴ�����ת�����
    TR_JISHU = 0;
    RS485_RX;
    //usart_interrupt_enable(EVAL_COM1, USART_INT_TBE);//���������ж�
}



//;-------------------------------------------------------------------
//rs485ͨ�Ŵ����ӳ���
//;-------------------------------------------------------------------
void RS485_PROG() 
{
    uchar i = 0;
    //data OBS_AIR,Smoke_AIR,OBS_N2,Smoke_N2;
    uint8_t temp8 = 0;
    uint16_t temp16 = 0;
    data t_anlog;
    if(RS485_NEED)//RS485��Ҫ����
    {
        RS485_NEED = 0;
        switch(rs_485_contror) //rs485ͨѶ���������
        {
        case 0xc2 : //1000 0010�D�D>�ɼ�������ѯ
        {
            /*
            �ش����
            1�ֽڣ� ������

            2�ֽڣ� ģ������ģ������4λ
            3�ֽڣ� ģ������ģ������8λ

            4�ֽڣ� ����ܹ���ģ����ֵ��4λ
            5�ֽڣ� ����ܹ���ģ����ֵ��8λ

            6�ֽڣ� ģ������ʣ�OBS��ֵ��4λ
            7�ֽڣ� ģ������ʣ�OBS��ֵ��8λ

            8�ֽڣ� У���

            */
            T_DATA[0] = rs_485_contror;

            T_DATA[1] = adcData.SmokeIn.data_8[1];
            T_DATA[2] = adcData.SmokeIn.data_8[0];

            T_DATA[3] = adcData.LDOPowerIn.data_8[1];
            T_DATA[4] = adcData.LDOPowerIn.data_8[0];

            T_DATA[5] = adcData.OBS.data_8[1];
            T_DATA[6] = adcData.OBS.data_8[0];

            t_anlog.data_16 = 0;
            for(i=0; i<=6; i++)               //��У���
                t_anlog.data_16 += T_DATA[i]; //����˫�ֽڣ���ֹ�������

            T_DATA[7] = t_anlog.data_8[0];//У���
            out_rs485(8);//�ظ�8�ֽ�����
            break;

        }

        /*
        OBS�궨���ͳһ�ظ�5�ֽڣ�
        	1�ֽڨD�D������
        	2�ֽڨD�DOBS����λ
        	3�ֽڨD�DOBS�Ͱ�λ
        	4�ֽڨD�DOBS_flag
        	5�ֽڨD�DУ���
        */
        case 0xc3 : //0000  0011�D�D>���̱궨
        {
            /*
            ���ط��ͣ�
            	1�ֽڨD�D�����ַ��
            	2�ֽڡ��������ַ��
            	3�ֽڡ����궨����
            	4�ֽڨD�D̽��ģ������ģ�����ɼ�ֵ����λ
            	5�ֽڡ���̽��ģ������ģ�����ɼ�ֵ�Ͱ�λ
            	6�ֽڨD�DOBS�궨ֵ��8��11λ
            	7�ֽڡ���OBS�궨ֵ��0��7λ
            	8�ֽڡ���У��λ
            ���յ���
            	1�ֽڨD�D�����ַ��
            	2�ֽڡ����궨����
            	3�ֽڨD�D̽��ģ������ģ�����ɼ�ֵ����λ
            	4�ֽڡ���̽��ģ������ģ�����ɼ�ֵ�Ͱ�λ
            	5�ֽڨD�DOBS�궨ֵ��8��11λ
            	6�ֽڡ���OBS�궨ֵ��0��7λ
            	7�ֽڡ���У��λ
            */
            t_anlog.data_16 = 0;
            for(i=0; i<=5; i++)               //��У���
                t_anlog.data_16 += R_DATA[i];//����˫�ֽڣ���ֹ�������
            if(R_DATA[6] == t_anlog.data_8[0])//У����ȷ
            {
                if(R_DATA[1] == 0)//���̱궨,�����궨
                {
                    N2_OBS_F  = 1 ;                //���̵��������Ѿ��洢
                    OBS_FLAG|=0X02 ;								//OBS_FLAG.1,�������̱��


                    Smoke_N2.data_8[1] = R_DATA[2];
                    Smoke_N2.data_8[0] = R_DATA[3];
                    OBS_N2.data_8[1] = R_DATA[4];
                    OBS_N2.data_8[0] = R_DATA[5];
                    //д��ǰ�������������ֵ��OBSֵ
                    temp16 = AT24CXX_ReadLenByte(0x01,2);
                    if(temp16 != Smoke_N2.data_16)	AT24CXX_WriteLenByte(0x01,Smoke_N2.data_16,2);

                    temp16 = AT24CXX_ReadLenByte(0x03,2);
                    if(temp16 != OBS_N2.data_16)	AT24CXX_WriteLenByte(0x03,OBS_N2.data_16,2);

                }
                if(R_DATA[1] == 1)//���̱궨�������궨
                {
                    AIR_OBS_F  = 1 ;                //���������Ѿ��洢
                    OBS_FLAG|=0X01 ;								//OBS_FLAG.0,�������̱��


                    Smoke_AIR.data_8[1] = R_DATA[2];
                    Smoke_AIR.data_8[0] = R_DATA[3];
                    OBS_AIR.data_8[1] = R_DATA[4];
                    OBS_AIR.data_8[0] = R_DATA[5];
                    //д��ǰ���������ֵ��OBSֵ
                    temp16 = AT24CXX_ReadLenByte(0x05,2);
                    if(temp16 != Smoke_AIR.data_16)	AT24CXX_WriteLenByte(0x05,Smoke_AIR.data_16,2);

                    temp16 = AT24CXX_ReadLenByte(0x07,2);
                    if(temp16 != OBS_AIR.data_16)	AT24CXX_WriteLenByte(0x07,OBS_AIR.data_16,2);

                }
                if(R_DATA[1] == 2)//�����̱궨
                {

                    N2_OBS_F   = 0 ;          //���㵪��OBSƫ���Ѿ��궨���
                    OBS_OVER_F = 0 ;          //����OBS�궨������ϱ��
                    OBS_FLAG  &=0X0f9 ;       //����OBS�궨��������	OBS_FLAG.1,������ǣ�OBS_FLAG.2,������ϱ��
                }
                if(R_DATA[1] == 3)//�����̱궨
                {
                    AIR_OBS_F  = 0 ;          //�������OBSƫ���Ѿ��궨���
                    OBS_OVER_F = 0 ;          //����OBS�궨������ϱ��
                    OBS_FLAG  &=0X0fa;        //����OBS�궨��������//OBS_FLAG.0,������ǣ�OBS_FLAG.2,������ϱ��
                }
                if(R_DATA[1] == 4)//ȫ��궨
                {
                    AIR_OBS_F  = 0 ;          //�������OBSƫ���Ѿ��궨��ǣ�OBS_FLAG.0,�������
                    N2_OBS_F   = 0 ;          //���㵪��OBSƫ���Ѿ��궨��ǣ�OBS_FLAG.1,�������
                    OBS_OVER_F = 0 ;          //����OBS�궨������ϱ�ǣ�OBS_FLAG.2,������ϱ��
                    OBS_FLAG  &=0X0f8;        //����OBS�궨��������
                }
                if(1 < R_DATA[1] && R_DATA[1] < 5)//ֻҪ���������궨ֵ�������ԭ���ı궨����ֵ
                {

                    OBS_Y.data_16 = 0;
                    temp16 = AT24CXX_ReadLenByte(0x09,2);
                    if(temp16 != OBS_Y.data_16)	AT24CXX_WriteLenByte(0x09,OBS_Y.data_16,2);

                    OBS_BIAS = 0;
                    temp16 = AT24CXX_ReadLenByte(0x0b,2);
                    if(temp16 != OBS_BIAS)	AT24CXX_WriteLenByte(0x0b,OBS_BIAS,2);
                }

                if ((( N2_OBS_F & AIR_OBS_F ) == 1 ) & ( OBS_OVER_F == 0 ))
                {
                    Smoke_N2.data_16 = AT24CXX_ReadLenByte(0x01,2);//����ǰ���������OBSֵ
                    OBS_N2.data_16 = AT24CXX_ReadLenByte(0x03,2);

                    Smoke_AIR.data_16 = AT24CXX_ReadLenByte(0x05,2);//����ǰ���������OBSֵ
                    OBS_AIR.data_16 = AT24CXX_ReadLenByte(0x07,2);
                    if (( Smoke_AIR.data_16 > Smoke_N2.data_16 )                      //���������ż���
                            & ( OBS_AIR.data_16 > OBS_N2.data_16 ))
                    {
                        OBS_OVER_F = 1 ;
                        //N2_OBS_F   = 0 ;
                        //AIR_OBS_F  = 0 ;
                        OBS_FLAG   = 0X07;
                        Smoke_AIR.data_16 -= Smoke_N2.data_16;//������ģ������ֵ
                        OBS_AIR.data_16 -= OBS_N2.data_16;//����OBS��ֵ

                        OBS_Y.data_16 = ((Smoke_AIR.data_16)<<4) / (OBS_AIR.data_16)  ;//�����ӦOBSϵ��ֵ,�Ŵ�16��


                        Smoke_AIR.data_16 = ((OBS_Y.data_16 * OBS_N2.data_16)>>4) ;//�������б�ʵõ�����ģ����ֵ
                        if(Smoke_N2.data_16 > Smoke_AIR.data_16 )
                            OBS_BIAS = Smoke_N2.data_16  - Smoke_AIR.data_16  ;//���㴿�������µ�ģ����ֵ
                        else  {
                            OBS_BIAS = Smoke_AIR.data_16  - Smoke_N2.data_16   ;//���㴿�������µ�ģ����ֵ
                            OBS_BIAS |= 0X8000;//����ƫ��Ϊ�����
                        }

                        temp16 = AT24CXX_ReadLenByte(0x09,2);
                        if(temp16 != OBS_Y.data_16)	AT24CXX_WriteLenByte(0x09,OBS_Y.data_16,2);//д��ǰOBS�궨ϵ��

                        temp16 = AT24CXX_ReadLenByte(0x0b,2);
                        if(temp16 != OBS_BIAS)	AT24CXX_WriteLenByte(0x0b,OBS_BIAS,2);//д��ǰOBS�궨ģ��������

                    }
                }

                temp8 = AT24CXX_ReadOneByte(0x0d);
                if(temp8 != OBS_FLAG)	AT24CXX_WriteOneByte(0x0d,OBS_FLAG);//д��ǰOBS�궨���

                //�ظ�5�ֽ�
                T_DATA[0] = rs_485_contror;
                T_DATA[1] = OBS_Y.data_8[1];
                T_DATA[2] = OBS_Y.data_8[0];
                T_DATA[3] = OBS_FLAG;
                t_anlog.data_16 = 0;
                for(i=0; i<=3; i++)               //��У���
                    t_anlog.data_16 += T_DATA[i]; //����˫�ֽڣ���ֹ�������

                T_DATA[4] = t_anlog.data_8[0];//У���
                out_rs485(5);//�ظ�5�ֽ�����
            }
            else//У�����
            {

                TR_JISHU = 0;//���½���
            }
            break;
        }
        case 0xc4 : //0000  0100�D�D>�궨��������
        {
            //�ظ�5�ֽ�
            T_DATA[0] = rs_485_contror;
            T_DATA[1] = OBS_Y.data_8[1];
            T_DATA[2] = OBS_Y.data_8[0];
            T_DATA[3] = OBS_FLAG;
            t_anlog.data_16 = 0;
            for(i=0; i<=3; i++)               //��У���
                t_anlog.data_16 += T_DATA[i]; //����˫�ֽڣ���ֹ�������

            T_DATA[4] = t_anlog.data_8[0];//У���
            out_rs485(5);//�ظ�5�ֽ�����
            break;
        }
        default :
        {
            break;
        }
        }
        rs_485_contror = 0 ;//���㴦�������
        TR_JISHU = 0;
    }
}


//;-------------------------------------------------------------------
//int_RX();    	//rs485ͨ�Ŷ�ȡ�жϴ�����
//;-------------------------------------------------------------------
/*
̽���������ַ�뷢��2�ֽڣ�
1�ֽڨD�D�����ַ��
2�ֽڡ��������ַ��
*/
void int_RX(void)
{
    //timer_enable(TIMER13);
    //timer_counter_value_config(TIMER13,0);
    //�����ַ�����β�һ��ʱ��Ҫ��֤TR_JISHUΪ0��������һ�ν�����������
	
    if(TR_JISHU == 0)//RS485�������������ַ��
    {
        R_DATA[0] = usart_data_receive(USART0);//���������ַ��

        if( net_control_data == R_DATA[0]) //��ͬ��˵���ǵ�2�ν��������ַ�ֽ�
        {
            net_control_data = 0;
            esadd = R_DATA[0] >> 4;      //ȡ��ַ��
            esdata  = R_DATA[0]&0X0F;      //ȡ����
            rs_485_contror = R_DATA[0] ; //�˴����淢�������������,���������ݽ��մ�����
            switch(esdata)
            {
            case 0x01 : //0001�D�D>Ԥ��
            {
                //
                break;
            }
            case 0x02 : //0010�D�D>ģ��ɼ�������ѯ
            {
                if( esadd == benji_add) //��ַ��ȷ
                {
                    RS485_NEED = 1;//RS485�账���,�ǣ�1,��Ҫ����,��0,���账��
                    rs_485_contror = R_DATA[0]  ;//rs485ͨѶ���������
                }
                break;
            }
            case 0x03: //0011�D�D>OBS�궨,���̱궨
            {
                /*
                ���ط��ͣ�
                	1�ֽڨD�D�����ַ��
                	2�ֽڡ��������ַ��
                	3�ֽڡ����궨����
                	4�ֽڨD�D̽��ģ������ģ�����ɼ�ֵ����λ
                	5�ֽڡ���̽��ģ������ģ�����ɼ�ֵ�Ͱ�λ
                	6�ֽڨD�DOBS�궨ֵ��8��11λ
                	7�ֽڡ���OBS�궨ֵ��0��7λ
                	8�ֽڡ���У��λ

                */
                if( esadd == benji_add) //��ַ��ȷ
                {
                    TR_JISHU = 1;//�շ���������ʼ����
                    R_counter = 7;//����8�ֽڣ������ַ���ط����գ���Ҫ��һ
                    rs_485_contror = R_DATA[0]  ;//rs485ͨѶ���������
                }
                break;
            }
            case 0x04 : //0100�D�D>OBS�궨������ѯ
            {
                if( esadd == benji_add ) //��ַ��ȷ
                {
                    RS485_NEED = 1;//RS485�账���,�ǣ�1,��Ҫ����,��0,���账��
                    rs_485_contror = R_DATA[0]  ;//rs485ͨѶ���������
                }
                break;
            }

            default : //δ֪����
            {
                break;
            }
            }
        }
        else//��δ���յ������ַ��
        {
            net_control_data = R_DATA[0];//��һ������ȷ�������ַ��
        }
    }
    else//���������ֽ�
    {
        R_DATA[TR_JISHU++] = usart_data_receive(EVAL_COM1);
        if(TR_JISHU == R_counter)//�Ѿ����յ��㹻�ֽ�
        {
            //У��λУ��
            RS485_NEED = 1;      //;RS485�账����=1,��Ҫ,=0,����
        }
    }
    if(RS485_NEED == 1)//RS485�账����=1,˵������ͨ�Ž��ճɹ�
    {
        //****************//
    }
}


//;-------------------------------------------------------------------
//int_TX();    	//rs485ͨ�ŷ����жϴ�����
//;-------------------------------------------------------------------
/*
���������ַ�뷢��2�ֽڣ�
1�ֽڨD�D������
2�ֽڡ�����ַ��
*/
void int_TX(void)
{
    if ( TR_JISHU < T_counter )//����û������
    {
        usart_data_transmit(USART0, T_DATA[TR_JISHU++]);
    }
    else { //���ݷ�����
        usart_interrupt_disable(USART0, USART_INT_TBE);
        TR_JISHU       = 0;
        T_counter      = 0;
        R_counter      = 0;
        rs_485_contror = 0;  //rs485ͨѶ���������
        RS485_RX_Statue = 1; //rs485��Ҫ�ý���
    }
}




#include "UdpPacket.h"
#include "lwip_comm.h"
#include "usart.h"
#include "led.h"
#include "lwip/api.h"
#include "lwip/lwip_sys.h"
#include "string.h"
#include "TaskManage.h"		//ͳһ�������ȼ��Ͷ�ջ��С

//UDP������
TaskHandle_t UDPTask_Handler;

u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ�����
//UDP������������
const char *udp_demo_sendbuf="Apollo STM32F4/F7 NETCONN UDP demo send data\r\n";
u8 udp_flag;							//UDP���ݷ��ͱ�־λ


//udp������
static void udp_thread(void *arg)
{
	err_t err;
	static struct netconn *udpconn;
	static struct netbuf  *recvbuf;
	static struct netbuf  *sentbuf;
	struct ip_addr destipaddr;
	u32 data_len = 0;
	struct pbuf *q;
	
	LWIP_UNUSED_ARG(arg);
	udpconn = netconn_new(NETCONN_UDP);  //����һ��UDP����
	udpconn->recv_timeout = 10;  		
	
	if(udpconn != NULL)  //����UDP���ӳɹ�
	{
		err = netconn_bind(udpconn,IP_ADDR_ANY,UDP_DEMO_PORT); 
		IP4_ADDR(&destipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]); //����Ŀ��IP��ַ
    netconn_connect(udpconn,&destipaddr,UDP_DEMO_PORT); 	//���ӵ�Զ������
		if(err == ERR_OK)//�����
		{
			while(1)
			{
				if((udp_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					sentbuf = netbuf_new();
					netbuf_alloc(sentbuf,strlen((char *)udp_demo_sendbuf));
					memcpy(sentbuf->p->payload,(void*)udp_demo_sendbuf,strlen((char*)udp_demo_sendbuf));
					err = netconn_send(udpconn,sentbuf);  	//��netbuf�е����ݷ��ͳ�ȥ
					if(err != ERR_OK)
					{
						printf("����ʧ��\r\n");
						netbuf_delete(sentbuf);      //ɾ��buf
					}
					udp_flag &= ~LWIP_SEND_DATA;	//������ݷ��ͱ�־
					netbuf_delete(sentbuf);      	//ɾ��buf
				}	
				
				netconn_recv(udpconn,&recvbuf); //��������
				if(recvbuf != NULL)          //���յ�����
				{ 
					taskENTER_CRITICAL(); //���ж�
					memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
						else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					taskEXIT_CRITICAL(); //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣
					printf("%s\r\n",udp_demo_recvbuf);  //��ӡ���յ�������
					netbuf_delete(recvbuf);      //ɾ��buf
				}
				else 
				vTaskDelay(5);  //��ʱ5ms
			}
		}else printf("UDP��ʧ��\r\n");
	}else printf("UDP���Ӵ���ʧ��\r\n");
}


//����UDP�߳�
//����ֵ��0 UDP�̴߳����ɹ�
//        others UDP�̴߳���ʧ��
uint8_t udp_demo_init(void)
{
	uint8_t res;
	
	taskENTER_CRITICAL();	 //���ж�,�����ٽ���
	res = xTaskCreate((TaskFunction_t)udp_thread,//����UDP�߳�
					(const char*  )"udp_thread_task",
					(uint16_t     )UDP_STK_SIZE,
					(void*        )NULL,
					(UBaseType_t  )UDP_PRIO,
					(TaskHandle_t*)&UDPTask_Handler);
  taskEXIT_CRITICAL();		//���жϣ��˳��ٽ���
	
	return res;
}

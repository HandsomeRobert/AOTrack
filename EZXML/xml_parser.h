#ifndef __XML_PARSER_H
#define __XML_PARSER_H
#include "sys.h"

//�߿�ڵ�
typedef struct Parser_Border_t{
	u8 type, effect, which, speed;
	//0: �ޱ߿� 1: ��ɫ�߿� 2: ��ɫ�߿�, �߿�ͼ��, ��Ч����, ��Ч�ٶ�
	u32 border_clr;	//�߿���ɫ
}Parser_Border_t;
//�ļ��ڵ�
typedef struct Parser_File_Node_t{
	u8 type;					//�ļ����� 0:��Ƶ 1:ͼƬ
	char *path;					//�ļ�·��
	u8 enter_effect, enter_time;//ͼƬ������Ч, ����ʱ��
	u32 stay_time;				//ͼƬͣ��ʱ��
	struct Parser_File_Node_t *next;
}Parser_File_Node_t;
//��ͨ���ڽڵ�
typedef struct Parser_Window_t{
	char *name;				//������
	u8 type;				//����
	int x_y_w_h[4]; 		//��ʼλ��(x,y)����(w,h)
	Parser_Border_t border;			//�߿�����
	struct Parser_Window_t *next;
}Parser_Window_t;
//��Ŀҳ�ڵ�
typedef struct Parser_Program_t{
	char *name; 			//��Ŀ��
	char is_inter_cut; 		//0: �ǲ岥��1: �岥��Ŀ
	int x_y_w_h[4]; 			//��ʼλ��(x,y)����(w,h)
	Parser_Border_t border; 			//�߿�����
	char *date_scope; 		//���ڷ�Χ
	char *time_scope; 		//ʱ�䷶Χ
	u8 week_sope[7]; 		//���ڷ�Χ
	char *gps_start_time; 	//ͬ���ֲ���ʼʱ�� -1��δ����
	u32 gps_time_unit; 		// GPSͬ���ֲ���Ŀ��λʱ��-1��δ����
	Parser_Window_t window;
	struct Parser_Program_t *next;
}Parser_Program_t;
//���ڵ�
typedef struct Parser_LoopProgram_t{
	u8 mode;
	Parser_Program_t *program;
}Parser_LoopProgram_t;
//�ṹ�嶨��
extern Parser_LoopProgram_t lp_t;
extern Parser_Program_t p_t;
extern Parser_Window_t w_t;
extern Parser_Border_t b_t;
//����һ��Ŀ¼·��
void parseXml(const char* path);
void parseXmlTest(const char* path);


#endif //__XML_PARSER_H

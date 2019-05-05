#ifndef __XML_PARSER_H
#define __XML_PARSER_H
#include "sys.h"

//边框节点
typedef struct Parser_Border_t{
	u8 type, effect, which, speed;
	//0: 无边框 1: 花色边框 2: 纯色边框, 边框图案, 特效类型, 特效速度
	u32 border_clr;	//边框颜色
}Parser_Border_t;
//文件节点
typedef struct Parser_File_Node_t{
	u8 type;					//文件类型 0:视频 1:图片
	char *path;					//文件路径
	u8 enter_effect, enter_time;//图片进场特效, 进场时间
	u32 stay_time;				//图片停留时间
	struct Parser_File_Node_t *next;
}Parser_File_Node_t;
//普通窗口节点
typedef struct Parser_Window_t{
	char *name;				//窗口名
	u8 type;				//类型
	int x_y_w_h[4]; 		//起始位置(x,y)与宽高(w,h)
	Parser_Border_t border;			//边框属性
	struct Parser_Window_t *next;
}Parser_Window_t;
//节目页节点
typedef struct Parser_Program_t{
	char *name; 			//节目名
	char is_inter_cut; 		//0: 非插播；1: 插播节目
	int x_y_w_h[4]; 			//起始位置(x,y)与宽高(w,h)
	Parser_Border_t border; 			//边框属性
	char *date_scope; 		//日期范围
	char *time_scope; 		//时间范围
	u8 week_sope[7]; 		//星期范围
	char *gps_start_time; 	//同步轮播开始时间 -1表未设置
	u32 gps_time_unit; 		// GPS同步轮播节目单位时长-1表未设置
	Parser_Window_t window;
	struct Parser_Program_t *next;
}Parser_Program_t;
//根节点
typedef struct Parser_LoopProgram_t{
	u8 mode;
	Parser_Program_t *program;
}Parser_LoopProgram_t;
//结构体定义
extern Parser_LoopProgram_t lp_t;
extern Parser_Program_t p_t;
extern Parser_Window_t w_t;
extern Parser_Border_t b_t;
//传入一个目录路径
void parseXml(const char* path);
void parseXmlTest(const char* path);


#endif //__XML_PARSER_H

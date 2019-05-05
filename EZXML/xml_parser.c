//#include "sys.h"
//#include "delay.h"  
//#include "usart.h"
//#include "ezxml.h"
//#include "xml_parser.h"
//#include "utility.h"


////结构体定义
//Parser_LoopProgram_t lp_t;
//Parser_Program_t p_t;
//Parser_Window_t w_t;
//Parser_Border_t b_t;

//void parseXml(const char* path){
//	ezxml_t lp, program, window;
//	char *xywh;
//	int count = 0;
//	Parser_Program_t *p = NULL;
//	Parser_Window_t *w = NULL;
//	p = &p_t;
//	w = &w_t;
//	printf("parser begin-----------------------------------\r\n");
//	lp = ezxml_parse_file(path);
//	lp_t.mode = Util_Str2Int(ezxml_attr(lp, "mode"));
//	printf("mode: %d\r\n", lp_t.mode);
//	for (program = ezxml_child(lp, "Program"); program; program = program->next) {
//		p->name = (char *)ezxml_attr(program, "name");
//		p->is_inter_cut = Util_Str2Int(ezxml_attr(program, "is_inter-cut"));
//		xywh = ezxml_child(program, "XYWH")->txt;
//		Util_Str2Buff(p->x_y_w_h, xywh, ",", 4);
//		printf("pro_x_y_w_h: %d, %d, %d, %d\r\n", p->x_y_w_h[0], p->x_y_w_h[1], p->x_y_w_h[2], p->x_y_w_h[3]);
//		for (window = ezxml_child(program, "Window"); window; window = window->next) {
//			w->name = (char *)ezxml_attr(window, "name");
//			xywh = ezxml_child(window, "XYWH")->txt;
//			Util_Str2Buff(w->x_y_w_h, xywh, ",", 4);
//			printf("p:%s, window name: %s\r\n", p->name, w->name);
//			printf("win_x_y_w_h: %d, %d, %d, %d", w->x_y_w_h[0], w->x_y_w_h[1], w->x_y_w_h[2], w->x_y_w_h[3]);
//			if(window->next){//申请下一个窗口的内存
//				printf("window has next!!!\r\n");
//				w->next = mymalloc(SRAMIN, sizeof(Parser_Window_t));
//				w = w->next;
//			}
//		}
//		while(w_t.next){//释放窗口结构体申请的内存
//			w = w_t.next;
//			w_t.next = w->next;
//			myfree(SRAMIN,w);
//		}
//		if(program->next){//申请下一个节目页的内存
//			printf("program has next!!!\r\n");
//			p->next = mymalloc(SRAMIN, sizeof(Parser_Program_t));
//			p = p->next;
//		}
//	}
//	while(p_t.next){//释放节目页结构体申请的内存
//		p = p_t.next;
//		p_t.next = p->next;
//		myfree(SRAMIN,p);
//	}
//	ezxml_free(lp);
//}
//void parseXmlTest(const char* path){
//	ezxml_t f1 = ezxml_parse_file(path), team, driver;
//	const char *teamname;
//	printf("parser begin-----------------------------------\r\n");
//	for (team = ezxml_child(f1, "team"); team; team = team->next) {
//		teamname = ezxml_attr(team, "name");
//		for (driver = ezxml_child(team, "driver"); driver; driver = driver->next) {
//			printf("%s, %s: %s\n", ezxml_child(driver, "name")->txt, teamname,
//				   ezxml_child(driver, "points")->txt);
//		}
//	}
//	ezxml_free(f1);
//}



µ±¿ª·¢°åÁªÍøÍê³ÉÒÔºó¿ÉÒÔÔÚµçÄÔÉÏÏò¿ª·¢°å·¢ËÍping°üÀ´²âÊÔÍøÂçÊÇ·ñÍ¨³©¡£
	
	±¾µØIP£º192.168.66.10£¬Ô¶¶ËIP£º192.168.66.2£¬¶Ë¿Ú5000¡£ÓÃÂ·ÓÉÆ÷Ê±ÇëÖ±½ÓÔÚ³ÌÐòÖÐÐÞ¸ÄÒªÁ¬½ÓµÄ·þÎñÆ÷IPµØÖ·¡£
	Á¬½Ó³É¹¦ºó×Ô¶¯·¢ËÍ100¸ö0¡£²âÊÔÊ±ÇëÖ±½ÓÁ¬½Ó´®¿Ú£¬·½±ã²é¿´¸÷ÖÖÐÅÏ¢
	
×¢ÒâÊÂÏî:

	3,±¾Àý³ÌÔÚ³õÊ¼»¯ÍøÂçµÄÊ±ºòÐèÒªÁ¬½ÓÍøÏß£¬·ñÔò³õÊ¼»¯²»³É¹¦£¡£¡

	°å¿¨¼àÌý¶Ë¿Ú£º192.168.66.2£º5000
	µçÄÔ¼àÌý¶Ë¿Ú£º192.168.66.10£º5001

	 
¿ª·¢¼ÇÂ¼
2019.3.5£º
	×Ü½á£º
	Á½¸öÖ÷Ïß³ÌTcpPacketServer,DataTransferManage·Ö±ðÓÃÓÚ¹ÜÀíSOCKETºÍSOCKETÊý¾Ý½ÓÊÕ
	1. TcpPacketServer¼ì²âµ½ÓÐÐÂµÄsocketÁ¬½ÓÊ±±ä»áÍ¬Ê±Õë¶ÔÀ´·ÃµÄClientµÄIPÍ¬Ê±½¨Á¢NetConnRecvºÍNetConnSendÊÕ·¢socket¡£
	2. DataTransferMangeÓÃÓÚ²»¶ÏÉ¨ÃèÊÇ·ñÓÐÊÕ·¢Êý¾Ý£¬É¨Ãè¼ä¸ôÎª100ms
	3. ×Ü¹²ÓÐ6¸öÏß³Ì£¬³ýÈ¥ÉÏÃæÁ½¸öÏß³ÌÍâ£¬»¹ÓÐÈý¸ö·Ö±ðÎª£º
	   Start_task: ÏµÍ³Æô¶¯Ïß³Ì£¬Æô¶¯ºó×Ô¶¯É¾³ý
           TCPIP_THREAD : LWIPÄÚºËÏß³Ì
	   StatusIndicate£º×´Ì¬ÏÔÊ¾Ïß³Ì£¬LED 1sÉÁÒ»´Î
	   DHCPÈÎÎñÏß³Ì
	   tcp_client_thread£ºTCP ¿Í»§¶ËÏß³Ì
	4. ÓÃÁËµÄÄÚ´æ²»ÐèÒªÊ±Ò»¶¨ÒªÊÍ·Å£¬·ñÔò»áÔì³ÉÄÚ´æÐ¹Â©£¡£¡£¡Èçstruct netbuf *recvnetbuf;¶¨¶¨ÒåµÄrecvnetbuf²»ÓÃÊ±Ò»¶¨ÒªÊÍ·Å£¡
	¹¦ÄÜ£º
	  1. ×î¶à¿ÉÒÔÁ¬½Ó8¸öclient,16¸ösocket(Receive,Send),ÊµÏÖÍ¬Ê±ÊÕ·¢¡£
	
	³öÏÖµÄÎÊÌâ£º
	  1. Ê¹ÓÃµÄÊÇÊý×éÀ´´æ´¢socket£¬NetConnRecv/NetConnSend[MaxClinets]¡£µ±¹Ø±ÕÄ³Ò»¸öÁ´½ÓÊ±£¬ÓÉÓÚforÑ­»·ÈÔ»á±éÀúÉ¾ÁËµÄnetconn
	     Ôì³É±¨´í£¬Assertion "netconn_accept: invalid recvmbox" failed at line 464 in ..\LWIP\lwip-1.4.1\src\api\api_lib.c
			AssertError: .../FreeRTOS/queue.c,729/730

2019.3.8:
	Ìí¼ÓÁË±àÂëÆ÷ºÍ¹âµç´«¸ÐÆ÷¼ì²â

2019.3.11:
	Ê¹ÓÃsession+clientIDµÄÐÎÊ½À´Í³Ò»¹ÜÀísocket½Ó¿Ú£¬ÏÖÔÚ¿ÉÒÔËæÊ±¿ª¹ØÈÎÒâµÄclient
	ÐÂÔöÊ¶±ðÖØ¸´clientIDÁ¬½Ó£¬ÈçÓÐÔòÉ¾³ýÐÂµÄnetconnRecv.µ«ÎªÁË¿´µ½clientNumÊýÁ¿µÄÔö³¤£¬ÔÝÊ±ÆÁ±Î´Ë¹¦ÄÜ
	Êä³öÄ¿Ç°ËùÁ¬½ÓµÄclientÊýÁ¿
	Ê¹ÓÃ×¢Òâ£ºµÃÔÚPCÉÏ´´½¨Ò»¸öTCP Server¸øSTM32À´´´½¨NetConnSendÍ¨µÀ£¬ÒòÎªÒ»¸öSession±ØÐëµÃÓÐNetConnRecvºÍNetConnSend£¬²»È»»á·¢ËÍ/½ÓÊÕÊý¾Ý³ö´í
2019.3.13 
	Ìí¼ÓTracking³ÌÐò£¬¼òµ¥µÄÈýµã¸ú×Ù£¬¸ù¾ÝÏà¶ÔÎ»ÖÃ£¬ÓÉÓÚ±àÂëÆ÷µÄÖµÐè²»¶Ï¶ÁÈ¡£¬»á³öÏÖÒ»¶¨³Ì¶ÈµÄEncoderÊýÖµÆ«²î£¬so,ÈÏÎªÔÚ400´¦ÍùºóÑÓÐø20¸ötimeÄÚµ½´ïµÄÊýÖµ¶¼ÈÏÎªÊÇ400ÕâÒ»Ê±¿Ì

2019.3.14
	¼ÓÈëTimer·ÂÕæÓÃÀ´´úÌæ±àÂëÆ÷£¬TimerÖÜÆÚÎª10ms£¬ ¶ÔÓ¦Èý¸ö¸ú×ÙµãµÄÏà¶ÔÔ­µãÎ»ÖÃ·Ö±ðÎª1000£¬ 2000 £¬ 3000. Ò²¾ÍÊÇ10sºó»áÅÄÕÕ£»20sºó»á´¥·¢´«¸ÐÆ÷£»30sºó»á¸ø³öPusherÐÅºÅ
	
2019.3.18
	Ìí¼ÓÊý¾Ý×°ÔØÆô¶¯½ø³Ì£¬Ê¹ÓÃ¶þÖµÐÅºÅÁ¿À´Çý¶¯
	
2019.3.26
	¹âµç¼ì²âÊ¹ÓÃ¶¨Ê±Æ÷TIM2¸ÄÎªTIM10(1us¼ÆÊýÖÜÆÚ£¬×î´ó²»´¥·¢ÖÐ¶ÏÄÜ²¶»ñ65msµÄÂö³å),TIM2&TIM5ÓÃÓÚÍâ½Ó±àÂëÆ÷
	Ìí¼ÓSD¿¨Çý¶¯£¬SDRAM, FATFS,ezxml£¬À´´ÓÍâÖÃSD¿¨¶ÁÈ¡ÎÄ¼þºÍÐ´ÈëÎÄ¼þ
	¼ÓÈëÁËNAND==>256MBÄÚ´æºÍQSPI==>32MBÄÚ´æÇý¶¯
	ÎªÄÚ²¿SRAMºÍÍâ²¿SDRAM¼ÓÁËMPUÄÚ´æ±£»¤
	¼ÓÈëXML½âÎöÏß³Ì£¨5SÉ¨ÃèÒ»´ÎÊÇ·ñÖØÐÂ½âÎö²ÎÊý£©£¬¿ÉÒÔÕýÈ·½âÎöTracking.xmlÎÄ¼þ
	XML½âÎöÊ¹ÓÃµÄÄÚ´æÊÇÍâÖÃSDRAM£¬Ö÷ÒªÊ¹ÓÃº¯Êý£¬myalloc(SRAMEX,XXX,XXX)ºÍmyrealloc(SRAMEX, XXX,XXX)
	
	
	×¢Òâ£ºÄ¿Ç°XMLÎÄ¼þ½âÎö½öÖ§³ÖÃû×ÖÓ¢ÎÄ£¬ÆäËüËùÓÐ²ÎÊý¾ùÎªÊý×Ö»òÕßboolÐÍ£¡£¡£¡
	
2019.3.27
	¼ÓÈëÍêÕûµÄÊµ¼Ê¸ú×Ù¹ý³Ì£¬µ«ÊÇÎ´²âÊÔ¡£
	Ä¿Ç°³öÏÖBUGÎÞ·¨Á¬½ÓTCP SEVERÉÐ²»ÖªµÀÊÇºÍÔ­Òò£¬µÈ´ý½â¾ö
	È·ÈÏÊÇÓÉÓÚÒÔÌ«Íø¿Ú×ÊÔ´ÅäÖÃ´íÎó,ÐÂÅäÖÃµÄ×ÊÔ´ÓëÔ­ÓÐÒÔÌ«Íø×ÊÔ´Ïà³åÍ»Ôì³ÉµÄ£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡(ÒÑÔÚ2019.4.1°æ½â¾ö)

2019.3.28
	ÒÆ³ýÁËNAND==>256MBÄÚ´æºÍQSPI==>32MBÄÚ´æÇý¶¯£¬ÒòÎªÓÃ²»ÉÏ
	ÔÚdiskio.cÖÐ°ÑQSPI FLASHºÍNANDµÄÖ§³ÖÈ¥µô,ÕâÁ½ÕßÓëLAN8720.CÖÐµÄ×ÊÔ´³öÏÖÁË³åÍ»£¬
	°´×¡RESET¿ÉÒÔÖØÆôLAN£¬µ«ÊÇ¶Ïµçºó¾ÍÎÞ·¨ÖØÆôLANÁË
	PS!!:¾¹È»ÖØÐÂÏÂÔØ³ÌÐòÒ²²»ÐÐ£¬µÃÏÂÖ®Ç°µÄ³ÌÐò¼¤»îÒ»ÏÂLANµÄÄ³¸ö¶«Î÷ÔÙÏÂÔØÒªÅÜµÄ³ÌÐò²ÅÄÜÖ´ÐÐ.....ÐþÑ§~~~(ÒÑÔÚ2019.4.1°æ½â¾ö)
	
	¸ú×ÙÉè¼ÆÓÐÎÊÌâ£¬ÐèÖØÐÂÉè¼Æ¸ú×Ù¹ý³Ì....Ó¦¸ÃÊÇEncoderNum> ÏàÓ¦µÄTargetValue¾ÍÁ¢Âí´¥·¢£¬ÇÒ²»ÄÜÖØ¸´´¥·¢ºÍ±àÂëÆ÷·´×ªÊ±´¥·¢...(ÒÑÔÚ2019.3.29°æ½â¾ö)
	TIM10ÎÞ·¨²¶»ñ´óÓÚ65536usµÄÂö³å£¬½øÈëÖÐ¶ÏÓÐÎÊÌâ
	
2019.3.29
	¼ÓÈë¶ÔÏóÉú´æÖÜÆÚFLAG==objectAliveFlag, Ä¬ÈÏÌÞ³ý´¦Îª¶ÔÏóÉúÃüÖÜÆÚµÄ½áÊøµã£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡£¡
	´Ë°æ±¾¿ÉÒÔÍêÃÀ×·×Ù¶ÔÏó£¬Ö»Òª¹ýÁËTarget¾Í¿ÉÒÔÖ´ÐÐ¶¯×÷£¬¼È²»»áÖØ¸´Ö´ÐÐ£¬Ò²²»»áÂÒ´¥·¢£¬±àÂëÆ÷ËÙ¶ÈÎÞÂÛ¿ìÂý¾ù¿É£¬yes!
	¾ÍÊÇTrackingÅÜÒ»´ÎµÄÊ±¼äºÃÏñÓÐµã³¤....¸ú×ÙÊµÏÖµÄÂß¼­ÓëÊ±¼äÍ³¼ÆµÃ¼ÓÉÏ
	LAN½Ó¿Ú¶Ïµç¾Í¹ÒÁËµÄÎÊÌâÈÔÎ´½â¾ö....(ÒÑÔÚ2019.4.1°æ½â¾ö)

2019.4.1
	½â¾öÁËLAN½Ó¿Ú¶Ïµç¾ÍÎÞ·¨Ê¹ÓÃµÄÎÊÌâ£¬Ô­ÒòÊÇ°ÑPCF8574_Init()È¥µôÁË£¬²»ÄÜÈ¥µôÒòÎªÆäÖÐÓÐETH_RESET_IOÒÔÌ«Íø¸´Î»Òý½Å£¬°üº¬ÒÔÌ«Íø¶ÔLAN¿ÚÏà¹ØIOÓë×ÊÔ´µÄ³õÊ¼»¯£¡£¡£¡£¡£¡PCF8574ÎªI/OÀ©Õ¹Ä£¿é
	¶ÔÏóÄÜ°´¸ú×Ù¶ÎµÄ´«µÝË³ÐòÒ»²½Ò»²½×ßÁË....¶ø²»ÊÇËùÓÐµÄ²Î¿¼¶¼ÊÇÍ¬Ò»¸öBornEncoderNum

2019.4.2
	ÓÉÓÚÊäÈë²¶»ñÊ¹ÓÃ¶¨Ê±Æ÷µÄ»°¶¨Ê±Æ÷»á²»¶Ï×Ô¶¯¼ÆÊý£¬Èç¹ûÊ¹ÓÃ16Î»¶¨Ê±Æ÷£¬ÄÇÃ´Ã¿¸ô65ms±ã»á´¥·¢Ò»´ÎÖÐ¶Ï£¬Ì«ÏûºÄ×ÊÔ´ÁË£¬¾ö¶¨¸ÄÓÃ32Î»¶¨Ê±Æ÷£¬¶ø±àÂëÆ÷¸ÄÓÃ16Î»¶¨Ê±Æ÷£¡£¡£¡
	ÐÞ¸Ä±àÂëÆ÷ºÍ¹âµçËùÊ¹ÓÃµÄ±àÂëÆ÷£¬±àÂëÆ÷¸ÄÓÃTIM1&TIM8£¬¹âµç¸ÄÓÃTIM2&TIM5£¬²âÊÔÎ´ÄÜ²¶»ñ¹âµçºÍ±àÂëÆ÷ÊýÖµ£¬ÈÔ´ý½â¾ö£¡£¡£¡£¡£¡£¡£¡

2019.4.3
	½â¾ö4.2µÄÎÊÌâ£¬¹âµçÊÇÒòÎªIO¿ÚÉèÖÃ´íÎó£¬±àÂëÆ÷ÊÇÒòÎª£¬±àÂëÆ÷1Ê¹ÓÃTIM1µÄTIM1_CH1N/CH2N, ÊäÈë²¶»ñ²»ÄÜÊ¹ÓÃCH1N/CH2N£¬±ØÐëÊ¹ÓÃCH1/CH2£¬ÓÉÓÚÏà¹Ø³åÍ»£¬ÎÞ·¨Ê¹ÓÃTIM1£¬¹ÊÐÞ¸Ä¹âµçÊ¹ÓÃTIM2&TIM5,
±àÂëÆ÷Ê¹ÓÃTIM3&TIM8!!!
	
2019.4.5
	¶ÔÃ¿¸ö¶ÔÏóÌí¼ÓÈý¸ö±êÖ¾Î»£ºobjectDelieverdFlag, objectPosition, delieverDestination; ÓÃÓÚÊµÏÖ¶¯×÷µÄ´«µÝ
	×¢Òâ£ºobjectPositionÊôÐÔºÍmodulePositionÊôÐÔÓÐ³åÍ»£¬ÐèÒªÐÞ¸Ä
2019.4.6
	È¥µôÁËmodulePosition¸ÄÎªÁËobjectModulePosition,ÔÚµ¥¸ö¹âµçµÄ²âÊÔÇé¿öÏÂÊµÏÖÁË¶¯×÷µÄ´«µÝ£¬¶à¸ö¹âµçÒ²ÐÐ£¬µ«»¹Î´²âÊÔ£¬¶à¸ö±àÂëÆ÷Ò²»¹Î´²âÊÔ£¬µÈ´ýÍ³Ò»¸øÓ²¼þ×ÊÔ´±àÂë

2019.4.10
	µÚÒ»¸ö¸ú×Ù¶ÎencoderNumber - moduleQueueTemp->DelieverdEncoderNumÕâÀï¿ÉÄÜ»áÈ¡µÃ¿ÕÖ¸ÕëÖ¸ÏòµÄÖµ»á³öÏÖÒì³£.....
	Ê¹ÓÃif(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)µÄ·½Ê½¿ÉÄÜ³öÏÖ±àÂëÆ÷µÃÏÈÓÚÏµÍ³×TrackingWindow¸ö´óÐ¡²ÅÄÜ´´½¨¶ÔÏó

2019.4.12
	½â¾öµÚ¶þ¸ö¹âµçÎÞ·¨Ê¹ÓÃÎÊÌâ£¬Ó²¼þI/O³õÊ¼»¯´íÎóµ¼ÖÂµÄ¡£¡£¡£Ê¹ÓÃÁ´±íÒÔ¼°ÕÅ×ÜÍÆ¼öµÄ·½Ê½È«ÐÂÉè¼ÆÁË¸ú×Ù·½°¸£¬ÊµÏÖÁËµ¥¶Î¸ú×Ù£¬´ý½â¾ö¶à¶Î¸ú×ÙºÍ´«µÝ
	
2019.4.13
	³ÌÐòÊÇÐ´ÍêÁË£¬µ«ÊÇObjectDetectionÖÐµÄvTaskDelay(1)²»ÖªµÀÎªÊ²Ã´²»ÄÜÓÃ£¬»á±¨³ÌÐòÒâÍâÍ£Ö¹¡£¡£¡£AssetError:..\FreeRTOS\portable\RVDS\ARM_CM7\r0p1\port.c,265

2019.4.14
	¸ú×Ù³õ²½ÊµÏÖ...
	½â¾ö4.13µÄÎÊÌâ£¬Ô­À´²»ÊÇObjectDetectionÖÐ³öÎÊÌâÁË£¬¶øÊÇTrackÏß³ÌÀïÒâÍâµÄÊ¹ÓÃÁËbreak...............ÎÒÐÞ¸ÄºóÃ»¸Ä»ØÀ´..........
	ParametersLoadÏß³Ì³öÎÊÌâÁË£¬Ã¿¸ö¸ú×Ù¶ÎµÄ¶ÔÏó½âÎöµÄÊ±ºòj_cycyle²¢Î´ÇåÁã£¬µ¼ÖÂ¶¯×÷Êýµþ¼Ó£¬Ç°Ãæ¼¸¸ö¾ÍÎ´½âÎöµ½....
	³öÏÖµÚ¶þ¶Î¸ú×Ù¶ÎÎÞ·¨´«µÝµ½µÚÈý¶Î£¬Module_i = 0, Destination = 0.¡£¡£ÔÝÎ´ÖªÉ¶ÎÊÌâ
2019.4.17
	½â¾öÁÐ±íÌí¼ÓÓëÉ¾³ýÔªËØÎÊÌâ£¬µ«ÊÇ·¢ÏÖ£¬»áÒ»Ö±Ñ­»·
	for(pActionList = ObjectInModuleList[Module_i]->next;pActionList != ObjectInModuleList[Module_i]; pActionList = pActionList->next)
	Ò»Ö±ÔÚmodule_i = 0 ´¦Ñ­»·¡£¡£¡£ÒòÎªpActionList²»ÖªµÀÎªºÎ±»¸³ÖµÁËlist1µÄÖ¸Õë£¬È»ºóÔÚlist0ÖÐÅÐ¶ÏÓÐÎÞµ½´ïÍ·½áµã£¬ËùÒÔÏÝÈëËÀÑ­»·
	ÎÊÌâµã1£ºÎªÊ²Ã´pActionList»á±»ÒâÍâµÄ¸³ÓèList1µÄÖµ£»
		  2£º¿´¿´ÄÜ·ñ´ÓÅÐ¶ÏÍ·½áµãÈëÊÖ£¨´ÓÑ­»·Ìõ¼þÈëÊÖ£©....
	½â¾öÁË£º½«TrackingµÄÓÅÏÈ¼¶¸ßÓÚObjectDection¾Í¿ÉÒÔÁË£¬ÒòÎªÔÚÖ´ÐÐObjectTackOver´«µÝÊ±±»ObjectDetection´ò¶ÏµÄ»°¾Í»áÒâÍâÐÞ¸ÄpActionListµÄµØÖ·....
	µ¥¶ÔÏó¼ì²âÃ»ÎÊÌâ£¬¶à¶ÔÏó¼ì²â³öÏÖBUG£¡£¡£¡£¡£¡£¡£¡£¡
2019.4.18
	»¹ÊÇ»áÒâÍâÏÝÈëËÀÑ­»·...¶øÇÒÁ´±íµÄ·½·¨Ð§ÂÊ²»¸ß...»¹²»ÈçÊ¹ÓÃÊý×éÁÐ±íÀ´Íê³ÉÀ´µÃ¿ì...
	1. Á´±íÉè¼ÆÓÐÎÊÌâ
	2. ObjecBuffer¹ÜÀíÓÐÎÊÌâ£¬»á½«64½øÈëCreateObjectBufferµ¼ÖÂÒâÍâ´íÎó²úÉú

2019.4.19
	µ¥¹âµçµ¥±àÂëÆ÷´«µÝ´´½¨µÈ²âÊÔÍ¨¹ý¡£½â¾öÉÏÃæµÄÎÊÌâ£¬»á½øÈëËÀÑ­»·ÊÇÒòÎªObjectBufferµÄ¶ÔÏóÓÃÁËÖ®ºóÎ´Çå³þ±ê¼ÇÎªfalse£¬MyListÖÐÒ²ÓÐ²¿·Ö³ö´í£¬
	²¢²»ÊÇÒòÎªÓÅÏÈ¼¶µÄÎÊÌâ£¬×îºÃÍø¶ÓÁÐÖÐ·¢ËÍÊý¾ÝÊ±²»Òª±»´ò¶Ï
	PS£º×ÜµÄÀ´ËµËãÊÇÍê³ÉÁË»ù±¾µÄ¸ú×ÙÁË...

	
2019.4.24 
	¼ÓÈëÐ´xml£¬µ«ÊÇÓÐBUG¡£¡£¡££¬Ð´ÁËÖ®ºó¾ÍÎÞ·¨¶ÁÁË...£¬ÁíÍâÐ´µÄ¶«Î÷Ò²Ã»ÓÐ»Ø³µ»»ÐÐ·û...
		ÖØÐÂÐÞ¸ÄObjectDetection³ÌÐò£¬ÓÅ»¯£¬µ«ÊÇÓÐBUG, µ±¸ú×Ù¶Î1´«µÝµ½¸ú×Ù¶Î2£¬ÓÐ´«µÝÐÅºÅ£¬µ«ÊÇÃ»ÓÐ¹âµç±»´¥·¢£¬Ôò²»»á´«µÝ¶¯×÷£¬ÄÇÃ´²»»á´¥·¢ÌÞ³ý£¬
	Ò²¾ÍÊÇObjectAliveFlag²»»á±»ÖÃfalse£¬Ò²¾Í»á³öÏÖObjectBufferÒç³ö´íÎó

2019.5.5 
	ÕÒµ½ÁË<AssetError:..\FreeRTOS\queue.c,1244> ³öÏÖµÄ´íÎóÔÚÓÚParametersLoadÖÐµÄ£¬xSemaphoreTake(OnSysRestart, 1000) ´Ë´¦»ñÈ¡¿ÕµÄÐÅºÅÁ¿Ê¹ÓÃÁË¿ÕÖ¸Õë³ö´í£¡£¡£¡
	½«×èÈûÊ±¼ä1000¸ÄÎª0¾Í¿ÉÒÔÁË£¿£¿£¿ÎªÊ²Ã´..........
	ÎªÉ¶Õâ¸ö¿ÉÒÔÊ¹ÓÃ....xSemaphoreTake(OnLoadParametersFromPC, 1000) == pdTRUE
	Í¨ÐÅ³É¹¦£¬´®¿ÚÖúÊÖ·¢ËÍ×Ö·û'1'»áÖØÆô°å¿¨¡£
Ä¿Ç°²ÎÊý¼ÓÔØ£¬¶ÔÏóÌ½²â£¬¸ú×Ù£¬Í¨ÐÅ£¬½ÓÊÕµ½µÄÊý¾Ý´¦Àí£¬¶¯×÷Ö´ÐÐ»ù±¾¹¦ÄÜÒÑ×ßÍ¨
	
2019.5.6 
	½â¾öBug1£¬TCPÖ»ÄÜÁ¬½Ó5´ÎµÄÎÊÌâ£¬Ô­ÒòÊÇÊ¹ÓÃnetconn_closeÖ»ÊÇ¹Ø±ÕÁË¶Ë¿Ú£¬Î´ÊÍ·Å×ÊÔ´µ¼ÖÂnew netconn£¨Ó¦Ê¹ÓÃnetconn_delete£©µÄÊ±ºòÉêÇë²»µ½¿Õ¼ä....£¬µ¼ÖÂÄÚ´æÐ¹Â©£¡£¡£¡
2019.5.7
	·¢ÏÖBug7¿ÉÄÜÔ­ÒòÊÇDMA½ÓÊÕËÙ¶ÈÌ«ÂýÁË£¬¿Õ¼äÂúÁË£¬×èÈûÁË£¬ÒòÎªÓÃµçÄÔÈí¼þ1S·¢Ò»´Î¾ÍÃ»É¶ÎÊÌâ¡£
	µ«ÊÇ£¬Ò²¾ÍXISPEKÈí¼þÁ¬»á×èÈû£¬ÎÒ×Ô¼ºµÄÓÃµ÷ÊÔÖúÊÖ²»»á³öÏÖ×èÈû£¨1msÖÜÆÚÑ­»··¢ËÍ£©....

2019.5.12
	Ìí¼ÓµÄTCPProtocolÊý¾Ý°üÉú³É£¬È»ºóÊ¹ÓÃDataTransoferManageÄÚµÄÊý¾Ý·¢ËÍ¿ÉÒÔ·¢ËÍ¸øPC£¬²âÊÔÍ¨ÐÅ³É¹¦¡£PCÈí¼þ½çÃæ»á£¨¸ú×Ù¿ØÖÆÆô¶¯£¬ÔÚObjectDetectionº¯ÊýÄÚÓÃCreateStartTrackingPacket·¢ËÍÊý¾Ý£©
½çÃæ»á³öÏÖ¡¶¿ØÖÆÆ÷¸ú×ÙÆô¶¯¡·×ÖÑù¡£²âÊÔÍ¨¹ý¡£½ÓÏÂÀ´¾ÍÊÇÍêÉÆÍ¨ÐÅºÍ¸ÄÓÃÊý×éÀ´ÊµÏÖ¸ú×Ù£¬¶¯Ì¬Á´±íËÙ¶ÈÂý£¬²»ÎÈ¶¨¡£¡£¡£

2019.5.13
	Ê¹ÓÃÊý×éÀ´ÊµÏÖTracking£¬¸ÄµôÁËÁ´±í£¬Á´±íËÙ¶ÈÂý¡££¨TrackingÖÜÆÚËÙ¶È4-15ms, ObjectDetectionÖÜÆÚÔÚ12-14ms×óÓÒ£©
	ObjectTakeOver ¶¯×÷ºÄÊ±£º9ms,  ÆäËü¶¯×÷4ms¡£¡£¡£
	×¢£ºÊ¹ÓÃ¶ÓÁÐµÄÊ±ºòÒ»¶¨Òª³õÊ¼»¯£¬¼´xQueueCreate,²»È»»á±¨´í?AssetError:..\FreeRTOS\queue.c,1245£¬1244
2019.5.14
	Ìí¼ÓÁËÒ»Ð©°üÉú³Éº¯ÊýÔÚTCPProtocolÄÚ
2019.5.15
	Ê¹ÓÃÖ¸ÕëÀ´·ÃÎÊÃ¿¸öÄ£¿éÀïµÄ¶¯×÷ºÍÏà¹ØÅäÖÃ¡£¡£¡££¨Ö÷ÒªÐÞ¸ÄÁËParametersLoadÄÚµÄÄÚÈÝ£©
2019.5.16
	ÐÞ¸´ÎÞ·¨ÊµÏÖActionRequestMachineData, ÊÇÔÚParametersLoadÎó¸ÄÁËstr_ActionRequestMachineData¡£¡£¡£¡£¡£¡£
	¼ÓÈëÁËTimer6ÓÃÓÚÍ³¼ÆÃ¿¸öÏß³ÌµÄÔËÐÐÊ±¼ä¡£
	ÍùObjectDetectionºÍTrackingÄÚ¼ÓÈëÁËTCPProtocolÐ­Òé£¬RunIn, RunOut¾ùÄ¬ÈÏÍùClientID = ClientServer¶Ë·¢ËÍ
	·¢ÏÖ×îºÄÊ±µÄÊÇprintf´®¿ÚÊä³öº¯Êý £¬ ºÍTrackingµÄ±éÀúËùÓÐÁÐ±íµÄforÑ­»·£¬
	Tracking¿ÕÔØ1374us×óÓÒ, ¸ºÔØÎª2200us×óÓÒ
	ObjectDetection ¿ÕÔØÎª420us×óÓÒ£¬ ¸ºÔØÎª1450us×óÓÒ
2019.5.21
	ËùÓÐÊý¾Ý¶¼·¢ÍùClientServer==10¶Ë¿Ú£¬Ã¿¸öSession¸÷½¨Á¢Ò»¸öBufferSendºÍBufferRecv£¬ÓÃÓÚ½ÓÊÕ·¢ËÍ»º³å£¬²»¶¯Ì¬·ÖÅäÄÚ´æ£¬²ÉÓÃ¿Õ¼ä»»Ê±¼ä£¬Ê¹ÓÃÒ»¸ö10¸ö´óÐ¡µÄÖ¸ÕëÊý×é£¬Ã¿¸öÖ¸Õë·ÖÅä128bytes.
	DataTransferManage¸ºÔð½«·¢ËÍ»º³åÇøµÄÊý¾Ý·¢ËÍ³öÈ¥£¬¸ºÔð½ÓÊÕÊý¾Ý²¢´æµ½½ÓÊÕÊý¾Ý»º³åÇøºó½»¸øDataProcessÀ´´¦Àí£¡£¡£¡
	Session[sessionID].BufferSend[i].pBufferData
	ÔÝÎªµ¥Client£¬ËùÓÐÊý¾Ý¶¼·¢ÍùClientServer,¶àClientÔÝÎ´²âÊÔ£¬Ä¿Ç°»¹ÓÐÎÊÌâ
2019.6.5 
	³É¹¦Á¬½ÓPC£¬³É¹¦¶Ô½ÓPCÉÏµÄÖ÷³ÌÐòºÍInspection³ÌÐò£¬ÄÜ·¢ËÍ´¥·¢Ïà»ú£¬´¥·¢IO£¬´¥·¢ÌÞ³ýÃüÁî¸ø¼ì²â³ÌÐò¡£Á¬½Ó³É¹¦£¬µÈ´ýÏÂÒ»²½¹¤×÷
2019.6.10
	1.½ÓÊÕÊý¾Ý²»Õý³££¬²¢²»»áÍêÕû½ÓÊÕÒ»¸öPKBG  XXXX PKED,·Ö±ð´æ´¢ÔÚ¶à¸öBufferÀï£¬µ¼ÖÂ½ÓÊÕ²»Õý³£¡£
	2.Ä¿Ç°µÄ½ÓÊÕ·½Ê½Èç¹û²»¼°Ê±´¦ÀíSession[sessionID].BufferRecv[i]ÀïµÄÊý¾Ý»áÒç³öµ½Session[sessionID].BufferSend[i]µ¼ÖÂÂÒÍùÍâ·¢Êý¾Ý¡£
ÉÏ»úÊµ²â³öÎÊÌâ²¹³ä£º
	1. ActionExecuteQueue³öÎÊÌâ£¬ÓÉÓÚÑÓÊ±µÄ³öÏÖ£¬Ê¹ÓÃµÄ´®ÐÐÑÓÊ±¶ø²»ÊÇ²¢ÐÐÑÓÊ±£¬´¦ÀíÓÐÎÊÌâ¡£
	2. ·¢ËÍËÙ¶ÈÌ«Âý£¬BufferSend is full£¬Êý¾ÝÎÞ·¨¼°Ê±µÄ·¢ËÍ¡£Object BufferÒ²×èÈûÁË£¬ÎÞ·¨°´Ê±´¦Àí
	3. ËÙ¶ÈÌ«Âý£¬×èÈû£¬
	4. Ïà»ú´¥·¢IDÎÞÐ§¡£
	
209.6.18
	1. May happen only two Client connect to STM32, just Restart the Xispek Software can solve it(Resume to three Clients).
	2. Recv_timeout must be set larger to zero.
	3. Successfully Use a whole area to store the receive data.
ADD:
	1. the reason why cannot create new Object is triggerInteval is used short, but value too Large, overflow to -XXX, so 
	   triggerInterval >  ModuleConfig[Module_i].Debounce failed. And Cannot Create New Object!!!!!!
	   Add ActionExecute will cause Damage Bug after Create 8 Objects
2019.6.20
	1. error: After Normal Create 51 Object, the program will be lost in ETH_IRQHandler, go into while() dead cycle!!!
       the Reason result in DataTransferManage.c where use break to jump out  "while(Session[Session_i].BufferSend[j].IsBufferAlive)"
		Oh, it lost again(After Create 456 Object...)......
	2. "while(ETH_GetRxPktSize(ETH_Handler.RxDesc))"==>Modify while loop to if Temporarily.....have a try, Create Object seems normally...
		At Present, Not Find Bug....Maybe exist.
2019.6.21
	1. Add ActionExecute to Program...Runtime Test, Can Stablly create object and trigger action...(test one night with 59550 Object)
	   But not finish the Receive DataProcess
2019.6.22
	1. Add A new way to DataProcess: use *PHead and *PTail to recv and process Data
ºÄÊ±·ÖÎö£»
	1. mymalloc(SRAMEX, 128) ·ÖÅäÄÚ´æÊ±¼äÎª800us×óÓÒ£¬myfreeÔÚ70us×óÓÒ£¬ËùÒÔÍÆ¼ö²»ÒªÈ¥¶¯Ì¬¿ª±ÙÄÚ´æ£¡£¡£¡×ñÑ­£º¿Õ¼ä»»Ê±¼ä£¡£¡£¡
	2. netconn_writeºÄÊ±³¤£¬´ó¸ÅÎª2356us-->5023us
	3. printfÖØ¶¨ÒåÊä³öµ½´®¿ÚUSART1, ºÄÊ±ºÜ³¤£¬ÐÔÄÜ²âÊÔÊ±²»Ó¦¸ÃÊ¹ÓÃprintf~
	4. /*ConsumeTime:720us*/xTaskGenericNotifyºÄÊ±ºÜ´ó£¬Îª720us×óÓÒ
	5. SOCKET·¢ËÍÊý¾ÝºÄÊ±ºÜ³¤£¬¸÷Ä£Ê½ÏÂºÄÊ±//NETCONN_NOFLAG:2830 - 5082  ==> NETCONN_COPY:3424-->4824 NETCONN_NOCOPY::3454-->5016 NETCONN_MORE::3424-->4956
	6. ObjectDetectionºÄÊ±£ºNo-Load 485us,Loaded==>1126/1314/1700
	7. ºÄÊ±±È½Ï³¤µÄÐèÒª¿¼ÂÇÒì²½´¦Àí£¬±ÈÈçnetconn_writeºÄÊ±³¤£¬¿´¿´ÊÇ²»ÊÇÓÐÒì²½´¦Àíº¯Êý...
	
×¢ÒâÊÂÏî£º
	1. 

Bug Report£º
XXXX1. 2019.3.14£º(2019.5.6 Solved½â¾öÁË,netconn_close¸ÄÎªnetconn_delete¼´¿É)
XXXXClientÓëSTM32£¨TecpServer£©¶à´ÎÁ¬½Ó&¶Ï¿ªºó£ 
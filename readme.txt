

当开发板联网完成以后可以在电脑上向开发板发送ping包来测试网络是否通畅。
	
	本地IP：192.168.66.10，远端IP：192.168.66.2，端口5000。用路由器时请直接在程序中修改要连接的服务器IP地址。
	连接成功后自动发送100个0。测试时请直接连接串口，方便查看各种信息
	
注意事项:

	3,本例程在初始化网络的时候需要连接网线，否则初始化不成功！！

	板卡监听端口：192.168.66.2：5000
	电脑监听端口：192.168.66.10：5001

	 
开发记录
2019.3.5：
	总结：
	两个主线程TcpPacketServer,DataTransferManage分别用于管理SOCKET和SOCKET数据接收
	1. TcpPacketServer检测到有新的socket连接时变会同时针对来访的Client的IP同时建立NetConnRecv和NetConnSend收发socket。
	2. DataTransferMange用于不断扫描是否有收发数据，扫描间隔为100ms
	3. 总共有6个线程，除去上面两个线程外，还有三个分别为：
	   Start_task: 系统启动线程，启动后自动删除
           TCPIP_THREAD : LWIP内核线程
	   StatusIndicate：状态显示线程，LED 1s闪一次
	   DHCP任务线程
	   tcp_client_thread：TCP 客户端线程
	4. 用了的内存不需要时一定要释放，否则会造成内存泄漏！！！如struct netbuf *recvnetbuf;定定义的recvnetbuf不用时一定要释放！
	功能：
	  1. 最多可以连接8个client,16个socket(Receive,Send),实现同时收发。
	
	出现的问题：
	  1. 使用的是数组来存储socket，NetConnRecv/NetConnSend[MaxClinets]。当关闭某一个链接时，由于for循环仍会遍历删了的netconn
	     造成报错，Assertion "netconn_accept: invalid recvmbox" failed at line 464 in ..\LWIP\lwip-1.4.1\src\api\api_lib.c
			AssertError: .../FreeRTOS/queue.c,729/730

2019.3.8:
	添加了编码器和光电传感器检测

2019.3.11:
	使用session+clientID的形式来统一管理socket接口，现在可以随时开关任意的client
	新增识别重复clientID连接，如有则删除新的netconnRecv.但为了看到clientNum数量的增长，暂时屏蔽此功能
	输出目前所连接的client数量
	使用注意：得在PC上创建一个TCP Server给STM32来创建NetConnSend通道，因为一个Session必须得有NetConnRecv和NetConnSend，不然会发送/接收数据出错
2019.3.13 
	添加Tracking程序，简单的三点跟踪，根据相对位置，由于编码器的值需不断读取，会出现一定程度的Encoder数值偏差，so,认为在400处往后延续20个time内到达的数值都认为是400这一时刻

2019.3.14
	加入Timer仿真用来代替编码器，Timer周期为10ms， 对应三个跟踪点的相对原点位置分别为1000， 2000 ， 3000. 也就是10s后会拍照；20s后会触发传感器；30s后会给出Pusher信号
	
2019.3.18
	添加数据装载启动进程，使用二值信号量来驱动
	
2019.3.26
	光电检测使用定时器TIM2改为TIM10(1us计数周期，最大不触发中断能捕获65ms的脉冲),TIM2&TIM5用于外接编码器
	添加SD卡驱动，SDRAM, FATFS,ezxml，来从外置SD卡读取文件和写入文件
	加入了NAND==>256MB内存和QSPI==>32MB内存驱动
	为内部SRAM和外部SDRAM加了MPU内存保护
	加入XML解析线程（5S扫描一次是否重新解析参数），可以正确解析Tracking.xml文件
	XML解析使用的内存是外置SDRAM，主要使用函数，myalloc(SRAMEX,XXX,XXX)和myrealloc(SRAMEX, XXX,XXX)
	
	
	注意：目前XML文件解析仅支持名字英文，其它所有参数均为数字或者bool型！！！
	
2019.3.27
	加入完整的实际跟踪过程，但是未测试。
	目前出现BUG无法连接TCP SEVER尚不知道是和原因，等待解决
	确认是由于以太网口资源配置错误,新配置的资源与原有以太网资源相冲突造成的！！！！！！！！！！！(已在2019.4.1版解决)

2019.3.28
	移除了NAND==>256MB内存和QSPI==>32MB内存驱动，因为用不上
	在diskio.c中把QSPI FLASH和NAND的支持去掉,这两者与LAN8720.C中的资源出现了冲突，
	按住RESET可以重启LAN，但是断电后就无法重启LAN了
	PS!!:竟然重新下载程序也不行，得下之前的程序激活一下LAN的某个东西再下载要跑的程序才能执行.....玄学~~~(已在2019.4.1版解决)
	
	跟踪设计有问题，需重新设计跟踪过程....应该是EncoderNum> 相应的TargetValue就立马触发，且不能重复触发和编码器反转时触发...(已在2019.3.29版解决)
	TIM10无法捕获大于65536us的脉冲，进入中断有问题
	
2019.3.29
	加入对象生存周期FLAG==objectAliveFlag, 默认剔除处为对象生命周期的结束点！！！！！！！！！！！！！！！！！！！！！！
	此版本可以完美追踪对象，只要过了Target就可以执行动作，既不会重复执行，也不会乱触发，编码器速度无论快慢均可，yes!
	就是Tracking跑一次的时间好像有点长....跟踪实现的逻辑与时间统计得加上
	LAN接口断电就挂了的问题仍未解决....(已在2019.4.1版解决)

2019.4.1
	解决了LAN接口断电就无法使用的问题，原因是把PCF8574_Init()去掉了，不能去掉因为其中有ETH_RESET_IO以太网复位引脚，包含以太网对LAN口相关IO与资源的初始化！！！！！PCF8574为I/O扩展模块
	对象能按跟踪段的传递顺序一步一步走了....而不是所有的参考都是同一个BornEncoderNum

2019.4.2
	由于输入捕获使用定时器的话定时器会不断自动计数，如果使用16位定时器，那么每隔65ms便会触发一次中断，太消耗资源了，决定改用32位定时器，而编码器改用16位定时器！！！
	修改编码器和光电所使用的编码器，编码器改用TIM1&TIM8，光电改用TIM2&TIM5，测试未能捕获光电和编码器数值，仍待解决！！！！！！！

2019.4.3
	解决4.2的问题，光电是因为IO口设置错误，编码器是因为，编码器1使用TIM1的TIM1_CH1N/CH2N, 输入捕获不能使用CH1N/CH2N，必须使用CH1/CH2，由于相关冲突，无法使用TIM1，故修改光电使用TIM2&TIM5,
编码器使用TIM3&TIM8!!!
	
2019.4.5
	对每个对象添加三个标志位：objectDelieverdFlag, objectPosition, delieverDestination; 用于实现动作的传递
	注意：objectPosition属性和modulePosition属性有冲突，需要修改
2019.4.6
	去掉了modulePosition改为了objectModulePosition,在单个光电的测试情况下实现了动作的传递，多个光电也行，但还未测试，多个编码器也还未测试，等待统一给硬件资源编码

2019.4.10
	第一个跟踪段encoderNumber - moduleQueueTemp->DelieverdEncoderNum这里可能会取得空指针指向的值会出现异常.....
	使用if(encoderNumber - moduleQueueTemp->DelieverdEncoderNum < ModuleConfig[Module_i].TrackingWindow)的方式可能出现编码器得先于系统譚rackingWindow个大小才能创建对象

2019.4.12
	解决第二个光电无法使用问题，硬件I/O初始化错误导致的。。。使用链表以及张总推荐的方式全新设计了跟踪方案，实现了单段跟踪，待解决多段跟踪和传递
	
2019.4.13
	程序是写完了，但是ObjectDetection中的vTaskDelay(1)不知道为什么不能用，会报程序意外停止。。。AssetError:..\FreeRTOS\portable\RVDS\ARM_CM7\r0p1\port.c,265

2019.4.14
	跟踪初步实现...
	解决4.13的问题，原来不是ObjectDetection中出问题了，而是Track线程里意外的使用了break...............我修改后没改回来..........
	ParametersLoad线程出问题了，每个跟踪段的对象解析的时候j_cycyle并未清零，导致动作数叠加，前面几个就未解析到....
	出现第二段跟踪段无法传递到第三段，Module_i = 0, Destination = 0.。。暂未知啥问题
2019.4.17
	解决列表添加与删除元素问题，但是发现，会一直循环
	for(pActionList = ObjectInModuleList[Module_i]->next;pActionList != ObjectInModuleList[Module_i]; pActionList = pActionList->next)
	一直在module_i = 0 处循环。。。因为pActionList不知道为何被赋值了list1的指针，然后在list0中判断有无到达头结点，所以陷入死循环
	问题点1：为什么pActionList会被意外的赋予List1的值；
		  2：看看能否从判断头结点入手（从循环条件入手）....
	解决了：将Tracking的优先级高于ObjectDection就可以了，因为在执行ObjectTackOver传递时被ObjectDetection打断的话就会意外修改pActionList的地址....
	单对象检测没问题，多对象检测出现BUG！！！！！！！！
2019.4.18
	还是会意外陷入死循环...而且链表的方法效率不高...还不如使用数组列表来完成来得快...
	1. 链表设计有问题
	2. ObjecBuffer管理有问题，会将64进入CreateObjectBuffer导致意外错误产生

2019.4.19
	单光电单编码器传递创建等测试通过。解决上面的问题，会进入死循环是因为ObjectBuffer的对象用了之后未清楚标记为false，MyList中也有部分出错，
	并不是因为优先级的问题，最好网队列中发送数据时不要被打断
	PS：总的来说算是完成了基本的跟踪了...

	
2019.4.24 
	加入写xml，但是有BUG。。。，写了之后就无法读了...，另外写的东西也没有回车换行符...
		重新修改ObjectDetection程序，优化，但是有BUG, 当跟踪段1传递到跟踪段2，有传递信号，但是没有光电被触发，则不会传递动作，那么不会触发剔除，
	也就是ObjectAliveFlag不会被置false，也就会出现ObjectBuffer溢出错误

2019.5.5 
	找到了<AssetError:..\FreeRTOS\queue.c,1244> 出现的错误在于ParametersLoad中的，xSemaphoreTake(OnSysRestart, 1000) 此处获取空的信号量使用了空指针出错！！！
	将阻塞时间1000改为0就可以了？？？为什么..........
	为啥这个可以使用....xSemaphoreTake(OnLoadParametersFromPC, 1000) == pdTRUE
	通信成功，串口助手发送字符'1'会重启板卡。
目前参数加载，对象探测，跟踪，通信，接收到的数据处理，动作执行基本功能已走通
	Test Robert
	
Bug Report：
	1. 2019.3.14：
	Client与STM32（TecpServer）多次连接&断开后，无法再次连接STM32了，第6次连接（单个client）时会出现报错：
		主机192.168.66.10连接上服务器,主机端口号为:49534
		Assertion "netconn_connect: invalid conn" failed at line 197 in ..\LWIP\lwip-1.4.1\src\api\api_lib.c
		TCP_Server Connect Failed!!!
		Assertion "netconn_write: invalid conn" failed at line 605 in ..\LWIP\lwip-1.4.1\src\api\api_lib.c
		Send data Failed,Please check it in DataTransferManage.c 
		
	第六次连接后断开时也报错，为Assertion "netconn_writAssertion "netconn_close: invalid conn" failed at line 668 in ..\LWIP\lwip-1.4.1\src\api\api_lib.c
	！！！猜测是第六次连接时，Session[i]	的NetConnSend 没有被创建，即访问了不存在的NetConnSend端口导致的错误。	
	！！至于为什么第6次之后就不能再连上PC的TCP Server了，有待探索。。。。
	2. for循环扫描Netconn[]数组会扫描到不存在的NETCONN，但我好像设了一个移位buffer将此问题解决了，yes,我可真棒呀
	3. emmm....电脑端连接TCP NETCONN使用串口调式软件无法快速发送数据，也就是说无法完整发送数据，得加个Buffer来缓冲...先发到Buffer再由NETCONN去调用函数去发送数据....

	4. 当编码器数值溢出时可能出现BUG，即所有跟踪段的参考设计 可能都会出错.....
		Solve,直接用两个数相减，得到的就是两者的距离，如0-65534 = 2
	5. EncoderNumber>其给定值是可能导致错误，会清零+定时器溢出次数*65536所以会导致错误产生，标记一下。
	
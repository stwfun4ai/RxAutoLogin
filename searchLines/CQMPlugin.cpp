/*********************************************************************
	欢迎您为按键精灵制作插件！
	这是按键精灵的插件模板 091026版

	请编译Release版的插件使用，Debug版的插件会导致按键精灵出现异常错误

  	如果有其他问题，欢迎通过以下方式和我们取得联系
	Email: hi@vrbrothers.com
	论坛: bbs.ajjl.cn
	
	插件模板更新记录:
	091026
	解决由于MFC的功能不完善导致所有的插件函数都在主线程中执行的问题。
	这个问题会导致插件效率低下，而且可能互相阻塞，甚至阻塞主线程。
*********************************************************************/

/*********************************************************************
 * 以下的内容，一般情况下您不需要看懂，也不需要进行修改
*********************************************************************/
#include "stdafx.h"
#include "CQMPlugin.h"
#include "QMLibraryStub.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <psapi.h>
#include <WinSock2.h>				//网络连接的头

#pragma comment(lib, "psapi.lib" )	//进程状态 API
#pragma comment(lib, "ws2_32.lib")	//网络连接API需要的静态库
#define MAX_RECV_BUFFER_LEN 3000	//接收buffer最大长度

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CQMPlugin, CCmdTarget)
	//{{AFX_MSG_MAP(CQMPlugin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifdef _DEBUG
	#pragma comment(lib,"CommonLibD.lib")
#else
	#pragma comment(lib,"CommonLib.lib")
#endif

BEGIN_DISPATCH_MAP(CQMPlugin, CCmdTarget)
	DISP_FUNCTION(CQMPlugin, "Get_Plugin_Description", GetPluginDescription, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CQMPlugin, "Get_Plugin_Interpret_Template", GetPluginInterpretTemplate, VT_BSTR, VTS_BSTR)
	//{{AFX_DISPATCH_MAP(CQMPlugin)
	DISP_FUNCTION(CQMPlugin, "getPartServerLines", getPartServerLines, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CQMPlugin, "getByteArray", getByteArray, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CQMPlugin, "writeData2Clickable", writeData2Clickable, VT_I4, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CQMPlugin, "writeData", writeData, VT_I4, VTS_I4 VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CQMPlugin, "getModuleSize", getModuleSize, VT_I4, VTS_I4 VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

IMPLEMENT_OLECREATE(CQMPlugin, "QMPlugin.", 0x55734646, 0x126E, 0x4FC8, 0xB4, 0x30, 0x1C, 0x61, 0x43, 0x48, 0x4A, 0xA9)

BOOL g_bEnablePluginValidate = TRUE;
/*********************************************************************
 * 以上的内容，一般情况下您不需要看懂，也不需要进行修改
*********************************************************************/

/*********************************************************************
	按键精灵的每个插件可以支持若干条命令。
	如果您需要写一条自己的命令，需要遵循以下步骤：
	1. 打开文件CQMPlugin.cpp，就是您现在看到的这个文件
	2. 在Visual C++ 6.0的菜单中选择 View->ClassWizard
	3. 在弹出的对话框中选择Automation面板，按Add Method按钮
	4. 依次填写您的插件函数的名字、返回类型和参数类型
	   需要注意的是，如果返回值是字符串类型，请选择BSTR
	   如果参数是字符串类型，请选择LPCTSTR
	   如果需要传地址的参数（可用于返回多个值），参数类型请选择VARIANT*，如下面的GetScreenResolution插件函数所示
	5. 按OK按钮，您会看到CQMPlugin.cpp有了一个空白的函数，您只需要填写函数功能即可
	6. 您也可以在Automation面板中选择删除某个已有的函数，并同时在下面的程序中删除对应的函数体
	7. 在下面的m_DescriptionList表格中填写您的插件说明和函数说明
	8. 编译形成DLL文件，把DLL文件放在按键精灵的Plugin目录下
	9. 打开按键精灵，进入编辑脚本界面，打开“插件命令”面板，编写脚本测试你的插件！
*********************************************************************/

/*********************************************************************
 * 名称:m_DescriptionList
 * 你可以把它看成一个表格，表格中的每一行代表了一条插件命令。
 * 每行第一个字符串是插件命令的名字（如果为空，则代表整个插件）；第二个字符串是这条插件命令的解释；
 * 第三个字符串是插件命令的显示模板信息，其中的$1 $2 ... 等内容在显示时将用第一个参数、第二个参数、... 代替
 * 当你需要新增一条插件命令的时候，就需要在表格里面加一行。
*********************************************************************/
CQMPlugin::DESCRIPTION_LIST CQMPlugin::m_DescriptionList[] = 
{
	{"","Rx相关",""},
	{"getPartServerLines","获取分区服务器的线路状况(字符串分区,字符串服务器(末尾加\"(新服)\"))[0满,1未满,逗号分隔]",""},
	{"getByteArray","获取分区返回数据(字符串分区)[字节数组],",""},
	{"writeData","修改内存数据(长整型窗口句柄,字符串内存地址,字符串二进制数据(小端))[0异常,1成功]",""},
	{"writeData2Clickable","修改禁用状态(长整型窗口句柄,字符串内存地址('|'分隔的十六进制字符串地址))[0异常,1成功]",""},
	{"getModuleSize","获取dll模块大小(长整型窗口句柄,字符串dll模块名称)[0异常]",""}
};

//这行您不需要看懂，也不需要修改
const int CQMPlugin::m_iDescriptionListSize = sizeof(CQMPlugin::m_DescriptionList) / sizeof(CQMPlugin::DESCRIPTION_LIST);

/*********************************************************************
 * 下面是各个插件命令的处理函数，函数名和插件名是对应的。
 * 如果要增加一个插件命令，通常Visual C++会帮您写一个空的处理函数
 * 如果要删除一个插件命令，别忘了把对应的处理函数也一起删掉。
*********************************************************************/


/*********************************************************************
 * 下面是公共处理函数。
 * OnLoad 函数会在插件被加载时调用，返回值通常为TRUE。如果返回FALSE，表示插件拒绝被加载
 * OnLoad 函数的第一个参数表示加载动态链接库的Instance Handle，第二个参数暂时保留
 * OnExit 函数会在插件被卸载是调用，没有返回值
*********************************************************************/
BOOL OnLoad(HMODULE hModule, int iReserved)
{
	return TRUE;
}

void OnExit()
{
	return;
}

//生成[0,(range-1)]之间的随机整数
int getRandNum(int range)
{
	srand(time(0));	//设置随机种子
	return rand()%range;	
}

//获取对应大区IP地址
char* getAddr(const char *part)
{
	/*	
	网四	碧海-13
	电一	傲雪凌霜-16
			山海雄关-12 
	电六	四海-14
			逍遥-12
			星霜-13
	双一	武林争霸-15
			纵横四海-12
			纵横四海2服-12
	*/
	//CString s = "壮志凌云,叱咤武林,斗转星移,王者之争,龙争虎斗,雪原,雄霸,登峰造极,雷霆万钧,硝烟四起,玄武破天,碧海,傲雪凌霜,山海雄关,战火重燃,狂龙降天,幻影密路,长空,气贯长虹,烽火天下,盖世无双,东岳,玄湖,紫月,义薄云天,凤舞九州,英雄豪杰,沉香,四海,逍遥,霸业,苍月,星霜,纵横四海,纵横四海2服";

	int n = getRandNum(3);	//设置一般IP随机数
	//gethostbyname()

	if (strcmp(part, "网通一区") == 0)
	{
		switch(getRandNum(5))
		{
			case 0:
				return "218.25.75.33";	
			case 1:
				return "218.25.75.34";
			case 2:
				return "218.25.75.35";
			case 3:
				return "218.25.75.36";
			case 4:
				return "218.25.75.37";
		}
	}
	if (strcmp(part, "网通二区") == 0)
	{
		switch(n)
		{
			case 0:
				return "218.25.57.18";	
			case 1:
				return "218.25.57.19";
			case 2:
				return "218.25.57.18";	//Session.cfg 为.20		单个服务器ip减少了？
		}
	}
	if (strcmp(part, "网通三区") == 0)
	{
		switch(n)
		{
			case 0:
				return "218.25.104.33";	
			case 1:
				return "218.25.104.34";
			case 2:
				return "218.25.104.33";	//Session.cfg 为.35
		}
	}
	if (strcmp(part, "网通四区") == 0)
	{
		switch(n)
		{
			case 0:
				return "218.25.57.83";	
			case 1:
				return "218.25.57.84";
			case 2:
				return "218.25.57.85";
		}
	}
	if (strcmp(part, "电信一区") == 0)
	{
		switch(getRandNum(6))
		{
			case 0:
				return "118.184.187.62";
			case 1:
				return "118.184.187.63";	
			case 2:
				return "118.184.187.64";
			case 3:
				return "118.184.187.86";	
			case 4:
				return "118.184.187.87";
			case 5:
				return "118.184.187.88";
		}
	}
	if (strcmp(part, "电信二区") == 0)
	{
		switch(n)
		{
			case 0:
				return "118.184.187.67";	
			case 1:
				return "118.184.187.68";
			case 2:
				return "118.184.187.69";
		}
	}
	if (strcmp(part, "电信三区") == 0)
	{
		switch(n)
		{
			case 0:
				return "118.184.185.11";	
			case 1:
				return "118.184.185.12";
			case 2:
				return "118.184.185.11";
		}
	}
	if (strcmp(part, "电信四区") == 0)
	{
		switch(n)
		{
			case 0:
				return "118.184.185.33";	
			case 1:
				return "118.184.185.34";
			case 2:
				return "118.184.185.33";
		}
	}
	if (strcmp(part, "电信五区") == 0)
	{
		switch(n)
		{
			case 0:
				return "118.184.185.83";	
			case 1:
				return "118.184.185.84";
			case 2:
				return "118.184.185.85";
		}
	}
	if (strcmp(part, "电信六区") == 0)
	{
		switch(getRandNum(6))
		{
			case 0:
				return "118.184.185.13";
			case 1:
				return "118.184.185.32";	
			case 2:
				return "118.184.185.35";
			case 3:
				return "118.184.185.106";
			case 4:
				return "118.184.185.107";
			case 5:
				return "118.184.185.130";
		}
	}
	if (strcmp(part, "双线一区") == 0)
	{
		switch(getRandNum(5))
		{
			case 0:
				return "118.184.189.33";
			case 1:
				return "118.184.189.34";	
			case 2:
				return "118.184.189.35";
			case 3:
				return "118.184.185.36";
			case 4:
				return "118.184.185.37";
			default :
				return "118.184.189.33";
		}
	}else{
		return "";
	}
}

//使用非阻塞模式设置超时500ms
char * getLinesBuffer(const char *part, int &nBuffLen)
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		return "";
	}
	SOCKET client = socket(PF_INET, SOCK_STREAM, 0);
	if(client == INVALID_SOCKET){	//INVALID_SOCKET = -1	
		WSACleanup();
		return "";
	}

	//set Send and Recv timeout	单位ms
	int nNetTimeout = 500; 
	//设置发送超时	SocketError = -1
	if(setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout, sizeof(nNetTimeout)) == SOCKET_ERROR){ 
		closesocket(client);
		WSACleanup();
		return "";
	}
	//设置接收超时
	if(setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(nNetTimeout)) == SOCKET_ERROR){
		closesocket(client);
		WSACleanup();
		return "";
	}
	//设置非阻塞方式连接
	unsigned long ul = 1;
	if(ioctlsocket(client, FIONBIO, &ul) == SOCKET_ERROR){ //-1
		closesocket(client);
		WSACleanup();
		return "";
	}

	//连接
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(getAddr(part));
	sockAddr.sin_port = htons(13100+(unsigned short)getRandNum(5));

	connect(client, (SOCKADDR*)&sockAddr, sizeof(sockAddr));

	//select 模型，即设置超时
	fd_set w;	//描述符集 测试连接是否可用
	FD_ZERO(&w);	//清空
	//struct timeval tv = {1, 0};	 //时间结构体 = {秒，微秒}
	struct timeval tv = {0, 500000};	//500ms
	//tv.tv_sec = 1; 	//超时1s
	//tv.tv_usec =0;	//一百万分之一秒，微秒

	FD_SET(client, &w);	//一个描述符可以添加多个socket
	int ret = select(0, 0, &w, 0, &tv);
	if(ret <= 0 ){	//超时返回0，发生错误返回-1
		//printf("6 ret=%d\n", ret);
		closesocket(client);
		WSACleanup();
		return "";
	}
	//一般非阻塞较难控制，再设回阻塞模式
	ul= 0 ;
	if(ioctlsocket(client, FIONBIO, &ul) == SOCKET_ERROR){ //-1
		closesocket (client);
		WSACleanup();
		return "";
	}
	
	//正常发送接收
	int cmdData = 0x8016;
	if (send(client, (char*)&cmdData, sizeof(int), NULL) <= 0){
		printf("发送指令失败，错误代码为：%d\n", GetLastError());
		closesocket(client);
		WSACleanup();
		return "";
	}
	//接收服务器传回的数据
	static char szRecvBuffer[MAX_RECV_BUFFER_LEN] = { 0 };
	int recvLen = recv(client, szRecvBuffer, MAX_RECV_BUFFER_LEN, NULL);
	if (recvLen <= 0){	//0连接关闭，-1接收失败
		printf("接收数据失败，错误代码为：%d\n", GetLastError());
		closesocket(client);
		WSACleanup();
		return "";
	}

	closesocket(client);//关闭网络连接
	WSACleanup();
	
	nBuffLen = recvLen;
	return szRecvBuffer;
}

/* input hex string, format: 0xAA111 or AA111 or abc11 */
//十六进制字符串转整型
DWORD hexStr2Long(char* p)
{
    DWORD n = 0;
    char* q = p;
    while(*q)	/* reach its tail */
        q++;
    if(*p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X'))
        p += 2;	/* skip "0x" or "0X" */
    while(*p){
        int c;
        if(*p >= '0' && *p <= '9')
            c = *p - '0';
        else if(*p >= 'A' && *p <= 'F')
            c = *p - 'A' + 0xA;
        else if(*p >= 'a' && *p <= 'f')
            c = *p - 'a' + 0xA;
        else
            return 0;	/* invalid char */
        
        n += c << ((DWORD)(q - p - 1) * 4);
        p++;
    }
    return n;
}
//十六进制字符串转数组 (每2个字符占1byte)，返回原始长度/2
int hexStr2Bytes(unsigned char *src, unsigned char *out)
{
	if (strlen((char *)src) % 2 != 0) {
		printf("字符串长度异常不能整除2！ \n");
		return 0;
	}
    int len = strlen((char *)src) / 2;
    int i = 0;
    for(i = 0; i < len; i++)
    {
        unsigned char m, n;
        if(src[2*i] >= 'A' && src[2*i] <= 'F')
			m = src[2*i] - 'A' + 10;
		else if(src[2*i] >= 'a' && src[2*i] <= 'f')
			m = src[2*i] - 'a' + 10;
        else 
			m = src[2*i] - '0';

        if(src[2*i + 1] >= 'A' && src[2*i + 1] <= 'F')
			n = src[2*i + 1] - 'A' + 10;
		else if(src[2*i + 1] >= 'a' && src[2*i + 1] <= 'f')
			n = src[2*i + 1] - 'a' + 10;
        else
			n = src[2*i + 1] - '0' ;
        out[i] = (m << 4) | n;
		printf("out[i]=%x \n", out[i]);
    }
	
	return len;
}

//获取分区服务器所有线路状况(分区,服务器)[0满,1未满,逗号分隔]
BSTR CQMPlugin::getPartServerLines(LPCTSTR part, LPCTSTR server) 
{
	CString strResult;
	if(strlen(part) == 0 || strlen(server) == 0)
		return strResult.AllocSysString();
	int nBuffLen = 0;
	unsigned char * p = (unsigned char *)getLinesBuffer(part, nBuffLen);	//某一大区所有服务器线路数据
	if(*p == '\0' || nBuffLen == 0)
		return strResult.AllocSysString();

	int d1[20] = {0};	//server转字符转整型比较 少写点行较短
	for(int i=0; i<strlen(server); i++)
		d1[i] = (unsigned char)server[i];
	char dest[20];
	int serverNameLen = 0, lines = 0, lineNameLen = 0;
	for(i = 0; i < nBuffLen; i++){
		//先检查4字节服务器名 服务器最少4字节
		if(*p == d1[0] && *(p+1) == d1[1] && *(p+2) == d1[2] && *(p+3) == d1[3]){
			if(strlen(server) == *(p-2)){ //服务器名长度不符合跳过
				memset(dest, '\0', sizeof(dest));
				memcpy(dest, p, strlen(server));//*(p-2));	//复制服务器名完整字节	
				if(!strcmp(dest, server)){	//此时指针指向总服务器名
					serverNameLen = *(p-2);	//服务器名长度
					p += serverNameLen + 6;
					lines = *p;	//总线数
					if(lines >= 20)	//最大16条线，超过20直接返回空
						return strResult.AllocSysString();
					p += 4;	//指向具体线路1
					for(int j = 1; j <= lines; j++){
						p += 2;
						lineNameLen = *p;		//具体线路名称长度
						p += 2 + lineNameLen;
						strResult += (*p == 0xfa ?  "0," : "1,");	//具体线路百分比
						if (j != lines)
							p += 2;	//指向下一线路
					}
					strResult = strResult.Left(strResult.GetLength() - 1);	//去除尾部多余逗号
					//printf("所有线路状况 = %s \n", strResult.c_str());	//打印必须使用.c_str()！！！
					break;
				}
			}
		}
		p++;
	}

	return strResult.AllocSysString();
}

//获取分区服务器字节数组(分区)[字节数组]
BSTR CQMPlugin::getByteArray(LPCTSTR part) 
{
	CString strResult, tempStr;
	if(strlen(part) == 0)
		return strResult.AllocSysString();

	//printf("分区服务器字节数组 - %s ", part);
	strResult += part;

	int nBuffLen = 0;
	unsigned char * p = (unsigned char *)getLinesBuffer(part, nBuffLen);
	//printf("得到的数据%d：\n", nBuffLen);
	strResult += "-数据 ";
	tempStr.Format("%d ",nBuffLen);
	strResult += tempStr;
	strResult += " 字节：";
	for(int i = 0; i < nBuffLen; i++){	
		tempStr.Format("%02X ", p[i]);
		strResult += tempStr;
		//printf("%02X ", p[i]);
	}

	//printf("\n");

	return strResult.AllocSysString();
}

//修改只读内存禁用状态，修改禁用按钮为可用	0x58->0x50
//多地址为"|"分隔的十六进制字符串地址
long CQMPlugin::writeData2Clickable(long hwnd, LPCTSTR addrs) 
{
	if (hwnd == 0 || strlen(addrs) == 0)
		return 0;
	char str[300];	//str不能为常量
	strcpy(str, addrs);	//拷贝字符串
	const char s[2]= "|";	//分隔符可为集合
	char * token;	//分割后的字符串地址
	DWORD addr = 0, rb = 0;	//整型地址与读取内存数据

    DWORD ret = 0, pid = 0, oldprot, newprot;
	HANDLE hProcess = NULL;
	BYTE wb[1] = {0x50};	//写入内存数据
	
	//根据窗口句柄获取进程PID
	GetWindowThreadProcessId((HWND)hwnd, &pid);
	if(pid != 0){
		//根据PID获取进程句柄
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		if(hProcess != NULL){
			token = strtok(str, s);	//分割获取第一个字符串	
			while(token != NULL){
				if(strlen(token) > 8){	
					printf("addr = %s   地址长度超过 8 跳过！\n", token);
					//ret = 0;
					token = strtok(NULL, s);	//剩余字符串保存到static，so传NULL可继续分割
					continue;
				}
				addr = hexStr2Long(token);	//十六进制字符串转整型
				if(addr == 0 || addr == 0xFFFFFFFF){
					printf("addr = %s   地址有误跳过！\n", token);
					//ret = 0;
					token = strtok(NULL, s);	//剩余字符串保存到static，so传NULL可继续分割
					continue;
				}
				rb = 0;	//清零重新赋值避免读取失败取上一次的结果值
				ReadProcessMemory(hProcess, (LPCVOID)addr, &rb, 1, NULL);
				printf("addr = %X   , rb = %X\n", addr, rb);
				if( rb == 0x58 ){
					printf("style为0x58可改！\n");
					//更改保护权限 -> 修改内存 -> 还原保护权限
					VirtualProtectEx(hProcess, (LPVOID)addr, 1, PAGE_EXECUTE_READWRITE, &oldprot);
					WriteProcessMemory(hProcess, (LPVOID)addr, wb, 1, NULL);
					VirtualProtectEx(hProcess, (LPVOID)addr, 1, oldprot, &newprot);
					ret = 1;	//成功修改1个地址则返回1
				}
				token = strtok(NULL, s);	//剩余字符串保存到static，so传NULL可继续分割
			}
			CloseHandle(hProcess);
		}
	}
	return ret;
}

//修改权限后写入内存
long CQMPlugin::writeData(long hwnd, LPCTSTR addr, LPCTSTR data) 
{	//data需要限制最大写入字节吗？|| strlen(data) > 60 ？
	if (hwnd == 0 || strlen(addr) == 0 || strlen(addr) > 8 || strlen(data) == 0)
		return 0;

	DWORD addrLong = 0;
	char addr1[20];	
	strcpy(addr1, addr);	//拷贝字符串
	addrLong = hexStr2Long(addr1);	//字符串地址转为整型
	if(addrLong == 0 || addrLong == 0xFFFFFFFF){
		printf("地址越界！\n");
		return 0;
	}
	//去除data中的空格
	char data1[70];
	strcpy(data1, data);	//拷贝字符串
	int i = 0, j = 0;
    while (data1[i] != '\0') {//遍历字符串
        if (data1[i] != ' ')
            data1[j++] = data1[i];
        i++;
    }
    data1[j] = '\0';
	printf("data去除空格后=%s \n",data1);

	unsigned char wd[20] = {0};	//数组给个20大小初始化
	int len = 0;
	len = hexStr2Bytes((unsigned char *)data1, wd);	//十六进制字符串数据转为数组 (每2个字符占1byte)
	printf("strlen(wd)=%d\n", strlen((char*)wd));
	if (len == 0){
		printf("待写入长度为0异常结束！\n");
		return 0;
	}
	DWORD pid = 0, oldprot, newprot;
	HANDLE hProcess = NULL;
	//根据窗口句柄获取进程PID
	GetWindowThreadProcessId((HWND)hwnd, &pid);
	if(pid != 0){
		//根据PID获取进程句柄
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		if(hProcess != NULL){
			//更改保护权限 -> 修改内存 -> 还原保护权限
			VirtualProtectEx(hProcess, (LPVOID)addrLong, 1, PAGE_EXECUTE_READWRITE, &oldprot);
			WriteProcessMemory(hProcess, (LPVOID)addrLong, wd, len, NULL);
			VirtualProtectEx(hProcess, (LPVOID)addrLong, 1, oldprot, &newprot);
			CloseHandle(hProcess);
		}
	}
	return 1;	
}


//获取dll模块大小
long CQMPlugin::getModuleSize(long hwnd, LPCTSTR dllName) 
{
	if(hwnd == 0 || strlen(dllName) == 0)
		return 0;
	DWORD pid = 0;
	HANDLE hProcess = NULL;
	GetWindowThreadProcessId((HWND)hwnd, &pid);
	if(pid == 0)
		return 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if(hProcess == NULL)
		return 0;
	// 定义模块句柄数组
	HMODULE hModules[1024];
	DWORD ret = 0, cbNeeded;
	// 枚举当前进程的模块句柄
	if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
		// 计算模块数量
		int moduleCount = cbNeeded / sizeof(HMODULE);
		for (int i = 0; i < moduleCount; i++) {
			char moduleFilePath[256];
			// 获取模块文件路径
			if (GetModuleFileNameEx(hProcess, hModules[i], moduleFilePath, sizeof(moduleFilePath))) {
				// 子串比较文件名
				if (strstr(moduleFilePath, dllName) != NULL) {
					// 获取DLL模块内存信息
					MODULEINFO moduleInfo;
					if (GetModuleInformation(hProcess, hModules[i], &moduleInfo, sizeof(moduleInfo))) {
						//"DLL内存地址:"  moduleInfo.lpBaseOfDll ;
						ret = moduleInfo.SizeOfImage;
						break;
					}
				}
			}
		}
	}
	CloseHandle(hProcess);
	return ret;
}

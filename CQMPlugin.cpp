/*********************************************************************
	��ӭ��Ϊ�����������������
	���ǰ�������Ĳ��ģ�� 091026��

	�����Release��Ĳ��ʹ�ã�Debug��Ĳ���ᵼ�°�����������쳣����

  	������������⣬��ӭͨ�����·�ʽ������ȡ����ϵ
	Email: hi@vrbrothers.com
	��̳: bbs.ajjl.cn
	
	���ģ����¼�¼:
	091026
	�������MFC�Ĺ��ܲ����Ƶ������еĲ�������������߳���ִ�е����⡣
	�������ᵼ�²��Ч�ʵ��£����ҿ��ܻ��������������������̡߳�
*********************************************************************/

/*********************************************************************
 * ���µ����ݣ�һ�������������Ҫ������Ҳ����Ҫ�����޸�
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
#include <WinSock2.h>				//�������ӵ�ͷ

#pragma comment(lib, "psapi.lib" )	//����״̬ API
#pragma comment(lib, "ws2_32.lib")	//��������API��Ҫ�ľ�̬��
#define MAX_RECV_BUFFER_LEN 3000	//����buffer��󳤶�

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
 * ���ϵ����ݣ�һ�������������Ҫ������Ҳ����Ҫ�����޸�
*********************************************************************/

/*********************************************************************
	���������ÿ���������֧�����������
	�������Ҫдһ���Լ��������Ҫ��ѭ���²��裺
	1. ���ļ�CQMPlugin.cpp�����������ڿ���������ļ�
	2. ��Visual C++ 6.0�Ĳ˵���ѡ�� View->ClassWizard
	3. �ڵ����ĶԻ�����ѡ��Automation��壬��Add Method��ť
	4. ������д���Ĳ�����������֡��������ͺͲ�������
	   ��Ҫע����ǣ��������ֵ���ַ������ͣ���ѡ��BSTR
	   ����������ַ������ͣ���ѡ��LPCTSTR
	   �����Ҫ����ַ�Ĳ����������ڷ��ض��ֵ��������������ѡ��VARIANT*���������GetScreenResolution���������ʾ
	5. ��OK��ť�����ῴ��CQMPlugin.cpp����һ���հ׵ĺ�������ֻ��Ҫ��д�������ܼ���
	6. ��Ҳ������Automation�����ѡ��ɾ��ĳ�����еĺ�������ͬʱ������ĳ�����ɾ����Ӧ�ĺ�����
	7. �������m_DescriptionList�������д���Ĳ��˵���ͺ���˵��
	8. �����γ�DLL�ļ�����DLL�ļ����ڰ��������PluginĿ¼��
	9. �򿪰������飬����༭�ű����棬�򿪡���������壬��д�ű�������Ĳ����
*********************************************************************/

/*********************************************************************
 * ����:m_DescriptionList
 * ����԰�������һ����񣬱���е�ÿһ�д�����һ��������
 * ÿ�е�һ���ַ����ǲ����������֣����Ϊ�գ������������������ڶ����ַ����������������Ľ��ͣ�
 * �������ַ����ǲ���������ʾģ����Ϣ�����е�$1 $2 ... ����������ʾʱ���õ�һ���������ڶ���������... ����
 * ������Ҫ����һ����������ʱ�򣬾���Ҫ�ڱ�������һ�С�
*********************************************************************/
CQMPlugin::DESCRIPTION_LIST CQMPlugin::m_DescriptionList[] = 
{
	{"","Rx���",""},
	{"getPartServerLines","��ȡ��������������·״��(�ַ�������,�ַ���������(ĩβ��\"(�·�)\"))[0��,1δ��,���ŷָ�]",""},
	{"getByteArray","��ȡ������������(�ַ�������)[�ֽ�����],",""},
	{"writeData","�޸��ڴ�����(�����ʹ��ھ��,�ַ����ڴ��ַ,�ַ�������������(С��))[0�쳣,1�ɹ�]",""},
	{"writeData2Clickable","�޸Ľ���״̬(�����ʹ��ھ��,�ַ����ڴ��ַ('|'�ָ���ʮ�������ַ�����ַ))[0�쳣,1�ɹ�]",""},
	{"getModuleSize","��ȡdllģ���С(�����ʹ��ھ��,�ַ���dllģ������)[0�쳣]",""}
};

//����������Ҫ������Ҳ����Ҫ�޸�
const int CQMPlugin::m_iDescriptionListSize = sizeof(CQMPlugin::m_DescriptionList) / sizeof(CQMPlugin::DESCRIPTION_LIST);

/*********************************************************************
 * �����Ǹ����������Ĵ��������������Ͳ�����Ƕ�Ӧ�ġ�
 * ���Ҫ����һ��������ͨ��Visual C++�����дһ���յĴ�����
 * ���Ҫɾ��һ�������������˰Ѷ�Ӧ�Ĵ�����Ҳһ��ɾ����
*********************************************************************/


/*********************************************************************
 * �����ǹ�����������
 * OnLoad �������ڲ��������ʱ���ã�����ֵͨ��ΪTRUE���������FALSE����ʾ����ܾ�������
 * OnLoad �����ĵ�һ��������ʾ���ض�̬���ӿ��Instance Handle���ڶ���������ʱ����
 * OnExit �������ڲ����ж���ǵ��ã�û�з���ֵ
*********************************************************************/
BOOL OnLoad(HMODULE hModule, int iReserved)
{
	return TRUE;
}

void OnExit()
{
	return;
}

//����[0,(range-1)]֮����������
int getRandNum(int range)
{
	srand(time(0));	//�����������
	return rand()%range;	
}

//��ȡ��Ӧ����IP��ַ
char* getAddr(const char *part)
{
	/*	
	����	�̺�-13
	��һ	��ѩ��˪-16
			ɽ���۹�-12 
	����	�ĺ�-14
			��ң-12
			��˪-13
	˫һ	��������-15
			�ݺ��ĺ�-12
			�ݺ��ĺ�2��-12
	*/
	//CString s = "׳־����,߳������,��ת����,����֮��,��������,ѩԭ,�۰�,�Ƿ��켫,�������,��������,��������,�̺�,��ѩ��˪,ɽ���۹�,ս����ȼ,��������,��Ӱ��·,����,���᳤��,�������,������˫,����,����,����,�屡����,�������,Ӣ�ۺ���,����,�ĺ�,��ң,��ҵ,����,��˪,�ݺ��ĺ�,�ݺ��ĺ�2��";

	int n = getRandNum(3);	//����һ��IP�����
	//gethostbyname()

	if (strcmp(part, "��ͨһ��") == 0)
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
	if (strcmp(part, "��ͨ����") == 0)
	{
		switch(n)
		{
			case 0:
				return "218.25.57.18";	
			case 1:
				return "218.25.57.19";
			case 2:
				return "218.25.57.18";	//Session.cfg Ϊ.20		����������ip�����ˣ�
		}
	}
	if (strcmp(part, "��ͨ����") == 0)
	{
		switch(n)
		{
			case 0:
				return "218.25.104.33";	
			case 1:
				return "218.25.104.34";
			case 2:
				return "218.25.104.33";	//Session.cfg Ϊ.35
		}
	}
	if (strcmp(part, "��ͨ����") == 0)
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
	if (strcmp(part, "����һ��") == 0)
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
	if (strcmp(part, "���Ŷ���") == 0)
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
	if (strcmp(part, "��������") == 0)
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
	if (strcmp(part, "��������") == 0)
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
	if (strcmp(part, "��������") == 0)
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
	if (strcmp(part, "��������") == 0)
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
	if (strcmp(part, "˫��һ��") == 0)
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

//ʹ�÷�����ģʽ���ó�ʱ500ms
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

	//set Send and Recv timeout	��λms
	int nNetTimeout = 500; 
	//���÷��ͳ�ʱ	SocketError = -1
	if(setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, (char *)&nNetTimeout, sizeof(nNetTimeout)) == SOCKET_ERROR){ 
		closesocket(client);
		WSACleanup();
		return "";
	}
	//���ý��ճ�ʱ
	if(setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(nNetTimeout)) == SOCKET_ERROR){
		closesocket(client);
		WSACleanup();
		return "";
	}
	//���÷�������ʽ����
	unsigned long ul = 1;
	if(ioctlsocket(client, FIONBIO, &ul) == SOCKET_ERROR){ //-1
		closesocket(client);
		WSACleanup();
		return "";
	}

	//����
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(getAddr(part));
	sockAddr.sin_port = htons(13100+(unsigned short)getRandNum(5));

	connect(client, (SOCKADDR*)&sockAddr, sizeof(sockAddr));

	//select ģ�ͣ������ó�ʱ
	fd_set w;	//�������� ���������Ƿ����
	FD_ZERO(&w);	//���
	//struct timeval tv = {1, 0};	 //ʱ��ṹ�� = {�룬΢��}
	struct timeval tv = {0, 500000};	//500ms
	//tv.tv_sec = 1; 	//��ʱ1s
	//tv.tv_usec =0;	//һ�����֮һ�룬΢��

	FD_SET(client, &w);	//һ��������������Ӷ��socket
	int ret = select(0, 0, &w, 0, &tv);
	if(ret <= 0 ){	//��ʱ����0���������󷵻�-1
		//printf("6 ret=%d\n", ret);
		closesocket(client);
		WSACleanup();
		return "";
	}
	//һ����������ѿ��ƣ����������ģʽ
	ul= 0 ;
	if(ioctlsocket(client, FIONBIO, &ul) == SOCKET_ERROR){ //-1
		closesocket (client);
		WSACleanup();
		return "";
	}
	
	//�������ͽ���
	int cmdData = 0x8016;
	if (send(client, (char*)&cmdData, sizeof(int), NULL) <= 0){
		printf("����ָ��ʧ�ܣ��������Ϊ��%d\n", GetLastError());
		closesocket(client);
		WSACleanup();
		return "";
	}
	//���շ��������ص�����
	static char szRecvBuffer[MAX_RECV_BUFFER_LEN] = { 0 };
	int recvLen = recv(client, szRecvBuffer, MAX_RECV_BUFFER_LEN, NULL);
	if (recvLen <= 0){	//0���ӹرգ�-1����ʧ��
		printf("��������ʧ�ܣ��������Ϊ��%d\n", GetLastError());
		closesocket(client);
		WSACleanup();
		return "";
	}

	closesocket(client);//�ر���������
	WSACleanup();
	
	nBuffLen = recvLen;
	return szRecvBuffer;
}

/* input hex string, format: 0xAA111 or AA111 or abc11 */
//ʮ�������ַ���ת����
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
//ʮ�������ַ���ת���� (ÿ2���ַ�ռ1byte)������ԭʼ����/2
int hexStr2Bytes(unsigned char *src, unsigned char *out)
{
	if (strlen((char *)src) % 2 != 0) {
		printf("�ַ��������쳣��������2�� \n");
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

//��ȡ����������������·״��(����,������)[0��,1δ��,���ŷָ�]
BSTR CQMPlugin::getPartServerLines(LPCTSTR part, LPCTSTR server) 
{
	CString strResult;
	if(strlen(part) == 0 || strlen(server) == 0)
		return strResult.AllocSysString();
	int nBuffLen = 0;
	unsigned char * p = (unsigned char *)getLinesBuffer(part, nBuffLen);	//ĳһ�������з�������·����
	if(*p == '\0' || nBuffLen == 0)
		return strResult.AllocSysString();

	int d1[20] = {0};	//serverת�ַ�ת���ͱȽ� ��д���н϶�
	for(int i=0; i<strlen(server); i++)
		d1[i] = (unsigned char)server[i];
	char dest[20];
	int serverNameLen = 0, lines = 0, lineNameLen = 0;
	for(i = 0; i < nBuffLen; i++){
		//�ȼ��4�ֽڷ������� ����������4�ֽ�
		if(*p == d1[0] && *(p+1) == d1[1] && *(p+2) == d1[2] && *(p+3) == d1[3]){
			if(strlen(server) == *(p-2)){ //�����������Ȳ���������
				memset(dest, '\0', sizeof(dest));
				memcpy(dest, p, strlen(server));//*(p-2));	//���Ʒ������������ֽ�	
				if(!strcmp(dest, server)){	//��ʱָ��ָ���ܷ�������
					serverNameLen = *(p-2);	//������������
					p += serverNameLen + 6;
					lines = *p;	//������
					if(lines >= 20)	//���16���ߣ�����20ֱ�ӷ��ؿ�
						return strResult.AllocSysString();
					p += 4;	//ָ�������·1
					for(int j = 1; j <= lines; j++){
						p += 2;
						lineNameLen = *p;		//������·���Ƴ���
						p += 2 + lineNameLen;
						strResult += (*p == 0xfa ?  "0," : "1,");	//������·�ٷֱ�
						if (j != lines)
							p += 2;	//ָ����һ��·
					}
					strResult = strResult.Left(strResult.GetLength() - 1);	//ȥ��β�����ය��
					//printf("������·״�� = %s \n", strResult.c_str());	//��ӡ����ʹ��.c_str()������
					break;
				}
			}
		}
		p++;
	}

	return strResult.AllocSysString();
}

//��ȡ�����������ֽ�����(����)[�ֽ�����]
BSTR CQMPlugin::getByteArray(LPCTSTR part) 
{
	CString strResult, tempStr;
	if(strlen(part) == 0)
		return strResult.AllocSysString();

	//printf("�����������ֽ����� - %s ", part);
	strResult += part;

	int nBuffLen = 0;
	unsigned char * p = (unsigned char *)getLinesBuffer(part, nBuffLen);
	//printf("�õ�������%d��\n", nBuffLen);
	strResult += "-���� ";
	tempStr.Format("%d ",nBuffLen);
	strResult += tempStr;
	strResult += " �ֽڣ�";
	for(int i = 0; i < nBuffLen; i++){	
		tempStr.Format("%02X ", p[i]);
		strResult += tempStr;
		//printf("%02X ", p[i]);
	}

	//printf("\n");

	return strResult.AllocSysString();
}

//�޸�ֻ���ڴ����״̬���޸Ľ��ð�ťΪ����	0x58->0x50
//���ַΪ"|"�ָ���ʮ�������ַ�����ַ
long CQMPlugin::writeData2Clickable(long hwnd, LPCTSTR addrs) 
{
	if (hwnd == 0 || strlen(addrs) == 0)
		return 0;
	char str[300];	//str����Ϊ����
	strcpy(str, addrs);	//�����ַ���
	const char s[2]= "|";	//�ָ�����Ϊ����
	char * token;	//�ָ����ַ�����ַ
	DWORD addr = 0, rb = 0;	//���͵�ַ���ȡ�ڴ�����

    DWORD ret = 0, pid = 0, oldprot, newprot;
	HANDLE hProcess = NULL;
	BYTE wb[1] = {0x50};	//д���ڴ�����
	
	//���ݴ��ھ����ȡ����PID
	GetWindowThreadProcessId((HWND)hwnd, &pid);
	if(pid != 0){
		//����PID��ȡ���̾��
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		if(hProcess != NULL){
			token = strtok(str, s);	//�ָ��ȡ��һ���ַ���	
			while(token != NULL){
				if(strlen(token) > 8){	
					printf("addr = %s   ��ַ���ȳ��� 8 ������\n", token);
					//ret = 0;
					token = strtok(NULL, s);	//ʣ���ַ������浽static��so��NULL�ɼ����ָ�
					continue;
				}
				addr = hexStr2Long(token);	//ʮ�������ַ���ת����
				if(addr == 0 || addr == 0xFFFFFFFF){
					printf("addr = %s   ��ַ����������\n", token);
					//ret = 0;
					token = strtok(NULL, s);	//ʣ���ַ������浽static��so��NULL�ɼ����ָ�
					continue;
				}
				rb = 0;	//�������¸�ֵ�����ȡʧ��ȡ��һ�εĽ��ֵ
				ReadProcessMemory(hProcess, (LPCVOID)addr, &rb, 1, NULL);
				printf("addr = %X   , rb = %X\n", addr, rb);
				if( rb == 0x58 ){
					printf("styleΪ0x58�ɸģ�\n");
					//���ı���Ȩ�� -> �޸��ڴ� -> ��ԭ����Ȩ��
					VirtualProtectEx(hProcess, (LPVOID)addr, 1, PAGE_EXECUTE_READWRITE, &oldprot);
					WriteProcessMemory(hProcess, (LPVOID)addr, wb, 1, NULL);
					VirtualProtectEx(hProcess, (LPVOID)addr, 1, oldprot, &newprot);
					ret = 1;	//�ɹ��޸�1����ַ�򷵻�1
				}
				token = strtok(NULL, s);	//ʣ���ַ������浽static��so��NULL�ɼ����ָ�
			}
			CloseHandle(hProcess);
		}
	}
	return ret;
}

//�޸�Ȩ�޺�д���ڴ�
long CQMPlugin::writeData(long hwnd, LPCTSTR addr, LPCTSTR data) 
{	//data��Ҫ�������д���ֽ���|| strlen(data) > 60 ��
	if (hwnd == 0 || strlen(addr) == 0 || strlen(addr) > 8 || strlen(data) == 0)
		return 0;

	DWORD addrLong = 0;
	char addr1[20];	
	strcpy(addr1, addr);	//�����ַ���
	addrLong = hexStr2Long(addr1);	//�ַ�����ַתΪ����
	if(addrLong == 0 || addrLong == 0xFFFFFFFF){
		printf("��ַԽ�磡\n");
		return 0;
	}
	//ȥ��data�еĿո�
	char data1[70];
	strcpy(data1, data);	//�����ַ���
	int i = 0, j = 0;
    while (data1[i] != '\0') {//�����ַ���
        if (data1[i] != ' ')
            data1[j++] = data1[i];
        i++;
    }
    data1[j] = '\0';
	printf("dataȥ���ո��=%s \n",data1);

	unsigned char wd[20] = {0};	//�������20��С��ʼ��
	int len = 0;
	len = hexStr2Bytes((unsigned char *)data1, wd);	//ʮ�������ַ�������תΪ���� (ÿ2���ַ�ռ1byte)
	printf("strlen(wd)=%d\n", strlen((char*)wd));
	if (len == 0){
		printf("��д�볤��Ϊ0�쳣������\n");
		return 0;
	}
	DWORD pid = 0, oldprot, newprot;
	HANDLE hProcess = NULL;
	//���ݴ��ھ����ȡ����PID
	GetWindowThreadProcessId((HWND)hwnd, &pid);
	if(pid != 0){
		//����PID��ȡ���̾��
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		if(hProcess != NULL){
			//���ı���Ȩ�� -> �޸��ڴ� -> ��ԭ����Ȩ��
			VirtualProtectEx(hProcess, (LPVOID)addrLong, 1, PAGE_EXECUTE_READWRITE, &oldprot);
			WriteProcessMemory(hProcess, (LPVOID)addrLong, wd, len, NULL);
			VirtualProtectEx(hProcess, (LPVOID)addrLong, 1, oldprot, &newprot);
			CloseHandle(hProcess);
		}
	}
	return 1;	
}


//��ȡdllģ���С
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
	// ����ģ��������
	HMODULE hModules[1024];
	DWORD ret = 0, cbNeeded;
	// ö�ٵ�ǰ���̵�ģ����
	if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
		// ����ģ������
		int moduleCount = cbNeeded / sizeof(HMODULE);
		for (int i = 0; i < moduleCount; i++) {
			char moduleFilePath[256];
			// ��ȡģ���ļ�·��
			if (GetModuleFileNameEx(hProcess, hModules[i], moduleFilePath, sizeof(moduleFilePath))) {
				// �Ӵ��Ƚ��ļ���
				if (strstr(moduleFilePath, dllName) != NULL) {
					// ��ȡDLLģ���ڴ���Ϣ
					MODULEINFO moduleInfo;
					if (GetModuleInformation(hProcess, hModules[i], &moduleInfo, sizeof(moduleInfo))) {
						//"DLL�ڴ��ַ:"  moduleInfo.lpBaseOfDll ;
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

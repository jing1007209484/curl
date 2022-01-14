#pragma once
#include "curl/curl.h"
#include <windows.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <lm.h>
#pragma comment(lib, "netapi32.lib")
using namespace std;
// enum curl_coding
// {
// 	utf_8,
// };
// struct user_WRITEDATA
// {
// 	string Transcoding;//ת��
// 	string data;//��������
// };

struct memory {
	char *response;
	size_t size;
};


class Curl_toos
{
public:
	Curl_toos();
	~Curl_toos();
	//ȫ��Ψһ����
	static void global_init();
	//----------------------------------------------------------------------------------------------


	//HTTP
	//post�ύ��ʽ  �ύurl  �Զ����ͷ   ת��Ĭ��utf-8תchar    ����post����  ��ȡ���ر���   ʱ��    http���������� �˻����� root:root
	string Get_http_url(string Method, string url, map<string, string>Header = {}, string Transcoding = "UTF-8", string send = "", string *header_data = NULL, int TIMEOUT_MS=5000,string Form_accoun_password="");
	//curl ��İ汾
	string GetVersion();
	//���ص�ǰ������ֽ���
	size_t Get_http_siz();
	//���ص�ǰ���������
	char* Get_http_url_buff();

	//url ����
	string Get_curl_Unescape(const char *url);
	//url ������
	string Get_curl_escape(const char* url);


	//���ô���
	void Set_PROXY(string PROXY);



	//curl�ص�
	static size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata);//���շ�����Ӧͷ�ص�
	static size_t write_callback( char *ptr, size_t size, size_t nmemb, void *userdata);//������ҳ���ݻص�
	
	//�м��ַ���ȡ
	static	string Http_Getstring(const char* buff, string data1, string data2);
	static	wstring Http_Getstring(const WCHAR* buff, wstring data1, wstring data2);
	//��ȡȫ����ҳ��ͬ��Ϣ
	static	void Http_Getstring_vector(const char *http_buff, string left, string right, vector<string>&data);
	static	void Http_Getstring_vector(const WCHAR *http_buff, wstring left, wstring wright, vector<wstring>&data);
	//----------------------------------------------------------------------------
	//�ļ�����
	//д
	static	int  File_Write(const char* buff, const char* txt,bool Delete_File =false);
	static	int File_Write(const WCHAR* buff, const WCHAR* txt,bool Delete_File =false);
	//��
	static	string File_Read(const char* txt);
	static	wstring File_Read(const WCHAR* txt);
	//txt����
	static void File_Handle(const char* txt, vector<string>& txt_buff);
	static	void File_Handle(const WCHAR* txt, vector<wstring>& txt_buff);

	//�����ļ���д
	static string GetPrivateProfile(string lpAppName, string lpKeyName, string lpFileName);
	static wstring GetPrivateProfile(wstring lpAppName, wstring lpKeyName, wstring lpFileName);


	static void File_WritePrivateProfile(string lpAppName, string lpKeyName, string data, string lpFileName);
	static void File_WritePrivateProfile(wstring lpAppName, wstring lpKeyName, wstring data, wstring lpFileName);
	//--------------------------------------------------------------------------

	//utf-8ת char
	static string u82mb(const char* cont);
	//���ַ�ת���ַ�
	static	wstring AnsiToUnicode(const char* szStr);
	//���ַ�ת���ֽ�
	static string UnicodeToAnsi(const wchar_t* szStr);

	//Linuxʱ��� ��ת
	static time_t strTime2unix(const char timeStamp[]);

	//Ӣ������ת����������
	static string date2String(string date);


	//��ȡ����ʱ��
	static string GetTIme(__int64 n=0);
	//����ʱ���
	static UINT64 GetTIme_unix();

	//��ȡ����ʱ��
	static string Get_Internet();

	//----------------------------------------------------------------------------



	//----------------------------------------------------------------------------
private:
	CURL *m_curl_handle=NULL;//CURL���͵�ָ��

	//����http ���ص�ԭʼ����
	memory m_userp;
};


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
// 	string Transcoding;//转码
// 	string data;//接受数据
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
	//全局唯一调用
	static void global_init();
	//----------------------------------------------------------------------------------------------


	//HTTP
	//post提交方式  提交url  自定义表头   转码默认utf-8转char    发送post数据  获取返回标题   时间    http服务器弹窗 账户密码 root:root
	string Get_http_url(string Method, string url, map<string, string>Header = {}, string Transcoding = "UTF-8", string send = "", string *header_data = NULL, int TIMEOUT_MS=5000,string Form_accoun_password="");
	//curl 库的版本
	string GetVersion();
	//返回当前请求的字节数
	size_t Get_http_siz();
	//返回当前请求的数据
	char* Get_http_url_buff();

	//url 解码
	string Get_curl_Unescape(const char *url);
	//url 编码码
	string Get_curl_escape(const char* url);


	//设置代理
	void Set_PROXY(string PROXY);



	//curl回调
	static size_t header_callback(char *buffer, size_t size, size_t nitems, void *userdata);//接收返回响应头回调
	static size_t write_callback( char *ptr, size_t size, size_t nmemb, void *userdata);//接收网页数据回调
	
	//中间字符提取
	static	string Http_Getstring(const char* buff, string data1, string data2);
	static	wstring Http_Getstring(const WCHAR* buff, wstring data1, wstring data2);
	//获取全部网页相同信息
	static	void Http_Getstring_vector(const char *http_buff, string left, string right, vector<string>&data);
	static	void Http_Getstring_vector(const WCHAR *http_buff, wstring left, wstring wright, vector<wstring>&data);
	//----------------------------------------------------------------------------
	//文件操作
	//写
	static	int  File_Write(const char* buff, const char* txt,bool Delete_File =false);
	static	int File_Write(const WCHAR* buff, const WCHAR* txt,bool Delete_File =false);
	//读
	static	string File_Read(const char* txt);
	static	wstring File_Read(const WCHAR* txt);
	//txt换行
	static void File_Handle(const char* txt, vector<string>& txt_buff);
	static	void File_Handle(const WCHAR* txt, vector<wstring>& txt_buff);

	//配置文件读写
	static string GetPrivateProfile(string lpAppName, string lpKeyName, string lpFileName);
	static wstring GetPrivateProfile(wstring lpAppName, wstring lpKeyName, wstring lpFileName);


	static void File_WritePrivateProfile(string lpAppName, string lpKeyName, string data, string lpFileName);
	static void File_WritePrivateProfile(wstring lpAppName, wstring lpKeyName, wstring data, wstring lpFileName);
	//--------------------------------------------------------------------------

	//utf-8转 char
	static string u82mb(const char* cont);
	//单字符转宽字符
	static	wstring AnsiToUnicode(const char* szStr);
	//宽字符转单字节
	static string UnicodeToAnsi(const wchar_t* szStr);

	//Linux时间戳 互转
	static time_t strTime2unix(const char timeStamp[]);

	//英文日期转换中文日期
	static string date2String(string date);


	//获取本地时间
	static string GetTIme(__int64 n=0);
	//返回时间戳
	static UINT64 GetTIme_unix();

	//获取网络时间
	static string Get_Internet();

	//----------------------------------------------------------------------------



	//----------------------------------------------------------------------------
private:
	CURL *m_curl_handle=NULL;//CURL类型的指针

	//返回http 返回的原始数据
	memory m_userp;
};


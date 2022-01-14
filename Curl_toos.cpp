#include "stdafx.h"

#include "Curl_toos.h"
#include "time.h"

Curl_toos::Curl_toos()
{
	//初始化一个CURL类型的指针
	m_curl_handle=curl_easy_init();
	m_userp.response = NULL;
	m_userp.size = 0;
}


Curl_toos::~Curl_toos()
{

	//清除curl操作.
	curl_easy_cleanup(m_curl_handle);
	m_curl_handle = NULL;

	if (m_userp.response)
	{
		free(m_userp.response);
		m_userp.response = NULL;
	}
}

std::string Curl_toos::GetVersion()
{
	return curl_version();
}

size_t Curl_toos::Get_http_siz()
{
	return m_userp.size;
}

char* Curl_toos::Get_http_url_buff()
{
	return m_userp.response;
}

string Curl_toos::Get_curl_Unescape(const char *url)
{
	char *buff = curl_easy_unescape(m_curl_handle, url, NULL, NULL);

	string url_Unescape = buff;
	curl_free(buff);
	return url_Unescape;
}

std::string Curl_toos::Get_curl_escape(const char* url)
{
	char *buff= curl_easy_escape(m_curl_handle, url, NULL);

	string url_escape = buff;
	curl_free(buff);
	return url_escape;

}

void Curl_toos::Set_PROXY(string PROXY)
{
	curl_easy_setopt(m_curl_handle, CURLOPT_PROXY, PROXY.c_str());
}

void Curl_toos::global_init()
{
	//唯一初始化
	curl_global_init(CURL_GLOBAL_ALL);

}
size_t Curl_toos::write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t realsize = size * nmemb;
	struct memory *mem = (struct memory *)userdata;

	char *data = (char*)realloc(mem->response, mem->size + realsize + 1);
	if (data == NULL)
		return 0;  /* out of memory! */

	mem->response = data;
	memcpy(&(mem->response[mem->size]), ptr, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;

	return realsize;
}
size_t Curl_toos::header_callback(char *buffer, size_t size, size_t nitems, void *userdata)
{
	if (userdata!=NULL)
	{
		char *buf = new char[nitems + 1];
		memset(buf, 0, nitems + 1);
		memcpy(buf, buffer, nitems);
		string *data = (string*)userdata;
		*data += buf;
		delete[]buf;
	}
	return nitems;
}



std::string Curl_toos::Get_http_url(string Method, string url, map<string, string>Header, string Transcoding, string send, string *header_data, int TIMEOUT, string Form_accoun_password)
{
	//重置 句柄
	//curl_easy_reset(m_curl_handle);

	if (m_userp.response)
	{
		free(m_userp.response);
		m_userp.response = NULL;
	}
	m_userp.size = 0;

	curl_easy_setopt(m_curl_handle, CURLOPT_URL, url.c_str());
	struct curl_slist * list = NULL;
	if (Header.size()!=0)
	{
		map<string, string>::iterator it;
		for (it = Header.begin(); it != Header.end();it++)
		{
			list = curl_slist_append(list, (it->first + ":"+it->second).c_str());
		}
		curl_easy_setopt(m_curl_handle, CURLOPT_HTTPHEADER, list);
	}


	curl_easy_setopt(m_curl_handle, CURLOPT_NOSIGNAL, 3);//不要安装信号处理程序
	curl_easy_setopt(m_curl_handle, CURLOPT_TIMEOUT_MS, TIMEOUT);//连接超时 毫秒秒   0永不超市
	
	curl_easy_setopt(m_curl_handle, CURLOPT_WRITEFUNCTION, write_callback);//接收消息的回调



	curl_easy_setopt(m_curl_handle, CURLOPT_WRITEDATA, &m_userp);//接受网页回调用户数据

	curl_easy_setopt(m_curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);//验证证书  0不验证  1验证
	curl_easy_setopt(m_curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);//检测服务器证书名称

//	curl_easy_setopt(m_curl_handle, CURLOPT_HEADER, 1);// 将响应头信息和相应体一起传给用户数据

	curl_easy_setopt(m_curl_handle, CURLOPT_HEADERFUNCTION, header_callback);//标题回调
	curl_easy_setopt(m_curl_handle, CURLOPT_HEADERDATA, header_data);//标题回调用户数据

	curl_easy_setopt(m_curl_handle, CURLOPT_FOLLOWLOCATION, 1);//遵循重定向

	curl_easy_setopt(m_curl_handle, CURLOPT_MAXREDIRS, 3);//重定向 查找次数，防止查找太深 -1无限制

	curl_easy_setopt(m_curl_handle, CURLOPT_AUTOREFERER, 1L);//自动更新 重定向之后设置Referer：标头字段
	if (Form_accoun_password!="")
	{
		curl_easy_setopt(m_curl_handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
		curl_easy_setopt(m_curl_handle, CURLOPT_USERPWD, Form_accoun_password.c_str());
	}

	if (_stricmp(Method.c_str(),"post")==0)
	{

		curl_easy_setopt(m_curl_handle, CURLOPT_POSTFIELDSIZE, send.size());//post发送数据的大小
		curl_easy_setopt(m_curl_handle, CURLOPT_POSTFIELDS, send.c_str());
		curl_easy_setopt(m_curl_handle, CURLOPT_POST, 1L);
		
	}
	CURLcode  res;
	res = curl_easy_perform(m_curl_handle);
	curl_slist_free_all(list);
	if (res != CURLE_OK)
	{
		printf("%s\n", curl_easy_strerror(res));
		return "";
	}

	if (_stricmp(Transcoding.c_str(),"utf-8")==0)
	{
		return u82mb(m_userp.response);
	}
	return m_userp.response;
}

int Curl_toos::File_Write(const char* buff, const char* txt, bool Delete_File)
{
	if (Delete_File)
	{
		DeleteFileA(txt);
	}
	HANDLE hanele = CreateFileA(txt, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hanele == (HANDLE)-1)
	{
		hanele = CreateFileA(txt, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	if (hanele==(HANDLE)-1)
	{
		return 1;
	}
	string data = File_Read(txt);
	if (data != "")
	{
		data += "\r\n";
	}
	data += buff;
	OVERLAPPED op = { 0 };
	DWORD tten;
	::WriteFile(hanele, data.c_str(), data.length(), &tten, &op);
	CloseHandle(hanele);
	return 0;
}

int Curl_toos::File_Write(const WCHAR* buff, const WCHAR* txt, bool Delete_File)
{
	return File_Write(UnicodeToAnsi(buff).c_str(), UnicodeToAnsi(txt).c_str(), Delete_File);
}

std::string Curl_toos::File_Read(const char* txt)
{
	HANDLE hanele = CreateFileA(txt, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hanele == (HANDLE)-1)
	{
		hanele = CreateFileA(txt, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	OVERLAPPED apped = { 0 };
	char *buff = new char[GetFileSize(hanele, NULL) + 1];
	memset(buff, 0, GetFileSize(hanele, NULL) + 1);
	ReadFile(hanele, buff, GetFileSize(hanele, NULL), NULL, &apped);

	CloseHandle(hanele);
	string data1 = buff;
	delete[]buff;
	return data1;
}

std::wstring Curl_toos::File_Read(const WCHAR* txt)
{
	string data = File_Read(UnicodeToAnsi(txt).c_str());
	return AnsiToUnicode(data.c_str());
}

void Curl_toos::File_Handle(const char* txt, vector<string>& txt_buff)
{
	string buff = txt;
	int tm = -1;
	string data;
	while ((tm = buff.find("\r\n")) != -1)
	{
		data = buff.substr(0, tm);
		buff.erase(0, tm + 2);
		if (data=="")
		{
			continue;
		}
		txt_buff.push_back(data);
	}
	if (buff.length() != 0)
	{
		txt_buff.push_back(buff);
	}
}

void Curl_toos::File_Handle(const WCHAR* txt, vector<wstring>& txt_buff)
{
	wstring buff = txt;
	int tm = -1;
	wstring data;
	while ((tm = buff.find(L"\r\n")) != -1)
	{
		data = buff.substr(0, tm);
		buff.erase(0, tm + 2);
		txt_buff.push_back(data);
	}
	if (buff.length() != 0)
	{
		txt_buff.push_back(buff);
	}
}

string Curl_toos::GetPrivateProfile(string lpAppName, string lpKeyName, string lpFileName)
{
	TCHAR szValue[MAX_PATH + 1] = {0};
	GetPrivateProfileString(AnsiToUnicode(lpAppName.c_str()).c_str()
		, AnsiToUnicode(lpKeyName.c_str()).c_str(),L"", szValue, MAX_PATH, AnsiToUnicode( lpFileName.c_str()).c_str());
	return UnicodeToAnsi(szValue);
}

wstring Curl_toos::GetPrivateProfile(wstring lpAppName, wstring lpKeyName, wstring lpFileName)
{
	TCHAR szValue[MAX_PATH + 1] = { 0 };
	GetPrivateProfileString(lpAppName.c_str(), lpKeyName.c_str(), L"", szValue, MAX_PATH, lpFileName.c_str());
	return szValue;

}

void Curl_toos::File_WritePrivateProfile(string lpAppName, string lpKeyName, string data, string lpFileName)
{
	WritePrivateProfileString(AnsiToUnicode(lpFileName.c_str()).c_str(), AnsiToUnicode(lpKeyName.c_str()).c_str(), AnsiToUnicode(data.c_str()).c_str(), AnsiToUnicode(lpFileName.c_str()).c_str());
}

void Curl_toos::File_WritePrivateProfile(wstring lpAppName, wstring lpKeyName, wstring data, wstring lpFileName)
{
	WritePrivateProfileString(lpAppName.c_str(), lpKeyName.c_str(), data.c_str(), lpFileName.c_str());
}

std::string Curl_toos::u82mb(const char* cont)
{
	if (NULL == cont)
	{
		return "";
	}

	int num = MultiByteToWideChar(CP_UTF8, NULL, cont, -1, NULL, NULL);
	if (num <= 0)
	{
		return "";
	}
	wchar_t* buffw = new (std::nothrow) wchar_t[num];
	wmemset(buffw, 0, num);
	if (NULL == buffw)
	{
		return "";
	}
	MultiByteToWideChar(CP_UTF8, NULL, cont, -1, buffw, num);
	int len = WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, NULL, NULL, NULL, NULL);
	if (len <= 0)
	{
		delete[] buffw;
		return "";
	}
	char* lpsz = new (std::nothrow) char[len + 1];
	memset(lpsz, 0, sizeof(char)*(len + 1));
	if (NULL == lpsz)
	{
		delete[] buffw;
		return "";
	}
	WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, lpsz, len, NULL, NULL);
	lpsz[len] = '\0';
	string data = lpsz;
	delete[] buffw;
	delete[]lpsz;
	return data;
}
std::wstring Curl_toos::AnsiToUnicode(const char* szStr)
{
	if (strlen(szStr) == 0 || szStr == NULL)
	{
		return _T("");
	}
	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);
	if (nLen == 0)
	{
		return _T("");
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);

	wstring  data = pResult;
	delete[]pResult;
	return data;
}

std::string Curl_toos::UnicodeToAnsi(const wchar_t* szStr)
{
	if (wcslen(szStr) == 0 || szStr == NULL)
	{
		return "";
	}
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return "";
	}
	char* pResult = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
	string data = pResult;
	delete[]pResult;
	return data;
}

std::string Curl_toos::Http_Getstring(const char* http_buff, string data1, string data2)
{
	string buff = http_buff;
	if (data1==""&&data2=="")
	{
		return buff;
	}
	int num = 0;
	if (data1=="")
	{
		num=buff.find(data2);
		if (num!=-1)
		{
			buff = buff.replace(num,buff.size()-num, "");
			return buff;
		}
		return"";
	}if (data2=="")
	{
		num = buff.find(data1);
		if (num!=-1)
		{
			return buff.substr(buff.find(data1) + data1.size());
		}
		return "";
	}


	const char *str = strstr(buff.c_str(), data1.c_str());
//	num = buff.size();
	if (str)
	{
		buff = str;
		buff.erase(0, data1.size());
		num = buff.find(data2);
		if (num!=-1)
		{
			return buff.substr(0, num);
		}

	}
	return "";
}

std::wstring Curl_toos::Http_Getstring(const WCHAR* buff, wstring data1, wstring data2)
{
	return AnsiToUnicode(Http_Getstring(UnicodeToAnsi(buff).c_str(), UnicodeToAnsi(data1.c_str()), UnicodeToAnsi(data2.c_str())).c_str());
}

void Curl_toos::Http_Getstring_vector(const char *http_buff, string left, string right, vector<string>&data)
{
	data.clear();
	string buff = http_buff;
	int jisu = 0;
	string txt;
	while (true)
	{
		txt = Http_Getstring(buff.c_str(), left, right);
		if (txt == "")
		{
			break;
		}
		data.push_back(txt);
		buff.erase(0, buff.find(left) + left.size());
	}
}

void Curl_toos::Http_Getstring_vector(const WCHAR *http_buff, wstring left, wstring right, vector<wstring>&data)
{
	wstring buff = http_buff;
	int jisu = 0;
	wstring txt;
	while (true)
	{
		txt = Http_Getstring(buff.c_str(), left, right);
		if (txt == L"")
		{
			break;
		}
		data.push_back(txt);
		buff.erase(0, buff.find(left) + left.size());
	}
}


time_t Curl_toos::strTime2unix(const char timeStamp[])
{

	struct tm tm;
	memset(&tm, 0, sizeof(tm));
	sscanf_s(timeStamp, "%d-%d-%d %d:%d:%d",
		&tm.tm_year, &tm.tm_mon, &tm.tm_mday,
		&tm.tm_hour, &tm.tm_min, &tm.tm_sec);
	tm.tm_year -= 1900;
	tm.tm_mon--;
	return mktime(&tm);
}

std::string Curl_toos::date2String(string date)
{
	//time(L"May 7, 2020 10:00:12 AM");
	int year=0, mday=0, hour=0, min=0, sec=0;
	char mon[1024] = { 0 }, am_pm[1024] = { 0 };
	char REtime[1024] = { 0 };
	sscanf_s(date.c_str(), "%s %d, %d %d:%d:%d %s", mon,sizeof(mon), &mday, &year, &hour, &min, &sec, am_pm,sizeof(am_pm));

	vector<string>tim = { "January","February","March","April","May","June","July","August","September","October","November","December" };

	vector<string>::iterator it;
	int i = 1;
	for (it=tim.begin();it!=tim.end();it++,i++)
	{
		if ((*it)==mon)
		{
			if (_stricmp(am_pm,"AM")!=0)
			{
				sprintf_s(REtime, "%d-%d-%d %d:%d:%d", year, i, mday, hour+12, min, sec);
			}
			else
			{
				sprintf_s(REtime, "%d-%d-%d %d:%d:%d", year, i, mday, hour, min, sec);
			}
		}
	}
	return REtime;
}

std::string Curl_toos::GetTIme(__int64 n)
{
	time_t t;
	if (n!=0)
		t = n;
	else
		t = time(NULL);
	char strTime[1024] = { 0 };
	time_t timeStamp2 = (time_t)t;
	struct tm time2;
	localtime_s(&time2, &timeStamp2);
	strftime(strTime, sizeof(strTime) - 1, "%Y-%m-%d %H:%M:%S", &time2);
	return strTime;
}

UINT64 Curl_toos::GetTIme_unix()
{
	return time(NULL);
}

std::string Curl_toos::Get_Internet()
{
	LPTIME_OF_DAY_INFO pBuf = NULL;
	NET_API_STATUS nStatus;
	LPTSTR pszServerName = NULL;
	char time[1024] = { 0 };
	nStatus = NetRemoteTOD((LPCWSTR)pszServerName,(LPBYTE *)&pBuf);
	if (nStatus == NERR_Success)
	{
		if (pBuf != NULL)
		{
			sprintf_s(time, "%d-%02d-%02d %02d:%02d:%02d", pBuf->tod_year, pBuf->tod_month, pBuf->tod_day,  pBuf->tod_hours, pBuf->tod_mins, pBuf->tod_secs);
		}
	}
	else
	{
		fprintf(stderr, "A system error has occurred: %d\n", nStatus);
		return "";
	}
	if (pBuf != NULL)
		NetApiBufferFree(pBuf);


	return time;

}

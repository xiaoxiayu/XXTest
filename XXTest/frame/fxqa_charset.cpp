
#include "fxqa_charset.h"

int HexToDec(char *s)  
{  
	char *p = s;
	if(*p == '\0')  
		return 0;  

	while(*p == '0')  
		p++;  

	int dec = 0;  
	char c;  

	while(c = *p++)  
	{  
		dec <<= 4;  

		if(c >= '0' && c <= '9')  
		{  
			dec += c - '0';  
			continue;  
		}  
		if(c >= 'a' && c <= 'f')  
		{  
			dec += c - 'a' + 10;  
			continue;  
		}  
		if(c >= 'A' && c <= 'F')  
		{  
			dec += c - 'A' + 10;  
			continue;  
		}    
		return -1;  
	}
	return dec;  
}  

char *ascii_to_utf8(unsigned char c)
{
	char *out;

	if(c < 128)
	{
		out = (char *)calloc(2, sizeof(char));
		out[0] = c;
		out[1] = '\0';
	}
	else
	{
		out = (char *)calloc(3, sizeof(char));
		out[1] = (c >> 6) | 0xC0;
		out[0] = (c & 0x3F) | 0x80;
		out[2] = '\0';
	}

	return out;
}
                                                                                                        

void UnicodePrint(unsigned char* Message, char*& wstr)       
{                                                                                                              
	int size, i;                                                                                           

	size=Message[0];                                                                                       
	if (Message[1] == 0) i=2;                                                                              
	else i=1;                                                                                      
	int k = 0;
	while (i<size)                                                                                 
	{                                                                                              
//		printf("%C",Message[i]);
		wstr[k] = Message[i];
		i++;   
		k++;
	}                                                                                              
}  

std::string ws2s(const std::wstring& ws)
{
#if defined(_FXQA_PLATFORM_QTANDROID_) || defined(_FXQA_PLATFORM_QTIOS_)
    QString qStr;
    qStr = QString::fromStdWString(ws);
    std::string stdStr = qStr.toStdString();
    return stdStr;
#endif
    std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
    setlocale(LC_ALL, "");
    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = 2 * ws.size() + 1;
    char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
	size_t i = 0;
	wcstombs_s(&i, _Dest, (size_t)_Dsize, _Source, (size_t)_Dsize);
#else
    wcstombs(_Dest,_Source,_Dsize);
#endif
    std::string result = _Dest;
    delete []_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

std::wstring s2ws(const std::string& s)
{
#if defined(_FXQA_PLATFORM_QTANDROID_) || defined(_FXQA_PLATFORM_QTIOS_)
    QString qStr;
    qStr = QString::fromStdString(s);
    std::wstring stdWStr = qStr.toStdWString();
    return stdWStr;
#endif
	setlocale(LC_ALL, "");
	const char* _Source = s.c_str();
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
	size_t _Dsize = 0;
	mbstowcs_s(&_Dsize, NULL, 0, _Source, 0);
	_Dsize += 1;//s.size() + 1;
#else
	size_t _Dsize = mbstowcs(0, _Source, 0) + 1;//s.size() + 1;
#endif
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
#ifdef _FXQA_PLATFORM_WINDOWS_RT_
	size_t i = 0;
	mbstowcs_s(&i, _Dest, (size_t)_Dsize, _Source, (size_t)_Dsize);
#else
	mbstowcs(_Dest,_Source,_Dsize);
#endif
	std::wstring result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

std::string IntToString(double number)
{
	std::stringstream ssm_Report;
	std::string ss_Report;
	ssm_Report << number;
	ssm_Report >> ss_Report;
	ssm_Report.clear();

	return ss_Report;
}

std::wstring IntToWString(double number)
{
	std::string intStr = IntToString(number);
	return s2ws(intStr);
}


std::string a2s(const char* p, int len)
{
	if(0 != len){
		std::string str(p, len);
		return str;
	}else{
		std::string str(p, strlen(p));
		return str;
	}
}


#ifdef _FXQA_PLATFORM_WINDOWS_WIN32_
int U2A(const wchar_t* szU,char* szA,size_t cnt)
{
	return WideCharToMultiByte (CP_ACP, 0, szU, -1, szA, cnt, NULL, NULL) ;
}
int A2U(const char* szA,wchar_t* szU,size_t cnt)
{
	return MultiByteToWideChar (CP_ACP, 0, szA, -1, szU, cnt) ;
}
/** 
* Wide-character to std::string
* @param[in] wide-character
* @return std::string
*/
std::string  U2A(const wchar_t* szU)
{
	int nRetCode=U2A(szU,0,0);
	if(0==nRetCode)
		return std::string();
	std::string str(nRetCode-1,'\0');
	U2A(szU,(char*)(str.c_str()),nRetCode);
	return str;
}
/** 
* String to wide-character
* @param[in] std::string
* @return wide-character
*/
std::wstring A2U(const char* szA)
{
	int nRetCode=A2U(szA,0,0);
	if(0==nRetCode)
		return std::wstring();
	std::wstring str(nRetCode-1,'\0');
	A2U(szA,(wchar_t*)(str.c_str()),nRetCode);
	return str;
}

/** 
* Utf-8 to std::string
* @param[in] uft-8
* @return std::string
*/
std::string Utf8ToStr(char* Utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)Utf8, -1, NULL,0);
	WCHAR * Unicode = new WCHAR[len+1];
	memset(Unicode, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)Utf8, -1, Unicode, len);
	std::string FileName = U2A(Unicode);
	delete []Unicode;
	return FileName;
}

/** 
* Utf-8 to unicode
* @param[in] uft-8
* @param[out] std::string
*/
void UTF_8ToUnicode(wchar_t* pOut,char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

/** 
* Unicode to utf8
* @param[in] utf8
* @param[out] std::string
*/
void UnicodeToUTF_8(wchar_t* pText, char* pOut)
{
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));

	return;
}

/** 
* Unicode to GB2312
* @param[in] unicode
* @param[out] GB2312
*/
void UnicodeToGB2312(wchar_t uData, char* pOut)
{
	WideCharToMultiByte(CP_ACP,NULL,&uData,1,pOut,sizeof(wchar_t),NULL,NULL);
	return;
}     

/** 
* GB2312 to Unicode
* @param[in] GB2312
* @param[out] unicode
*/
void Gb2312ToUnicode(char *gbBuffer, wchar_t* pOut)
{
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
	return ;
}

/** 
* GB2312 to Utf8
* @param[in] GB2312
* @param[in] length
* @param[out] utf8
*/
void GB2312ToUTF_8(char *pText, int pLen, std::string& pOut)
{
	char buf[4];
	int nLength = pLen* 3;
	char* rst = new char[nLength];

	memset(buf,0,4);
	memset(rst,0,nLength);

	int i = 0;
	int j = 0;      
	while(i < pLen)
	{
		if( *(pText + i) >= 0)
		{
			rst[j++] = pText[i++];
		}
		else
		{
			wchar_t pbuffer;
			Gb2312ToUnicode(pText+i, &pbuffer);

			UnicodeToUTF_8(&pbuffer, buf);

			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];    

			j += 3;
			i += 2;
		}
	}
	rst[j] = '\0';

	pOut = rst;             
	delete []rst;   

	return;
}

/** 
* Utf8 to GB2312
* @param[in] utf8
* @param[in] length
* @param[out] gb2312
*/
void UTF_8ToGB2312(char *pText, int pLen, std::string &pOut)
{
	char * newBuf = new char[pLen];
	char Ctemp[4];
	memset(Ctemp,0,4);

	int i =0;
	int j = 0;

	while(i < pLen)
	{
		if(pText[i] > 0)
		{
			newBuf[j++] = pText[i++];                       
		}
		else                 
		{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp,pText + i);

			UnicodeToGB2312(Wtemp, Ctemp);

			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	}
	newBuf[j] = '\0';
	pOut = newBuf;
	delete []newBuf;

	return; 
}


#endif // _FXQA_PLATFORM_WINDOWS_WIN32_

int cUxU8(char* pOut, wchar_t* pText){
    int ret = 0;
    unsigned char* pchar = (unsigned char *)pText;
    if(pText[0]<=0x7f){         // ASCII  0x00 ~ 0x7f
        if (pOut != NULL) {
            pOut[0] = (char)pchar[0];
        }
    }else if(pText[0]<=0x7ff){  // 0x080 ~ 0x7ff
        if (pOut != NULL) {
            pOut[0] = 0xc0|(pchar[1]<<2)|(pchar[0]>>6);
            pOut[1] = 0x80|(pchar[0]&0x3f);
        }
        ret = 1;
    }else{                      // 0x0800 ~ 0xFFFF
        if (pOut != NULL) {
            pOut[0] = 0xe0|(pchar[1]>>4);
            pOut[1] = 0x80|((pchar[1]&0x0f)<<2)|(pchar[0]>>6);
            pOut[2] = 0x80|(pchar[0]&0x3f);
        }
        ret = 2;
    }
    return ret;
}

int sUxU8(char* pOut, wchar_t* pText,int Len){
    int i,j;
    for(i=0,j=0;i<Len;i++,j++){
        if (pOut == NULL) {
             j+=cUxU8(NULL, &pText[i]);
        } else {
             j+=cUxU8(&pOut[j],&pText[i]);
        }

    }
    return j;
}

int cU8xU(wchar_t* pOut,char *pText){
    int ret = 0;
    char* uchar = (char *)pOut;
    unsigned cIn = (unsigned char)pText[0];
    if(cIn<0x80){              // ASCII  0x00 ~ 0x7f
		if (pOut != NULL) {
			pOut[0] = pText[0];
		}
    }else if(cIn<0xdf){
		if (pOut != NULL) {
			uchar[0] = (pText[0]<<6)|(pText[1]&0x3f);
			uchar[1] = (pText[0]>>2)&0x0f;
		}
        ret = 1;
    }else if(cIn<=0xef){
		if (pOut != NULL) {
			uchar[0] = (pText[1]<<6)|(pText[2]&0x3f);
			uchar[1] = (pText[0]<<4)|((pText[1]>>2)&0x0f);
		}
        ret = 2;
    }else if(cIn<0xf7){
		if (pOut != NULL) {
			uchar[0] = (pText[2]<<6)|(pText[3]&0x3f);
			uchar[1] = (pText[1]<<4)|((pText[2]>>2)&0x0f);
			uchar[2] = ((pText[0]<<2)&0x1c)|((pText[1]>>4)&0x03);
		}
        ret = 3;
    }
    return ret;
}

int sU8xU(wchar_t* pOut,char *pText,int Len){
    int i,j;
    for(i=0,j=0;i<Len;i++,j++){
		if (pOut == NULL) {
			i+=cU8xU(NULL,&pText[i]);
		} else {
			i+=cU8xU(&pOut[j],&pText[i]);
		} 
    }
    return j;
}

void WstringToUTF8(const std::wstring wStr, std::string& utf8Str) {
#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_)
	char* utf8_log_info = NULL;
	int utf8_len = sUxU8(utf8_log_info, (wchar_t*)wStr.c_str(), wStr.length());
	utf8_log_info = new char[utf8_len+1];
	memset(utf8_log_info, 0, utf8_len+1);
	sUxU8(utf8_log_info, (wchar_t*)wStr.c_str(), wStr.length());

	utf8Str = utf8_log_info;

	delete[]  utf8_log_info;
#else
	utf8Str = ws2s(wStr);
#endif
}
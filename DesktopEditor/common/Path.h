﻿#ifndef _BUILD_PATH_CROSSPLATFORM_H_
#define _BUILD_PATH_CROSSPLATFORM_H_

#include <string>
#include <string.h>
#include "File.h"

#ifdef WIN32
#include <wchar.h>
#elif LINUX
#include <libgen.h>
#elif MAC
#endif


namespace NSSystemPath
{
	static std::wstring GetDirectoryName(const std::wstring& strFileName)
	{
		std::wstring sRes;
#ifdef WIN32
		TCHAR tDrive[256];
		TCHAR tFolder[256];
		_tsplitpath( strFileName.c_str(), tDrive, tFolder, NULL, NULL );
		sRes.append(tDrive);
		sRes.append(tFolder);
		return sRes;
#elif LINUX
		BYTE* pUtf8 = NULL;
		LONG lLen = 0;
		NSFile::CUtf8Converter::GetUtf8StringFromUnicode(strFileName.c_str(), strFileName.length(), pUtf8, lLen, false);
		char* pDirName = dirname((char*)pUtf8);
		sRes = NSFile::CUtf8Converter::GetUnicodeStringFromUTF8((BYTE*)pDirName, strlen(pDirName));
		delete [] pUtf8;
#elif MAC
#endif
	}
	static std::wstring GetFileName(const std::wstring& strFileName)
	{
		std::wstring sRes;
#ifdef WIN32
		TCHAR tFilename[256];
		TCHAR tExt[256];
		_tsplitpath( strFileName.c_str(), NULL, NULL, tFilename, tExt );
		sRes.append(tFilename);
		sRes.append(tExt);
		return sRes;
#elif LINUX
		BYTE* pUtf8 = NULL;
		LONG lLen = 0;
		NSFile::CUtf8Converter::GetUtf8StringFromUnicode(strFileName.c_str(), strFileName.length(), pUtf8, lLen, false);
		char* pBaseName = basename((char*)pUtf8);
		sRes = NSFile::CUtf8Converter::GetUnicodeStringFromUTF8((BYTE*)pBaseName, strlen(pBaseName));
		delete [] pUtf8;
#elif MAC
#endif
		return sRes;
	}
	static std::wstring Combine(const std::wstring& strLeft, const std::wstring& strRight)
	{
		std::wstring sRes;
		bool bLeftSlash = false;
		bool bRightSlash = false;
		if(strLeft.length() > 0)
		{
			wchar_t cLeft = strLeft[strLeft.length() - 1];
			bLeftSlash = ('/' == cLeft) || ('\\' == cLeft);
		}
		if(strRight.length() > 0)
		{
			wchar_t cRight = strRight[0];
			bRightSlash = ('/' == cRight) || ('\\' == cRight);
		}
		if(bLeftSlash && bRightSlash)
		{
			sRes = strLeft + strRight.substr(1);
		}
		else if(!bLeftSlash && !bRightSlash)
			sRes = strLeft + _T("/") + strRight;
		else
			sRes = strLeft + strRight;
		return sRes;
	}
}

#endif //_BUILD_PATH_CROSSPLATFORM_H_
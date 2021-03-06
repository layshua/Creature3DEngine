/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/
#include <CRIOManager/crConvertUTF.h>
#include <CRCore/crNotify.h>

#include <string.h>
#include <wchar.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#define CP_GB2312 936
#define CP_BIG5 950
namespace CRIOManager
{

	std::string convertUTF16toUTF8(const std::wstring& s) { return convertUTF16toUTF8(s.c_str(), s.length()); }
	std::string convertUTF16toUTF8(const wchar_t* s) { return convertUTF16toUTF8(s, wcslen(s)); }

	std::wstring convertUTF8toUTF16(const std::string& s) { return convertUTF8toUTF16(s.c_str(), s.length()); }
	std::wstring convertUTF8toUTF16(const char* s) { return convertUTF8toUTF16(s, strlen(s)); }

	bool isGBCode(const std::string& strIn)
	{
		unsigned char ch1;
		unsigned char ch2;

		if (strIn.size() >= 2)
		{
			ch1 = (unsigned char)strIn.at(0);
			ch2 = (unsigned char)strIn.at(1);
			if (ch1 >= 176 && ch1 <= 247 && ch2 >= 160 && ch2 <= 254)
				return true;
			else return false;
		}
		else return false;
	}
	bool isGB2312(char head, char tail)
	{
		int iHead = head & 0xff;
		int iTail = tail & 0xff;
		return ((iHead >= 0xa1 && iHead <= 0xf7 &&
			iTail >= 0xa1 && iTail <= 0xfe) ? true : false);
	}

	bool isGBK(char head, char tail)
	{
		int iHead = head & 0xff;
		int iTail = tail & 0xff;
		return ((iHead >= 0x81 && iHead <= 0xfe &&
			(iTail >= 0x40 && iTail <= 0x7e ||
				iTail >= 0x80 && iTail <= 0xfe)) ? true : false);
	}

	bool isBIG5(char head, char tail)
	{
		int iHead = head & 0xff;
		int iTail = tail & 0xff;
		return ((iHead >= 0xa1 && iHead <= 0xf9 &&
			(iTail >= 0x40 && iTail <= 0x7e ||
				iTail >= 0xa1 && iTail <= 0xfe)) ? true : false);
	}
	std::string convertUTF16toUTF8(const wchar_t* source, unsigned sourceLength)
	{
#if defined(WIN32) && !defined(__CYGWIN__)
		if (sourceLength == 0)
		{
			return std::string();
		}

		int destLen = WideCharToMultiByte(/*CP_ACP*/CP_GB2312, 0, source, sourceLength, 0, 0, 0, 0);
		if (destLen <= 0)
		{
			CRCore::notify(CRCore::WARN) << "Cannot convert UTF-16 string to UTF-8." << std::endl;
			return std::string();
		}

		std::string sDest(destLen, '\0');
		//sDest.resize(destLen+1,0);
		destLen = WideCharToMultiByte(/*CP_ACP*/CP_GB2312, 0, source, sourceLength, &sDest[0], destLen, 0, 0);

		if (destLen <= 0)
		{
			CRCore::notify(CRCore::WARN) << "Cannot convert UTF-16 string to UTF-8." << std::endl;
			return std::string();
		}

		return sDest;
#else
		//TODO: Implement for other platforms
		CRCore::notify(CRCore::WARN) << "ConvertUTF16toUTF8 not implemented." << std::endl;
		return std::string();
#endif
	}

	std::wstring convertUTF8toUTF16(const char* source, unsigned sourceLength)
	{
#if defined(WIN32) && !defined(__CYGWIN__)
		if (sourceLength == 0)
		{
			return std::wstring();
		}
		int cp = /*(sourceLength>1 && isBIG5(source[0], source[1])) ? CP_BIG5:*/CP_GB2312;
		int destLen = MultiByteToWideChar(/*CP_ACP*/cp, 0, source, sourceLength, 0, 0);
		if (destLen <= 0)
		{
			CRCore::notify(CRCore::WARN) << "Cannot convert UTF-8 string to UTF-16." << std::endl;
			return std::wstring();
		}

		std::wstring sDest(destLen, L'\0');
		//sDest.resize(destLen+1,0);
		destLen = MultiByteToWideChar(/*CP_ACP*/cp, 0, source, sourceLength, &sDest[0], destLen);

		if (destLen <= 0)
		{
			CRCore::notify(CRCore::WARN) << "Cannot convert UTF-8 string to UTF-16." << std::endl;
			return std::wstring();
		}
		return sDest;
#else
		//TODO: Implement for other platforms
		CRCore::notify(CRCore::WARN) << "ConvertUTF8toUTF16 not implemented." << std::endl;
		return std::wstring();
#endif
	}

	std::string convertTCHARToWebUTF8(const std::wstring& s)
	{
		std::string utf8string;
		for (std::wstring::const_iterator itr = s.begin();
			itr != s.end();
			++itr)
		{
			unsigned int currentChar = *itr;
			if (currentChar < 0x80)
			{
				utf8string += (char)currentChar;
			}
			else if (currentChar < 0x800)
			{
				//utf8string += (char)(0xc0 | (currentChar >> 6));
				//utf8string += (char)(0x80 | (currentChar & 0x3f));

				utf8string += (char)((currentChar >> 6) | 128 | 64);
				utf8string += (char)(currentChar & 0x3F) | 128;
			}
			else if(currentChar < 0x10000)
			{
				//utf8string += (char)(0xe0 | (currentChar >> 12));
				//utf8string += (char)(0x80 | ((currentChar >> 6) & 0x3f));
				//utf8string += (char)(0x80 | (currentChar & 0x3f));

				utf8string += (char)((currentChar >> 12) | 128 | 64 | 32);
				utf8string += (char)((currentChar >> 6) & 0x3F) | 128;
				utf8string += (char)(currentChar & 0x3F) | 128;
			}
			else
			{
				utf8string += (char)((currentChar >> 18) | 128 | 64 | 32 | 16);
				utf8string += (char)((currentChar >> 12) & 0x3F) | 128;
				utf8string += (char)((currentChar >> 6) & 0x3F) | 128;
				utf8string += (char)(currentChar & 0x3F) | 128;
			}
		}
		return utf8string;
	}
}
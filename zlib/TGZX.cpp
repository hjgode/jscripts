
#include "define.h"
#include "unibase.h"
#include "zlib/ZLib.h"
#include "MortTypes.h"

#pragma comment(lib, "../Zlib/ZLib/ZLib.lib")

#define CC_SEP 1
#define CC_LEN 4
#define WINCE
// Values used in typeflag field
#define REGTYPE				'0'				// regular file
#define AREGTYPE			'\0'			// regular file
#define LNKTYPE				'1'				// link
#define SYMTYPE				'2'				// reserved
#define CHRTYPE				'3'				// character special
#define BLKTYPE				'4'				// block special
#define DIRTYPE				'5'				// directory
#define FIFOTYPE			'6'				// FIFO special
#define CONTTYPE			'7'				// reserved

//  GNU tar extensions
#define GNUTYPE_DUMPDIR		'D'				// file names from dumped directory
#define GNUTYPE_LONGLINK	'K'				// long link name
#define GNUTYPE_LONGNAME	'L'				// long file name
#define GNUTYPE_MULTIVOL	'M'				// continuation of file from another volume
#define GNUTYPE_NAMES		'N'				// file name that does not fit into main hdr
#define GNUTYPE_SPARSE		'S'				// sparse file
#define GNUTYPE_VOLHDR		'V'				// tape/volume hdr


#define MAX_BLOCK 512
#define MAX_FNAME 100

struct TARHDR
{
	CHAR name[100];               //   0
	CHAR mode[8];                 // 100
	CHAR uid[8];                  // 108
	CHAR gid[8];                  // 116
	CHAR size[12];                // 124
	CHAR mtime[12];               // 136
	CHAR chksum[8];               // 148
	CHAR typeflag;                // 156
	CHAR linkname[100];           // 157
	CHAR magic[6];                // 257
	CHAR version[2];              // 263
	CHAR uname[32];               // 265
	CHAR gname[32];               // 297
	CHAR devmajor[8];             // 329
	CHAR devminor[8];             // 337
	CHAR prefix[155];             // 345
};

typedef union _TARBUF
{
	TARHDR hdr;
	CHAR buf[MAX_BLOCK];
}
TARBUF, *PTARBUF;


// Convert octal digits to INT
INT GetOct(PSTR p, INT iWidth)
{
	INT iResult = 0;
	while (iWidth--)
	{
		CHAR c = *p++;
		if (c == 0)
		{
			break;
		}
		if (c == ' ')
		{
			continue;
		}
		if (c < '0' || c > '7')
		{
			return -1;
		}
		iResult = iResult * 8 + (c - '0');
	}
	return iResult;
}

HRESULT TGZX(PTSTR ptzCmd,CStr OutDir)
{
	long rc = 0;
	PTSTR ptzOutDir = (PTSTR)((PCTSTR)OutDir);//ptzCmd;

	CHAR szPath[MAX_BLOCK];
	UStrToAStr(szPath, ptzCmd, MAX_PATH);
	gzFile gz = gzopen(szPath, "rb");
	if (!gz)
	{
		return -1;
	}

	INT iGetHeader = 1;
	INT iRemaining = 0;
	BOOL bFail = FALSE;
	HANDLE hFile = NULL;
	TCHAR tzName[MAX_PATH];
	TCHAR tzPath[MAX_PATH];
	do
	{
		TARBUF buf;
		INT iLen = gzread(gz, &buf, MAX_BLOCK);
		if (iLen < 0)
		{
			return -2;
		}

		// Always expect complete blocks to process the tar information.
		if (iLen != MAX_BLOCK)
		{
			bFail = TRUE;
			iRemaining = 0;
		}

		// If we have to get a tar header
		if (iGetHeader >= 1)
		{
			// if we met the end of the tar or the end-of-tar block, we are done
			if (iLen == 0 || buf.hdr.name[0] == 0)
			{
				break;
			}

			if (iGetHeader == 1)
			{
				UAStrToStr(tzName, buf.hdr.name, MAX_PATH);
				UStrRep(tzName, '/', '\\');
			}
			else
			{
				iGetHeader = 1;
			}

			// Act according to the type flag
			switch (buf.hdr.typeflag)
			{
			case DIRTYPE:
				UStrPrint(tzPath, TEXT("%s\\%s\\"), ptzOutDir, tzName);
				UDirCreate(tzPath);
				break;

			case REGTYPE:
			case AREGTYPE:
				iRemaining = GetOct(buf.hdr.size, 12);
				if (iRemaining == -1)
				{
					bFail = TRUE;
					break;
				}
				else
				{
					UStrPrint(tzPath, TEXT("%s\\%s"), ptzOutDir, tzName);
					UDirCreate(tzPath);
					hFile = UFileOpen(tzPath, UFILE_WRITE);
					rc++;
				}
				iGetHeader = 0;
				break;

			case GNUTYPE_LONGLINK:
			case GNUTYPE_LONGNAME:
				iRemaining = GetOct(buf.hdr.size, 12);
				if (iRemaining < 0 || iRemaining >= MAX_BLOCK)
				{
					bFail = TRUE;
					break;
				}
				iLen = gzread(gz, szPath, MAX_BLOCK);
				if (iLen < 0)
				{
					return -4;
				}
				if (szPath[MAX_BLOCK - 1] != 0 || (INT) strlen(szPath) > iRemaining)
				{
					bFail = TRUE;
					break;
				}
				iGetHeader = 2;
				break;
			}
		}
		else
		{
			UINT uSize = (iRemaining > MAX_BLOCK) ? MAX_BLOCK : iRemaining;
			if (hFile)
			{
				UFileWrite(hFile, &buf, uSize);
			}
			iRemaining -= uSize;
		}

		if (iRemaining == 0)
		{
			iGetHeader = 1;
			if (hFile)
			{
				UFileClose(hFile);
				hFile = NULL;
			}
		}
	}while (!bFail);

	gzclose(gz);

	return rc;
}

/*
#include "reg.h"
TCHAR g_tzXVar[10][MAX_PATH] = {0};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get Registry root
HKEY GetRootKey(PTSTR& ptzKey)
{
	STATIC struct {HKEY hKey; TCHAR tzKey[20];} c_sRegRoot[] =
	{
		{HKEY_USERS, TEXT("HKU\\")},
		{HKEY_CURRENT_USER, TEXT("HKCU\\")},
		{HKEY_CLASSES_ROOT, TEXT("HKCR\\")},
		{HKEY_LOCAL_MACHINE, TEXT("HKLM\\")},
		{HKEY_USERS, TEXT("HKEY_USERS\\")},
		{HKEY_CURRENT_USER, TEXT("HKEY_CURRENT_USER\\")},
		{HKEY_CLASSES_ROOT, TEXT("HKEY_CLASSES_ROOT\\")},
		{HKEY_LOCAL_MACHINE, TEXT("HKEY_LOCAL_MACHINE\\")},
	};

	for (UINT i = 0; i < _NumOf(c_sRegRoot); i++)
	{
		UINT n = UStrMatch(c_sRegRoot[i].tzKey, ptzKey);
		if (c_sRegRoot[i].tzKey[n] == 0)
		{
			ptzKey += n;
			return c_sRegRoot[i].hKey;
		}
	}
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// REGistry eXecution
HRESULT REGX(PTSTR ptzCmd)
{
	HKEY hRoot = GetRootKey(ptzCmd);
	if (!hRoot)
	{
		return E_INVALIDARG;
	}

	PTSTR ptzVal = UStrChr(ptzCmd, '=');
	if (ptzVal)
	{
		*ptzVal++ = 0;
	}

	PTSTR ptzName = UStrRChr(ptzCmd, '\\');
	if (!ptzName)
	{
		return E_INVALIDARG;
	}
	else
	{
		*ptzName++ = 0;
	}

	HKEY hKey;
	HRESULT hResult = RegCreateKeyEx(hRoot, ptzCmd, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (hResult != S_OK)
	{
		return hResult;
	}

	if (ptzVal)
	{
		if (*ptzName == '#')
		{
			DWORD dwData = UStrToInt(ptzVal);
			hResult = RegSetValueEx(hKey, ptzName + 1, 0, REG_DWORD, (PBYTE) &dwData, sizeof(DWORD));
		}
		else if (*ptzName == '@')
		{
			UINT i = 0;
			PBYTE pbVal = (PBYTE) ptzVal;
			while (*ptzVal)
			{
				pbVal[i++] = (UChrToHex(ptzVal[0]) << 4) | UChrToHex(ptzVal[1]);
				while (*ptzVal && (*ptzVal++ != ','));
			}
			hResult = RegSetValueEx(hKey, ptzName + 1, 0, REG_BINARY, pbVal, i);
		}
		else
		{
			hResult = RegSetValueEx(hKey, ptzName, 0, REG_SZ, (PBYTE) ptzVal, (UStrLen(ptzVal) + 1) * sizeof(TCHAR));
		}
	}
	else
	{
		if (*ptzName == '-')
		{
			if (ptzName[1])
			{
				hResult = RegDeleteValue(hKey, ptzName + 1);
			}
			else
			{
				RegCloseKey(hKey);
				return RegDeleteKey(hRoot, ptzCmd);
			}
		}
		else if (*ptzName == '#')
		{
			DWORD dwSize = sizeof(hResult);
			RegQueryValueEx(hKey, ptzName + 1, NULL, NULL, (PBYTE) &hResult, &dwSize);
		}
		else
		{
			g_tzXVar[0][0] = 0;
			DWORD dwSize = sizeof(g_tzXVar[0]);
			hResult = RegQueryValueEx(hKey, ptzName, NULL, NULL, (PBYTE) g_tzXVar[0], &dwSize);
		}
	}

	RegCloseKey(hKey);
	return hResult;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

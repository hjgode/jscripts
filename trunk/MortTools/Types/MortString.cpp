#include "mortstring.h"
#include "string.h"
#include "tchar.h"

#ifndef DESKTOP
#define _tcsinc(x) x++
#define _tcsdec(x) x--
#endif

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CStr::CStr()
{
	void *data = malloc(sizeof(TCHAR));
    stringData.data = (TCHAR*)data;
    stringData.data[0] = '\0';
    stringData.nAllocLength = 1;
    stringData.nDataLength  = 1;
}

CStr::CStr(const CStr& stringSrc)
{
    stringData.data = (TCHAR*)malloc(stringSrc.stringData.nDataLength*sizeof(TCHAR));
    memcpy( stringData.data, stringSrc.stringData.data, stringSrc.stringData.nDataLength*sizeof(TCHAR) );
    stringData.nAllocLength = stringSrc.stringData.nDataLength;
    stringData.nDataLength  = stringSrc.stringData.nDataLength;
}

CStr::CStr(LPCTSTR lpsz)
{
	if ( lpsz != NULL )
	{
		int len = wcslen(lpsz)+1;
		stringData.data = (TCHAR*)malloc( len*sizeof(TCHAR) );
		wcscpy( stringData.data, lpsz );
		stringData.nAllocLength = len;
		stringData.nDataLength  = len;
	}
	else
	{
		stringData.data = NULL; // Nothing to free
		Empty();				// Empty new string
	}
}

void CStr::Empty()
{
    free(stringData.data);

    stringData.data = (TCHAR*)malloc(sizeof(TCHAR));
    stringData.data[0] = '\0';
    stringData.nAllocLength = 1;
    stringData.nDataLength  = 1;
}

CStr::~CStr()
{
    free(stringData.data);
}

//////////////////////////////////////////////////////////////////////////////
// Helpers for the rest of the implementation

void CStr::AllocCopy(CStr& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const
{
	// will clone the data attached to this string
	// allocating 'nExtraLen' characters
	// Places results in uninitialized string 'dest'
	// Will copy the part or all of original data to start of new string

	int nNewLen = nCopyLen + nExtraLen + 1;
    TCHAR *newData = (TCHAR*)malloc(nNewLen*sizeof(TCHAR));
    memcpy( newData, stringData.data+nCopyIndex, nCopyLen*sizeof(TCHAR) );
    newData[nNewLen-1]='\0';
    free( dest.stringData.data );
    dest.stringData.data = newData;
    dest.stringData.nAllocLength = nNewLen;
    dest.stringData.nDataLength  = nNewLen;
}

//////////////////////////////////////////////////////////////////////////////
// Assignment operators
//  All assign a new value to the string
//      (a) first see if the buffer is big enough
//      (b) if enough room, copy on top of old buffer, set size and type
//      (c) otherwise free old string data, and create a new one
//
//  All routines return the new string (but as a 'const CStr&' so that
//      assigning it again will cause a copy, eg: s1 = s2 = "hi there".
//

void CStr::AssignCopy(int nSrcLen, LPCTSTR lpszSrcData)
{
    free( stringData.data );

    stringData.data = (TCHAR*)malloc(nSrcLen*sizeof(TCHAR));
    memcpy( stringData.data, lpszSrcData, nSrcLen*sizeof(TCHAR) );
	stringData.data[nSrcLen-1] = '\0';
    stringData.nAllocLength = nSrcLen;
    stringData.nDataLength  = nSrcLen;
}

const CStr& CStr::operator=(const CStr& stringSrc)
{
	AssignCopy(stringSrc.stringData.nDataLength, stringSrc.stringData.data);

    return *this;
}

const CStr& CStr::operator=(LPCTSTR lpsz)
{
	AssignCopy(wcslen(lpsz)+1, lpsz);

	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// concatenation

// NOTE: "operator+" is done as friend functions for simplicity
//      There are three variants:
//          CStr + CStr
// and for ? = TCHAR, LPCTSTR
//          CStr + ?
//          ? + CStr

void CStr::ConcatCopy(int nSrc1Len, LPCTSTR lpszSrc1Data, int nSrc2Len, LPCTSTR lpszSrc2Data)
{
    // no space for two binary zeros needed...
	int nNewLen = nSrc1Len + nSrc2Len - 1;
	if (nNewLen != 0)
	{
        TCHAR *newData = (TCHAR*)malloc(nNewLen*sizeof(TCHAR));
		memcpy( newData, lpszSrc1Data, nSrc1Len*sizeof(TCHAR));
		memcpy( newData+nSrc1Len-1, lpszSrc2Data, nSrc2Len*sizeof(TCHAR));
        newData[nNewLen-1] = '\0';
        free( stringData.data );
        stringData.data = newData;
        stringData.nDataLength  = nNewLen;
        stringData.nAllocLength = nNewLen;
	}
    else
    {
        Empty();
    }
}

CStr operator+(const CStr& string1, const CStr& string2)
{
	CStr s;
	s.ConcatCopy(string1.stringData.nDataLength, string1.stringData.data,
		         string2.stringData.nDataLength, string2.stringData.data);
	return s;
}

CStr operator+(const CStr& string, LPCTSTR lpsz)
{
	CStr s;
	s.ConcatCopy(string.stringData.nDataLength, string.stringData.data,
		         wcslen(lpsz)+1, lpsz);
	return s;
}

CStr operator+(LPCTSTR lpsz, const CStr& string)
{
	CStr s;
	s.ConcatCopy(wcslen(lpsz)+1, lpsz,
                 string.stringData.nDataLength, string.stringData.data);
	return s;
}

//////////////////////////////////////////////////////////////////////////////
// concatenate in place

void CStr::ConcatInPlace(int nSrcLen, LPCTSTR lpszSrcData)
{
	//  -- the main routine for += operators

	// concatenating an empty string is a no-op!
	if (nSrcLen <= 1)
		return;

	int nNewLen = stringData.nDataLength + nSrcLen - 1;
    TCHAR *newData = (TCHAR*)malloc(nNewLen*sizeof(TCHAR));
    memcpy( newData, stringData.data, stringData.nDataLength*sizeof(TCHAR));
	memcpy( newData+stringData.nDataLength-1, lpszSrcData, nSrcLen*sizeof(TCHAR));
    newData[nNewLen-1] = '\0';
    free( stringData.data );
    stringData.data = newData;
    stringData.nDataLength  = nNewLen;
    stringData.nAllocLength = nNewLen;
}

const CStr& CStr::operator+=(LPCTSTR lpsz)
{
	ConcatInPlace(wcslen(lpsz)+1, lpsz);
	return *this;
}

const CStr& CStr::operator+=(TCHAR ch)
{
	ConcatInPlace(2, &ch);
	return *this;
}

const CStr& CStr::operator+=(const CStr& string)
{
	ConcatInPlace(string.stringData.nDataLength, string.stringData.data);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Advanced direct buffer access

LPTSTR CStr::GetBuffer(int nMinBufLength)
{
	ASSERT(nMinBufLength >= 0);

	if (nMinBufLength > stringData.nAllocLength)
	{
		// we have to grow the buffer
        TCHAR *newData = (TCHAR*)malloc(nMinBufLength*sizeof(TCHAR));
        memcpy( newData, stringData.data, stringData.nDataLength*sizeof(TCHAR));
        free( stringData.data );
        stringData.data = newData;
        stringData.nDataLength  = nMinBufLength;
        stringData.nAllocLength = nMinBufLength;
	}

	return stringData.data;
}

void CStr::ReleaseBuffer(int nNewLength)
{
    stringData.nDataLength = wcslen(stringData.data)+1;
    FreeExtra();
}

LPTSTR CStr::GetBufferSetLength(int nNewLength)
{
	return GetBuffer(nNewLength);
}

void CStr::FreeExtra()
{
	if (stringData.nDataLength != stringData.nAllocLength)
	{
        TCHAR *newData = (TCHAR*)malloc(stringData.nDataLength*sizeof(TCHAR));
        memcpy( newData, stringData.data, stringData.nDataLength*sizeof(TCHAR));
        free( stringData.data );
        stringData.data = newData;
        stringData.data[stringData.nDataLength-1] = '\0';
        stringData.nDataLength  = stringData.nDataLength;
        stringData.nAllocLength = stringData.nDataLength;
	}
}

CStr::operator LPCTSTR() const
{
	return (LPCTSTR)stringData.data;
}

int CStr::GetLength() const
{
    return stringData.nDataLength-1;
}

BOOL CStr::IsEmpty() const
{
    return (stringData.nDataLength==1);
}

TCHAR CStr::GetAt( int idx ) const
{
    if ( idx < stringData.nDataLength )
        return stringData.data[idx];
    else
        return '\0';
}

///////////////////////////////////////////////////////////////////////////////
// Commonly used routines (rarely used routines in STREX.CPP)

int CStr::Find(TCHAR ch) const
{
	return Find(ch, 0);
}

int CStr::Find(TCHAR ch, int nStart) const
{
	if (nStart >= stringData.nDataLength)
		return -1;

	// find first single character
	LPTSTR lpsz = wcschr(stringData.data + nStart, (TCHAR)ch);

	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - stringData.data);
}

int CStr::FindOneOf(LPCTSTR lpszCharSet) const
{
	LPTSTR lpsz = wcspbrk(stringData.data, lpszCharSet);
	return (lpsz == NULL) ? -1 : (int)(lpsz - stringData.data);
}

int CStr::ReverseFind(TCHAR ch, int Pos) const
{
	// find last single character
	LPTSTR lpszSrc = stringData.data + Pos;
	LPTSTR lpsz = wcsrchr(lpszSrc, (TCHAR) ch);

	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - stringData.data);
}

// find a sub-string (like strstr)
int CStr::Find(LPCTSTR lpszSub) const
{
	return Find(lpszSub, 0);
}

int CStr::Find(LPCTSTR lpszSub, int nStart) const
{
	if (nStart > stringData.nDataLength)
		return -1;

	// find first matching substring
	LPTSTR lpsz = wcsstr(stringData.data + nStart, lpszSub);

	// return -1 for not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - stringData.data);
}


int CStr::FindNoCase(LPCTSTR lpszSub, int nStart) const
{
	if (nStart > stringData.nDataLength)
		return -1;

	CStr NoCaseSub;
	CStr NoCaseMStr;
	AllocCopy(NoCaseMStr,stringData.nDataLength,0,0);
	NoCaseSub = lpszSub;
	wcsupr((LPTSTR)(LPCTSTR)NoCaseMStr);
	wcsupr((LPTSTR)(LPCTSTR)NoCaseSub);

	LPTSTR lpsz = wcsstr((LPCTSTR)NoCaseMStr + nStart, NoCaseSub);

	// return -1 for not found, distance from beginning otherwise

	return (lpsz == NULL) ? -1 : (int)(lpsz - (LPCTSTR)NoCaseMStr);
}

// check beginning of string
bool CStr::StartsWith(LPCTSTR lpszSub) const
{
	int len = wcslen( lpszSub );

	if ( stringData.nDataLength < len )
		return false;

	return ( wcsncmp( stringData.data, lpszSub, len ) == 0 );
}

CStr CStr::Get(LPCTSTR lpszBegin, LPCTSTR lpszEnd,LPCTSTR lpszPrevSrch,bool InCluded){
	int be=0;
	CStr tStr = stringData.data;
	if (lpszPrevSrch){
		be = tStr.Find(lpszPrevSrch);
		if (be == -1) return L"";
		be += wcslen(lpszPrevSrch);
	}
	if (be<0) be = 0;
	return Get(lpszBegin,lpszEnd,be,InCluded);
}

CStr CStr::Get(LPCTSTR lpszBegin, LPCTSTR lpszEnd,int &be,bool InCluded){
	int ed,lb,le;
	CStr tStr = stringData.data;
	if ((be = tStr.Find(lpszBegin,be))!=-1){
		lb = wcslen(lpszBegin);
		if (lpszEnd)
		{
			if ((ed = tStr.Find(lpszEnd,be+lb))!=-1){
				le = wcslen(lpszEnd);
				int beo = be;
				be = ed;
				if (InCluded)
				{	
					return tStr.Mid(beo,ed-beo+le);
				}
				else
				{
					return tStr.Mid(beo+lb,ed-(beo+lb));
				}
			}
		}
		else
		{
			if (InCluded)
			{
				return tStr.Mid(be);
			}
			else
			{
				return tStr.Mid(be+lb);
			}
		}
	}
	return L"";
}

CStr CStr::Get(LPCTSTR lpszBegin, LPCTSTR lpszEnd,bool InCluded){
	return Get(lpszBegin,lpszEnd,NULL,InCluded);
}


//Get substr from str in no case
CStr CStr::GetNoCase(LPCTSTR lpszBegin, LPCTSTR lpszEnd,LPCTSTR lpszPrevSrch,bool InCluded){
	int be=0;
	CStr tStr(stringData.data);
	if (lpszPrevSrch) 
	{
		be = tStr.FindNoCase(lpszPrevSrch);
		if (be == -1) return L"";
		be += wcslen(lpszPrevSrch);
	}
	if (be<0) be = 0;
	return GetNoCase(lpszBegin,lpszEnd,be,InCluded);
}

//Get substr from str in no case
CStr CStr::GetNoCase(LPCTSTR lpszBegin, LPCTSTR lpszEnd,int &be,bool InCluded){
	int ed,lb,le;
	CStr tStr(stringData.data);
	if ((be = tStr.FindNoCase(lpszBegin,be))!=-1){
		lb = wcslen(lpszBegin);
		if ((ed = tStr.FindNoCase(lpszEnd,be+lb))!=-1){
			le = wcslen(lpszEnd);
			int beo = be;
			be = ed + le;
			if (InCluded)
			{
				return tStr.Mid(beo,ed-beo+le);
			}
			else
			{
				return tStr.Mid(beo+lb,ed-(beo+lb));
			}
		}
	}
	return L"";
}

CStr CStr::GetNoCase(LPCTSTR lpszBegin, LPCTSTR lpszEnd,bool InCluded){
	return GetNoCase(lpszBegin,lpszEnd,NULL,InCluded);
}

//extern Debug(LPCTSTR ,long);
// check ending of string
// check ending of string
bool CStr::EndsWith(LPCTSTR lpszSub) const
{
	int len = wcslen( lpszSub );
	int olen ;
	olen = stringData.nDataLength-1;
	if ( olen < len )
		return false;

	return ( wcsncmp(stringData.data+(olen-len),lpszSub,len)==0);
}

void CStr::MakeUpper()
{
    wcsupr( stringData.data );
}

void CStr::MakeLower()
{
    wcslwr( stringData.data );
}

void CStr::TrimRight(LPCTSTR lpszTargetList)
{
	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)

	LPTSTR lpsz = stringData.data;
	LPTSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (wcschr(lpszTargetList, *lpsz) != NULL)
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at left-most matching character  
		*lpszLast = '\0';
		stringData.nDataLength = lpszLast - stringData.data + 1;
	}
}

void CStr::TrimRight(TCHAR chTarget)
{
	// find beginning of trailing matches
	// by starting at beginning (DBCS aware)

	LPTSTR lpsz = stringData.data;
	LPTSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (*lpsz == chTarget)
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at left-most matching character  
		*lpszLast = '\0';
		stringData.nDataLength = lpszLast - stringData.data + 1;
	}
}

void CStr::TrimRight()
{
	// find beginning of trailing spaces by starting at beginning (DBCS aware)

	LPTSTR lpsz = stringData.data;
	LPTSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (iswspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at trailing space start
		*lpszLast = '\0';
		stringData.nDataLength = lpszLast - stringData.data + 1;
	}
}

void CStr::TrimLeft(LPCTSTR lpszTargets)
{
	// if we're not trimming anything, we're not doing any work
	if (wcslen(lpszTargets) == 0)
		return;

	LPCTSTR lpsz = stringData.data;

	while (*lpsz != '\0')
	{
		if (wcschr(lpszTargets, *lpsz) == NULL)
			break;
		lpsz = _tcsinc(lpsz);
	}

	if (lpsz != stringData.data)
	{
		// fix up data and length
		int nDataLength = stringData.nDataLength - (lpsz - stringData.data);
        LPTSTR newData = (LPTSTR)malloc( nDataLength*sizeof(TCHAR) );
		memcpy(newData, lpsz, (nDataLength)*sizeof(TCHAR));
        free( stringData.data );
        stringData.data = newData;
		stringData.nDataLength  = nDataLength;
        stringData.nAllocLength = nDataLength;
	}
}

void CStr::TrimLeft(TCHAR chTarget)
{
	// find first non-matching character
	
	LPCTSTR lpsz = stringData.data;

	while (chTarget == *lpsz)
		lpsz = _tcsinc(lpsz);

	if (lpsz != stringData.data)
	{
		// fix up data and length
		int nDataLength = stringData.nDataLength - (lpsz - stringData.data);
        LPTSTR newData = (LPTSTR)malloc( nDataLength*sizeof(TCHAR) );
		memcpy(newData, lpsz, (nDataLength)*sizeof(TCHAR));
        free( stringData.data );
        stringData.data = newData;
		stringData.nDataLength  = nDataLength;
        stringData.nAllocLength = nDataLength;
	}
}

void CStr::TrimLeft()
{
	// find first non-space character

	LPCTSTR lpsz = stringData.data;

	while (iswspace(*lpsz))
		lpsz = _tcsinc(lpsz);

	if (lpsz != stringData.data)
	{
		// fix up data and length
		int nDataLength = stringData.nDataLength - (lpsz - stringData.data);
        LPTSTR newData = (LPTSTR)malloc( nDataLength*sizeof(TCHAR) );
		memcpy(newData, lpsz, (nDataLength)*sizeof(TCHAR));
        free( stringData.data );
        stringData.data = newData;
		stringData.nDataLength  = nDataLength;
        stringData.nAllocLength = nDataLength;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Very simple sub-string extraction

CStr CStr::Mid(int nFirst) const
{
	return Mid(nFirst, stringData.nDataLength - nFirst - 1);
}

CStr CStr::Mid(int nFirst, int nCount) const
{
	// out-of-bounds requests return sensible things
	if (nFirst < 0)
		nFirst = 0;
	if (nCount < 0)
		nCount = 0;

	if (nFirst + nCount >= stringData.nDataLength)
		nCount = stringData.nDataLength - nFirst - 1;
	if (nFirst >= stringData.nDataLength)
		nCount = 0;

	// optimize case of returning entire string
	if (nFirst == 0 && nFirst + nCount == stringData.nDataLength)
		return *this;

	CStr dest;
	AllocCopy(dest, nCount, nFirst, 0);
	return dest;
}

CStr CStr::Right(int nCount) const
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= stringData.nDataLength-1)
		return *this;

	CStr dest;
	AllocCopy(dest, nCount, stringData.nDataLength-nCount-1, 0);
	return dest;
}

CStr CStr::Left(int nCount) const
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= stringData.nDataLength-1)
		return *this;

	CStr dest;
	AllocCopy(dest, nCount, 0, 0);
	return dest;
}

int CStr::Replace(TCHAR chOld, TCHAR chNew)
{
	int nCount = 0;

	// short-circuit the nop case
	if (chOld != chNew)
	{
		// otherwise modify each character that matches in the string
		LPTSTR psz = stringData.data;
		LPTSTR pszEnd = psz + stringData.nDataLength;
		while (psz < pszEnd)
		{
			// replace instances of the specified character only
			if (*psz == chOld)
			{
				*psz = chNew;
				nCount++;
			}
			psz = _tcsinc(psz);
		}
	}
	return nCount;
}

int CStr::Replace(LPCTSTR lpszOld, LPCTSTR lpszNew)
{
	// can't have empty or NULL lpszOld

	int nSourceLen = wcslen(lpszOld);
	if (nSourceLen == 0)
		return 0;
	int nReplacementLen = wcslen(lpszNew);

	// loop once to figure out the size of the result string
	int nCount = 0;
	LPTSTR lpszStart = stringData.data;
	LPTSTR lpszEnd = stringData.data + stringData.nDataLength;
	LPTSTR lpszTarget;
	while (lpszStart < lpszEnd)
	{
		while ((lpszTarget = wcsstr(lpszStart, lpszOld)) != NULL)
		{
			nCount++;
			lpszStart = lpszTarget + nSourceLen;
		}
		lpszStart += lstrlen(lpszStart) + 1;
	}

	// if any changes were made, make them
	if (nCount > 0)
	{
		// if the buffer is too small, just
		//   allocate a new buffer (slow but sure)
		int nOldLength = stringData.nDataLength;
		int nNewLength =  nOldLength + (nReplacementLen-nSourceLen)*nCount;
		if (stringData.nAllocLength < nNewLength)
		{
			LPTSTR pstr = stringData.data;
			stringData.data = (LPTSTR)malloc( nNewLength*sizeof(TCHAR) );
            stringData.nAllocLength = nNewLength;
			memcpy(stringData.data, pstr, nOldLength*sizeof(TCHAR));
			free( pstr );
		}
		// else, we just do it in-place

		lpszStart = stringData.data;
		lpszEnd = stringData.data + stringData.nDataLength;

		// loop again to actually do the work
		while (lpszStart < lpszEnd)
		{
			while ( (lpszTarget = wcsstr(lpszStart, lpszOld)) != NULL)
			{
				int nBalance = nOldLength - (lpszTarget - stringData.data + nSourceLen);
				memmove(lpszTarget + nReplacementLen, lpszTarget + nSourceLen, 
					nBalance * sizeof(TCHAR));
 				memcpy(lpszTarget, lpszNew, nReplacementLen*sizeof(TCHAR));
				lpszStart = lpszTarget + nReplacementLen;
				lpszStart[nBalance-1] = '\0';
				nOldLength += (nReplacementLen - nSourceLen);
			}
			lpszStart += lstrlen(lpszStart) + 1;
		}
		ASSERT(stringData.data[nNewLength-1] == '\0');
		stringData.nDataLength = nNewLength;
	}

	return nCount;
}

// formatting (using wsprintf style formatting)
void CStr::Format(LPCTSTR lpszFormat, ...)
{
    free( stringData.data );

    stringData.nAllocLength = wcslen(lpszFormat)+16384;
    stringData.data = (LPTSTR)malloc( stringData.nAllocLength * sizeof(TCHAR) );

	va_list argList;
	va_start(argList, lpszFormat);
    stringData.nDataLength = _vsnwprintf( stringData.data, stringData.nAllocLength, lpszFormat, argList ) + 1;
	va_end(argList);

    FreeExtra();
}

int CStr::CompareNoCase(LPCTSTR string) const
{
    return wcsicmp( stringData.data, string);
}

int CStr::Compare(LPCTSTR string) const
{
    return wcscmp( stringData.data, string);
}

bool operator==(const CStr& s1, const CStr& s2)
{
    return ( wcscmp( (LPCTSTR)s1, (LPCTSTR)s2) == 0 );
}

bool operator==(const CStr& s1, LPCTSTR s2)
{
    return ( wcscmp( (LPCTSTR)s1, s2) == 0 );
}

bool operator==(LPCTSTR s1, const CStr& s2)
{
    return ( wcscmp( s1, (LPCTSTR)s2) == 0 );
}

bool operator!=(const CStr& s1, const CStr& s2)
{
    return ( wcscmp( (LPCTSTR)s1, (LPCTSTR)s2) != 0 );
}

bool operator!=(const CStr& s1, LPCTSTR s2)
{
    return ( wcscmp( (LPCTSTR)s1, s2) != 0 );
}

bool operator!=(LPCTSTR s1, const CStr& s2)
{
    return ( wcscmp( s1, (LPCTSTR)s2) != 0 );
}


/*
void CStr::MakeReverse()
{
	_tcsrev( stringData.data );
}

void CStr::SetAt(int nIndex, TCHAR ch)
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < stringData.nDataLength);

	stringData.data[nIndex] = ch;
}

///////////////////////////////////////////////////////////////////////////////
// CStr conversion helpers (these use the current system locale)

int AFX_CDECL _wcstombsz(char* mbstr, const wchar_t* wcstr, size_t count)
{
	if (count == 0 && mbstr != NULL)
		return 0;

	int result = ::WideCharToMultiByte(CP_ACP, 0, wcstr, -1,
		mbstr, count, NULL, NULL);
	ASSERT(mbstr == NULL || result <= (int)count);
	if (result > 0)
		mbstr[result-1] = 0;
	return result;
}

int AFX_CDECL _mbstowcsz(wchar_t* wcstr, const char* mbstr, size_t count)
{
	if (count == 0 && wcstr != NULL)
		return 0;

	int result = ::MultiByteToWideChar(CP_ACP, 0, mbstr, -1,
		wcstr, count);
	ASSERT(wcstr == NULL || result <= (int)count);
	if (result > 0)
		wcstr[result-1] = 0;
	return result;
}

LPWSTR AFXAPI AfxA2WHelper(LPWSTR lpw, LPCSTR lpa, int nChars)
{
	if (lpa == NULL)
		return NULL;
	ASSERT(lpw != NULL);
	// verify that no illegal character present
	// since lpw was allocated based on the size of lpa
	// don't worry about the number of chars
	lpw[0] = '\0';
	VERIFY(MultiByteToWideChar(CP_ACP, 0, lpa, -1, lpw, nChars));
	return lpw;
}

LPSTR AFXAPI AfxW2AHelper(LPSTR lpa, LPCWSTR lpw, int nChars)
{
	if (lpw == NULL)
		return NULL;
	ASSERT(lpa != NULL);
	// verify that no illegal character present
	// since lpa was allocated based on the size of lpw
	// don't worry about the number of chars
	lpa[0] = '\0';
	VERIFY(WideCharToMultiByte(CP_ACP, 0, lpw, -1, lpa, nChars, NULL, NULL));
	return lpa;
}

//////////////////////////////////////////////////////////////////////////////
// Assignment operators

const CStr& CStr::operator=(TCHAR ch)
{
	AssignCopy(1, &ch);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// less common string expressions

CStr AFXAPI operator+(const CStr& string1, TCHAR ch)
{
	CStr s;
	s.ConcatCopy(string1.stringData.nDataLength, string1.stringData.data, 1, &ch);
	return s;
}

CStr AFXAPI operator+(TCHAR ch, const CStr& string)
{
	CStr s;
	s.ConcatCopy(1, &ch, string.stringData.nDataLength, string.stringData.data);
	return s;
}

//////////////////////////////////////////////////////////////////////////////
// Advanced manipulation

int CStr::Delete(int nIndex, int nCount )
{
	if (nIndex < 0)
		nIndex = 0;
	int nNewLength = stringData.nDataLength;

#if defined(_WIN32_WCE)
	if (nCount > nNewLength)
		nCount = nNewLength;
#endif

	if (nCount > 0 && nIndex < nNewLength)
	{
		CopyBeforeWrite();
		int nBytesToCopy = nNewLength - (nIndex + nCount) + 1;

		WCE_FCTN(memcpy)(stringData.data + nIndex,
			stringData.data + nIndex + nCount, nBytesToCopy * sizeof(TCHAR));
		stringData.nDataLength = nNewLength - nCount;
	}

	return nNewLength;
}

int CStr::Insert(int nIndex, TCHAR ch)
{
	CopyBeforeWrite();

	if (nIndex < 0)
		nIndex = 0;

	int nNewLength = stringData.nDataLength;
	if (nIndex > nNewLength)
		nIndex = nNewLength;
	nNewLength++;

	if (stringData.nAllocLength < nNewLength)
	{
		CStrData* pOldData = GetData();
		LPTSTR pstr = stringData.data;
		AllocBuffer(nNewLength);
		WCE_FCTN(memcpy)(stringData.data, pstr, (pOldData->nDataLength+1)*sizeof(TCHAR));
		CStr::Release(pOldData);
	}

	// move existing bytes down
	WCE_FCTN(memcpy)(stringData.data + nIndex + 1,
		stringData.data + nIndex, (nNewLength-nIndex)*sizeof(TCHAR));
	stringData.data[nIndex] = ch;
	stringData.nDataLength = nNewLength;

	return nNewLength;
}

int CStr::Insert(int nIndex, LPCTSTR pstr)
{
	if (nIndex < 0)
		nIndex = 0;

	int nInsertLength = SafeStrlen(pstr);
	int nNewLength = stringData.nDataLength;
	if (nInsertLength > 0)
	{
		CopyBeforeWrite();
		if (nIndex > nNewLength)
			nIndex = nNewLength;
		nNewLength += nInsertLength;

		if (stringData.nAllocLength < nNewLength)
		{
			CStrData* pOldData = GetData();
			LPTSTR pstr = stringData.data;
			AllocBuffer(nNewLength);
			WCE_FCTN(memcpy)(stringData.data, pstr, (pOldData->nDataLength+1)*sizeof(TCHAR));
			CStr::Release(pOldData);
		}

		// move existing bytes down
		WCE_FCTN(memcpy)(stringData.data + nIndex + nInsertLength,
			stringData.data + nIndex, 
			(nNewLength-nIndex-nInsertLength+1)*sizeof(TCHAR));
		WCE_FCTN(memcpy)(stringData.data + nIndex,
			pstr, nInsertLength*sizeof(TCHAR));
		stringData.nDataLength = nNewLength;
	}

	return nNewLength;
}

int CStr::Remove(TCHAR chRemove)
{
	CopyBeforeWrite();

	LPTSTR pstrSource = stringData.data;
	LPTSTR pstrDest = stringData.data;
	LPTSTR pstrEnd = stringData.data + stringData.nDataLength;

	while (pstrSource < pstrEnd)
	{
		if (*pstrSource != chRemove)
		{
			*pstrDest = *pstrSource;
			pstrDest = _tcsinc(pstrDest);
		}
		pstrSource = _tcsinc(pstrSource);
	}
	*pstrDest = '\0';
	int nCount = pstrSource - pstrDest;
	stringData.nDataLength -= nCount;

	return nCount;
}

// strspn equivalent
CStr CStr::SpanIncluding(LPCTSTR lpszCharSet) const
{
	ASSERT(AfxIsValidString(lpszCharSet));
	return Left(_tcsspn(stringData.data, lpszCharSet));
}

// strcspn equivalent
CStr CStr::SpanExcluding(LPCTSTR lpszCharSet) const
{
	ASSERT(AfxIsValidString(lpszCharSet));
	return Left(_tcscspn(stringData.data, lpszCharSet));
}

//////////////////////////////////////////////////////////////////////////////
// Finding


/////////////////////////////////////////////////////////////////////////////
// CStr formatting

#define TCHAR_ARG   TCHAR
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char

#if defined(_WIN32_WCE_CEPC) || defined(_WIN32_WCE_EMULATION)
	#define DOUBLE_ARG  _AFX_DOUBLE
#else // _WIN32_WCE
#ifdef _X86_
	#define DOUBLE_ARG  _AFX_DOUBLE
#else
	#define DOUBLE_ARG  double
#endif
#endif // _WIN32_WCE

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64		0x40000

void CStr::FormatV(LPCTSTR lpszFormat, va_list argList)
{
	ASSERT(AfxIsValidString(lpszFormat));

	va_list argListSave = argList;

	// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;
	for (LPCTSTR lpsz = lpszFormat; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz = _tcsinc(lpsz)) == '%')
		{
			nMaxLen += _tclen(lpsz);
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
		{
			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(argList, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
				*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = _ttoi(lpsz);
			for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
				;
		}
		ASSERT(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz = _tcsinc(lpsz);

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(argList, int);
				lpsz = _tcsinc(lpsz);
			}
			else
			{
				nPrecision = _ttoi(lpsz);
				for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
					;
			}
			ASSERT(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (_tcsncmp(lpsz, _T("I64"), 3) == 0)
		{
			lpsz += 3;
			nModifier = FORCE_INT64;
#if !defined(_WIN32_WCE)
#if !defined(_X86_) && !defined(_ALPHA_)
			// __int64 is only available on X86 and ALPHA platforms
			ASSERT(FALSE);
#endif
#endif // _WIN32_WCE
		}
		else
		{
			switch (*lpsz)
			{
			// modifiers that affect size
			case 'h':
				nModifier = FORCE_ANSI;
				lpsz = _tcsinc(lpsz);
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				lpsz = _tcsinc(lpsz);
				break;

			// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz = _tcsinc(lpsz);
				break;
			}
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
		// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, TCHAR_ARG);
			break;
		case 'c'|FORCE_ANSI:
		case 'C'|FORCE_ANSI:
			nItemLen = 2;
			va_arg(argList, CHAR_ARG);
			break;
		case 'c'|FORCE_UNICODE:
		case 'C'|FORCE_UNICODE:
			nItemLen = 2;
			va_arg(argList, WCHAR_ARG);
			break;

		// strings
		case 's':
			{
				LPCTSTR pstrNextArg = va_arg(argList, LPCTSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
				   nItemLen = lstrlen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 'S':
			{
#ifndef _UNICODE
				LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6;  // "(null)"
				else
				{
				   nItemLen = wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
#else
				LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = lstrlenA(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
#endif
			}
			break;

		case 's'|FORCE_ANSI:
		case 'S'|FORCE_ANSI:
			{
				LPCSTR pstrNextArg = va_arg(argList, LPCSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = lstrlenA(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 's'|FORCE_UNICODE:
		case 'S'|FORCE_UNICODE:
			{
				LPWSTR pstrNextArg = va_arg(argList, LPWSTR);
				if (pstrNextArg == NULL)
				   nItemLen = 6; // "(null)"
				else
				{
				   nItemLen = wcslen(pstrNextArg);
				   nItemLen = max(1, nItemLen);
				}
			}
			break;
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
			nItemLen = max(nItemLen, nWidth);
		}
		else
		{
			switch (*lpsz)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				if (nModifier & FORCE_INT64)
					va_arg(argList, __int64);
				else
					va_arg(argList, int);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'e':
			case 'g':
			case 'G':
				va_arg(argList, DOUBLE_ARG);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'f':
				{
					double f;
					LPTSTR pszTemp;

					// 312 == strlen("-1+(309 zeroes).")
					// 309 zeroes == max precision of a double
					// 6 == adjustment in case precision is not specified,
					//   which means that the precision defaults to 6
					pszTemp = (LPTSTR)_alloca(max(nWidth, 312+nPrecision+6));

					f = va_arg(argList, double);
					_stprintf( pszTemp, _T( "%*.*f" ), nWidth, nPrecision+6, f );
					nItemLen = _tcslen(pszTemp);
				}
				break;

			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			// no output
			case 'n':
				va_arg(argList, int*);
				break;

			default:
				ASSERT(FALSE);  // unknown formatting option
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}

	GetBuffer(nMaxLen);
	VERIFY(_vstprintf(stringData.data, lpszFormat, argListSave) <= GetAllocLength());
	ReleaseBuffer();

	va_end(argListSave);
}

void AFX_CDECL CStr::Format(UINT nFormatID, ...)
{
	CStr strFormat;
	VERIFY(strFormat.LoadString(nFormatID) != 0);

	va_list argList;
	va_start(argList, nFormatID);
	FormatV(strFormat, argList);
	va_end(argList);
}

#if !defined(_WIN32_WCE)
// formatting (using FormatMessage style formatting)
void AFX_CDECL CStr::FormatMessage(LPCTSTR lpszFormat, ...)
{
	// format message into temporary buffer lpszTemp
	va_list argList;
	va_start(argList, lpszFormat);
	LPTSTR lpszTemp;

	if (::FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		lpszFormat, 0, 0, (LPTSTR)&lpszTemp, 0, &argList) == 0 ||
		lpszTemp == NULL)
	{
		AfxThrowMemoryException();
	}

	// assign lpszTemp into the resulting string and free the temporary
	*this = lpszTemp;
	LocalFree(lpszTemp);
	va_end(argList);
}

void AFX_CDECL CStr::FormatMessage(UINT nFormatID, ...)
{
	// get format string from string table
	CStr strFormat;
	VERIFY(strFormat.LoadString(nFormatID) != 0);

	// format message into temporary buffer lpszTemp
	va_list argList;
	va_start(argList, nFormatID);
	LPTSTR lpszTemp;
	if (::FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		strFormat, 0, 0, (LPTSTR)&lpszTemp, 0, &argList) == 0 ||
		lpszTemp == NULL)
	{
		AfxThrowMemoryException();
	}

	// assign lpszTemp into the resulting string and free lpszTemp
	*this = lpszTemp;
	LocalFree(lpszTemp);
	va_end(argList);
}
#endif // _WIN32_WCE

///////////////////////////////////////////////////////////////////////////////
// CStr support for template collections

#if _MSC_VER >= 1100
template<> void AFXAPI ConstructElements<CStr> (CStr* pElements, int nCount)
#else
void AFXAPI ConstructElements(CStr* pElements, int nCount)
#endif
{
	ASSERT(nCount == 0 ||
		AfxIsValidAddress(pElements, nCount * sizeof(CStr)));

	for (; nCount--; ++pElements)
		WCE_FCTN(memcpy)(pElements, &afxEmptyString, sizeof(*pElements));
}

#if _MSC_VER >= 1100
template<> void AFXAPI DestructElements<CStr> (CStr* pElements, int nCount)
#else
void AFXAPI DestructElements(CStr* pElements, int nCount)
#endif
{
	ASSERT(nCount == 0 ||
		AfxIsValidAddress(pElements, nCount * sizeof(CStr)));

	for (; nCount--; ++pElements)
		pElements->~CStr();
}

#if _MSC_VER >= 1100
template<> void AFXAPI CopyElements<CStr> (CStr* pDest, const CStr* pSrc, int nCount)
#else
void AFXAPI CopyElements(CStr* pDest, const CStr* pSrc, int nCount)
#endif
{
	ASSERT(nCount == 0 ||
		AfxIsValidAddress(pDest, nCount * sizeof(CStr)));
	ASSERT(nCount == 0 ||
		AfxIsValidAddress(pSrc, nCount * sizeof(CStr)));

	for (; nCount--; ++pDest, ++pSrc)
		*pDest = *pSrc;
}

#ifndef OLE2ANSI
#if _MSC_VER >= 1100
template<> UINT AFXAPI HashKey<LPCWSTR> (LPCWSTR key)
#else
UINT AFXAPI HashKey(LPCWSTR key)
#endif
{
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}
#endif

#if _MSC_VER >= 1100
template<> UINT AFXAPI HashKey<LPCSTR> (LPCSTR key)
#else
UINT AFXAPI HashKey(LPCSTR key)
#endif
{
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}
*/
UINT HashKey(LPCTSTR key)
{
	UINT nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}
///////////////////////////////////////////////////////////////////////////////


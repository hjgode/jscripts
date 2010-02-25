#ifndef MORTSTRING_H
#define MORTSTRING_H

#include "mortafx.h"

//HashKey
UINT HashKey(LPCTSTR key);

struct CStrData
{
    int nDataLength;        // length of data (including terminator)
    int nAllocLength;       // length of allocation
    LPTSTR data;
};

class CStr
{
public:
// Constructors

    // constructs empty CStr
    CStr();
    // copy constructor
    CStr(const CStr& stringSrc);
    // from a UNICODE string (converts to TCHAR)
    CStr(LPCTSTR lpsz);

// Attributes & Operations

    // get data length
    int GetLength() const;
    // TRUE if zero length
    BOOL IsEmpty() const;
    // clear contents to empty
    void Empty();

	// return single character at zero-based index
    TCHAR GetAt(int nIndex) const;
    // return single character at zero-based index
    //TCHAR operator[](int nIndex) const;
    // set a single character at zero-based index
    // void SetAt(int nIndex, TCHAR ch);
    // return pointer to const string
    operator LPCTSTR() const;

    // overloaded assignment

    // ref-counted copy from another CStr
    const CStr& operator=(const CStr& stringSrc);
    // set string content to single character
    //const CStr& operator=(TCHAR ch);
    // copy string content from UNICODE string (converts to TCHAR)
    const CStr& operator=(LPCTSTR lpsz);

    // string concatenation

    // concatenate from another CStr
    const CStr& operator+=(const CStr& string);

    // concatenate a single character
    const CStr& operator+=(TCHAR ch);
    // concatenate a UNICODE character after converting it to TCHAR
    const CStr& operator+=(LPCTSTR lpsz);

    friend CStr operator+(const CStr& string1, const CStr& string2);
    friend CStr operator+(const CStr& string, TCHAR ch);
    friend CStr operator+(TCHAR ch, const CStr& string);
    friend CStr operator+(const CStr& string, LPCTSTR lpsz);
    friend CStr operator+(LPCTSTR lpsz, const CStr& string);

    // string comparison

    // straight character comparison
    int Compare(LPCTSTR lpsz) const;
    // compare ignoring case
    int CompareNoCase(LPCTSTR lpsz) const;

    // simple sub-string extraction

    // return nCount characters starting at zero-based nFirst
    CStr Mid(int nFirst, int nCount) const;
    // return all characters starting at zero-based nFirst
    CStr Mid(int nFirst) const;
    // return first nCount characters in string
    CStr Left(int nCount) const;
    // return nCount characters from end of string
    CStr Right(int nCount) const;

    // upper/lower/reverse conversion

    // NLS aware conversion to uppercase
    void MakeUpper();
    // NLS aware conversion to lowercase
    void MakeLower();

    // trimming whitespace (either side)

    // remove whitespace starting from right edge
    void TrimRight();
    // remove whitespace starting from left side
    void TrimLeft();

    // trimming anything (either side)

    // remove continuous occurrences of chTarget starting from right
    void TrimRight(TCHAR chTarget);
    // remove continuous occcurrences of characters in passed string,
    // starting from right
    void TrimRight(LPCTSTR lpszTargets);
    // remove continuous occurrences of chTarget starting from left
    void TrimLeft(TCHAR chTarget);
    // remove continuous occcurrences of characters in
    // passed string, starting from left
    void TrimLeft(LPCTSTR lpszTargets);

    // advanced manipulation

    // replace occurrences of chOld with chNew
    int Replace(TCHAR chOld, TCHAR chNew);
    // replace occurrences of substring lpszOld with lpszNew;
    // empty lpszNew removes instances of lpszOld
    int Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);
    // remove occurrences of chRemove
    int Remove(TCHAR chRemove);
    // insert character at zero-based index; concatenates
    // if index is past end of string
    //int Insert(int nIndex, TCHAR ch);
    // insert substring at zero-based index; concatenates
    // if index is past end of string
    //int Insert(int nIndex, LPCTSTR pstr);
    // delete nCount characters starting at zero-based index
    //int Delete(int nIndex, int nCount = 1);

    // searching

    // find character starting at left, -1 if not found
    int Find(TCHAR ch) const;
    // find character starting at right
    int ReverseFind(TCHAR ch, int Pos = 0) const;
    // find character starting at zero-based index and going right
    int Find(TCHAR ch, int nStart) const;
    // find first instance of any character in passed string
    int FindOneOf(LPCTSTR lpszCharSet) const;
    // find first instance of substring
    int Find(LPCTSTR lpszSub) const;
    // find first instance of substring starting at zero-based index
    int Find(LPCTSTR lpszSub, int nStart) const;

	int FindNoCase(LPCTSTR lpszSub, int nStart = 0) const;
    // check beginning of string
    bool StartsWith(LPCTSTR lpszSub) const;
	bool EndsWith(LPCTSTR lpszSub) const;
    // simple formatting

    // printf-like formatting using passed string
    void __cdecl Format(LPCTSTR lpszFormat, ...);
    // printf-like formatting using referenced string resource
    void __cdecl Format(UINT nFormatID, ...);
    // printf-like formatting using variable arguments parameter
    //void FormatV(LPCTSTR lpszFormat, va_list argList);

    // formatting for localization (uses FormatMessage API)

    // format using FormatMessage API on passed string
    //void AFX_CDECL FormatMessage(LPCTSTR lpszFormat, ...);
    // format using FormatMessage API on referenced string resource
    //void AFX_CDECL FormatMessage(UINT nFormatID, ...);

    // input and output
    //friend CArchive& AFXAPI operator<<(CArchive& ar, const CStr& string);
    //friend CArchive& AFXAPI operator>>(CArchive& ar, CStr& string);

    // Windows support
    BOOL LoadString(UINT nID);          // load from string resource
                                        // 255 chars max

    // Access to string implementation buffer as "C" character array

    // get pointer to modifiable buffer at least as long as nMinBufLength
    LPTSTR GetBuffer(int nMinBufLength);
    // release buffer, setting length to nNewLength (or to first nul if -1)
    void ReleaseBuffer(int nNewLength = -1);
    // get pointer to modifiable buffer exactly as long as nNewLength
    LPTSTR GetBufferSetLength(int nNewLength);
    // release memory allocated to but unused by string
    void FreeExtra();

// Implementation
public:
    ~CStr();
    int GetAllocLength() const;

protected:
    // implementation helpers
    CStrData stringData;
    void Init();
    void AllocCopy(CStr& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	/*
    void AllocBuffer(int nLen);
    */
    void AssignCopy(int nSrcLen, LPCTSTR lpszSrcData);
    void ConcatCopy(int nSrc1Len, LPCTSTR lpszSrc1Data, int nSrc2Len, LPCTSTR lpszSrc2Data);
    void ConcatInPlace(int nSrcLen, LPCTSTR lpszSrcData);
    /*
    void CopyBeforeWrite();
    void AllocBeforeWrite(int nLen);
    void Release();
    static void PASCAL Release(CStrData* pData);
    static int PASCAL SafeStrlen(LPCTSTR lpsz);
    static void FASTCALL FreeData(CStrData* pData);
	*/
};

// Compare helpers
bool operator==(const CStr& s1, const CStr& s2);
bool operator==(const CStr& s1, LPCTSTR s2);
bool operator==(LPCTSTR s1, const CStr& s2);
bool operator!=(const CStr& s1, const CStr& s2);
bool operator!=(const CStr& s1, LPCTSTR s2);
bool operator!=(LPCTSTR s1, const CStr& s2);

/*
bool AFXAPI operator<(const CStr& s1, const CStr& s2);
bool AFXAPI operator<(const CStr& s1, LPCTSTR s2);
bool AFXAPI operator<(LPCTSTR s1, const CStr& s2);
bool AFXAPI operator>(const CStr& s1, const CStr& s2);
bool AFXAPI operator>(const CStr& s1, LPCTSTR s2);
bool AFXAPI operator>(LPCTSTR s1, const CStr& s2);
bool AFXAPI operator<=(const CStr& s1, const CStr& s2);
bool AFXAPI operator<=(const CStr& s1, LPCTSTR s2);
bool AFXAPI operator<=(LPCTSTR s1, const CStr& s2);
bool AFXAPI operator>=(const CStr& s1, const CStr& s2);
bool AFXAPI operator>=(const CStr& s1, LPCTSTR s2);
bool AFXAPI operator>=(LPCTSTR s1, const CStr& s2);
*/
#endif
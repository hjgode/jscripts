#ifndef __INIFILE_H
#define __INIFILE_H

#include "morttypes.h"

#define CP_UNICODE                  65111       // 2 byte unicode
#define CP_UNICODE_PREFIX           65112       // 2 byte unicode with prefix
#define CP_UTF8_PREFIX              65115       // UTF-8 with prefix

class CIniFile
{
// -------------------------------------------------------------------
//                          Variablen protected
// -------------------------------------------------------------------
public:
    CMapStrToPtr    Sections;

// -------------------------------------------------------------------
//                          Konstruktor/Destruktor
// -------------------------------------------------------------------
public:
	CIniFile();
	~CIniFile();

// -------------------------------------------------------------------
//                          öffentliche Funktionen
// -------------------------------------------------------------------
public:
	static  int      Split( const CStr &source, LPCTSTR sep, CStrArray &dest, BOOL trim = TRUE );
	static	int		 SetComPort( LPCTSTR filename, HANDLE file );
    static  BOOL     ReadFile( LPCTSTR filename, CStr &content, int size = -1, int cp = CP_ACP );
	static  int	     ReadWebFile( LPCTSTR url, CStr &content, int size = -1, int cp = CP_ACP );
    static  BOOL     WriteFile( LPCTSTR filename, CStr &content, BOOL append=FALSE, UINT cp=CP_ACP );

            void     Parse( LPCTSTR cont );
	        BOOL     Read( LPCTSTR filename );

    BOOL     ExistsSection( LPCTSTR section );
    /*
	COLORREF GetColor   ( LPCTSTR section, LPCTSTR element, COLORREF def = CLR_NONE, LPCTSTR defElement = NULL );
	COLORREF StringToColor( const CString &string );
	BOOL     GetBoolean ( LPCTSTR section, LPCTSTR element, BOOL     def = FALSE,    LPCTSTR defElement = NULL );
	RECT     GetPosition( LPCTSTR section, LPCTSTR element, RECT    *def = NULL,     LPCTSTR defElement = NULL );
	RECT	 StringToPos( const CString &string );
    void     GetPoint   ( LPCTSTR section, LPCTSTR element, CPoint &point );
	long	 GetInt     ( LPCTSTR section, LPCTSTR element, long     def,            LPCTSTR defElement = NULL );
	CFont   *GetFont    ( LPCTSTR section, LPCTSTR element, HDC dc, LPCTSTR defFont = L"Tahoma", BOOL defBold = FALSE, BOOL defItalics = FALSE, int defSize = 11, short factor = 72 );
    */
	CStr  GetString  ( LPCTSTR section, LPCTSTR element, LPCTSTR  def = L"",      LPCTSTR defElement = NULL );
};

#endif

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

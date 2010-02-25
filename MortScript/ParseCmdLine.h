// ParseCmdLine.h: interface for the CParseCmdLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSECMDLINE_H__D9C83A82_2DA4_4826_86D3_58C8246EB1C3__INCLUDED_)
#define AFX_PARSECMDLINE_H__D9C83A82_2DA4_4826_86D3_58C8246EB1C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CParseCmdLine  
{
public:
	CParseCmdLine();
	virtual ~CParseCmdLine();

	void ParseCmdLine( LPCTSTR cmdLine );

protected:
	void ParseParam( CStr &option, CStr &value, BOOL withFlag, BOOL withValue );

public:
	int		WaitForFile;
	BOOL	RegOnly;
	CStr Filename;
};

#endif // !defined(AFX_PARSECMDLINE_H__D9C83A82_2DA4_4826_86D3_58C8246EB1C3__INCLUDED_)

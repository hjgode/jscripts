#if !defined(AFX_DLGSTATUS_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_)
#define AFX_DLGSTATUS_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_

// DlgChoice.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgChoice dialog

#define WM_EXIT_STATUS	    WM_USER + 11
#define WM_UPDATE_DISPLAY   WM_USER + 12
#define WM_ADD_STATUS_MSG   WM_USER + 13
#define WM_CLEAR_STATUS_MSG WM_USER + 14
#define WM_SET_STATUS_LINES WM_USER + 15
#define WM_DEL_STATUS_MSG   WM_USER + 16

#define STATUS_STYLE_INVISIBLE 0
#define STATUS_STYLE_LIST 1
#define STATUS_STYLE_MSG 2

class CDlgStatus
{
// Construction
public:
	CDlgStatus( LPCTSTR file );   // standard constructor

	enum { IDD = IDD_STATUS };
	HWND	m_Entries;
	CStr	m_Info;
    CStrArray m_Strings;
    CStr	m_Title;

	CStr ScriptFile;
	bool ShowCancel, KeepOpen, ScriptFinished;
	int DisplayStyle;
	int MaxEntries;

// Implementation
public:
	void SetStatusType( int type );
	void AddStatusMsg( LPCTSTR message );
	void AppendStatusMsg( LPCTSTR message );
	void DelStatusMsg();
	void SetStatusLines( int lines );
	void ClearStatusLines();

	int DoModal();
};

extern HWND		   StatusWindow;
extern CDlgStatus *StatusDialog;
extern HANDLE	   StatusDialogFinished;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHOICE_H__D9EE585E_6A4C_43C3_A43F_DCB8BA9CED2C__INCLUDED_)

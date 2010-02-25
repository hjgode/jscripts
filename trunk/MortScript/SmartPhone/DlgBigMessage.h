#if !defined(AFX_BigMessage_H__A2449193_451B_4DB0_BE71_9AA89895EFC2__INCLUDED_)
#define AFX_BigMessage_H__A2449193_451B_4DB0_BE71_9AA89895EFC2__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CDlgBigMessage dialog

class CDlgBigMessage
{
// Construction
public:
	CDlgBigMessage();   // standard constructor

// Dialog Data
    static CDlgBigMessage* CurrentObject;
	enum { IDD = IDD_BIGMESSAGE };
	CStr	m_Text;
    CStr Title;

// Implementation
public:
	int DoModal();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWAIT_H__A2449193_451B_4DB0_BE71_9AA89895EFC2__INCLUDED_)

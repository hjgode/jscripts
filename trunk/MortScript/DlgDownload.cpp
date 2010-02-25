// DlgDownload.cpp : implementation file
//

#include "stdafx.h"
#include "MortStarter.h"
#include "DlgDownload.h"
#include "Afxmt.h"
#ifdef DESKTOP
#include "wininet.h"
#endif

#include "morttypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CStr Proxy;

CEvent  DLCancel;

DWORD WINAPI DownloadThread( LPVOID lpParameter )
{
    CDlgDownload *dlg = (CDlgDownload*)lpParameter;
    HINTERNET web = NULL, file = NULL;
    HANDLE    outFile = NULL;
    void  *buffer = NULL;
    ULONG  fileSize=0, readSize;
    ULONG  bufSize;
    BOOL   rc     = 0;

    dlg->Status = L"Connecting...";
    dlg->Progress = 0;
    dlg->Total = 0;
    dlg->PostMessage( WM_COMMAND, IDC_INFO, 0 );
    if ( Proxy.IsEmpty() )
    {
#ifdef DESKTOP
		web  = InternetOpen( L"JScripts", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
#else
        web  = InternetOpen( L"JScripts", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
#endif
    }
    else
    {
		web  = InternetOpen( L"JScripts", INTERNET_OPEN_TYPE_PROXY, (LPCTSTR)Proxy, NULL, 0 );
    }

    if ( web == NULL )
	{
        int error = GetLastError();
		rc = -1;
	}

    if ( rc == 0 )
    {
        dlg->Status.Format( L"Opening '%s'...", dlg->Source );
        dlg->PostMessage( WM_COMMAND, IDC_INFO, 0 );
	    file = InternetOpenUrl( web, dlg->Source, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
	    if ( file == NULL )
	    {
            int error = GetLastError();
		    rc = -2;
	    }
    }

    if ( rc == 0 )
    {
        outFile = CreateFile( dlg->Target, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
        if ( outFile == INVALID_HANDLE_VALUE ) rc = -6;
    }

    if ( rc == 0 )
    {
	    if ( _tcsncmp( dlg->Source, L"http", 4 ) == 0 )
	    {
		    // No http error?
		    TCHAR status[MAX_PATH];
            DWORD length=MAX_PATH;
		    if ( HttpQueryInfo( file, HTTP_QUERY_STATUS_CODE, status, &length, NULL ) )
		    {
			    if ( _ttol( status ) != 200 )
			    {
				    rc = _ttol( status );
			    }
                else
                {
                    DWORD length=sizeof(fileSize);
                    if ( !HttpQueryInfo( file, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &fileSize, &length, NULL ) )
                    {
                        fileSize = 0;
                    }
                }
		    }
	    }
        else
        {
#ifdef DESKTOP
            fileSize = 0;
#else
            fileSize = FtpGetFileSize( file, &fileSize );
#endif
        }
    }

    if ( rc == 0 )
    {
        if ( fileSize != 0 && fileSize < 64 * 1024 )
        {
            bufSize = (fileSize+1);
        }
        else
        {
            bufSize = 64 * 1024;
        }

        buffer   = malloc( bufSize );
        if ( buffer == NULL )
	    {
		    rc = -4;
        }
    }

    if ( rc == 0 )
    {
        dlg->Status.Format( L"Downloading '%s'...", dlg->Source );
        dlg->Progress = 0;
        dlg->Total = fileSize;
        dlg->PostMessage( WM_COMMAND, IDC_INFO, 0 );
        DWORD dummy;
        DWORD blockSize = 0;
        do
        {
            if ( WaitForSingleObject( DLCancel, 0 ) == WAIT_OBJECT_0 )
            {
                rc = -7;
                blockSize = 0;
            }
            else
            {
                if ( ! InternetQueryDataAvailable( file, &blockSize, 0, 0 ) )
                {
                    rc = -5;
                    readSize = 0;
                    blockSize = 0;
                    break;
                }

                if ( blockSize > 0 )
                {
                    UINT blockRead = 0;
                    do
                    {
                        if ( WaitForSingleObject( DLCancel, 0 ) == WAIT_OBJECT_0 )
                        {
                            rc = -7;
                            readSize = 0;
                            blockSize = 0;
                            break;
                        }

                        int getSize = (bufSize < (blockSize-blockRead)) ? bufSize : (blockSize-blockRead);
                        int stat = InternetReadFile( file, buffer, getSize, &readSize );
                        dlg->Progress += readSize;
                        blockRead     += readSize;
                        if ( stat == 0 )
                        {
                            int error = GetLastError();
                            rc = -5;
                        }
                        else
                        {
                            dlg->PostMessage( WM_COMMAND, IDC_INFO, 0 );
                            if ( readSize > 0 )
                                WriteFile( outFile, buffer, readSize, &dummy, NULL );
                        }
                    }
                    while ( rc == 0 && readSize > 0 && blockRead < blockSize );
                }
            }
        }
        while ( rc == 0 && blockSize > 0 );
    }

    if ( outFile != NULL ) CloseHandle( outFile );
    if ( file   != NULL ) InternetCloseHandle( file );
    if ( web    != NULL ) InternetCloseHandle( web );
    if ( buffer != NULL ) free(buffer);

    dlg->PostMessage( WM_COMMAND, IDCANCEL, 0 );

    dlg->Result = rc;

    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDlgDownload dialog


CDlgDownload::CDlgDownload(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDownload::IDD, pParent)
{
#ifndef DESKTOP
    m_bFullScreen = FALSE;
#endif
	//{{AFX_DATA_INIT(CDlgDownload)
	m_Info = _T("");
	//}}AFX_DATA_INIT
}


void CDlgDownload::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDownload)
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Text(pDX, IDC_INFO, m_Info);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDownload, CDialog)
	//{{AFX_MSG_MAP(CDlgDownload)
	ON_BN_CLICKED(IDC_INFO, OnInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDownload message handlers

void CDlgDownload::OnCancel() 
{
    DLCancel.SetEvent();

    DWORD exitCode;
    while ( GetExitCodeThread( ThreadHandle, &exitCode ) && exitCode == STILL_ACTIVE )
        WaitForSingleObject( ThreadHandle, 100 );

	CloseHandle( ThreadHandle );

	CDialog::OnCancel();
}

BOOL CDlgDownload::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    Status = L"Connecting...";
    Progress = 0;
    Total = 0;

    DLCancel.ResetEvent();
    ThreadHandle = CreateThread( NULL, NULL, DownloadThread, this, 0, 0 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDownload::OnInfo() 
{
	m_Info = Status;
    if ( Total == 0 )
    {
        m_Progress.SetRange32( 0, 1 );
        m_Progress.SetPos( 0 );
        if ( Progress != 0 )
        {
            CString txt;
            txt.Format( L"%dkB of ?", int(Progress/1024) );
            m_Progress.SetWindowText( txt );
        }
    }
    else
    {
        m_Progress.SetRange32( 0, Total );
        m_Progress.SetPos( Progress );
        CString txt;
        txt.Format( L"%dkB of %dkB", int(Progress/1024), int(Total/1024) );
        m_Progress.SetWindowText( txt );
    }
	UpdateData( FALSE );
}

void CDlgDownload::OnOK() 
{
	// CDialog::OnOK();
}

// ConnectionState.h: interface for the CConnectionState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTIONSTATE_H__8A049A83_089D_4C68_85AE_5724D4B14C52__INCLUDED_)
#define AFX_CONNECTIONSTATE_H__8A049A83_089D_4C68_85AE_5724D4B14C52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Connection.h"
//
// Derived class
//
class CConnectionState : public CConnection
{
public:
    typedef CConnection BASECLASS ;
    CConnectionState() {};
    ~CConnectionState(){};
    void SetOutputWindow(HWND hWnd) { m_hWnd=hWnd; }

    HRESULT IsAvailable( LPCTSTR pszPath=NULL, BOOL bProxy=FALSE )
    {
        HRESULT hr=BASECLASS::IsAvailable(pszPath, bProxy);
        if( SUCCEEDED( hr ))
        {
            if( hr == S_OK )
            {
                AddOutput(TEXT("We can quickly connect to that path.\r\n"));
            }
            else
                AddOutput(TEXT("No fast connection to that path.\r\n"));
        }
        else
        {
            AddOutput(TEXT("Unable using current configuration"));
        }
        return hr;
    }

    // Demonstrate how to override the notification methods.
protected:
    //
    // Called when we 
    //
    virtual HRESULT DoEstablishingConnection()
    {
        AddOutput(TEXT("\r\nEstablishing the connection\r\n %s, %s proxy\r\n"), GetNetworkPath(), GetProxy()?TEXT("using"):TEXT("without"));
        return BASECLASS::DoEstablishingConnection();
    }

    //
    // Called when there was an error while connecting
    // generally due to network connection not being available (no modem, no nic etc).
    //
    virtual HRESULT DoConnectingError()
    {
        if( SUCCEEDED( GetStatus() ))
            AddOutput(TEXT("Encountered state : %d\r\n"), GetStatus() );
        else
            AddOutput(TEXT("Encountered Error 0x%08x\r\n"), GetStatus() );
        return BASECLASS::DoConnectingError();
    }

    //
    // Called when a connection is now available.
    //
    virtual HRESULT DoConnected()
    {
        HRESULT hr=BASECLASS::DoConnected();
        if( IsProxyRequired() )
        {
            AddOutput(TEXT("Connected to network via proxy: %s\r\n"), GetProxyServer() );
        }
        else
        {
            AddOutput(TEXT("Connected directly to the network\r\n") );
        }
        return hr;
    }

    //
    // Called when the existing connection has been disconnected
    // by another network request.
    //
    virtual HRESULT DoDisconnected()
    {
        AddOutput(TEXT("We are disconnected\r\n") );
        return BASECLASS::DoDisconnected();
    }

    //
    // Called when we are waiting for the network to become available.
    //
    virtual HRESULT DoWaitingForConnection()
    {
        AddOutput(TEXT("We are waiting for the connection\r\n") );
        return BASECLASS::DoWaitingForConnection();
    }

    //
    // Called when we are waiting for the network to become available.
    //
    virtual HRESULT DoReleaseConnection()
    {
        AddOutput(TEXT("Relesed connection\r\n") );
        return BASECLASS::DoReleaseConnection();
    }

private:
    HWND    m_hWnd;

    //
    // Simple output window
    //
    void FAR _cdecl 
    AddOutputRaw(
                 LPTSTR text )
    {
        SendMessage(m_hWnd, EM_SETSEL, -1, -1);
        SendMessage(m_hWnd, EM_REPLACESEL, 0, (LPARAM)text );
    }

    void FAR _cdecl 
    AddOutput(
              LPTSTR lpszFormat,   
              ...) 
    {
        TCHAR szBuf[0x1000];
        int	cchAdd;
    
        cchAdd = wvsprintf((LPTSTR)szBuf, lpszFormat, (LPSTR)(&lpszFormat + 1));
    
        AddOutputRaw(szBuf);
    }

};

#endif // !defined(AFX_CONNECTIONSTATE_H__8A049A83_089D_4C68_85AE_5724D4B14C52__INCLUDED_)

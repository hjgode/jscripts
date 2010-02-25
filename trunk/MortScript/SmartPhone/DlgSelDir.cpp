#include "windows.h"
#include <windowsx.h>
#include <commctrl.h>
#ifndef PNA
#include <aygshell.h>
#endif
#include "mortafx.h"
#include "..\helpers.h"
#include "resource.h"
#include "DlgSelDir.h"

extern HINSTANCE g_hInst;  // Local copy of hInstance

CDlgSelDir* CDlgSelDir::CurrentObject = NULL;

#ifndef PNA
WNDPROC DirOldTreeViewProc;
LRESULT CALLBACK DirTreeViewProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_GETDLGCODE:
		return DLGC_WANTMESSAGE;

	case WM_KEYUP:
		{
			if ( wParam == 37 || wParam == 39 || wParam == 13 )
			{
				HTREEITEM currentItem = TreeView_GetSelection( CDlgSelDir::CurrentObject->m_Tree );
				if ( currentItem != NULL )
				{
					switch ( wParam )
					{
					case 39: // right
						TreeView_Expand( CDlgSelDir::CurrentObject->m_Tree, currentItem, TVE_EXPAND ); 
						break;
					case 37: // left
						TreeView_Expand( CDlgSelDir::CurrentObject->m_Tree, currentItem, TVE_COLLAPSE ); 
						break;
					case 13: // return
						TreeView_Expand( CDlgSelDir::CurrentObject->m_Tree, currentItem, TVE_TOGGLE ); 
						break;
					}
				}
			}
			return 0;
		}
	}
	return CallWindowProc( DirOldTreeViewProc, hwnd, uMsg, wParam, lParam );
}
#endif

BOOL CALLBACK selDirDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    HWND text    = GetDlgItem( hwndDlg, IDC_LABEL );
    HWND ok      = GetDlgItem( hwndDlg, IDOK );
	HWND cancel  = GetDlgItem( hwndDlg, IDCANCEL );

    switch ( uMsg )
    {
    case WM_INITDIALOG:
        {
			CDlgSelDir::CurrentObject->m_Tree    = GetDlgItem( hwndDlg, IDC_TREE );
            SendMessage (text, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgSelDir::CurrentObject->m_Info ); 
            SendMessage (hwndDlg, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgSelDir::CurrentObject->m_Title ); 

#ifndef PNA
			DirOldTreeViewProc = (WNDPROC)GetWindowLong( CDlgSelDir::CurrentObject->m_Tree, GWL_WNDPROC );
			SetWindowLong( CDlgSelDir::CurrentObject->m_Tree, GWL_WNDPROC, (long)DirTreeViewProc );
#endif

			int width  = GetSystemMetrics(SM_CXSCREEN);
			int height = GetSystemMetrics(SM_CYSCREEN);
			if ( width > 320 && height > 320 )
			{
				CDlgSelDir::CurrentObject->m_ImageList = ImageList_LoadBitmap( g_hInst, MAKEINTRESOURCE(IDB_DIR_ICONS_BIG), 34, 1, RGB( 255, 255, 255 ) );
			}
			else
			{
				CDlgSelDir::CurrentObject->m_ImageList = ImageList_LoadBitmap( g_hInst, MAKEINTRESOURCE(IDB_DIR_ICONS), 17, 1, RGB( 255, 255, 255 ) );
			}
			TreeView_SetImageList( CDlgSelDir::CurrentObject->m_Tree, CDlgSelDir::CurrentObject->m_ImageList, TVSIL_NORMAL );

			HTREEITEM rootItem	= CDlgSelDir::CurrentObject->InsertItem( TVI_ROOT, L"\\", 1, 1 );
			CDlgSelDir::CurrentObject->PopulateTree( _T(""), rootItem, CDlgSelDir::CurrentObject->m_Selected );
			TreeView_Expand( CDlgSelDir::CurrentObject->m_Tree, rootItem, TVE_EXPAND ); 
			CDlgSelDir::CurrentObject->m_Selected.Empty();

#ifdef PNA
			RECT rect;
			::SystemParametersInfo( SPI_GETWORKAREA, 0, &rect, 0 );
			MoveWindow( hwndDlg, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
#else
            SHMENUBARINFO info;
            info.cbSize = sizeof(info);
            info.hwndParent = hwndDlg;
			info.dwFlags = 0; 
            info.nToolBarId = IDR_OKCANCEL;
            info.hInstRes = g_hInst;
            info.nBmpId = 0;
            info.cBmpImages = 0;
            SHCreateMenuBar(&info); 

			SHINITDLGINFO shidi;
			memset(&shidi, 0, sizeof(SHINITDLGINFO));
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hwndDlg;
			SHInitDialog(&shidi);

			ShowWindow( ok, SW_HIDE );
			ShowWindow( cancel, SW_HIDE );
#endif

			SetForegroundWindow( hwndDlg );
            SetFocus( CDlgSelDir::CurrentObject->m_Tree );

            return TRUE;
        }
		break;

    case WM_COMMAND:
        if ( wParam == IDOK )
        {
            CDlgSelDir::CurrentObject->m_Selected = CDlgSelDir::CurrentObject->GetItemPath(NULL);
            EndDialog( hwndDlg, IDOK );
            //return TRUE;
        }
        if ( wParam == IDCANCEL )
        {
            CDlgSelDir::CurrentObject->m_Selected = L""; 
            EndDialog( hwndDlg, IDCANCEL );
            //return TRUE;
        }
		/*
        if ( HIWORD(wParam) == LBN_SELCHANGE )
        {
	        if ( CDlgSelDir::CurrentObject->m_Timeout > 0 )
	        {
		        KillTimer( hwndDlg, 1 );
		        CDlgSelDir::CurrentObject->countdown = CDlgSelDir::CurrentObject->m_Timeout;
		        CDlgSelDir::CurrentObject->m_Countdown.Format( L"%d", CDlgSelDir::CurrentObject->countdown );
                SendMessage (count, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgSelDir::CurrentObject->m_Countdown ); 
                SetTimer( hwndDlg, 1, 1000, NULL );
	        }
            return TRUE;
        }
        if ( HIWORD(wParam) == LBN_DBLCLK )
        {
            CDlgSelDir::CurrentObject->m_Selected = SendMessage (listbox, LB_GETCURSEL, 0, 0 ) + 1; 
            if ( CDlgSelDir::CurrentObject->m_Selected > 0 ) EndDialog( hwndDlg, IDOK );
            return TRUE;
        }
		*/
		return TRUE; //jwz::
		break;

	case WM_NOTIFY:
		{
			LPNMHDR nmhdr = (LPNMHDR)lParam;
			if ( nmhdr->code == NM_RETURN && nmhdr->hwndFrom != CDlgSelDir::CurrentObject->m_Tree && ::GetParent( nmhdr->hwndFrom ) == hwndDlg )
			{
				PostMessage( hwndDlg, WM_COMMAND, IDOK, NULL );
			}
			if ( nmhdr->code == NM_RETURN && nmhdr->hwndFrom == CDlgSelDir::CurrentObject->m_Tree )
			{
				HTREEITEM selItem = TreeView_GetSelection( CDlgSelDir::CurrentObject->m_Tree );
				if ( selItem != NULL )
					TreeView_Expand( CDlgSelDir::CurrentObject->m_Tree, selItem, TVE_TOGGLE );
			}

			if ( nmhdr->code == TVN_ITEMEXPANDING )
			{
				NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW*)lParam;

 				if( pNMTreeView->action == 2 )
				{
					//CStr msg;
					//msg.Format( L"old: %d/%d new: %d/%d", pNMTreeView->itemOld.hItem, pNMTreeView->itemOld.iImage, pNMTreeView->itemNew.hItem, pNMTreeView->itemNew.iImage );
					//MessageBox( NULL, msg, L"Debug", MB_SETFOREGROUND );

					// Update location display
					CStr sPath = CDlgSelDir::CurrentObject->GetItemPath( pNMTreeView->itemNew.hItem );
					// Refresh children
					if( TreeView_GetChild( CDlgSelDir::CurrentObject->m_Tree, pNMTreeView->itemNew.hItem ) == NULL )
					{
						if ( pNMTreeView->itemNew.iImage <= 3 || pNMTreeView->itemNew.lParam != NULL )
						{
							// Directories
    						CDlgSelDir::CurrentObject->PopulateTree( sPath, pNMTreeView->itemNew.hItem, L"" );
						}

						if( TreeView_GetSelection(CDlgSelDir::CurrentObject->m_Tree) != pNMTreeView->itemNew.hItem )
							TreeView_Select( CDlgSelDir::CurrentObject->m_Tree, pNMTreeView->itemNew.hItem, TVGN_CARET );
					}
					if( pNMTreeView->itemNew.iImage == 1 && TreeView_GetChild( CDlgSelDir::CurrentObject->m_Tree, pNMTreeView->itemNew.hItem ) != NULL )
					{
						pNMTreeView->itemNew.iImage = 0;
						pNMTreeView->itemNew.iSelectedImage = 0;
						pNMTreeView->itemNew.mask = TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
						TreeView_SetItem( CDlgSelDir::CurrentObject->m_Tree, &pNMTreeView->itemNew );
						//InvalidateRect( CDlgSelDir::CurrentObject->m_Tree, NULL, FALSE );
					}
				}
				else
				{
					if( pNMTreeView->itemNew.iImage == 0 )
					{
						//MessageBox( NULL, L"close folder", L"Debug", MB_SETFOREGROUND );
						pNMTreeView->itemNew.iImage = 1;
						pNMTreeView->itemNew.iSelectedImage = 1;
						pNMTreeView->itemNew.mask = TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
						TreeView_SetItem( CDlgSelDir::CurrentObject->m_Tree, &pNMTreeView->itemNew );
						//InvalidateRect( CDlgSelDir::CurrentObject->m_Tree, NULL, FALSE );
					}
				}
			}
		}
        return TRUE;
		break;

	case WM_SIZE:
		{
			if ( wParam == SIZE_RESTORED )
			{
				int   cx = LOWORD( lParam );
				int   cy = HIWORD( lParam );
				// Minimum smartphone screen size - cause some devices deliver 0x0... ^^
				if ( cx < 176 ) cx = 176;
				if ( cy < 168 ) cy = 168;

				RECT  rect, rect2;
				int   infoBottom;//,width, height;
				int   res = (cx>320)?2:1;

#ifdef PNA
				cy = cy - 26 * res;
#endif

				GetWindowRect( text, &rect );
				ScreenToClient( hwndDlg, &rect );
				rect.left = 4;
				rect.right = cx-4;
				rect.top = 4;

				if ( CDlgSelDir::CurrentObject->m_Info.IsEmpty() )
				{
					ShowWindow( text, SW_HIDE );
					infoBottom = 0;
				}
				else
				{
					HDC dc = ::GetWindowDC( text );
					CStrArray lines;
					SplitLines( dc, (LPCTSTR)CDlgSelDir::CurrentObject->m_Info, lines, rect.right-rect.left );
					CStr infoWrapped;
					for ( int i=0; i<lines.GetSize(); i++ )
					{
						infoWrapped += lines[i];
						if ( i<lines.GetSize()-1 ) infoWrapped += L"\n";
					}
					SetWindowText( text, infoWrapped );

					SIZE size;
					GetTextExtentPoint( dc, L"W", 1, &size );
					infoBottom = rect.bottom = rect.top + size.cy * lines.GetSize();
					::ReleaseDC( text, dc );
					MoveWindow( text, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
				}

				rect2.top = infoBottom + 4;
				rect2.left = 4;
				rect2.right = cx - 4;
				rect2.bottom = cy - 4;
				MoveWindow( CDlgSelDir::CurrentObject->m_Tree, rect2.left, rect2.top, rect2.right-rect2.left, rect2.bottom-rect2.top, TRUE );

#ifdef PNA
				rect.top = cy;
				rect.left = 2*res;
				rect.right = (cx/2) - 2*res;
				rect.bottom = cy + 24*res;
				MoveWindow( ok, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

				rect.left = (cx/2) + 2*res;
				rect.right = cx - 2*res;
				MoveWindow( cancel, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
#endif
			}
		}
		return TRUE;
		break;

    default:
        return FALSE;
    }
}


CDlgSelDir::CDlgSelDir()
{
	m_Info = _T("");
}

BOOL CDlgSelDir::PopulateTree( CStr path, HTREEITEM hParent, CStr selDir ) 
{
	BOOL bGotChildren = FALSE;						//True if a child is added.	
	
	CStr searchString = path + L"\\*";
	CStr filename;
	WIN32_FIND_DATA  findFileData;

	HANDLE ffh = FindFirstFile( searchString, &findFileData );
	if ( ffh != INVALID_HANDLE_VALUE )
	{
		if ( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
		    bGotChildren = TRUE;
			HTREEITEM treeItem	= InsertItem( hParent, findFileData.cFileName, 1, 1 );
		}

		while ( FindNextFile( ffh, &findFileData ) == TRUE )
		{
			if ( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
			{
				bGotChildren = TRUE;
				HTREEITEM treeItem	= InsertItem( hParent, findFileData.cFileName, 1, 1 );
			}
		}
		FindClose( ffh );
	}

	TreeView_SortChildren( m_Tree, hParent, 0 );

	return bGotChildren;
}


HTREEITEM CDlgSelDir::InsertItem( HTREEITEM hParent, LPCTSTR sText, int iImage, int children, DWORD itemData )
{
	TVINSERTSTRUCT InsertStruct;
	InsertStruct.hParent		= hParent;	
	InsertStruct.hInsertAfter	= TVI_LAST;
	InsertStruct.item.mask	    = TVIF_IMAGE | TVIF_TEXT | TVIF_CHILDREN | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	InsertStruct.item.pszText	= (LPTSTR)sText;
	InsertStruct.item.iImage	= iImage;
	InsertStruct.item.iSelectedImage = iImage;
	InsertStruct.item.cChildren = children;
	InsertStruct.item.lParam    = itemData;
	HTREEITEM item = TreeView_InsertItem( m_Tree, &InsertStruct );
    if ( !m_Selected.IsEmpty() )
    {
        CStr path = GetItemPath( item );
        if ( !path.IsEmpty() && m_Selected.Left( path.GetLength() ).CompareNoCase( path ) == 0 )
        {
            TreeView_Select( m_Tree, item, TVGN_CARET );
            PopulateTree( path, item, L"" );
        }
    }

	return item;
}

CStr CDlgSelDir::GetItemPath( HTREEITEM hItem ) 
{
	CStr sRet;

	TVITEM item;
	TCHAR  text[MAX_PATH];

    if ( hItem == NULL )
    {
        item.hItem = TreeView_GetSelection( m_Tree );
        if ( item.hItem == NULL ) return sRet;
    }
	else
		item.hItem = hItem;

	item.mask = TVIF_IMAGE|TVIF_TEXT|TVIF_PARAM;
	item.pszText = text;
	item.cchTextMax = MAX_PATH;
	TreeView_GetItem( m_Tree, &item );

    if ( item.iImage <= 3 )
    {
        //if ( item.lParam != NULL )
        //{
		//	MessageBox( NULL, L"lParam != NULL", L"Debug", MB_SETFOREGROUND );
            //LPNETRESOURCE lpnr = (LPNETRESOURCE)GetItemData( hItem );
            //sRet = lpnr->lpRemoteName;
        //}
        //else
        //{
			CStr textstring = item.pszText;
 	        do
	        {
		        //Add the directory name to the path.
		        if ( textstring != _T("\\") )
		        {
			        sRet = textstring + _T("\\")+ sRet; 
		        }
		        item.hItem = TreeView_GetParent( m_Tree, item.hItem );
				if ( item.hItem != NULL )
				{
					item.mask = TVIF_TEXT;
					item.pszText = text;
					item.cchTextMax = MAX_PATH;
					TreeView_GetItem( m_Tree, &item );
					textstring = item.pszText;
				}
	        } while( item.hItem != NULL && sRet.Left(2) != L"\\\\" );

            if ( sRet.Left(2) != L"\\\\" )
	            sRet = _T("\\") + sRet;
			if ( sRet.GetLength() > 1 )
		        sRet = sRet.Left( sRet.GetLength() - 1 );
        //}
    }

	return sRet;
}



int CDlgSelDir::DoModal()
{
    CurrentObject = this;
	INITCOMMONCONTROLSEX iccs;
	iccs.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccs.dwICC = ICC_TREEVIEW_CLASSES|ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&iccs);
    int result = DialogBox( g_hInst, MAKEINTRESOURCE( IDD ), NULL, selDirDialogProc );
	// int e = GetLastError();
	// CStr errMsg;
	// errMsg.Format( L"Fehler %d, IDD: %d, Inst.: %d", e, IDD, g_hInst );
	// MessageBox( NULL, errMsg, L"Debug", MB_OK );
	return result;
}

// end SPDlgTest.cpp

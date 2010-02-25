#include "windows.h"
#include <windowsx.h>
#include <commctrl.h>
#ifndef PNA
#include <aygshell.h>
#endif
#include "mortafx.h"
#include "..\helpers.h"
#include "resource.h"
#include "DlgSelFile.h"

extern HINSTANCE g_hInst;  // Local copy of hInstance
#define WM_SELFILE WM_USER+1

CDlgSelFile* CDlgSelFile::CurrentObject = NULL;

#ifndef PNA
WNDPROC FileOldTreeViewProc;
LRESULT CALLBACK FileTreeViewProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_GETDLGCODE:
		return DLGC_WANTMESSAGE;

	case WM_KEYUP:
		{
			if ( wParam == 37 || wParam == 39 || wParam == 13 )
			{
				HTREEITEM currentItem = TreeView_GetSelection( CDlgSelFile::CurrentObject->m_Tree );
				if ( currentItem != NULL )
				{
					int newCtrl = 0;
					switch ( wParam )
					{
					case 39: // right
						newCtrl = CDlgSelFile::CurrentObject->Save ? IDC_FILE : IDC_FILES;
						//TreeView_Expand( CDlgSelFile::CurrentObject->m_Tree, currentItem, TVE_EXPAND ); 
						break;
					case 37: // left
						newCtrl = IDC_FILES;
						// TreeView_Expand( CDlgSelFile::CurrentObject->m_Tree, currentItem, TVE_COLLAPSE ); 
						break;
					case 13: // return
						// int newCtrl = CDlgSelFile::CurrentObject->Save ? IDC_FILE : IDC_FILES;
						TreeView_Expand( CDlgSelFile::CurrentObject->m_Tree, currentItem, TVE_TOGGLE ); 
						break;
					}
					if ( newCtrl != 0 )
					{
						
						HWND ctrlWnd = GetDlgItem( GetParent( hwnd ), newCtrl );
						if ( newCtrl == IDC_FILES )
						{
							int sel = SendMessage( ctrlWnd, LB_GETCURSEL, 0, 0 );
							if ( sel < 0 ){
								SendMessage( ctrlWnd, LB_SETCURSEL, 0, 0 );
								//jwz:add for select only one file. begin
								int len = SendMessage( ctrlWnd, LB_GETTEXTLEN, 0, 0 );
								
								if (len>0){
									TCHAR *selFile = new TCHAR[len+1];
									SendMessage( ctrlWnd, LB_GETTEXT, 0, (LPARAM) selFile );
									//SendMessage( file, WM_SETTEXT, 0, (LPARAM) tmpFile );
									CDlgSelFile::CurrentObject->m_Selected = CDlgSelFile::CurrentObject->Dir;
									if ( CDlgSelFile::CurrentObject->m_Selected.Right(1) != L"\\" )
										CDlgSelFile::CurrentObject->m_Selected += L"\\";
									CDlgSelFile::CurrentObject->m_Selected += selFile;
									delete[] selFile;
								}
								//jwz:add end.
							}
						}
			            SetFocus( ctrlWnd );
					}
				}
				return 0;
			}
		}
	}
	return CallWindowProc( FileOldTreeViewProc, hwnd, uMsg, wParam, lParam );
}

WNDPROC FileOldListViewProc;
LRESULT CALLBACK FileListViewProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
	case WM_GETDLGCODE:
		return DLGC_WANTMESSAGE;

	case WM_KEYUP:
		{
			if ( wParam == 37 || wParam == 39 || wParam == 13 )
			{
				CDlgSelFile::CurrentObject->m_Selected = L""; //jwz:add for select only one file.
				HTREEITEM currentItem = TreeView_GetSelection( CDlgSelFile::CurrentObject->m_Tree );
				if ( currentItem != NULL )
				{
					switch ( wParam )
					{
					case 39: // right
			            SetFocus( GetDlgItem( GetParent( hwnd ), IDC_TREE ) );
						break;
					case 37: // left
						{
						int newCtrl = CDlgSelFile::CurrentObject->Save ? IDC_FILE : IDC_TREE;
			            SetFocus( GetDlgItem( GetParent( hwnd ), newCtrl ) );
						}
						break;
					case 13: // return
			            SetFocus( GetDlgItem( GetParent( hwnd ), IDC_TREE ) );
						// TreeView_Expand( CDlgSelDir::CurrentObject->m_Tree, currentItem, TVE_TOGGLE ); 
						break;
					}
				}
			}
		}
	}
	return CallWindowProc( FileOldListViewProc, hwnd, uMsg, wParam, lParam );
}
#endif

BOOL CALLBACK selFileDialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    HWND text    = GetDlgItem( hwndDlg, IDC_LABEL );
    HWND file    = GetDlgItem( hwndDlg, IDC_FILE );
    HWND files   = GetDlgItem( hwndDlg, IDC_FILES );
    HWND ok      = GetDlgItem( hwndDlg, IDOK );
	HWND cancel  = GetDlgItem( hwndDlg, IDCANCEL );

    switch ( uMsg )
    {
    case WM_INITDIALOG:
        {
			CDlgSelFile::CurrentObject->m_Tree    = GetDlgItem( hwndDlg, IDC_TREE );
            SendMessage (text, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgSelFile::CurrentObject->m_Info ); 
            SendMessage (hwndDlg, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) CDlgSelFile::CurrentObject->m_Title ); 

#ifndef PNA
			FileOldTreeViewProc = (WNDPROC)GetWindowLong( CDlgSelFile::CurrentObject->m_Tree, GWL_WNDPROC );
			SetWindowLong( CDlgSelFile::CurrentObject->m_Tree, GWL_WNDPROC, (long)FileTreeViewProc );
			FileOldListViewProc = (WNDPROC)GetWindowLong( files, GWL_WNDPROC );
			SetWindowLong( files, GWL_WNDPROC, (long)FileListViewProc );
#endif

			int width  = GetSystemMetrics(SM_CXSCREEN);
			int height = GetSystemMetrics(SM_CYSCREEN);
			if ( width > 320 && height > 320 )
			{
				CDlgSelFile::CurrentObject->m_ImageList = ImageList_LoadBitmap( g_hInst, MAKEINTRESOURCE(IDB_DIR_ICONS_BIG), 34, 1, RGB( 255, 255, 255 ) );
			}
			else
			{
				CDlgSelFile::CurrentObject->m_ImageList = ImageList_LoadBitmap( g_hInst, MAKEINTRESOURCE(IDB_DIR_ICONS), 17, 1, RGB( 255, 255, 255 ) );
			}
			TreeView_SetImageList( CDlgSelFile::CurrentObject->m_Tree, CDlgSelFile::CurrentObject->m_ImageList, TVSIL_NORMAL );

			HTREEITEM rootItem	= CDlgSelFile::CurrentObject->InsertItem( TVI_ROOT, L"\\", 1, 1 );
			CDlgSelFile::CurrentObject->PopulateTree( _T(""), rootItem, CDlgSelFile::CurrentObject->m_Selected );
			TreeView_Expand( CDlgSelFile::CurrentObject->m_Tree, rootItem, TVE_EXPAND );
			PostMessage( hwndDlg, WM_SELFILE, 0, 0 );

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

			if ( ! CDlgSelFile::CurrentObject->Save )
				ShowWindow( file, SW_HIDE );

			SetForegroundWindow( hwndDlg );
            SetFocus( CDlgSelFile::CurrentObject->m_Tree );

            //return TRUE;
        }
		return TRUE;
		break;

	case WM_SELFILE:
		{
			CStr selfile = CDlgSelFile::CurrentObject->m_Selected.Mid(CDlgSelFile::CurrentObject->m_Selected.ReverseFind('\\')+1);
			if ( ! selfile.IsEmpty() )
			{
				HTREEITEM selItem = TreeView_GetSelection( CDlgSelFile::CurrentObject->m_Tree );
				if ( selItem != NULL )
				{
					CDlgSelFile::CurrentObject->Dir = CDlgSelFile::CurrentObject->GetItemPath( selItem );
					SendMessage( files, LB_SELECTSTRING, -1, (LPARAM)(LPCTSTR)selfile );
					SendMessage( file, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)selfile );
				}
			}
			CDlgSelFile::CurrentObject->m_Selected.Empty();
			//return TRUE;//jwz::
		}
		return TRUE;
		break;

    case WM_COMMAND:
        if ( wParam == IDOK )
        {
            //CDlgSelFile::CurrentObject->m_Selected = L""; 
			//MessageBox( hwndDlg,CDlgSelFile::CurrentObject->m_Selected , CDlgSelFile::CurrentObject->m_Title, MB_ICONWARNING );
			int len = SendMessage( file, WM_GETTEXTLENGTH, 0, 0 );
		
			if ( !CDlgSelFile::CurrentObject->Dir.IsEmpty() && len > 0 )
			{
				TCHAR *selFile = new TCHAR[len+1];
				SendMessage( file, WM_GETTEXT, len+1, (LPARAM)selFile );

	            CDlgSelFile::CurrentObject->m_Selected = CDlgSelFile::CurrentObject->Dir;
				if ( CDlgSelFile::CurrentObject->m_Selected.Right(1) != L"\\" )
					CDlgSelFile::CurrentObject->m_Selected += L"\\";
				CDlgSelFile::CurrentObject->m_Selected += selFile;
				delete[] selFile;
	            EndDialog( hwndDlg, IDOK );
			} //jwz:add for select only one file. begin
			else if (!CDlgSelFile::CurrentObject->m_Selected.IsEmpty()){
				EndDialog( hwndDlg, IDOK );
			  //jwz:end
			}else{
					MessageBox( hwndDlg, L"No file selected", CDlgSelFile::CurrentObject->m_Title, MB_ICONWARNING );
			}
            //return TRUE;
        }
        if ( wParam == IDCANCEL )
        {
            CDlgSelFile::CurrentObject->m_Selected = L""; 
            EndDialog( hwndDlg, IDCANCEL );
            //return TRUE;
        }
        if ( HIWORD(wParam) == LBN_SELCHANGE )
        {
			int sel = SendMessage( files, LB_GETCURSEL, 0, 0 );
			if ( sel != LB_ERR )
			{
				int len = SendMessage( files, LB_GETTEXTLEN, sel, 0 );
				TCHAR *selFile = new TCHAR[len+1];
				SendMessage( files, LB_GETTEXT, sel, (LPARAM)selFile );
				//MessageBox( NULL, selFile, L"Debug", MB_SETFOREGROUND );
		        SendMessage( file, WM_SETTEXT, 0, (LPARAM)selFile ); 
				delete[] selFile;
			}
            //return TRUE;
        }
        if ( HIWORD(wParam) == LBN_DBLCLK )
        {
            PostMessage( hwndDlg, WM_COMMAND, IDOK, 0 );
            //return TRUE;
        }
        return TRUE;
		break;

	case WM_NOTIFY:
		{
			LPNMHDR nmhdr = (LPNMHDR)lParam;
			if ( nmhdr->code == NM_RETURN && nmhdr->hwndFrom != CDlgSelFile::CurrentObject->m_Tree && ::GetParent( nmhdr->hwndFrom ) == hwndDlg )
				PostMessage( hwndDlg, WM_COMMAND, IDOK, NULL );
			if ( nmhdr->code == NM_RETURN && nmhdr->hwndFrom == CDlgSelFile::CurrentObject->m_Tree )
				TreeView_Expand( CDlgSelFile::CurrentObject->m_Tree, TreeView_GetSelection( CDlgSelFile::CurrentObject->m_Tree ), TVE_TOGGLE );

			if ( nmhdr->code == TVN_ITEMEXPANDING )
			{
				NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW*)lParam;

 				if( pNMTreeView->action == 2 )
				{
					//CStr msg;
					//msg.Format( L"old: %d/%d new: %d/%d", pNMTreeView->itemOld.hItem, pNMTreeView->itemOld.iImage, pNMTreeView->itemNew.hItem, pNMTreeView->itemNew.iImage );
					//MessageBox( NULL, msg, L"Debug", MB_SETFOREGROUND );

					// Update location display
					CStr sPath = CDlgSelFile::CurrentObject->GetItemPath( pNMTreeView->itemNew.hItem );
					// Refresh children
					if( TreeView_GetChild( CDlgSelFile::CurrentObject->m_Tree, pNMTreeView->itemNew.hItem ) == NULL )
					{
						if ( pNMTreeView->itemNew.iImage <= 3 || pNMTreeView->itemNew.lParam != NULL )
						{
							// Directories
    						CDlgSelFile::CurrentObject->PopulateTree( sPath, pNMTreeView->itemNew.hItem, L"" );
						}

						if( TreeView_GetSelection(CDlgSelFile::CurrentObject->m_Tree) != pNMTreeView->itemNew.hItem )
							TreeView_Select( CDlgSelFile::CurrentObject->m_Tree, pNMTreeView->itemNew.hItem, TVGN_CARET );
					}
					if( pNMTreeView->itemNew.iImage == 1 && TreeView_GetChild( CDlgSelFile::CurrentObject->m_Tree, pNMTreeView->itemNew.hItem ) != NULL )
					{
						pNMTreeView->itemNew.iImage = 0;
						pNMTreeView->itemNew.iSelectedImage = 0;
						pNMTreeView->itemNew.mask = TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
						TreeView_SetItem( CDlgSelFile::CurrentObject->m_Tree, &pNMTreeView->itemNew );
						//InvalidateRect( CDlgSelFile::CurrentObject->m_Tree, NULL, FALSE );
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
						TreeView_SetItem( CDlgSelFile::CurrentObject->m_Tree, &pNMTreeView->itemNew );
						//InvalidateRect( CDlgSelFile::CurrentObject->m_Tree, NULL, FALSE );
					}
				}
			}
			if ( nmhdr->code == TVN_SELCHANGED )
			{
				NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW*)lParam;
				CDlgSelFile::CurrentObject->Dir = CDlgSelFile::CurrentObject->GetItemPath( pNMTreeView->itemNew.hItem );

				SendMessage( files, LB_RESETCONTENT, 0, 0 );
				WIN32_FIND_DATA  findFileData;
				CStr searchString = CDlgSelFile::CurrentObject->Dir;
				if ( searchString.Right(1) != L"\\" ) searchString += L"\\";
				searchString += CDlgSelFile::CurrentObject->m_Filter;

				HANDLE ffh = FindFirstFile( searchString, &findFileData );
				if ( ffh != INVALID_HANDLE_VALUE )
				{
					while ( TRUE )
					{
						if ( ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
						{
							SendMessage( files, LB_ADDSTRING, 0, (LPARAM)findFileData.cFileName );
							//HaveFile = true;
						}
						if ( FindNextFile( ffh, &findFileData ) == 0 ) break;
					}
					FindClose( ffh );
				}
				if ( !CDlgSelFile::CurrentObject->Save )
				{
			        SendMessage( file, WM_SETTEXT, 0, (LPARAM)L"" ); 
					//MessageBox( NULL, L"weg", L"Dir", MB_SETFOREGROUND );
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

				int scrWidth   = GetSystemMetrics(SM_CXSCREEN);
				int scrHeight  = GetSystemMetrics(SM_CYSCREEN);

				RECT  rect;//, rect2; //jwz::
				int   height, infoBottom;//,width;jwz::
				int   res = (cx>320)?2:1;

#ifdef PNA
				cy = cy - 26 * res;
#endif

				GetWindowRect( text, &rect );
				ScreenToClient( hwndDlg, &rect );
				rect.left = 4;
				rect.right = cx-4;
				rect.top = 4;

				if ( CDlgSelFile::CurrentObject->m_Info.IsEmpty() )
				{
					ShowWindow( text, SW_HIDE );
					infoBottom = 0;
				}
				else
				{
					HDC dc = ::GetWindowDC( text );
					CStrArray lines;
					SplitLines( dc, (LPCTSTR)CDlgSelFile::CurrentObject->m_Info, lines, rect.right-rect.left );
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

				int top = infoBottom;

				if ( scrWidth > scrHeight )
				{
					rect.left   = 4;
					rect.right  = cx / 2 - 2;
					rect.top    = top + 4;
					rect.bottom = cy - 4;
					MoveWindow( CDlgSelFile::CurrentObject->m_Tree, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

					GetWindowRect( file, &rect );
					ScreenToClient( hwndDlg, &rect );
					height  = rect.bottom - rect.top;
					rect.top    = top + 4;
					rect.bottom = 4+height;
					rect.left   = cx/2+2;
					rect.right  = cx-4;
					MoveWindow( file, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

					if ( CDlgSelFile::CurrentObject->Save )
						rect.top = rect.bottom+4;
					else
						rect.top = top + 4;

					rect.left = cx/2+2;
					rect.right = cx-4;
					rect.bottom = cy - 4;
					MoveWindow( files, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
				}
				else
				{
					// Portrait/Square: dirs on top, files on bottom
					rect.left   = 4;
					rect.right  = cx - 4;
					rect.top    = top + 4;
					rect.bottom = cy/2 - 4;
					MoveWindow( CDlgSelFile::CurrentObject->m_Tree, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

					GetWindowRect( file, &rect );
					ScreenToClient( hwndDlg, &rect );
					height  = rect.bottom - rect.top;
					rect.left   = 4;
					rect.right  = cx - 4;
					rect.top    = cy/2 + 4;
					rect.bottom = rect.top + height;
					MoveWindow( file, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

					if ( CDlgSelFile::CurrentObject->Save )
						rect.top = rect.bottom+1;
					else
						rect.top = cy/2 + 4;
					rect.bottom = cy - 4;
					MoveWindow( files, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );
				}

#ifdef PNA
				rect.top = cy;
				rect.left = 4;
				rect.right = (cx/2) - 4;
				rect.bottom = cy + 24*res;
				MoveWindow( ok, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE );

				rect.left = (cx/2) + 4;
				rect.right = cx - 4;
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


CDlgSelFile::CDlgSelFile()
{
	m_Info = _T("");
}

BOOL CDlgSelFile::PopulateTree( CStr path, HTREEITEM hParent, CStr selDir ) 
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


HTREEITEM CDlgSelFile::InsertItem( HTREEITEM hParent, LPCTSTR sText, int iImage, int children, DWORD itemData )
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

CStr CDlgSelFile::GetItemPath( HTREEITEM hItem ) 
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



int CDlgSelFile::DoModal()
{
    CurrentObject = this;
	INITCOMMONCONTROLSEX iccs;
	iccs.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccs.dwICC = ICC_TREEVIEW_CLASSES|ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&iccs);
    int result = DialogBox( g_hInst, MAKEINTRESOURCE( IDD ), NULL, selFileDialogProc );
	// int e = GetLastError();
	// CString errMsg;
	// errMsg.Format( L"Fehler %d, IDD: %d, Inst.: %d", e, IDD, g_hInst );
	// MessageBox( NULL, errMsg, L"Debug", MB_OK );
	return result;
}

// end SPDlgTest.cpp

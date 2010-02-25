// DirTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DirTreeCtrl.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl

CDirTreeCtrl::CDirTreeCtrl()
{
    StartPath.Empty();
}

CDirTreeCtrl::~CDirTreeCtrl()
{
}

void CDirTreeCtrl::Initialize()
{
    int width  = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
#ifndef DESKTOP
    if ( width > 320 && height > 320 )
    {
	    m_ImageList.Create( IDB_DIR_ICONS_BIG, 34, 1, RGB( 255, 255, 255 ) );
    }
    else
    {
#endif
	    m_ImageList.Create( IDB_DIR_ICONS, 17, 1, RGB( 255, 255, 255 ) );
#ifndef DESKTOP
    }
#endif
	SetImageList( &m_ImageList, TVSIL_NORMAL );
#ifndef DESKTOP
	HTREEITEM rootItem	= InsertItem( TVI_ROOT, L"\\", 1, 1 );
	PopulateTree( _T(""), rootItem );
	Expand( rootItem, TVE_EXPAND ); 
#else
	DWORD     instDrives;
	int       i;
	TCHAR     drive[3] = L"C:";
	BOOL      rc = TRUE;

	instDrives = GetLogicalDrives();
	if ( instDrives == 0 )
	{
		InsertItem( TVI_ROOT, L"C:", 1, 1 );	// Notlösung: C:
	}
	else
	{
		for ( i = 0; i < 26; i++ )
		{
			if ( ( instDrives & ( 1 << i ) ) != 0 )
			{
				drive[0] = (TCHAR)('A' + i);
				InsertItem( TVI_ROOT, drive, 1, 1 );
			}
		}
	}
#endif

	/*
    CString mru = theApp.RegReadString( HKEY_CURRENT_USER, L"\\Software\\Microsoft\\File Explorer\\DialogMRU", L"MRUList" );
    if ( ! mru.IsEmpty() )
    {
        HTREEITEM nwItem = InsertItem( TVI_ROOT, theApp.Network, 4, 1 );

        for ( int i=0; i<mru.GetLength(); i++ )
        {
            CString letter = mru.GetAt(i);
            CString conn = theApp.RegReadString( HKEY_CURRENT_USER, L"\\Software\\Microsoft\\File Explorer\\DialogMRU", letter );
            if ( ! conn.IsEmpty() )
            {
                InsertItem( nwItem, conn, 3, 1 );
            }
        }
    }
	*/

    StartPath.Empty();
}

CString CDirTreeCtrl::GetItemPath( HTREEITEM hItem ) 
{
	CString sRet;

    if ( hItem == NULL )
    {
        hItem = GetSelectedItem();
        if ( hItem == NULL ) return sRet;
    }

	int img, selImg;
	GetItemImage( hItem, img, selImg );

    if ( img <= 3 )
    {
        if ( GetItemData( hItem ) != NULL )
        {
            //LPNETRESOURCE lpnr = (LPNETRESOURCE)GetItemData( hItem );
            //sRet = lpnr->lpRemoteName;
        }
        else
        {
	        do
	        {
		        //Add the directory name to the path.
		        if ( GetItemText( hItem ) != _T("\\") )
		        {
			        sRet = GetItemText( hItem ) + _T("\\")+ sRet; 
		        }
		        hItem = GetParentItem( hItem );
	        } while( hItem && sRet.Left(2) != L"\\\\" );

#ifndef DESKTOP
            if ( sRet.Left(2) != L"\\\\" )
	            sRet = _T("\\") + sRet;
			if ( sRet.GetLength() > 1 )
		        sRet = sRet.Left( sRet.GetLength() - 1 );
#else
			if ( sRet.GetLength() > 3 )
		        sRet = sRet.Left( sRet.GetLength() - 1 );
#endif
        }
    }

	return sRet;
}

BOOL CDirTreeCtrl::PopulateTree( CString path, HTREEITEM hParent, CString selDir ) 
{
	BOOL bGotChildren = FALSE;						//True if a child is added.	
	
	CString searchString = path + "\\*";
	CString filename;
	WIN32_FIND_DATA  findFileData;

	HANDLE ffh = FindFirstFile( searchString, &findFileData );
	if ( ffh != INVALID_HANDLE_VALUE )
	{
		if ( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
		{
			if ( findFileData.cFileName[0] != '.' )
			{
			    bGotChildren = TRUE;
				HTREEITEM treeItem	= InsertItem( hParent, findFileData.cFileName, 1, 1 );
			}
		}

		while ( FindNextFile( ffh, &findFileData ) == TRUE )
		{
			if ( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
			{
				if ( findFileData.cFileName[0] != '.' )
				{
					bGotChildren = TRUE;
					HTREEITEM treeItem	= InsertItem( hParent, findFileData.cFileName, 1, 1 );
				}
			}
		}
		FindClose( ffh );
	}

	SortChildren( hParent );

	return bGotChildren;
}


HTREEITEM CDirTreeCtrl::InsertItem( HTREEITEM hParent, CString sText, int iImage, int children, DWORD itemData )
{
	//sText = WindowName( sText );
	TVINSERTSTRUCT InsertStruct;
	InsertStruct.hParent		= hParent;	
	InsertStruct.hInsertAfter	= TVI_LAST;
	InsertStruct.item.mask	    = TVIF_IMAGE | TVIF_TEXT | TVIF_CHILDREN | 
								  TVIF_SELECTEDIMAGE | TVIF_PARAM;
	InsertStruct.item.pszText	= sText.GetBuffer( sText.GetLength() );  sText.ReleaseBuffer();
	InsertStruct.item.iImage	= iImage;
	InsertStruct.item.iSelectedImage = iImage;
	InsertStruct.item.cChildren = children;
    HTREEITEM item = CTreeCtrl::InsertItem( &InsertStruct );
    SetItemData( item, itemData );

    if ( !StartPath.IsEmpty() )
    {
        CString path = GetItemPath( item );
        if ( !path.IsEmpty() && StartPath.Left( path.GetLength() ).CompareNoCase( path ) == 0 )
        {
            Select( item, TVGN_CARET );
            PopulateTree( path, item );
        }
    }

    return item;
}

BEGIN_MESSAGE_MAP(CDirTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CDirTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl message handlers

void CDirTreeCtrl::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    int img, selImg;
	GetItemImage( pNMTreeView->itemNew.hItem, img, selImg );

	if( pNMTreeView->action == 2 )
	{
		//Update location display
		CString sPath = GetItemPath( pNMTreeView->itemNew.hItem );
		//Refresh children
		if( !GetChildItem( pNMTreeView->itemNew.hItem ) )
		{
            if ( img <= 3 || GetItemData(pNMTreeView->itemNew.hItem) != NULL )
            {
                // Directories
    			PopulateTree( sPath, pNMTreeView->itemNew.hItem );
            }
            /*
            else
            {
                // Network
                EnumNetwork( pNMTreeView->itemNew.hItem );
            }
            */

			if( GetSelectedItem( ) != pNMTreeView->itemNew.hItem )
				SelectItem( pNMTreeView->itemNew.hItem );
		}
		if( img == 1 && GetChildItem( pNMTreeView->itemNew.hItem ) )
		{
			SetItemImage( pNMTreeView->itemNew.hItem, 0, 0 );
		}
	}
	else
	{
		if( img == 0 )
		{
			SetItemImage( pNMTreeView->itemNew.hItem, 1, 1 );
		}
	}

	*pResult = 0;
}


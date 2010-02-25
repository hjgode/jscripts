// unzip.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "unzip.h"
#include "zip.h"
#include "main.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef DESKTOP
#include "mortafx.h"
#endif
#include "..\types\morttypes.h"


HWND FeedbackWindow  = NULL;
UINT FeedbackMessage = WM_USER;

void _cdecl SetFeedback( HWND window, 
                         UINT message )
{
    FeedbackWindow  = window;
    FeedbackMessage = message;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

BOOL _cdecl UnzipToFileA( LPSTR zipFile, 
                          LPSTR containedFile, 
                          LPCTSTR targetFile
					    )
{
    unzFile uf = NULL;
    unz_file_info file_info;
    char filename_inzip[256];
    BOOL ok = TRUE;

    uf = unzOpen(zipFile);
    if ( uf != NULL )
    {
        void *buf = NULL;
        UINT size_buf;
        HANDLE target = NULL;
        int err;

        if ( unzLocateFile( uf, containedFile, 0 ) != UNZ_OK ) ok = FALSE;
        if ( ok && unzGetCurrentFileInfo( uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0 ) != UNZ_OK ) ok = FALSE;
        if ( ok && unzOpenCurrentFile(uf) != UNZ_OK ) ok = FALSE;
        if ( ok )
        {
            target = CreateFile( targetFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
            if ( target == INVALID_HANDLE_VALUE ) ok = FALSE;
        }

        if ( ok )
        {
            size_buf = 65536;
            if ( file_info.uncompressed_size < size_buf ) size_buf = file_info.uncompressed_size;
            buf = (void*)malloc(size_buf);
            if ( buf == NULL ) ok = FALSE;
        }

        if ( ok )
        {
            do
            {
                err = unzReadCurrentFile( uf, buf, size_buf );
                if (err<0)
                {
                    ok = FALSE;
                }
                if (err>0)
                {
                    ULONG written;
                    if ( WriteFile( target, buf, err, &written, NULL ) == 0 )
                    {
                        ok = FALSE;
                    }
                }
            }
            while (err>0 && ok);
        }

        if ( target != NULL )
            CloseHandle( target );

        if ( buf != NULL )
            free( buf );

        if ( uf != NULL )
            unzClose(uf);
    }
    else
        ok = FALSE;

    return ok;
}

BOOL _cdecl UnzipToFile( LPCTSTR zipFile, 
                         LPCTSTR containedFile, 
                         LPCTSTR targetFile
					   )
{
    char zipFileAscii[MAX_PATH];
    char containedFileAscii[MAX_PATH];

    WideCharToMultiByte( CP_ACP, 0, zipFile, -1, zipFileAscii, MAX_PATH, NULL, NULL );
    WideCharToMultiByte( 437, 0, containedFile, -1, containedFileAscii, MAX_PATH, NULL, NULL );

    return UnzipToFileA( zipFileAscii, 
                         containedFileAscii, 
                         targetFile
					   );
}


BOOL _cdecl UnzipToPathA( LPSTR zipFile, 
                          LPSTR containedFile, 
                          LPCTSTR targetPath
					    )
{
    TCHAR targetFile[MAX_PATH];
    wcscpy( targetFile, targetPath );
    wcscat( targetFile, L"\\" );

    TCHAR containedFileUnicode[MAX_PATH];
    MultiByteToWideChar( 437, 0, containedFile, strlen(containedFile)+1, (LPTSTR)containedFileUnicode, MAX_PATH );
    wcscat( targetFile, containedFileUnicode );

    return UnzipToFileA( zipFile, 
                         containedFile, 
                         targetFile
					   );
}

BOOL _cdecl UnzipToPath( LPCTSTR zipFile, 
                         LPCTSTR containedFile, 
                         LPCTSTR targetPath
					   )
{
    char zipFileAscii[MAX_PATH];
    char containedFileAscii[MAX_PATH];

    WideCharToMultiByte( CP_ACP, 0, zipFile, -1, zipFileAscii, MAX_PATH, NULL, NULL );
    WideCharToMultiByte( 437, 0, containedFile, -1, containedFileAscii, MAX_PATH, NULL, NULL );

    return UnzipToPathA( zipFileAscii, 
                         containedFileAscii, 
                         targetPath
					   );
}

/*
BOOL _cdecl UnzipAllToPathA( LPSTR zipFile, 
                             LPCTSTR targetPath
					       )
{
    unzFile uf = NULL;
    unz_file_info file_info;
    char filename_inzip[256];
    BOOL ok = TRUE;
	TCHAR targetFile[MAX_PATH];

    uf = unzOpen(zipFile);
    if ( uf != NULL )
    {
        void *buf = NULL;
        UINT size_buf;
        HANDLE target = NULL;
        int err;

	    size_buf = 65536;
		buf = (void*)malloc(size_buf);
		if ( buf == NULL ) ok = FALSE;

		if ( ok && unzGoToFirstFile(uf) != 0 )
			ok = FALSE;

		BOOL more = ok;
		while ( more )
		{
			if ( ok && unzGetCurrentFileInfo( uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0 ) != UNZ_OK ) ok = FALSE;
			if ( ok && unzOpenCurrentFile(uf) != UNZ_OK ) ok = FALSE;
			if ( ok )
			{
				TCHAR fileInZip[MAX_PATH], path[MAX_PATH];
			    MultiByteToWideChar( 437, 0, filename_inzip, -1, fileInZip, MAX_PATH );

				swprintf( targetFile, L"%s\\%s", targetPath, fileInZip );
				wcscpy( path, targetFile );
				LPTSTR bsPos = wcsrchr( path, '\\');
				if ( bsPos != NULL )
				{
					*bsPos = '\0';
					CreateDirectory( path, NULL );
				}
	            target = CreateFile( targetFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		        if ( target == INVALID_HANDLE_VALUE ) ok = FALSE;
	        }

			if ( ok )
			{
	            do
		        {
					err = unzReadCurrentFile( uf, buf, size_buf );
					if (err<0)
					{
						ok = FALSE;
					}
					if (err>0)
					{
						ULONG written;
						if ( WriteFile( target, buf, err, &written, NULL ) == 0 )
						{
							ok = FALSE;
						}
					}
				}
				while (err>0 && ok);
			}

			if ( target != NULL )
				CloseHandle( target );

			if ( !ok || unzGoToNextFile(uf) < 0 )
				more = FALSE;
		}

        if ( buf != NULL )
            free( buf );

        if ( uf != NULL )
            unzClose(uf);
    }
    else
        ok = FALSE;

    return ok;
}
*/

BOOL _cdecl UnzipPathToPathA( LPSTR zipFile, 
                              LPSTR zipPath,
                              LPCTSTR targetPath
					        )
{
    unzFile uf = NULL;
    unz_file_info file_info;
    char filename_inzip[256];
    BOOL ok = TRUE;
	TCHAR targetFile[MAX_PATH];
    int zipPathLen;

    if ( zipPath != NULL )
    {
        zipPathLen = strlen( zipPath );
        for ( int i=0; i<zipPathLen; i++ )
		{
            if ( zipPath[i] == '\\' )
			{
				zipPath[i] = '/'; // Paths are stored with / in zip archive
			}
		}
    }
    else
        zipPathLen = 0;

    uf = unzOpen(zipFile);
    if ( uf != NULL )
    {
        CreateDirectory( targetPath, NULL );

        void *buf = NULL;
        UINT size_buf;
        HANDLE target = NULL;
        int err;

	    size_buf = 65536;
		buf = (void*)malloc(size_buf);
		if ( buf == NULL ) ok = FALSE;

		if ( ok && unzGoToFirstFile(uf) != 0 )
			ok = FALSE;

		BOOL more = ok;
		while ( more )
		{
			if ( ok && unzGetCurrentFileInfo( uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0 ) != UNZ_OK ) ok = FALSE;
            if (   ok
                && (   zipPathLen == 0
                    || ( _strnicmp( filename_inzip, zipPath, zipPathLen ) == 0 && filename_inzip[zipPathLen] == '/' )
                   )
               )
            {
			    if ( unzOpenCurrentFile(uf) != UNZ_OK ) ok = FALSE;
			    if ( ok )
			    {
				    TCHAR fileInZip[MAX_PATH], path[MAX_PATH];
			        MultiByteToWideChar( 437, 0, filename_inzip, -1, fileInZip, MAX_PATH );

                    if ( zipPathLen > 0 )
   				        swprintf( targetFile, L"%s\\%s", targetPath, fileInZip+zipPathLen+1 );
                    else
                        swprintf( targetFile, L"%s\\%s", targetPath, fileInZip );

                    for ( int i=0; i<wcslen(targetFile); i++ )
                        if ( targetFile[i] == '/' ) targetFile[i] = '\\'; // Paths are stored with / in zip archive

				    wcscpy( path, targetFile );
				    LPTSTR bsPos = wcschr( path + (wcslen(targetPath)+zipPathLen), '\\' );
				    while ( bsPos != NULL )
				    {
					    *bsPos = '\0';
					    CreateDirectory( path, NULL );
					    *bsPos = '\\';
						if ( *(bsPos+1) == 0 )
							break;
						bsPos = wcschr( bsPos+1, '\\' );
				    }
                    if ( bsPos == NULL || *(bsPos+1) != 0 ) // Not a path entry
                    {
	                    target = CreateFile( targetFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		                if ( target == INVALID_HANDLE_VALUE ) ok = FALSE;
                    }
                    else
                    {
                        target = INVALID_HANDLE_VALUE;
                    }
	            }

			    if ( ok && target != INVALID_HANDLE_VALUE )
			    {
	                do
		            {
					    err = unzReadCurrentFile( uf, buf, size_buf );
					    if (err<0)
					    {
						    ok = FALSE;
					    }
					    if (err>0)
					    {
						    ULONG written;
						    if ( WriteFile( target, buf, err, &written, NULL ) == 0 )
						    {
							    ok = FALSE;
						    }
					    }
				    }
				    while (err>0 && ok);

			        if ( target != INVALID_HANDLE_VALUE )
				        CloseHandle( target );
			    }
            }

			if ( !ok || unzGoToNextFile(uf) < 0 )
				more = FALSE;
		}

        if ( buf != NULL )
            free( buf );

        if ( uf != NULL )
            unzClose(uf);
    }
    else
        ok = FALSE;

    return ok;
}



BOOL _cdecl UnzipPathToPath( LPCTSTR zipFile, 
                             LPCTSTR zipPath,
                             LPCTSTR targetPath
					       )
{
    char zipFileAscii[MAX_PATH];
    char zipPathAscii[MAX_PATH];

    WideCharToMultiByte( CP_ACP, 0, zipFile, -1, zipFileAscii, MAX_PATH, NULL, NULL );
    WideCharToMultiByte( 437,    0, zipPath, -1, zipPathAscii, MAX_PATH, NULL, NULL );

    return UnzipPathToPathA( zipFileAscii, 
                             zipPathAscii,
                             targetPath
					       );
}

BOOL _cdecl UnzipAllToPath( LPCTSTR zipFile, 
                            LPCTSTR targetPath
					      )
{
    char zipFileAscii[MAX_PATH];

    WideCharToMultiByte( CP_ACP, 0, zipFile, -1, zipFileAscii, MAX_PATH, NULL, NULL );

    return UnzipPathToPathA( zipFileAscii, 
                             NULL,
                             targetPath
					       );
}




void* _cdecl UnzipToMemoryA( LPSTR zipFile, 
                             LPSTR containedFile, 
                             UINT  *size
	 				       )
{
    unzFile uf = NULL;
    unz_file_info file_info;
    char filename_inzip[256];
    void *buf = NULL;

    uf = unzOpen(zipFile);
    if ( uf != NULL )
    {
        HANDLE target = NULL;
        int err;
        BOOL ok = TRUE;

        if ( unzLocateFile( uf, containedFile, 0 ) != UNZ_OK ) ok = FALSE;
        if ( ok && unzGetCurrentFileInfo( uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0 ) != UNZ_OK ) ok = FALSE;
        if ( ok && unzOpenCurrentFile(uf) != UNZ_OK ) ok = FALSE;

        if ( ok )
        {
            if ( file_info.uncompressed_size > *size ) ok = FALSE;
        }

        if ( ok )
        {
            if ( file_info.uncompressed_size < *size )
                *size = file_info.uncompressed_size;
            buf = (void*)malloc(*size);
            if ( buf == NULL ) ok = FALSE;
        }

        if ( ok )
        {
            err = unzReadCurrentFile( uf, buf, *size );
            if (err<0)
            {
                ok = FALSE;
            }
        }

        if ( uf != NULL )
            unzClose(uf);

        if ( buf != NULL && !ok )
        {
            free( buf );
            buf = NULL;
        }
    }

    return buf;
}

void* _cdecl UnzipToMemory( LPCTSTR zipFile, 
                            LPCTSTR containedFile, 
                            UINT    *size
            			  )
{
    char zipFileAscii[MAX_PATH];
    char containedFileAscii[MAX_PATH];

    WideCharToMultiByte( CP_ACP, 0, zipFile, -1, zipFileAscii, MAX_PATH, NULL, NULL );
    WideCharToMultiByte( 437, 0, containedFile, -1, containedFileAscii, MAX_PATH, NULL, NULL );

    return UnzipToMemoryA( zipFileAscii, 
                           containedFileAscii, 
                           size
					     );
}




BOOL AddFilesToZip( zipFile zf, LPCTSTR rootPath, LPCTSTR relPath, LPCTSTR files, LPCTSTR pathInZip, BOOL recursive, int compLevel )
{
    WIN32_FIND_DATA  findFileData;
    int count = 0;

	TCHAR searchString[MAX_PATH], sourcePath[MAX_PATH], sourceFile[MAX_PATH], fileInZip[MAX_PATH];
	char  containedFile[MAX_PATH];

	wcscpy( searchString, rootPath );
	wcscat( searchString, L"\\" );
	if ( relPath != NULL )
	{
		wcscat( searchString, relPath );
		wcscat( searchString, L"\\" );
	}
	wcscpy( sourcePath, searchString );
	wcscat( searchString, files );

    CMortStringArray subdirs;

	BOOL ok = TRUE;
    HANDLE ffh = FindFirstFile( searchString, &findFileData );
    while ( ffh != INVALID_HANDLE_VALUE && ok )
    {
        if ( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
        {
			wcscpy( sourceFile, sourcePath );
			wcscat( sourceFile, findFileData.cFileName );

            if ( pathInZip == NULL || wcslen( pathInZip ) == 0 )
                fileInZip[0]='\0';
            else
            {
				wcscpy( fileInZip, pathInZip );
				wcscat( fileInZip, L"\\" );
            }

			if ( relPath != NULL )
			{
				wcscat( fileInZip, relPath );
				wcscat( fileInZip, L"\\" );
			}
			wcscat( fileInZip, findFileData.cFileName );
			WideCharToMultiByte( 437, 0, fileInZip, -1, containedFile, MAX_PATH, NULL, NULL );

			HANDLE target = NULL;
			int err = ZIP_OK;

			DWORD size_read;
			zip_fileinfo zi;
			unsigned long crcFile=0;

			zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
			zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
			zi.dosDate = 0;
			zi.internal_fa = 0;
			zi.external_fa = 0;

			HANDLE tmpFile = CreateFile( sourceFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			if ( tmpFile != NULL )
			{
				BY_HANDLE_FILE_INFORMATION fileInformation;
				FILETIME ftLocal;
				SYSTEMTIME ls;

				GetFileAttributes( sourceFile );
				GetFileInformationByHandle( tmpFile, &fileInformation );
				FileTimeToLocalFileTime(&(fileInformation.ftLastWriteTime),&ftLocal);
				FileTimeToSystemTime( &ftLocal, &ls );
				// Convert SYSTEMTIME to tm
				zi.tmz_date.tm_sec  = ls.wSecond;
				zi.tmz_date.tm_min  = ls.wMinute;
				zi.tmz_date.tm_hour = ls.wHour;
				zi.tmz_date.tm_mday = ls.wDay;
				zi.tmz_date.tm_mon  = ls.wMonth -1;
				zi.tmz_date.tm_year = ls.wYear - 1900;
				//zi.tmz_date.tm_wday = ls.wDayOfWeek;
				//FileTimeToDosDateTime(&ftLocal,((LPWORD)zi.dosDate)+1,((LPWORD)zi.dosDate)+0);

	            UINT size_buf = 16384;
                void *buf = (void*)malloc(size_buf);
		        if ( buf == NULL )
			        err = ZIP_ERRNO;

                if ( err == ZIP_OK )
                {
                    // Replace backslashes with slashes, otherwise overwrite doesn't work properly
                    for ( int i=0; i<strlen(containedFile); i++ )
                    {
                        if ( containedFile[i] == '\\' )
                            containedFile[i] = '/';
                    }
				    err = zipOpenNewFileInZip3( zf, containedFile, &zi,
										     NULL,0,NULL,0,NULL /* comment*/,
										     (compLevel != 0) ? Z_DEFLATED : 0,
										     compLevel,0,
										     /* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
										     -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
										     NULL,0);
                }

				if (err == ZIP_OK)
				{
					do
					{
						err = ZIP_OK;
						BOOL fok = ::ReadFile( tmpFile, buf, size_buf, &size_read, NULL );
						// (int)FileRead(buf,1,size_buf,fin);
						if ( fok == FALSE )
						{
							err = ZIP_ERRNO;
						}

						if (size_read>0)
						{
							err = zipWriteInFileInZip (zf,buf,size_read);
						}
					} while ((err == ZIP_OK) && (size_read>0));
				}

				if ( tmpFile != NULL )
				   CloseHandle( tmpFile );

				if ( err >= 0 )
				{
					err = zipCloseFileInZip( zf );
				}

                if ( buf != NULL )
                    free( buf );

				if ( err < 0 )
					ok = FALSE;
			}
        }

        if ( FindNextFile( ffh, &findFileData ) == 0 ) break;
    }
    if ( ffh != INVALID_HANDLE_VALUE ) FindClose( ffh );

    if ( ok && recursive )
    {
	    wcscpy( searchString, rootPath );
	    wcscat( searchString, L"\\" );
	    if ( relPath != NULL )
	    {
		    wcscat( searchString, relPath );
		    wcscat( searchString, L"\\" );
	    }
	    wcscat( searchString, L"*.*" );

        ffh = FindFirstFile( searchString, &findFileData );
        while ( ffh != INVALID_HANDLE_VALUE )
        {
            if ( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 )
            {
                subdirs.Add( findFileData.cFileName );
            }
            if ( FindNextFile( ffh, &findFileData ) == 0 ) break;
        }
        if ( ffh != INVALID_HANDLE_VALUE ) FindClose( ffh );
    }

    for ( int i=0; i<subdirs.GetSize() && ok; i++ )
    {
		if ( relPath == NULL )
			sourceFile[0]='\0';
		else
		{
			wcscpy( sourceFile, relPath );
			wcscat( sourceFile, L"\\" );
		}
		wcscat( sourceFile, subdirs.GetAt(i) );
		ok = AddFilesToZip( zf, rootPath, sourceFile, files, pathInZip, recursive, compLevel );
    }

    return ok;
}

BOOL _cdecl ZipFromFilesToZipPathA( LPSTR zipFileName, 
                                    LPCTSTR sourceFile,
                                    LPCTSTR pathInZip,
						            BOOL recursive,
                                    int  compLevel,
                                    BOOL overwrite
					              )
{
    zipFile zf;
    int errclose;
    BOOL ok = TRUE;

    zf = zipOpen(zipFileName,overwrite?APPEND_STATUS_CREATE:APPEND_STATUS_ADDINZIP);

    if ( zf != NULL )
    {
		TCHAR path[MAX_PATH];
		wcscpy( path, sourceFile );
		LPTSTR bsPos = wcsrchr( path, '\\');
		if ( bsPos != NULL )
			*bsPos = '\0';
		else
			ok = FALSE;

		if ( ok )
		{
			ok = AddFilesToZip( zf, path, NULL, bsPos+1, pathInZip, recursive, compLevel );
		}


        errclose = zipClose(zf,NULL);
    }
    else
        ok = FALSE;

    //if (errclose != ZIP_OK)
    //    printf("error in closing %s\n",filename_try);

    return ok;
}


BOOL _cdecl ZipFromFilesToZipPath( LPCTSTR zipFileName, 
                                   LPCTSTR sourceFile,
                                   LPCTSTR pathInZip,
 						           BOOL recursive,
                                   int  compLevel
					             )
{
    char zipFileAscii[MAX_PATH];

    WideCharToMultiByte( CP_ACP, 0, zipFileName, -1, zipFileAscii, MAX_PATH, NULL, NULL );

    BOOL overwrite = TRUE;
    if ( GetFileAttributes( zipFileName ) != -1 )
        overwrite = FALSE;

	return ZipFromFilesToZipPathA( zipFileAscii, 
                                   sourceFile,
                                   pathInZip,
						           recursive,
                                   compLevel,
                                   overwrite
					             );
}

BOOL _cdecl ZipFromFilesA( LPSTR zipFileName, 
                           LPCTSTR sourceFile,
						   BOOL recursive,
                           int  compLevel,
                           BOOL overwrite
					    )
{
    return ZipFromFilesToZipPathA( zipFileName, sourceFile, NULL, recursive, compLevel, overwrite );
}


BOOL _cdecl ZipFromFiles( LPCTSTR zipFileName, 
                          LPCTSTR sourceFile,
						  BOOL recursive,
                          int  compLevel
					    )
{
    return ZipFromFilesToZipPath( zipFileName, sourceFile, NULL, recursive, compLevel );
}

BOOL _cdecl ZipFromFileA( LPSTR zipFileName, 
                          LPSTR containedFile, 
                          LPCTSTR sourceFile,
                          int  compLevel,
                          BOOL overwrite
					    )
{
    zipFile zf;
    int errclose;
    BOOL ok = TRUE;

    zf = zipOpen(zipFileName,overwrite?APPEND_STATUS_CREATE:APPEND_STATUS_ADDINZIP);

    if ( zf != NULL )
    {
        void *buf = NULL;
        UINT size_buf;
        HANDLE target = NULL;
        int err;

        DWORD size_read;
        zip_fileinfo zi;
        unsigned long crcFile=0;

        zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
        zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
        zi.dosDate = 0;
        zi.internal_fa = 0;
        zi.external_fa = 0;

        HANDLE tmpFile = CreateFile( sourceFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        if ( tmpFile != NULL )
        {
            BY_HANDLE_FILE_INFORMATION fileInformation;
            FILETIME ftLocal;
            SYSTEMTIME ls;

            GetFileAttributes( sourceFile );
            GetFileInformationByHandle( tmpFile, &fileInformation );
            FileTimeToLocalFileTime(&(fileInformation.ftLastWriteTime),&ftLocal);
            FileTimeToSystemTime( &ftLocal, &ls );
            // Convert SYSTEMTIME to tm
            zi.tmz_date.tm_sec  = ls.wSecond;
            zi.tmz_date.tm_min  = ls.wMinute;
            zi.tmz_date.tm_hour = ls.wHour;
            zi.tmz_date.tm_mday = ls.wDay;
            zi.tmz_date.tm_mon  = ls.wMonth -1;
            zi.tmz_date.tm_year = ls.wYear - 1900;
            //zi.tmz_date.tm_wday = ls.wDayOfWeek;
            //FileTimeToDosDateTime(&ftLocal,((LPWORD)zi.dosDate)+1,((LPWORD)zi.dosDate)+0);

            for ( int i=0; i<strlen(containedFile); i++ )
            {
                if ( containedFile[i] == '\\' )
                    containedFile[i] = '/';
            }
            err = zipOpenNewFileInZip3( zf, containedFile, &zi,
                                     NULL,0,NULL,0,NULL /* comment*/,
                                     (compLevel != 0) ? Z_DEFLATED : 0,
                                     compLevel,0,
                                     /* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
                                     -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                                     NULL,0);

            if (err == ZIP_OK)
            {
                size_buf = 16384;
                buf = (void*)malloc(size_buf);
                if ( buf == NULL )
                    err = ZIP_ERRNO;
            }

            if (err == ZIP_OK)
            {
                do
                {
                    err = ZIP_OK;
                    BOOL fok = ::ReadFile( tmpFile, buf, size_buf, &size_read, NULL );
                    // (int)FileRead(buf,1,size_buf,fin);
                    if ( fok == FALSE )
                    {
                        err = ZIP_ERRNO;
                    }

                    if (size_read>0)
                    {
                        err = zipWriteInFileInZip (zf,buf,size_read);
                    }
                } while ((err == ZIP_OK) && (size_read>0));
            }

            if ( tmpFile != NULL )
               CloseHandle( tmpFile );

            if ( err >= 0 )
            {
                err = zipCloseFileInZip( zf );
            }

            if ( err < 0 )
                ok = FALSE;
        }

        errclose = zipClose(zf,NULL);
    }
    else
        ok = FALSE;

    //if (errclose != ZIP_OK)
    //    printf("error in closing %s\n",filename_try);

    return ok;
}

BOOL _cdecl ZipFromFile( LPCTSTR zipFile, 
                         LPCTSTR containedFile, 
                         LPCTSTR sourceFile,
                         int  compLevel
			           )
{
    char zipFileAscii[MAX_PATH];
    char containedFileAscii[MAX_PATH];

    WideCharToMultiByte( CP_ACP, 0, zipFile, -1, zipFileAscii, MAX_PATH, NULL, NULL );
    WideCharToMultiByte( 437, 0, containedFile, -1, containedFileAscii, MAX_PATH, NULL, NULL );

    BOOL overwrite = TRUE;
    if ( GetFileAttributes( zipFile ) != -1 )
        overwrite = FALSE;

    return ZipFromFileA( zipFileAscii, 
                         containedFileAscii, 
                         sourceFile,
                         compLevel,
                         overwrite
					   );
}

BOOL _cdecl ZipFromMemoryA( LPSTR zipFileName, 
                            LPSTR containedFile, 
                            void  *data,
                            DWORD size,
                            int   compLevel
					      )
{
    zipFile zf;
    int errclose;
    BOOL ok = TRUE;

    zf = zipOpen(zipFileName,APPEND_STATUS_ADDINZIP);

    if ( zf != NULL )
    {
        void *buf = NULL;
        HANDLE target = NULL;
        int err;

        zip_fileinfo zi;
        unsigned long crcFile=0;

        zi.dosDate = 0;
        zi.internal_fa = 0;
        zi.external_fa = 0;

        SYSTEMTIME ls;
        GetLocalTime( &ls );
        // Convert SYSTEMTIME to tm
        zi.tmz_date.tm_sec  = ls.wSecond;
        zi.tmz_date.tm_min  = ls.wMinute;
        zi.tmz_date.tm_hour = ls.wHour;
        zi.tmz_date.tm_mday = ls.wDay;
        zi.tmz_date.tm_mon  = ls.wMonth -1;
        zi.tmz_date.tm_year = ls.wYear - 1900;

        err = zipOpenNewFileInZip3( zf, containedFile, &zi,
                                 NULL,0,NULL,0,NULL /* comment*/,
                                 (compLevel != 0) ? Z_DEFLATED : 0,
                                 compLevel,0,
                                 /* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
                                 -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                                 NULL,0);

        if (err == ZIP_OK)
        {
            err = zipWriteInFileInZip(zf,data,size);
        }

        if ( err >= 0 )
        {
            err = zipCloseFileInZip( zf );
        }

        if ( err < 0 )
            ok = FALSE;

        errclose = zipClose(zf,NULL);
    }
    else
        ok = FALSE;

    //if (errclose != ZIP_OK)
    //    printf("error in closing %s\n",filename_try);

    return ok;
}

BOOL _cdecl ZipFromMemory( LPCTSTR zipFile, 
                           LPCTSTR containedFile, 
                           void  *data,
                           DWORD size,
                           int  compLevel
			             )
{
    char zipFileAscii[MAX_PATH];
    char containedFileAscii[MAX_PATH];

    WideCharToMultiByte( CP_ACP, 0, zipFile, -1, zipFileAscii, MAX_PATH, NULL, NULL );
    WideCharToMultiByte( 437, 0, containedFile, -1, containedFileAscii, MAX_PATH, NULL, NULL );

    return ZipFromMemoryA( zipFileAscii, 
                           containedFileAscii, 
                           data,
                           size,
                           compLevel
					     );
}

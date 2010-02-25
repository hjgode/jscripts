#define DLL_EXPORT extern "C" _declspec(dllexport)

DLL_EXPORT void _cdecl SetFeedback( HWND window, 
                                    UINT message );

DLL_EXPORT BOOL _cdecl UnzipToPath( LPCTSTR zipFile, 
                                    LPCTSTR containedFile, 
                                    LPCTSTR targetPath
				                  );

DLL_EXPORT BOOL _cdecl UnzipToFile( LPCTSTR zipFile, 
                                    LPCTSTR containedFile, 
                                    LPCTSTR targetFile
					              );

DLL_EXPORT BOOL _cdecl UnzipPathToPath( LPCTSTR zipFile, 
                                        LPCTSTR zipPath,
                                        LPCTSTR targetPath
				                      );

DLL_EXPORT BOOL _cdecl UnzipAllToPath( LPCTSTR zipFile, 
									   LPCTSTR targetPath
								     );

DLL_EXPORT void* _cdecl UnzipToMemory( LPCTSTR zipFile, 
                                       LPCTSTR containedFile, 
                                       UINT    *size
            			             );

DLL_EXPORT BOOL _cdecl ZipFromFile( LPCTSTR zipFile, 
                                    LPCTSTR containedFile, 
                                    LPCTSTR sourceFile,
                                    int  compLevel
				                  );

DLL_EXPORT BOOL _cdecl ZipFromFiles( LPCTSTR zipFileName, 
									 LPCTSTR sourceFile,
									 BOOL recursive,
									 int  compLevel
								   );

DLL_EXPORT BOOL _cdecl ZipFromFilesToZipPath( LPCTSTR zipFileName, 
									          LPCTSTR sourceFile,
                                              LPCTSTR pathInZip,
									          BOOL recursive,
									          int  compLevel
								            );

DLL_EXPORT BOOL _cdecl ZipFromMemory( LPCTSTR zipFile, 
                                      LPCTSTR containedFile, 
                                      void *data,
                                      DWORD size,
                                      int  compLevel
    			                    );


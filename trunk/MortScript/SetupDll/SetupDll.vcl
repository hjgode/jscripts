<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: SetupDll - Win32 (WCE ARMV4) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOKUME~1\Mirko\LOKALE~1\Temp\RSP1132.tmp" with contents
[
/nologo /W3 /D _WIN32_WCE=300 /D WIN32_PLATFORM_PSPC=310 /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=300 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "_USRDLL" /D "SETUPDLL_EXPORTS" /Fo"..\temp\ARMV4Rel\setupdll/" /O2 /MC /c 
"D:\Projekte\EVC\MortScript\SetupDll\setup.cpp"
]
Creating command line "clarm.exe @C:\DOKUME~1\Mirko\LOKALE~1\Temp\RSP1132.tmp" 
Creating temporary file "C:\DOKUME~1\Mirko\LOKALE~1\Temp\RSP1133.tmp" with contents
[
commctrl.lib coredll.lib /nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /incremental:no /pdb:"..\exe\ARMV4Rel/setup.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /def:".\SetupDll.def" /out:"../exe/ARMV4Rel/setup.dll" /implib:"..\exe\ARMV4Rel/setup.lib" /subsystem:windowsce,3.00 /align:"4096" /MACHINE:ARM 
\Projekte\EVC\MortScript\temp\ARMV4Rel\setupdll\setup.obj
]
Creating command line "link.exe @C:\DOKUME~1\Mirko\LOKALE~1\Temp\RSP1133.tmp"
<h3>Output Window</h3>
Compiling...
setup.cpp
Linking...
   Creating library ..\exe\ARMV4Rel/setup.lib and object ..\exe\ARMV4Rel/setup.exp




<h3>Results</h3>
setup.dll - 0 error(s), 0 warning(s)
</pre>
</body>
</html>

<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: Autorun - Win32 (WCE ARMV4) Release SP--------------------
</h3>
<h3>Command Lines</h3>
Creating command line "rc.exe /l 0x409 /fo"../temp/ARMV4RelSP/autorun/Autorun.res" /d UNDER_CE=420 /d _WIN32_WCE=420 /d "NDEBUG" /d "UNICODE" /d "_UNICODE" /d "WIN32_PLATFORM_WFSP=200" /d "ARM" /d "_ARM_" /d "ARMV4" /r "D:\Projekte\EVC\MortScript\Autorun\Autorun.rc"" 
Creating temporary file "C:\DOKUME~1\Mirko\LOKALE~1\Temp\RSP10CB.tmp" with contents
[
/nologo /W3 /D _WIN32_WCE=420 /D "WIN32_PLATFORM_WFSP=200" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /Fp"../temp/ARMV4RelSP/autorun/Autorun.pch" /Yu"stdafx.h" /Fo"../temp/ARMV4RelSP/autorun/" /O2 /MC /c 
"D:\Projekte\EVC\MortScript\Autorun\Autorun.cpp"
]
Creating command line "clarm.exe @C:\DOKUME~1\Mirko\LOKALE~1\Temp\RSP10CB.tmp" 
Creating temporary file "C:\DOKUME~1\Mirko\LOKALE~1\Temp\RSP10CC.tmp" with contents
[
/nologo /W3 /D _WIN32_WCE=420 /D "WIN32_PLATFORM_WFSP=200" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /Fp"../temp/ARMV4RelSP/autorun/Autorun.pch" /Yc"stdafx.h" /Fo"../temp/ARMV4RelSP/autorun/" /O2 /MC /c 
"D:\Projekte\EVC\MortScript\Autorun\StdAfx.cpp"
]
Creating command line "clarm.exe @C:\DOKUME~1\Mirko\LOKALE~1\Temp\RSP10CC.tmp" 
Creating temporary file "C:\DOKUME~1\Mirko\LOKALE~1\Temp\RSP10CD.tmp" with contents
[
commctrl.lib coredll.lib aygshell.lib note_prj.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:no /pdb:"../exe/ARMV4RelSP/Autorun.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"../exe/ARMV4RelSP/Autorun.exe" /subsystem:windowsce,4.20 /align:"4096" /MACHINE:ARM 
\Projekte\EVC\MortScript\temp\ARMV4RelSP\autorun\Autorun.obj
\Projekte\EVC\MortScript\temp\ARMV4RelSP\autorun\StdAfx.obj
\Projekte\EVC\MortScript\temp\ARMV4RelSP\autorun\Autorun.res
]
Creating command line "link.exe @C:\DOKUME~1\Mirko\LOKALE~1\Temp\RSP10CD.tmp"
<h3>Output Window</h3>
Compiling resources...
Compiling...
StdAfx.cpp
Compiling...
Autorun.cpp
Linking...
Signing D:\Projekte\EVC\MortScript\exe\ARMV4RelSP\Autorun.exe
Warning: This file is signed, but not timestamped.
Succeeded




<h3>Results</h3>
Autorun.exe - 0 error(s), 0 warning(s)
</pre>
</body>
</html>

<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: MortRunnerSP - Win32 (WCE ARMV4) Release SP--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\jwz\LOCALS~1\Temp\RSP1D2.tmp" with contents
[
/nologo /Oxs /I "..\MortTools\Types" /D "SMARTPHONE" /D _WIN32_WCE=300 /D WIN32_PLATFORM_PSPC=310 /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=300 /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "CELE" /D "JWZ" /Fr"temp/ARMV4RelSP/MortScript/" /Fp"temp/ARMV4RelSP/MortScript/MortScriptSP.pch" /YX /Fo"temp/ARMV4RelSP/MortScript/" /MC /c 
"D:\ROM\Mort-1010\jscripts\MortScript\Interpreter.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\Helpers.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\Functions.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\FunctionsFileReg.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\jwz\LOCALS~1\Temp\RSP1D2.tmp" 
Creating temporary file "C:\DOCUME~1\jwz\LOCALS~1\Temp\RSP1D3.tmp" with contents
[
commctrl.lib coredll.lib aygshell.lib Wininet.lib cellcore.lib ..\MortTools\Types.lib bthutil.lib /nologo /base:"0x00010000" /stack:0x100000,0x10000 /entry:"WinMainCRTStartup" /incremental:no /pdb:"exe/ARMV4RelSP/JScripts.pdb" /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"exe/ARMV4RelSP/JScripts.exe" /subsystem:windowsce,3.00 /align:"4096" /MACHINE:ARM 
".\temp\ARMV4RelSP\MortScript\MortRunnerSP.obj"
".\temp\ARMV4RelSP\MortScript\ParseCmdLine.obj"
".\temp\ARMV4RelSP\MortScript\DlgBigMessage.obj"
".\temp\ARMV4RelSP\MortScript\DlgChoice.obj"
".\temp\ARMV4RelSP\MortScript\DlgInput.obj"
".\temp\ARMV4RelSP\MortScript\DlgSelDir.obj"
".\temp\ARMV4RelSP\MortScript\DlgSelFile.obj"
".\temp\ARMV4RelSP\MortScript\DlgWait.obj"
".\temp\ARMV4RelSP\MortScript\Interpreter.obj"
".\temp\ARMV4RelSP\MortScript\FctFlight.obj"
".\temp\ARMV4RelSP\MortScript\Helpers.obj"
".\temp\ARMV4RelSP\MortScript\IniFile.obj"
".\temp\ARMV4RelSP\MortScript\variables.obj"
".\temp\ARMV4RelSP\MortScript\Commands.obj"
".\temp\ARMV4RelSP\MortScript\CommandsData.obj"
".\temp\ARMV4RelSP\MortScript\CommandsDialogs.obj"
".\temp\ARMV4RelSP\MortScript\CommandsFileReg.obj"
".\temp\ARMV4RelSP\MortScript\CommandsSystem.obj"
".\temp\ARMV4RelSP\MortScript\CommandsWindows.obj"
".\temp\ARMV4RelSP\MortScript\Functions.obj"
".\temp\ARMV4RelSP\MortScript\FunctionsData.obj"
".\temp\ARMV4RelSP\MortScript\FunctionsDialogs.obj"
".\temp\ARMV4RelSP\MortScript\FunctionsFileReg.obj"
".\temp\ARMV4RelSP\MortScript\FunctionsSystem.obj"
".\temp\ARMV4RelSP\MortScript\FunctionsWindows.obj"
".\temp\ARMV4RelSP\MortScript\CommandsStatus.obj"
".\temp\ARMV4RelSP\MortScript\DlgStatus.obj"
".\temp\ARMV4RelSP\MortScript\MortRunnerSP.res"
]
Creating command line "link.exe @C:\DOCUME~1\jwz\LOCALS~1\Temp\RSP1D3.tmp"
<h3>Output Window</h3>
Compiling...
Interpreter.cpp
Helpers.cpp
Functions.cpp
FunctionsFileReg.cpp
Linking...
Creating temporary file "C:\DOCUME~1\jwz\LOCALS~1\Temp\RSP1D6.tmp" with contents
[
/nologo /o"exe/ARMV4RelSP/MortScriptSP.bsc" 
".\temp\ARMV4RelSP\MortScript\MortRunnerSP.sbr"
".\temp\ARMV4RelSP\MortScript\ParseCmdLine.sbr"
".\temp\ARMV4RelSP\MortScript\DlgBigMessage.sbr"
".\temp\ARMV4RelSP\MortScript\DlgChoice.sbr"
".\temp\ARMV4RelSP\MortScript\DlgInput.sbr"
".\temp\ARMV4RelSP\MortScript\DlgSelDir.sbr"
".\temp\ARMV4RelSP\MortScript\DlgSelFile.sbr"
".\temp\ARMV4RelSP\MortScript\DlgWait.sbr"
".\temp\ARMV4RelSP\MortScript\Interpreter.sbr"
".\temp\ARMV4RelSP\MortScript\FctFlight.sbr"
".\temp\ARMV4RelSP\MortScript\Helpers.sbr"
".\temp\ARMV4RelSP\MortScript\IniFile.sbr"
".\temp\ARMV4RelSP\MortScript\variables.sbr"
".\temp\ARMV4RelSP\MortScript\Commands.sbr"
".\temp\ARMV4RelSP\MortScript\CommandsData.sbr"
".\temp\ARMV4RelSP\MortScript\CommandsDialogs.sbr"
".\temp\ARMV4RelSP\MortScript\CommandsFileReg.sbr"
".\temp\ARMV4RelSP\MortScript\CommandsSystem.sbr"
".\temp\ARMV4RelSP\MortScript\CommandsWindows.sbr"
".\temp\ARMV4RelSP\MortScript\Functions.sbr"
".\temp\ARMV4RelSP\MortScript\FunctionsData.sbr"
".\temp\ARMV4RelSP\MortScript\FunctionsDialogs.sbr"
".\temp\ARMV4RelSP\MortScript\FunctionsFileReg.sbr"
".\temp\ARMV4RelSP\MortScript\FunctionsSystem.sbr"
".\temp\ARMV4RelSP\MortScript\FunctionsWindows.sbr"
".\temp\ARMV4RelSP\MortScript\CommandsStatus.sbr"
".\temp\ARMV4RelSP\MortScript\DlgStatus.sbr"]
Creating command line "bscmake.exe @C:\DOCUME~1\jwz\LOCALS~1\Temp\RSP1D6.tmp"
Creating browse info file...
<h3>Output Window</h3>




<h3>Results</h3>
JScripts.exe - 0 error(s), 0 warning(s)
</pre>
</body>
</html>

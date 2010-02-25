<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: MortRunnerSP - Win32 (WCE ARMV4) Release SP jwz--------------------
</h3>
<h3>Command Lines</h3>
Creating command line "rc.exe /l 0x409 /fo"temp/ARMV4RelSP/MortScript/MortRunnerSP.res" /i "SmartPhone" /d UNDER_CE=420 /d _WIN32_WCE=420 /d "NDEBUG" /d "UNICODE" /d "_UNICODE" /d "WIN32_PLATFORM_WFSP=200" /d "ARM" /d "_ARM_" /d "ARMV4" /r "D:\ROM\Mort-1010\jscripts\MortScript\SmartPhone\MortRunnerSP.rc"" 
Creating temporary file "C:\DOCUME~1\jwz\LOCALS~1\Temp\RSP3FF.tmp" with contents
[
/nologo /Oxs /I "..\MortTools\Types" /D "SMARTPHONE" /D _WIN32_WCE=300 /D WIN32_PLATFORM_PSPC=310 /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=300 /D "UNICODE" /D "_UNICODE" /D "JWZ" /Fr"temp/ARMV4RelSP/MortScript/" /Fp"temp/ARMV4RelSP/MortScript/MortScriptSP.pch" /YX /Fo"temp/ARMV4RelSP/MortScript/" /D /MC /c 
"D:\ROM\Mort-1010\jscripts\MortScript\SmartPhone\MortRunnerSP.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\ParseCmdLine.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\SmartPhone\DlgBigMessage.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\SmartPhone\DlgChoice.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\SmartPhone\DlgInput.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\SmartPhone\DlgSelDir.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\SmartPhone\DlgSelFile.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\SmartPhone\DlgWait.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\Interpreter.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\FctFlight.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\Helpers.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\IniFile.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\variables.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\Commands.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\CommandsData.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\CommandsDialogs.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\CommandsFileReg.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\CommandsSystem.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\CommandsWindows.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\Functions.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\FunctionsData.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\FunctionsDialogs.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\FunctionsFileReg.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\FunctionsSystem.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\FunctionsWindows.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\CommandsStatus.cpp"
"D:\ROM\Mort-1010\jscripts\MortScript\DlgStatus.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\jwz\LOCALS~1\Temp\RSP3FF.tmp" 
Creating temporary file "C:\DOCUME~1\jwz\LOCALS~1\Temp\RSP400.tmp" with contents
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
Creating command line "link.exe @C:\DOCUME~1\jwz\LOCALS~1\Temp\RSP400.tmp"
<h3>Output Window</h3>
Compiling resources...
Compiling...
MortRunnerSP.cpp
ParseCmdLine.cpp
DlgBigMessage.cpp
DlgChoice.cpp
DlgInput.cpp
DlgSelDir.cpp
DlgSelFile.cpp
DlgWait.cpp
Interpreter.cpp
FctFlight.cpp
Helpers.cpp
IniFile.cpp
variables.cpp
Commands.cpp
CommandsData.cpp
CommandsDialogs.cpp
CommandsFileReg.cpp
CommandsSystem.cpp
CommandsWindows.cpp
Functions.cpp
FunctionsData.cpp
FunctionsDialogs.cpp
FunctionsFileReg.cpp
FunctionsSystem.cpp
FunctionsWindows.cpp
CommandsStatus.cpp
DlgStatus.cpp
Linking...




<h3>Results</h3>
JScripts.exe - 0 error(s), 0 warning(s)
</pre>
</body>
</html>

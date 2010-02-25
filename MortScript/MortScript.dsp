# Microsoft Developer Studio Project File - Name="MortScript" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MortScript - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MortScript.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MortScript.mak" CFG="MortScript - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MortScript - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MortScript - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MortScript - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "exe\ReleaseWin"
# PROP Intermediate_Dir "temp\ReleaseWin\MortScript"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\MortTools\types" /D "DESKTOP" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Version.lib winmm.lib ..\MortTools\lib\ReleaseWin\types.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "MortScript - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "exe\DebugWin"
# PROP Intermediate_Dir "temp\DebugWin\MortScript"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\MortTools\types" /D "DESKTOP" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "JWZ" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Version.lib winmm.lib ..\MortTools\lib\DebugWin\types.lib Kernel32.lib user32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"exe\DebugWin/jScripts.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MortScript - Win32 Release"
# Name "MortScript - Win32 Debug"
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VC6\MortScript.rc
# End Source File
# Begin Source File

SOURCE=.\MortScriptApp.cpp
# End Source File
# Begin Source File

SOURCE=.\MortScriptApp.h
# End Source File
# Begin Source File

SOURCE=.\ParseCmdLine.cpp
# End Source File
# Begin Source File

SOURCE=.\ParseCmdLine.h
# End Source File
# Begin Source File

SOURCE=.\VC6\Resource.h
# End Source File
# Begin Source File

SOURCE=.\VC6\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\VC6\StdAfx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\VC6\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\VC6\res\MortScript.ico
# End Source File
# Begin Source File

SOURCE=.\VC6\res\runfile1.ico
# End Source File
# Begin Source File

SOURCE=.\VC6\res\runner.ico
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DirTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DirTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\DlgBigMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBigMessage.h
# End Source File
# Begin Source File

SOURCE=.\DlgChoice.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgChoice.h
# End Source File
# Begin Source File

SOURCE=.\DlgDownload.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDownload.h
# End Source File
# Begin Source File

SOURCE=.\DlgInput.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInput.h
# End Source File
# Begin Source File

SOURCE=.\DlgSelDir.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSelDir.h
# End Source File
# Begin Source File

SOURCE=.\DlgStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgStatus.h
# End Source File
# Begin Source File

SOURCE=.\DlgWait.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWait.h
# End Source File
# End Group
# Begin Group "Commands"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Commands.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandsData.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandsData.h
# End Source File
# Begin Source File

SOURCE=.\CommandsDialogs.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandsDialogs.h
# End Source File
# Begin Source File

SOURCE=.\CommandsFileReg.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandsFileReg.h
# End Source File
# Begin Source File

SOURCE=.\CommandsStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandsStatus.h
# End Source File
# Begin Source File

SOURCE=.\CommandsSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandsSystem.h
# End Source File
# Begin Source File

SOURCE=.\CommandsWindows.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandsWindows.h
# End Source File
# End Group
# Begin Group "Functions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Functions.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionsData.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionsData.h
# End Source File
# Begin Source File

SOURCE=.\FunctionsDialogs.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionsDialogs.h
# End Source File
# Begin Source File

SOURCE=.\FunctionsFileReg.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionsFileReg.h
# End Source File
# Begin Source File

SOURCE=.\FunctionsSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionsSystem.h
# End Source File
# Begin Source File

SOURCE=.\FunctionsWindows.cpp
# End Source File
# Begin Source File

SOURCE=.\FunctionsWindows.h
# End Source File
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interpreter.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpreter.h
# End Source File
# End Group
# Begin Group "Helpers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\Helpers.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\variables.cpp
# End Source File
# Begin Source File

SOURCE=.\variables.h
# End Source File
# End Group
# End Target
# End Project

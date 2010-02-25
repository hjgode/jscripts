# Microsoft Developer Studio Project File - Name="Types" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Types - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Types.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Types.mak" CFG="Types - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Types - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Types - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Types - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\ReleaseWin"
# PROP Intermediate_Dir "..\temp\ReleaseWin\Types"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "DESKTOP" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Types - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\DebugWin"
# PROP Intermediate_Dir "..\temp\DebugWin\Types"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "DESKTOP" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Types - Win32 Release"
# Name "Types - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MapIntToInt.cpp
# End Source File
# Begin Source File

SOURCE=.\MapMortStringToInt.cpp
# End Source File
# Begin Source File

SOURCE=.\MapMortStringToPtr.cpp
# End Source File
# Begin Source File

SOURCE=.\MapMortStringToString.cpp
# End Source File
# Begin Source File

SOURCE=.\MapMortStringToValue.cpp
# End Source File
# Begin Source File

SOURCE=.\MortPtrArray.cpp
# End Source File
# Begin Source File

SOURCE=.\MortString.cpp
# End Source File
# Begin Source File

SOURCE=.\MortStringArray.cpp
# End Source File
# Begin Source File

SOURCE=.\MortUIntArray.cpp
# End Source File
# Begin Source File

SOURCE=.\Value.cpp
# End Source File
# Begin Source File

SOURCE=.\ValueArray.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MapIntToInt.h
# End Source File
# Begin Source File

SOURCE=.\MapMortStringToInt.h
# End Source File
# Begin Source File

SOURCE=.\MapMortStringToPtr.h
# End Source File
# Begin Source File

SOURCE=.\MapMortStringToString.h
# End Source File
# Begin Source File

SOURCE=.\MapMortStringToValue.h
# End Source File
# Begin Source File

SOURCE=.\mortafx.h
# End Source File
# Begin Source File

SOURCE=.\MortPtrArray.h
# End Source File
# Begin Source File

SOURCE=.\MortString.h
# End Source File
# Begin Source File

SOURCE=.\MortStringArray.h
# End Source File
# Begin Source File

SOURCE=.\MortTypes.h
# End Source File
# Begin Source File

SOURCE=.\MortUIntArray.h
# End Source File
# Begin Source File

SOURCE=.\Value.h
# End Source File
# Begin Source File

SOURCE=.\ValueArray.h
# End Source File
# End Group
# End Target
# End Project

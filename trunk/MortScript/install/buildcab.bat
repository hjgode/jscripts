@echo off
REM
REM Sample batch file used to compile the CabWiz INF file
REM
REM
REM Replace <CabWiz-Path> with the full path to the CabWiz.exe file
REM Replace <INF-Path> with the full path to the Sample.INF file
REM

upx --compress-icons=0 ..\exe\ARMV4Rel\MortScript.exe
upx --compress-icons=0 ..\exe\ARMV4RelPNA\MortScript.exe
upx --compress-icons=0 ..\exe\ARMV4RelSP\MortScript.exe

rem touch -t200710310606.06 ..\exe\*\*.*
touch ..\exe\*\*.*

rem touch -t200710310606.06 bin\*
rem touch -t200710310606.06 bin\*\*.*
touch bin\*
touch bin\*\*.*

..\..\MortTools\cabwiz\CabWiz "MortScript-PPC.inf" /err err-ppc.log /cpu ARM
..\..\MortTools\cabwiz\CabWizSP "MortScript-SP.inf" /err err-sp.log /cpu ARM
..\..\MortTools\cabwiz\CabWiz "MortScript-PNA.inf" /err err-pna.log /cpu ARM

rem touch -t200710310606.06 MortScript*.cab
touch MortScript*.cab

..\..\MortTools\cabwiz\ezsetup -l english -i MortScript-PPC.ini -r MortScript.txt -e eula-en.txt -o MortScript-PPC.exe
..\..\MortTools\cabwiz\ezsetup -l english -i MortScript-SP.ini -r MortScript.txt -e eula-en.txt -o MortScript-SP.exe
..\..\MortTools\cabwiz\ezsetup -l english -i MortScript-PNA.ini -r MortScript.txt -e eula-en.txt -o MortScript-PNA.exe

rem touch -t200710310606.06 MortScript*.exe
touch MortScript*.exe

del *.tmp 2> NUL

mkdir setup 2> NUL
mkdir cab 2> NUL
mkdir bin 2> NUL

if "%1"=="" GOTO end
del MortScript-%1.zip 2> NUL
del setup\MortScript-%1-*.exe 2> NUL
del cab\MortScript-%1-*.cab 2> NUL
move MortScript-PPC.exe setup\MortScript-%1-PPC.exe
move MortScript-SP.exe  setup\MortScript-%1-SP.exe
move MortScript-PNA.exe setup\MortScript-%1-PNA.exe
move MortScript-PPC.arm.cab cab\MortScript-%1-PPC.cab
move MortScript-SP.arm.cab  cab\MortScript-%1-SP.cab
move MortScript-PNA.arm.cab cab\MortScript-%1-PNA.cab
rem c:\programme\winrar\rar a MortScript-%1-Setup.zip MortScript-%1-*.exe
rem c:\programme\winrar\rar a MortScript-%1-CABs.zip MortScript-%1-*.cab
c:\programme\winrar\winrar a MortScript-%1.zip setup\MortScript-%1-*.exe cab\MortScript-%1-*.cab
xcopy /y /i ..\exe\ARMV4Rel\*.exe    bin\PPC
xcopy /y /i ..\exe\ARMV4Rel\*.dll    bin\PPC
xcopy /y /i ..\exe\ARMV4RelPNA\*.exe bin\PNA
xcopy /y /i ..\exe\ARMV4RelPNA\*.dll bin\PNA
rem xcopy /y /i ..\ARMV4RelPNA\autorun.exe    bin\PNA
xcopy /y /i ..\exe\ARMV4RelSP\*.exe  bin\SP
xcopy /y /i ..\exe\ARMV4RelSP\*.dll  bin\SP
xcopy /y /i ..\..\MortTools\exe\ARMV4Rel\mortzip.dll  bin\PPC
xcopy /y /i ..\..\MortTools\exe\ARMV4Rel\mortzip.dll  bin\PNA
xcopy /y /i ..\..\MortTools\exe\ARMV4RelSP\mortzip.dll bin\SP
xcopy /y /i ..\exe\ReleaseWin\MortScript.exe  bin\PC
xcopy /y /i ..\exe\ReleaseWin\AutoRun.exe  bin\PC
xcopy /y /i ..\..\MortTools\exe\ReleaseWin\MortZip.dll  bin\PC

rem touch -t200710310606.06 bin\PC\*.*
rem touch -t200710310606.06 MortScript-Anleitung.pdf
rem touch -t200710310606.06 MortScript-Manual.pdf

touch bin\PC\*.*
touch MortScript-Anleitung.pdf
touch MortScript-Manual.pdf

c:\programme\winrar\winrar a -r MortScript-%1.zip bin\*.*
c:\programme\winrar\winrar a MortScript-%1.zip contents.txt readme.txt liesmich.txt commands.txt MortScript-Manual.pdf MortScript-Anleitung.pdf
c:\programme\winrar\winrar c -zcontents.txt MortScript-%1.zip

:end

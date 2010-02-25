<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: Types - Win32 (WCE ARMV4) Release SP--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\jwz\LOCALS~1\Temp\RSPA07.tmp" with contents
[
/nologo /W3 /D _WIN32_WCE=420 /D "WIN32_PLATFORM_WFSP=200" /D "NDEBUG" /D "ARM" /D "_ARM_" /D "ARMV4" /D UNDER_CE=420 /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"..\temp\ARMV4RelSP\Types/Types.pch" /YX /Fo"..\temp\ARMV4RelSP\Types/" /O2 /MC /c 
"D:\ROM\Mort-1010\MortTools\Types\MortString.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\jwz\LOCALS~1\Temp\RSPA07.tmp" 
Creating temporary file "C:\DOCUME~1\jwz\LOCALS~1\Temp\RSPA08.tmp" with contents
[
/nologo /out:"..\Types.lib" 
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\MapIntToInt.obj"
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\MapMortStringToInt.obj"
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\MapMortStringToPtr.obj"
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\MapMortStringToString.obj"
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\MapMortStringToValue.obj"
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\MortPtrArray.obj"
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\MortString.obj"
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\MortStringArray.obj"
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\MortUIntArray.obj"
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\Value.obj"
"\ROM\Mort-1010\MortTools\temp\ARMV4RelSP\Types\ValueArray.obj"
]
Creating command line "link.exe -lib @C:\DOCUME~1\jwz\LOCALS~1\Temp\RSPA08.tmp"
<h3>Output Window</h3>
Compiling...
MortString.cpp
Creating library...
Signing D:\ROM\Mort-1010\MortTools\Types.lib
Error: Signing Failed.  Result = 800b0003, (-2146762749)




<h3>Results</h3>
Types.lib - 0 error(s), 0 warning(s)
</pre>
</body>
</html>

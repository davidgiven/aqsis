# Microsoft Developer Studio Project File - Name="aqslcomp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=aqslcomp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "aqslcomp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "aqslcomp.mak" CFG="aqslcomp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "aqslcomp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "aqslcomp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "aqslcomp - Win32 Profile" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "aqslcomp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Library\Release"
# PROP Intermediate_Dir "..\Object\Release\aqslcomp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Render" /I "..\libslparse" /I "..\libcodegenvm" /I "..\libaqsistypes" /I "..\libaqsistypes\win32\intel" /I "..\libargparse" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "AQSIS_DYNAMIC_LINK" /D _qBUILDING=BUILD_AQSLCOMP /D BUILD_AQSLCOMP=1 /FR /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 libargparse.lib log4cpp.lib libcodegenvm.lib libslparse.lib libaqsistypes.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"..\Library\Release"

!ELSEIF  "$(CFG)" == "aqslcomp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Library\Debug"
# PROP Intermediate_Dir "..\Object\Debug\aqslcomp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\Render" /I "..\libslparse" /I "..\libcodegenvm" /I "..\libaqsistypes" /I "..\libaqsistypes\win32\intel" /I "..\libargparse" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "AQSIS_DYNAMIC_LINK" /D _qBUILDING=BUILD_AQSLCOMP /D BUILD_AQSLCOMP=1 /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libargparse.lib log4cpp.lib libcodegenvm.lib libslparse.lib libaqsistypes.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\Library\Debug"

!ELSEIF  "$(CFG)" == "aqslcomp - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "aqslcomp___Win32_Profile"
# PROP BASE Intermediate_Dir "aqslcomp___Win32_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Library\Profile"
# PROP Intermediate_Dir "..\Object\Profile\aqslcomp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Render" /I "..\libaqsistypes" /I "..\libaqsistypes\win32\intel" /I "..\libslparse" /I "..\libcodegenvm" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "AQSIS_DYNAMIC_LINK" /D _qBUILDING=BUILD_AQSLCOMP /D BUILD_AQSLCOMP=1 /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "..\Render" /I "..\libslparse" /I "..\libcodegenvm" /I "..\libaqsistypes" /I "..\libaqsistypes\win32\intel" /I "..\libargparse" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "AQSIS_DYNAMIC_LINK" /D _qBUILDING=BUILD_AQSLCOMP /D BUILD_AQSLCOMP=1 /FR /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libargparse.lib log4cpp.lib libcodegenvm.lib libslparse.lib libaqsistypes.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"..\Library\Release"
# ADD LINK32 libargparse.lib log4cpp.lib libcodegenvm.lib libslparse.lib libaqsistypes.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /debug /machine:I386 /libpath:"..\Library\Profile"

!ENDIF 

# Begin Target

# Name "aqslcomp - Win32 Release"
# Name "aqslcomp - Win32 Debug"
# Name "aqslcomp - Win32 Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\aqslcomp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

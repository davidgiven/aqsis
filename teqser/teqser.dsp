# Microsoft Developer Studio Project File - Name="teqser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=teqser - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "teqser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "teqser.mak" CFG="teqser - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "teqser - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "teqser - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "teqser - Win32 Profile" (based on "Win32 (x86) Console Application")
!MESSAGE "teqser - Win32 MPatrol" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "teqser - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Library\Release"
# PROP Intermediate_Dir "..\Object\Release\teqser"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Object\Release\ribber" /I "..\Object\Release\render" /I "..\Render" /I "..\libaqsistypes" /I "..\libaqsistypes\win32\intel" /I "..\libargparse" /D "_CONSOLE" /D "_MBCS" /D "PLUGINS" /D "AQSIS_DYNAMIC_LINK" /D _qBUILDING=BUILD_TEQSER /D BUILD_TEQSER=1 /D "NDEBUG" /D "WIN32" /FR /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 libargparse.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /libpath:"..\Library\Release"

!ELSEIF  "$(CFG)" == "teqser - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Library\Debug"
# PROP Intermediate_Dir "..\Object\Debug\teqser"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\Object\Debug\ribber" /I "..\Object\Debug\render" /I "..\Render" /I "..\libaqsistypes" /I "..\libaqsistypes\win32\intel" /I "..\libargparse" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "PLUGINS" /D "WIN32" /D "AQSIS_DYNAMIC_LINK" /D _qBUILDING=BUILD_TEQSER /D BUILD_TEQSER=1 /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libargparse.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcd.lib" /pdbtype:sept /libpath:"..\Library\Debug"

!ELSEIF  "$(CFG)" == "teqser - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "teqser___Win32_Profile"
# PROP BASE Intermediate_Dir "teqser___Win32_Profile"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Library\Profile"
# PROP Intermediate_Dir "..\Object\Profile\teqser"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "..\Object\Release\ribber" /I "..\Object\Release\render" /I "..\Render" /I "..\libaqsistypes" /I "..\libaqsistypes\win32\intel" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "PLUGINS" /D "WIN32" /D "AQSIS_DYNAMIC_LINK" /D _qBUILDING=BUILD_TEQSER /D BUILD_TEQSER=1 /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "..\Object\Release\ribber" /I "..\Object\Release\render" /I "..\Render" /I "..\libaqsistypes" /I "..\libaqsistypes\win32\intel" /I "..\libargparse" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "PLUGINS" /D "WIN32" /D "AQSIS_DYNAMIC_LINK" /D _qBUILDING=BUILD_TEQSER /D BUILD_TEQSER=1 /FR /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libargparse.lib log4cpp.lib libz.lib librib2.lib librib2ri.lib libshadervm.lib libshaderexecenv.lib libddmsimple.lib libtiff.lib libaqsistypes.lib libaqsis.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /libpath:"..\Library\Release"
# ADD LINK32 libargparse.lib /nologo /subsystem:console /profile /debug /machine:I386 /nodefaultlib:"libc.lib" /libpath:"..\Library\Profile"

!ELSEIF  "$(CFG)" == "teqser - Win32 MPatrol"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "teqser___Win32_MPatrol"
# PROP BASE Intermediate_Dir "teqser___Win32_MPatrol"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Library\MPatrol"
# PROP Intermediate_Dir "..\Object\MPatrol\teqser"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "..\Object\Release\ribber" /I "..\Object\Release\render" /I "..\Render" /I "..\libaqsistypes" /I "..\libaqsistypes\win32\intel" /I "..\libargparse" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "PLUGINS" /D "WIN32" /D "AQSIS_DYNAMIC_LINK" /D _qBUILDING=BUILD_TEQSER /D BUILD_TEQSER=1 /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "..\Object\Release\ribber" /I "..\Object\Release\render" /I "..\Render" /I "..\libaqsistypes" /I "..\libaqsistypes\win32\intel" /I "..\libargparse" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "PLUGINS" /D "WIN32" /D "AQSIS_DYNAMIC_LINK" /D _qBUILDING=BUILD_TEQSER /D BUILD_TEQSER=1 /FR /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libargparse.lib /nologo /subsystem:console /profile /debug /machine:I386 /nodefaultlib:"libc.lib" /libpath:"..\Library\Profile"
# ADD LINK32 libargparse.lib /nologo /subsystem:console /profile /debug /machine:I386 /nodefaultlib:"libc.lib" /libpath:"..\Library\MPatrol"

!ENDIF 

# Begin Target

# Name "teqser - Win32 Release"
# Name "teqser - Win32 Debug"
# Name "teqser - Win32 Profile"
# Name "teqser - Win32 MPatrol"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\teqser.cpp
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

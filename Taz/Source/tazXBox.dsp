# Microsoft Developer Studio Project File - Name="tazXBox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=tazXBox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tazXBox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tazXBox.mak" CFG="tazXBox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tazXBox - Xbox Release" (based on "Xbox Application")
!MESSAGE "tazXBox - Xbox Debug" (based on "Xbox Application")
!MESSAGE "tazXBox - Xbox Pseudo Release" (based on "Xbox Application")
!MESSAGE "tazXBox - Xbox Consumer Demo" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TeamPhoenix/Taz/sourceXBOX", FLEAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe

!IF  "$(CFG)" == "tazXBox - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /I "C:\babel\xbox\include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6 /OPT:REF
# ADD LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib "C:\program files\microsoft xbox sdk\xbox\lib\wmvdec.lib" "C:\binkxbox\binkxbox.lib" /nologo /incremental:yes /map /machine:I386 /out:"Release/tazXBoxR.exe" /subsystem:xbox /fixed:no /tmp /OPT:REF
# SUBTRACT LINK32 /pdb:none /debug
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000
# ADD XBE /nologo /testid:"0x4947000D" /testname:"Taz: Wanted" /stack:0x40000 /initflags:0x0 /debug /out:"Release/tazXBoxR.xbe" /limitmem /titleinfo:"C:\taz\xboxinfo.txt" /titleimage:"C:\taz\xboxtitle.bmp" /testratings:4 /defaultsaveimage:"C:\taz\xboxsave.bmp"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying packages
PostBuild_Cmds=xbcp /d /y c:\taz\paks\*.xbp  xe:\xdemos\application	xbcp /d /y c:\taz\source\streams.xbp  xe:\xdemos\application	xbcp /d /y c:\taz\source\splshscr.xbp  xe:\xdemos\application	xbcp /d /y c:\taz\source\text.xbp  xe:\xdemos\application
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /Gi /GX /Zi /Od /I "C:\babel\xbox\include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_TAZDEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib "C:\program files\microsoft xbox sdk\xbox\lib\wmvdecd.lib" /nologo /map /debug /machine:I386 /out:"Debug/tazXBoxD.exe" /subsystem:xbox /fixed:no /tmp
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /testid:"0x4947000D" /testname:"Taz: Wanted [debug]" /stack:0x40000 /initflags:0x0 /debug /out:"Debug/tazXBoxD.xbe" /limitmem /titleinfo:"C:\taz\xboxinfo.txt" /titleimage:"C:\taz\xboxtitle.bmp" /testratings:4 /defaultsaveimage:"C:\taz\xboxsave.bmp"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copyin pakijes!
PostBuild_Cmds=xbcp /d /y c:\taz\paks\*.xbp  xe:\xdemos\application	xbcp /d /y c:\taz\source\streams.xbp  xe:\xdemos\application	xbcp /d /y c:\taz\source\splshscr.xbp  xe:\xdemos\application	xbcp /d /y c:\taz\source\text.xbp  xe:\xdemos\application
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "tazXBox___Xbox_Pseudo_Release"
# PROP BASE Intermediate_Dir "tazXBox___Xbox_Pseudo_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Pseudo"
# PROP Intermediate_Dir "Pseudo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "C:\babel\xbox\include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /W3 /Gm /Gi /GX /Zi /Od /Gy /I "C:\babel\xbox\include" /D "WIN32" /D "_XBOX" /D "_NDEBUG" /YX /FD /G6 /Zvc6 /c
# SUBTRACT CPP /nologo /Fr
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /debug /machine:I386 /out:"Debug/tazXBoxD.exe" /subsystem:xbox /fixed:no /debugtype:vc6
# SUBTRACT BASE LINK32 /profile
# ADD LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /debug /machine:I386 /out:"Pseudo/tazXBoxP.exe" /subsystem:xbox /fixed:no /debugtype:vc6
# SUBTRACT LINK32 /profile
XBE=imagebld.exe
# ADD BASE XBE /nologo /testname:"Taz: Wanted [debug]" /stack:0x80000 /debug /out:"Debug/tazXBoxD.xbe"
# ADD XBE /nologo /testname:"Taz: Wanted [pseudo-release]" /stack:0x80000 /debug /out:"Pseudo/tazXBoxP.xbe" /titleinfo:"C:\taz\xboxinfo.txt" /titleimage:"C:\taz\xboxtitle.bmp" /defaultsaveimage:"C:\taz\xboxtitle.bmp"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying packages
PostBuild_Cmds=xbcp /d /y c:\taz\paks\*.xbp  xe:\tazxbox	xbcp /d /y c:\taz\source\streams.xbp  xe:\tazxbox	xbcp /d /y c:\taz\source\splshscr.xbp  xe:\tazxbox	xbcp /d /y c:\taz\source\text.xbp  xe:\tazxbox
# End Special Build Tool

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "tazXBox___Xbox_Consumer_Demo"
# PROP BASE Intermediate_Dir "tazXBox___Xbox_Consumer_Demo"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "tazXBox___Xbox_Consumer_Demo"
# PROP Intermediate_Dir "tazXBox___Xbox_Consumer_Demo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zi /Od /I "C:\babel\xbox\include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /GX /Zi /Od /I "C:\babel\xbox\include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "CONSUMERDEMO" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib "C:\program files\microsoft xbox sdk\xbox\lib\wmvdec.lib" "C:\binkxbox\binkxbox.lib" /nologo /incremental:yes /debug /machine:I386 /out:"Release/tazXBoxR.exe" /subsystem:xbox /fixed:no /debugtype:vc6 /OPT:REF
# SUBTRACT BASE LINK32 /map
# ADD LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib "C:\program files\microsoft xbox sdk\xbox\lib\wmvdec.lib" "C:\binkxbox\binkxbox.lib" /nologo /incremental:yes /machine:I386 /out:"Release/tazdemo.exe" /subsystem:xbox /fixed:no /tmp /OPT:REF
# SUBTRACT LINK32 /pdb:none /debug
XBE=imagebld.exe
# ADD BASE XBE /nologo /testid:"0" /testname:"Taz: Wanted [release]" /stack:0x10000 /initflags:0x0 /debug /out:"Release/tazXBoxR.xbe" /titleinfo:"C:\taz\xboxinfo.txt" /titleimage:"C:\taz\xboxtitle.bmp" /testratings:5 /defaultsaveimage:"C:\taz\xboxsave.bmp"
# ADD XBE /nologo /testid:"0" /testname:"Taz: Wanted [demo]" /stack:0x10000 /initflags:0x3 /out:"Release/tazXBoxR.xbe" /titleinfo:"C:\taz\xboxinfo.txt" /testratings:5 /dontmodifyhd
# SUBTRACT XBE /debug
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying packages
PostBuild_Cmds=xbcp /d /y c:\taz\source\text.xbp  xe:\xdemos\application
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "tazXBox - Xbox Release"
# Name "tazXBox - Xbox Debug"
# Name "tazXBox - Xbox Pseudo Release"
# Name "tazXBox - Xbox Consumer Demo"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Babel Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\debug\bActor\bActor.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\debug\bGui\bGui.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\debug\bDisplay\bDisplay.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\debug\bKernel\bKernel.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\debug\bInput\bInput.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\debug\bMaths\bMaths.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\debug\bSound\bSound.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Babel Release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\release\bActor\bActor.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\release\bDisplay\bDisplay.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\release\bGui\bGui.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\release\bInput\bInput.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\release\bKernel\bKernel.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\release\bMaths\bMaths.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Babel\Xbox\lib\release\bSound\bSound.lib

!IF  "$(CFG)" == "tazXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Pseudo Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tazXBox - Xbox Consumer Demo"

!ENDIF 

# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "Character AI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\alligator.cpp
# End Source File
# Begin Source File

SOURCE=.\anvilgang.cpp
# End Source File
# Begin Source File

SOURCE=.\apes.cpp
# End Source File
# Begin Source File

SOURCE=.\bears.cpp
# End Source File
# Begin Source File

SOURCE=.\bjshellac.cpp
# End Source File
# Begin Source File

SOURCE=.\buzzard.cpp
# End Source File
# Begin Source File

SOURCE=.\buzzardscripts.cpp
# End Source File
# Begin Source File

SOURCE=.\constructionbot.cpp
# End Source File
# Begin Source File

SOURCE=.\constructionworker.cpp
# End Source File
# Begin Source File

SOURCE=.\doggy.cpp
# End Source File
# Begin Source File

SOURCE=.\genericai.cpp
# End Source File
# Begin Source File

SOURCE=.\minerbot.cpp
# End Source File
# Begin Source File

SOURCE=.\museumguard.cpp
# End Source File
# Begin Source File

SOURCE=.\nastycan.cpp
# End Source File
# Begin Source File

SOURCE=.\securitybot.cpp
# End Source File
# Begin Source File

SOURCE=.\shark.cpp
# End Source File
# Begin Source File

SOURCE=.\taskforce.cpp
# End Source File
# Begin Source File

SOURCE=.\tazcatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\tribalchief.cpp
# End Source File
# Begin Source File

SOURCE=.\tweety.cpp
# End Source File
# Begin Source File

SOURCE=.\zookeeper.cpp
# End Source File
# End Group
# Begin Group "Environment"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\breakables.cpp
# End Source File
# Begin Source File

SOURCE=.\bumper.cpp
# End Source File
# Begin Source File

SOURCE=.\clangerreset.cpp
# End Source File
# Begin Source File

SOURCE=.\destructibles.cpp
# End Source File
# Begin Source File

SOURCE=.\doors.cpp
# End Source File
# Begin Source File

SOURCE=.\escalator.cpp
# End Source File
# Begin Source File

SOURCE=.\fallingobjects.cpp
# End Source File
# Begin Source File

SOURCE=.\famethrowa.cpp
# End Source File
# Begin Source File

SOURCE=.\floorpolisherwantedposter.cpp
# End Source File
# Begin Source File

SOURCE=.\foodrandomiser.cpp
# End Source File
# Begin Source File

SOURCE=.\fountain.cpp
# End Source File
# Begin Source File

SOURCE=.\fountainextrasystem.cpp
# End Source File
# Begin Source File

SOURCE=.\ice.cpp
# End Source File
# Begin Source File

SOURCE=.\lava.cpp
# End Source File
# Begin Source File

SOURCE=.\magnet.cpp
# End Source File
# Begin Source File

SOURCE=.\mailbox.cpp
# End Source File
# Begin Source File

SOURCE=.\musicandsavagebeasts.cpp
# End Source File
# Begin Source File

SOURCE=.\phonebox.cpp
# End Source File
# Begin Source File

SOURCE=.\portal.cpp
# End Source File
# Begin Source File

SOURCE=.\projectile.cpp
# End Source File
# Begin Source File

SOURCE=.\scenicobjects.cpp
# End Source File
# Begin Source File

SOURCE=.\slides.cpp
# End Source File
# Begin Source File

SOURCE=.\speedboost.cpp
# End Source File
# Begin Source File

SOURCE=.\splatobjects.cpp
# End Source File
# Begin Source File

SOURCE=.\springyobjects.cpp
# End Source File
# Begin Source File

SOURCE=.\steppingstone.cpp
# End Source File
# Begin Source File

SOURCE=.\swingingobjects.cpp
# End Source File
# Begin Source File

SOURCE=.\tiltingobjects.cpp
# End Source File
# Begin Source File

SOURCE=.\transporter.cpp
# End Source File
# Begin Source File

SOURCE=.\tumbleweed.cpp
# End Source File
# Begin Source File

SOURCE=.\wantedposter.cpp
# End Source File
# Begin Source File

SOURCE=.\water.cpp
# End Source File
# Begin Source File

SOURCE=.\zapper.cpp
# End Source File
# End Group
# Begin Group "Effects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aircon.cpp
# End Source File
# Begin Source File

SOURCE=.\arrow.cpp
# End Source File
# Begin Source File

SOURCE=.\cartoonscaling.cpp
# End Source File
# Begin Source File

SOURCE=.\cheesegun.cpp
# End Source File
# Begin Source File

SOURCE=.\clouds.cpp
# End Source File
# Begin Source File

SOURCE=.\costumes.cpp
# End Source File
# Begin Source File

SOURCE=.\decals.cpp
# End Source File
# Begin Source File

SOURCE=.\eating.cpp
# End Source File
# Begin Source File

SOURCE=.\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\electrocution.cpp
# End Source File
# Begin Source File

SOURCE=.\envmap.cpp
# End Source File
# Begin Source File

SOURCE=.\explode.cpp
# End Source File
# Begin Source File

SOURCE=.\fade.cpp
# End Source File
# Begin Source File

SOURCE=.\fire.cpp
# End Source File
# Begin Source File

SOURCE=.\Fireball.cpp
# End Source File
# Begin Source File

SOURCE=.\fireworks.cpp
# End Source File
# Begin Source File

SOURCE=.\Flies.cpp
# End Source File
# Begin Source File

SOURCE=.\flocking.cpp
# End Source File
# Begin Source File

SOURCE=.\footalign.cpp
# End Source File
# Begin Source File

SOURCE=.\Glare.cpp
# End Source File
# Begin Source File

SOURCE=.\glowaroundplinth.cpp
# End Source File
# Begin Source File

SOURCE=.\haar.cpp
# End Source File
# Begin Source File

SOURCE=.\halo.cpp
# End Source File
# Begin Source File

SOURCE=.\holysmoke.cpp
# End Source File
# Begin Source File

SOURCE=.\laser.cpp
# End Source File
# Begin Source File

SOURCE=.\lightning.cpp
# End Source File
# Begin Source File

SOURCE=.\lights.cpp
# End Source File
# Begin Source File

SOURCE=.\mirrorball.cpp
# End Source File
# Begin Source File

SOURCE=.\MonitorFX.cpp
# End Source File
# Begin Source File

SOURCE=.\phoneboxeffect.cpp
# End Source File
# Begin Source File

SOURCE=.\polyeffects.cpp
# End Source File
# Begin Source File

SOURCE=.\postcard.cpp
# End Source File
# Begin Source File

SOURCE=.\precipitation.cpp
# End Source File
# Begin Source File

SOURCE=.\rapperblast.cpp
# End Source File
# Begin Source File

SOURCE=.\rocketscience.cpp
# End Source File
# Begin Source File

SOURCE=.\rumble.cpp
# End Source File
# Begin Source File

SOURCE=.\securitybox.cpp
# End Source File
# Begin Source File

SOURCE=.\shadow.cpp
# End Source File
# Begin Source File

SOURCE=.\shatter.cpp
# End Source File
# Begin Source File

SOURCE=.\shockwave.cpp
# End Source File
# Begin Source File

SOURCE=.\sparks.cpp
# End Source File
# Begin Source File

SOURCE=.\spineffect.cpp
# End Source File
# Begin Source File

SOURCE=.\tarbubble.cpp
# End Source File
# Begin Source File

SOURCE=.\tartantaz.cpp
# End Source File
# Begin Source File

SOURCE=.\thoughtbubble.cpp
# End Source File
# Begin Source File

SOURCE=.\VideoFX.cpp
# End Source File
# Begin Source File

SOURCE=.\voodoocloud.cpp
# End Source File
# Begin Source File

SOURCE=.\voodoospelleffect.cpp
# End Source File
# Begin Source File

SOURCE=.\waterfall.cpp
# End Source File
# Begin Source File

SOURCE=.\waterrings.cpp
# End Source File
# Begin Source File

SOURCE=.\watersplash.cpp
# End Source File
# Begin Source File

SOURCE=.\zzz.cpp
# End Source File
# End Group
# Begin Group "Physics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\collision.cpp
# End Source File
# Begin Source File

SOURCE=.\collisioncache.cpp
# End Source File
# Begin Source File

SOURCE=.\physics.cpp
# End Source File
# End Group
# Begin Group "Vehicles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\atlasphere.cpp
# End Source File
# Begin Source File

SOURCE=.\floorpolisher.cpp
# End Source File
# Begin Source File

SOURCE=.\hovercraft.cpp
# End Source File
# Begin Source File

SOURCE=.\minecart.cpp
# End Source File
# Begin Source File

SOURCE=.\pneumaticplunger.cpp
# End Source File
# Begin Source File

SOURCE=.\rocketbike.cpp
# End Source File
# Begin Source File

SOURCE=.\trolley.cpp
# End Source File
# Begin Source File

SOURCE=.\vehicles.cpp
# End Source File
# End Group
# Begin Group "Special Case"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\icedomespecial.cpp
# End Source File
# Begin Source File

SOURCE=.\ResetTutorial.cpp
# End Source File
# Begin Source File

SOURCE=.\snowball.cpp
# End Source File
# Begin Source File

SOURCE=.\special.cpp
# End Source File
# Begin Source File

SOURCE=.\tutorial.cpp
# End Source File
# End Group
# Begin Group "System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\actors.cpp
# End Source File
# Begin Source File

SOURCE=.\animation.cpp
# End Source File
# Begin Source File

SOURCE=.\attach.cpp
# End Source File
# Begin Source File

SOURCE=.\characters.cpp
# End Source File
# Begin Source File

SOURCE=.\Cheats.cpp
# End Source File
# Begin Source File

SOURCE=.\chrisscene.cpp
# End Source File
# Begin Source File

SOURCE=.\claudescene.cpp
# End Source File
# Begin Source File

SOURCE=.\collectibles.cpp
# End Source File
# Begin Source File

SOURCE=.\Colour.cpp
# End Source File
# Begin Source File

SOURCE=.\control.cpp
# End Source File
# Begin Source File

SOURCE=.\countdown.cpp
# End Source File
# Begin Source File

SOURCE=.\debris.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\demo.cpp
# End Source File
# Begin Source File

SOURCE=.\display.cpp
# End Source File
# Begin Source File

SOURCE=.\endoflevelstats.cpp
# End Source File
# Begin Source File

SOURCE=.\events.cpp
# End Source File
# Begin Source File

SOURCE=.\files.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\gui.cpp
# End Source File
# Begin Source File

SOURCE=.\health.cpp
# End Source File
# Begin Source File

SOURCE=.\highscores.cpp
# End Source File
# Begin Source File

SOURCE=.\invisibleobjects.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkedList.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\map.cpp
# End Source File
# Begin Source File

SOURCE=.\mapfile.cpp
# End Source File
# Begin Source File

SOURCE=.\maths.cpp
# End Source File
# Begin Source File

SOURCE=.\MemCard.cpp
# End Source File
# Begin Source File

SOURCE=.\mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\music.cpp
# End Source File
# Begin Source File

SOURCE=.\objectviewer.cpp
# End Source File
# Begin Source File

SOURCE=.\ParseMap.cpp
# End Source File
# Begin Source File

SOURCE=.\paths.cpp
# End Source File
# Begin Source File

SOURCE=.\pause.cpp
# End Source File
# Begin Source File

SOURCE=.\PhilScene.cpp
# End Source File
# Begin Source File

SOURCE=.\player.cpp
# End Source File
# Begin Source File

SOURCE=.\playerstats.cpp
# End Source File
# Begin Source File

SOURCE=.\Rectangle.cpp
# End Source File
# Begin Source File

SOURCE=.\scene.cpp
# End Source File
# Begin Source File

SOURCE=.\sfx.cpp
# End Source File
# Begin Source File

SOURCE=.\sonystuff.cpp
# End Source File
# Begin Source File

SOURCE=.\splashscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\StartScene.cpp
# End Source File
# Begin Source File

SOURCE=.\status.cpp
# End Source File
# Begin Source File

SOURCE=.\subtitle.cpp
# End Source File
# Begin Source File

SOURCE=.\textures.cpp
# End Source File
# Begin Source File

SOURCE=.\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\tonyscene.cpp
# End Source File
# Begin Source File

SOURCE=.\trail.cpp
# End Source File
# Begin Source File

SOURCE=.\trigger.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\Vec.cpp
# End Source File
# Begin Source File

SOURCE=.\volume.cpp
# End Source File
# End Group
# Begin Group "Camera"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\cameracase.cpp
# End Source File
# Begin Source File

SOURCE=.\cutscene.cpp
# End Source File
# Begin Source File

SOURCE=.\KOScam.cpp
# End Source File
# Begin Source File

SOURCE=.\quakecam.cpp
# End Source File
# Begin Source File

SOURCE=.\smashcamera.cpp
# End Source File
# End Group
# Begin Group "Multiplayer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\destruct.cpp
# End Source File
# Begin Source File

SOURCE=.\diner.cpp
# End Source File
# Begin Source File

SOURCE=.\multiplayer.cpp
# End Source File
# Begin Source File

SOURCE=.\race.cpp
# End Source File
# Begin Source File

SOURCE=.\Tournament.cpp
# End Source File
# End Group
# Begin Group "Bosses"

# PROP Default_Filter ""
# Begin Group "mechatweety"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\chargeattack.cpp
# End Source File
# Begin Source File

SOURCE=.\crateattack.cpp
# End Source File
# Begin Source File

SOURCE=.\electricityattack.cpp
# End Source File
# Begin Source File

SOURCE=.\mechatweety.cpp
# End Source File
# Begin Source File

SOURCE=.\mtweetymagnet.cpp
# End Source File
# Begin Source File

SOURCE=.\mtweetyscripts.cpp
# End Source File
# Begin Source File

SOURCE=.\redshield.cpp
# End Source File
# Begin Source File

SOURCE=.\tazbossitems.cpp
# End Source File
# Begin Source File

SOURCE=.\transportereffect.cpp
# End Source File
# Begin Source File

SOURCE=.\tweetytransporter.cpp
# End Source File
# End Group
# Begin Group "WestBoss"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BrickDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\GolfBar.cpp
# End Source File
# Begin Source File

SOURCE=.\WestBoss.cpp
# End Source File
# Begin Source File

SOURCE=.\WestBossExp.cpp
# End Source File
# End Group
# Begin Group "ZooBoss"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ZooBoss.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Boss.cpp
# End Source File
# Begin Source File

SOURCE=.\bossgamecontinue.cpp
# End Source File
# Begin Source File

SOURCE=.\CityBoss.cpp
# End Source File
# Begin Source File

SOURCE=.\tazboss1.cpp
# End Source File
# End Group
# Begin Group "Power-ups"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bubblegum.cpp
# End Source File
# Begin Source File

SOURCE=.\chillipepper.cpp
# End Source File
# Begin Source File

SOURCE=.\hiccup.cpp
# End Source File
# Begin Source File

SOURCE=.\invisibletaz.cpp
# End Source File
# Begin Source File

SOURCE=.\superburp.cpp
# End Source File
# Begin Source File

SOURCE=.\superburpeffect.cpp
# End Source File
# End Group
# Begin Group "frontend"

# PROP Default_Filter ""
# Begin Group "Menu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\audiomenu.cpp
# End Source File
# Begin Source File

SOURCE=.\extrasmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\frontendmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\gallerymenu.cpp
# End Source File
# Begin Source File

SOURCE=.\optionsmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\recordsmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\secretsmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\startgamemenu.cpp
# End Source File
# Begin Source File

SOURCE=.\twoplayermenu.cpp
# End Source File
# End Group
# Begin Group "Book"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BookList.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckBox.cpp
# End Source File
# Begin Source File

SOURCE=.\HealthBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Icon.cpp
# End Source File
# Begin Source File

SOURCE=.\PageItemFX.cpp
# End Source File
# Begin Source File

SOURCE=.\PaperBook.cpp
# End Source File
# Begin Source File

SOURCE=.\popup.cpp
# End Source File
# Begin Source File

SOURCE=.\Slider.cpp
# End Source File
# Begin Source File

SOURCE=.\Spinner.cpp
# End Source File
# Begin Source File

SOURCE=.\Table.cpp
# End Source File
# Begin Source File

SOURCE=.\TextBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TextFX.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\credits.cpp
# End Source File
# Begin Source File

SOURCE=.\death.cpp
# End Source File
# Begin Source File

SOURCE=.\endgamecredits.cpp
# End Source File
# Begin Source File

SOURCE=.\frontendanimation.cpp
# End Source File
# Begin Source File

SOURCE=.\frontendcamera.cpp
# End Source File
# Begin Source File

SOURCE=.\frontendmain.cpp
# End Source File
# Begin Source File

SOURCE=.\langaugeanimation.cpp
# End Source File
# Begin Source File

SOURCE=.\languagemain.cpp
# End Source File
# Begin Source File

SOURCE=.\prompt.cpp
# End Source File
# Begin Source File

SOURCE=.\thwap.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Group "Character AI Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\alligator.h
# End Source File
# Begin Source File

SOURCE=.\anvilgang.h
# End Source File
# Begin Source File

SOURCE=.\apes.h
# End Source File
# Begin Source File

SOURCE=.\bears.h
# End Source File
# Begin Source File

SOURCE=.\bjshellac.h
# End Source File
# Begin Source File

SOURCE=.\buzzard.h
# End Source File
# Begin Source File

SOURCE=.\buzzardscripts.h
# End Source File
# Begin Source File

SOURCE=.\constructionbot.h
# End Source File
# Begin Source File

SOURCE=.\constructionworker.h
# End Source File
# Begin Source File

SOURCE=.\doggy.h
# End Source File
# Begin Source File

SOURCE=.\genericai.h
# End Source File
# Begin Source File

SOURCE=.\minerbot.h
# End Source File
# Begin Source File

SOURCE=.\museumguard.h
# End Source File
# Begin Source File

SOURCE=.\nastycan.h
# End Source File
# Begin Source File

SOURCE=.\securitybot.h
# End Source File
# Begin Source File

SOURCE=.\shark.h
# End Source File
# Begin Source File

SOURCE=.\taskforce.h
# End Source File
# Begin Source File

SOURCE=.\tazcatcher.h
# End Source File
# Begin Source File

SOURCE=.\tribalchief.h
# End Source File
# Begin Source File

SOURCE=.\tweety.h
# End Source File
# Begin Source File

SOURCE=.\zookeeper.h
# End Source File
# End Group
# Begin Group "Environment Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\breakables.h
# End Source File
# Begin Source File

SOURCE=.\bumper.h
# End Source File
# Begin Source File

SOURCE=.\clangerreset.h
# End Source File
# Begin Source File

SOURCE=.\destructibles.h
# End Source File
# Begin Source File

SOURCE=.\doors.h
# End Source File
# Begin Source File

SOURCE=.\escalator.h
# End Source File
# Begin Source File

SOURCE=.\fallingobjects.h
# End Source File
# Begin Source File

SOURCE=.\famethrowa.h
# End Source File
# Begin Source File

SOURCE=.\floorpolisherwantedposter.h
# End Source File
# Begin Source File

SOURCE=.\foodrandomiser.h
# End Source File
# Begin Source File

SOURCE=.\fountain.h
# End Source File
# Begin Source File

SOURCE=.\fountainextrasystem.h
# End Source File
# Begin Source File

SOURCE=.\ice.h
# End Source File
# Begin Source File

SOURCE=.\lava.h
# End Source File
# Begin Source File

SOURCE=.\magnet.h
# End Source File
# Begin Source File

SOURCE=.\mailbox.h
# End Source File
# Begin Source File

SOURCE=.\musicandsavagebeasts.h
# End Source File
# Begin Source File

SOURCE=.\phonebox.h
# End Source File
# Begin Source File

SOURCE=.\portal.h
# End Source File
# Begin Source File

SOURCE=.\projectile.h
# End Source File
# Begin Source File

SOURCE=.\scenicobjects.h
# End Source File
# Begin Source File

SOURCE=.\slides.h
# End Source File
# Begin Source File

SOURCE=.\speedboost.h
# End Source File
# Begin Source File

SOURCE=.\splatobjects.h
# End Source File
# Begin Source File

SOURCE=.\springyobjects.h
# End Source File
# Begin Source File

SOURCE=.\steppingstone.h
# End Source File
# Begin Source File

SOURCE=.\swingingobjects.h
# End Source File
# Begin Source File

SOURCE=.\tiltingobjects.h
# End Source File
# Begin Source File

SOURCE=.\transporter.h
# End Source File
# Begin Source File

SOURCE=.\tumbleweed.h
# End Source File
# Begin Source File

SOURCE=.\wantedposter.h
# End Source File
# Begin Source File

SOURCE=.\water.h
# End Source File
# Begin Source File

SOURCE=.\zapper.h
# End Source File
# End Group
# Begin Group "Effects Headers"

# PROP Default_Filter ""
# Begin Group "Shadows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\shadow.h
# End Source File
# Begin Source File

SOURCE=.\ShadowExtra.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\aircon.h
# End Source File
# Begin Source File

SOURCE=.\arrow.h
# End Source File
# Begin Source File

SOURCE=.\cartoonscaling.h
# End Source File
# Begin Source File

SOURCE=.\cheesegun.h
# End Source File
# Begin Source File

SOURCE=.\clouds.h
# End Source File
# Begin Source File

SOURCE=.\Colour.h
# End Source File
# Begin Source File

SOURCE=.\costumes.h
# End Source File
# Begin Source File

SOURCE=.\decals.h
# End Source File
# Begin Source File

SOURCE=.\eating.h
# End Source File
# Begin Source File

SOURCE=.\effects.h
# End Source File
# Begin Source File

SOURCE=.\electrocution.h
# End Source File
# Begin Source File

SOURCE=.\envmap.h
# End Source File
# Begin Source File

SOURCE=.\explode.h
# End Source File
# Begin Source File

SOURCE=.\fade.h
# End Source File
# Begin Source File

SOURCE=.\fire.h
# End Source File
# Begin Source File

SOURCE=.\Fireball.h
# End Source File
# Begin Source File

SOURCE=.\fireworks.h
# End Source File
# Begin Source File

SOURCE=.\Flies.h
# End Source File
# Begin Source File

SOURCE=.\flocking.h
# End Source File
# Begin Source File

SOURCE=.\footalign.h
# End Source File
# Begin Source File

SOURCE=.\Glare.h
# End Source File
# Begin Source File

SOURCE=.\haar.h
# End Source File
# Begin Source File

SOURCE=.\halo.h
# End Source File
# Begin Source File

SOURCE=.\holysmoke.h
# End Source File
# Begin Source File

SOURCE=.\laser.h
# End Source File
# Begin Source File

SOURCE=.\lightning.h
# End Source File
# Begin Source File

SOURCE=.\lights.h
# End Source File
# Begin Source File

SOURCE=.\mirrorball.h
# End Source File
# Begin Source File

SOURCE=.\phoneboxeffect.h
# End Source File
# Begin Source File

SOURCE=.\polyeffects.h
# End Source File
# Begin Source File

SOURCE=.\PongDust.cpp
# End Source File
# Begin Source File

SOURCE=.\postcard.h
# End Source File
# Begin Source File

SOURCE=.\precipitation.h
# End Source File
# Begin Source File

SOURCE=.\rapperblast.h
# End Source File
# Begin Source File

SOURCE=.\rocketscience.h
# End Source File
# Begin Source File

SOURCE=.\rumble.h
# End Source File
# Begin Source File

SOURCE=.\screenfx.h
# End Source File
# Begin Source File

SOURCE=.\securitybox.h
# End Source File
# Begin Source File

SOURCE=.\shatter.h
# End Source File
# Begin Source File

SOURCE=.\shockwave.h
# End Source File
# Begin Source File

SOURCE=.\sparks.h
# End Source File
# Begin Source File

SOURCE=.\spineffect.h
# End Source File
# Begin Source File

SOURCE=.\tarbubble.h
# End Source File
# Begin Source File

SOURCE=.\tartantaz.h
# End Source File
# Begin Source File

SOURCE=.\thoughtbubble.h
# End Source File
# Begin Source File

SOURCE=.\VideoFX.h
# End Source File
# Begin Source File

SOURCE=.\voodoocloud.h
# End Source File
# Begin Source File

SOURCE=.\voodoospelleffect.h
# End Source File
# Begin Source File

SOURCE=.\waterfall.h
# End Source File
# Begin Source File

SOURCE=.\waterrings.h
# End Source File
# Begin Source File

SOURCE=.\watersplash.h
# End Source File
# Begin Source File

SOURCE=.\zzz.h
# End Source File
# End Group
# Begin Group "Physics Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\collision.h
# End Source File
# Begin Source File

SOURCE=.\collisioncache.h
# End Source File
# Begin Source File

SOURCE=.\physics.h
# End Source File
# End Group
# Begin Group "Vehicle Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\atlassphere.h
# End Source File
# Begin Source File

SOURCE=.\floorpolisher.h
# End Source File
# Begin Source File

SOURCE=.\hovercraft.h
# End Source File
# Begin Source File

SOURCE=.\minecart.h
# End Source File
# Begin Source File

SOURCE=.\pneumaticplunger.h
# End Source File
# Begin Source File

SOURCE=.\rocketbike.h
# End Source File
# Begin Source File

SOURCE=.\trolley.h
# End Source File
# Begin Source File

SOURCE=.\vehicles.h
# End Source File
# End Group
# Begin Group "Special Case Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\glowaroundplinth.h
# End Source File
# Begin Source File

SOURCE=.\icedomespecial.h
# End Source File
# Begin Source File

SOURCE=.\MonitorFX.h
# End Source File
# Begin Source File

SOURCE=.\snowball.h
# End Source File
# Begin Source File

SOURCE=.\special.h
# End Source File
# Begin Source File

SOURCE=.\tutorial.h
# End Source File
# End Group
# Begin Group "System Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\actors.h
# End Source File
# Begin Source File

SOURCE=.\animation.h
# End Source File
# Begin Source File

SOURCE=.\attach.h
# End Source File
# Begin Source File

SOURCE=.\characters.h
# End Source File
# Begin Source File

SOURCE=.\Cheats.h
# End Source File
# Begin Source File

SOURCE=.\chrisscene.h
# End Source File
# Begin Source File

SOURCE=.\claudescene.h
# End Source File
# Begin Source File

SOURCE=.\collectibles.h
# End Source File
# Begin Source File

SOURCE=.\control.h
# End Source File
# Begin Source File

SOURCE=.\countdown.h
# End Source File
# Begin Source File

SOURCE=.\debris.h
# End Source File
# Begin Source File

SOURCE=.\debug.h
# End Source File
# Begin Source File

SOURCE=.\demo.h
# End Source File
# Begin Source File

SOURCE=.\display.h
# End Source File
# Begin Source File

SOURCE=.\Dungeon.h
# End Source File
# Begin Source File

SOURCE=.\endoflevelstats.h
# End Source File
# Begin Source File

SOURCE=.\events.h
# End Source File
# Begin Source File

SOURCE=.\files.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\GlobalSettings.h
# End Source File
# Begin Source File

SOURCE=.\gui.h
# End Source File
# Begin Source File

SOURCE=.\health.h
# End Source File
# Begin Source File

SOURCE=.\highscores.h
# End Source File
# Begin Source File

SOURCE=.\invisibleobjects.h
# End Source File
# Begin Source File

SOURCE=.\LinkedList.h
# End Source File
# Begin Source File

SOURCE=.\LoopArray.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\map.h
# End Source File
# Begin Source File

SOURCE=.\mapfile.h
# End Source File
# Begin Source File

SOURCE=.\maths.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\me.h
# End Source File
# Begin Source File

SOURCE=.\MemCard.h
# End Source File
# Begin Source File

SOURCE=.\mouse.h
# End Source File
# Begin Source File

SOURCE=.\music.h
# End Source File
# Begin Source File

SOURCE=.\objectviewer.h
# End Source File
# Begin Source File

SOURCE=.\ParseMap.h
# End Source File
# Begin Source File

SOURCE=.\paths.h
# End Source File
# Begin Source File

SOURCE=.\pause.h
# End Source File
# Begin Source File

SOURCE=.\PhilScene.h
# End Source File
# Begin Source File

SOURCE=.\player.h
# End Source File
# Begin Source File

SOURCE=.\playerstats.h
# End Source File
# Begin Source File

SOURCE=.\Rectangle.h
# End Source File
# Begin Source File

SOURCE=.\scene.h
# End Source File
# Begin Source File

SOURCE=.\sfx.h
# End Source File
# Begin Source File

SOURCE=.\sonystuff.h
# End Source File
# Begin Source File

SOURCE=.\splashscreen.h
# End Source File
# Begin Source File

SOURCE=.\Stack.h
# End Source File
# Begin Source File

SOURCE=.\status.h
# End Source File
# Begin Source File

SOURCE=.\subtitle.h
# End Source File
# Begin Source File

SOURCE=.\taz.h
# End Source File
# Begin Source File

SOURCE=.\text.h
# End Source File
# Begin Source File

SOURCE=.\text2.h
# End Source File
# Begin Source File

SOURCE=.\textures.h
# End Source File
# Begin Source File

SOURCE=.\timer.h
# End Source File
# Begin Source File

SOURCE=.\tonyscene.h
# End Source File
# Begin Source File

SOURCE=.\trail.h
# End Source File
# Begin Source File

SOURCE=.\trigger.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\Vec.h
# End Source File
# End Group
# Begin Group "Camera Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=.\cameracase.h
# End Source File
# Begin Source File

SOURCE=.\cutscene.h
# End Source File
# Begin Source File

SOURCE=.\KOScam.h
# End Source File
# Begin Source File

SOURCE=.\quakecam.h
# End Source File
# End Group
# Begin Group "Multiplayer Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\destruct.h
# End Source File
# Begin Source File

SOURCE=.\diner.h
# End Source File
# Begin Source File

SOURCE=.\multiplayer.h
# End Source File
# Begin Source File

SOURCE=.\race.h
# End Source File
# End Group
# Begin Group "Boss Headers"

# PROP Default_Filter ""
# Begin Group "mechatweety headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\chargeattack.h
# End Source File
# Begin Source File

SOURCE=.\crateattack.h
# End Source File
# Begin Source File

SOURCE=.\electricityattack.h
# End Source File
# Begin Source File

SOURCE=.\mechatweety.h
# End Source File
# Begin Source File

SOURCE=.\mtweetyscripts.h
# End Source File
# Begin Source File

SOURCE=.\tazbossitems.h
# End Source File
# Begin Source File

SOURCE=.\transportereffect.h
# End Source File
# Begin Source File

SOURCE=.\tweetytransporter.h
# End Source File
# End Group
# Begin Group "WestBoss headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BrickDisplay.h
# End Source File
# Begin Source File

SOURCE=.\GolfBar.h
# End Source File
# Begin Source File

SOURCE=.\WestBoss.h
# End Source File
# Begin Source File

SOURCE=.\WestBossExp.h
# End Source File
# End Group
# Begin Group "ZooBoss headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ZooBoss.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Boss.h
# End Source File
# Begin Source File

SOURCE=.\bossgamecontinue.h
# End Source File
# Begin Source File

SOURCE=.\CityBoss.h
# End Source File
# Begin Source File

SOURCE=.\tazboss1.h
# End Source File
# End Group
# Begin Group "Power-up Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bubblegum.h
# End Source File
# Begin Source File

SOURCE=.\chillipepper.h
# End Source File
# Begin Source File

SOURCE=.\hiccup.h
# End Source File
# Begin Source File

SOURCE=.\invisibletaz.h
# End Source File
# Begin Source File

SOURCE=.\superburp.h
# End Source File
# Begin Source File

SOURCE=.\superburpeffect.h
# End Source File
# End Group
# Begin Group "frontend headers"

# PROP Default_Filter ""
# Begin Group "Menu Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\audiomenu.h
# End Source File
# Begin Source File

SOURCE=.\extrasmenu.h
# End Source File
# Begin Source File

SOURCE=.\frontendmenu.h
# End Source File
# Begin Source File

SOURCE=.\gallerymenu.h
# End Source File
# Begin Source File

SOURCE=.\optionsmenu.h
# End Source File
# Begin Source File

SOURCE=.\recordsmenu.h
# End Source File
# Begin Source File

SOURCE=.\secretsmenu.h
# End Source File
# Begin Source File

SOURCE=.\startgamemenu.h
# End Source File
# Begin Source File

SOURCE=.\twoplayermenu.h
# End Source File
# End Group
# Begin Group "Book headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BookList.h
# End Source File
# Begin Source File

SOURCE=.\CheckBox.h
# End Source File
# Begin Source File

SOURCE=.\Icon.h
# End Source File
# Begin Source File

SOURCE=.\PageItemFX.h
# End Source File
# Begin Source File

SOURCE=.\popup.h
# End Source File
# Begin Source File

SOURCE=.\Slider.h
# End Source File
# Begin Source File

SOURCE=.\Spinner.h
# End Source File
# Begin Source File

SOURCE=.\Table.h
# End Source File
# Begin Source File

SOURCE=.\TextFX.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\credits.h
# End Source File
# Begin Source File

SOURCE=.\death.h
# End Source File
# Begin Source File

SOURCE=.\endgamecredits.h
# End Source File
# Begin Source File

SOURCE=.\frontendanimation.h
# End Source File
# Begin Source File

SOURCE=.\frontendcamera.h
# End Source File
# Begin Source File

SOURCE=.\frontendmain.h
# End Source File
# Begin Source File

SOURCE=.\languageanimation.h
# End Source File
# Begin Source File

SOURCE=.\languagemain.h
# End Source File
# Begin Source File

SOURCE=.\thwap.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\HealthBar.h
# End Source File
# Begin Source File

SOURCE=.\main.txt
# End Source File
# Begin Source File

SOURCE=.\mtweetymagnet.h
# End Source File
# Begin Source File

SOURCE=.\PaperBook.h
# End Source File
# Begin Source File

SOURCE=.\PongDust.h
# End Source File
# Begin Source File

SOURCE=.\prompt.h
# End Source File
# Begin Source File

SOURCE=.\redshield.h
# End Source File
# Begin Source File

SOURCE=.\smashcamera.h
# End Source File
# Begin Source File

SOURCE=.\TextBox.h
# End Source File
# Begin Source File

SOURCE=.\tournament.h
# End Source File
# Begin Source File

SOURCE=.\volume.h
# End Source File
# Begin Source File

SOURCE=.\XboxStrings.h
# End Source File
# End Target
# End Project

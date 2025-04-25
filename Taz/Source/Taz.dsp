# Microsoft Developer Studio Project File - Name="Taz" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Taz - Win32 Debug Consumer Demo
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Taz.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Taz.mak" CFG="Taz - Win32 Debug Consumer Demo"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Taz - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Taz - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Taz - Win32 Debug Consumer Demo" (based on "Win32 (x86) Application")
!MESSAGE "Taz - Win32 Release Consumer Demo" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TeamPhoenix/Taz/Source", MFRBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Taz - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GX /O2 /Ob2 /I "c:\babel\pc\include" /I "c:\dxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Taz.exe"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /I "c:\babel\pc\include" /I "c:\dxsdk\include" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_TAZDEBUG" /D "_MBCS" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib dxerr8.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Taz.exe"
# SUBTRACT LINK32 /profile /incremental:no /map /nodefaultlib

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Taz___Win32_Debug_Consumer_Demo"
# PROP BASE Intermediate_Dir "Taz___Win32_Debug_Consumer_Demo"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Taz___Win32_Debug_Consumer_Demo"
# PROP Intermediate_Dir "Taz___Win32_Debug_Consumer_Demo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /I "c:\babel\pc\include" /I "c:\dxsdk\include" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_TAZDEBUG" /D "_MBCS" /Fr /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /I "c:\babel\pc\include" /I "c:\dxsdk\include" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "_TAZDEBUG" /D "_MBCS" /D "CONSUMERDEMO" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib dxerr8.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Taz.exe"
# SUBTRACT BASE LINK32 /profile /incremental:no /map /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib dxerr8.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Taz.exe"
# SUBTRACT LINK32 /profile /incremental:no /map /nodefaultlib

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Taz___Win32_Release_Consumer_Demo"
# PROP BASE Intermediate_Dir "Taz___Win32_Release_Consumer_Demo"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Taz___Win32_Release_Consumer_Demo"
# PROP Intermediate_Dir "Taz___Win32_Release_Consumer_Demo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MT /W3 /GX /O2 /Ob2 /I "c:\babel\pc\include" /I "c:\dxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GX /O2 /Ob2 /I "c:\babel\pc\include" /I "c:\dxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "CONSUMERDEMO" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Taz.exe"
# SUBTRACT BASE LINK32 /nodefaultlib
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Taz.exe"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "Taz - Win32 Release"
# Name "Taz - Win32 Debug"
# Name "Taz - Win32 Debug Consumer Demo"
# Name "Taz - Win32 Release Consumer Demo"
# Begin Group "Source Files"

# PROP Default_Filter ".cpp"
# Begin Group "Character AI"

# PROP Default_Filter ""
# Begin Group "beaky"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\buzzard.cpp
# End Source File
# Begin Source File

SOURCE=.\buzzardscripts.cpp
# End Source File
# End Group
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

SOURCE=.\arrow.cpp
# End Source File
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

SOURCE=.\MonitorFX.cpp
# End Source File
# Begin Source File

SOURCE=.\phoneboxeffect.cpp
# End Source File
# Begin Source File

SOURCE=.\polyeffects.cpp
# End Source File
# Begin Source File

SOURCE=.\PongDust.cpp
# End Source File
# Begin Source File

SOURCE=.\postcard.cpp
# End Source File
# Begin Source File

SOURCE=.\precipitation.cpp
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

SOURCE=.\thwap.cpp
# End Source File
# Begin Source File

SOURCE=.\transportereffect.cpp
# End Source File
# Begin Source File

SOURCE=.\VideoFX.cpp
# End Source File
# Begin Source File

SOURCE=.\voodoocloud.cpp
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

SOURCE=.\prompt.cpp
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

SOURCE=.\atlasphere.cpp
# End Source File
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
# End Group
# Begin Source File

SOURCE=.\mirrorball.cpp
# End Source File
# Begin Source File

SOURCE=.\rapperblast.cpp
# End Source File
# Begin Source File

SOURCE=.\tumbleweed.cpp
# End Source File
# Begin Source File

SOURCE=.\voodoospelleffect.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Group "Character AI Headers"

# PROP Default_Filter ""
# Begin Group "beaky headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\buzzard.h
# End Source File
# Begin Source File

SOURCE=.\buzzardscripts.h
# End Source File
# End Group
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

SOURCE=.\arrow.h
# End Source File
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

SOURCE=.\glowaroundplinth.h
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

SOURCE=.\phoneboxeffect.h
# End Source File
# Begin Source File

SOURCE=.\polyeffects.h
# End Source File
# Begin Source File

SOURCE=.\PongDust.h
# End Source File
# Begin Source File

SOURCE=.\postcard.h
# End Source File
# Begin Source File

SOURCE=.\precipitation.h
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

SOURCE=.\thwap.h
# End Source File
# Begin Source File

SOURCE=.\transportereffect.h
# End Source File
# Begin Source File

SOURCE=.\VideoFX.h
# End Source File
# Begin Source File

SOURCE=.\voodoocloud.h
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

SOURCE=.\libsceedemo.h
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

SOURCE=.\prompt.h
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
# Begin Source File

SOURCE=.\volume.h
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
# Begin Source File

SOURCE=.\smashcamera.h
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
# Begin Source File

SOURCE=.\tournament.h
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

SOURCE=.\mtweetymagnet.h
# End Source File
# Begin Source File

SOURCE=.\mtweetyscripts.h
# End Source File
# Begin Source File

SOURCE=.\redshield.h
# End Source File
# Begin Source File

SOURCE=.\rocketscience.h
# End Source File
# Begin Source File

SOURCE=.\tazbossitems.h
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

SOURCE=.\atlassphere.h
# End Source File
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
# Begin Group "Book Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CheckBox.h
# End Source File
# Begin Source File

SOURCE=.\HealthBar.h
# End Source File
# Begin Source File

SOURCE=.\Icon.h
# End Source File
# Begin Source File

SOURCE=.\PageItemFX.h
# End Source File
# Begin Source File

SOURCE=.\PaperBook.h
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

SOURCE=.\TextBox.h
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
# End Group
# Begin Source File

SOURCE=.\mirrorball.h
# End Source File
# Begin Source File

SOURCE=.\rapperblast.h
# End Source File
# Begin Source File

SOURCE=.\tumbleweed.h
# End Source File
# Begin Source File

SOURCE=.\voodoospelleffect.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\taz.ico
# End Source File
# Begin Source File

SOURCE=.\taz.rc
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\main.txt
# End Source File
# Begin Source File

SOURCE=.\main.xls
# End Source File
# Begin Source File

SOURCE=.\ReleaseLog.txt
# End Source File
# End Group
# Begin Group "Babel headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\actor.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\babel.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bDisplay.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bfc.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bGui.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bInput.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bMaths.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bSound.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\buildNumber.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\camera.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fCamera\CBCameraBase.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fScene\CBCollision.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fCamera\CBFlyAroundCamera.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fGui\CBGuiButton.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fGui\CBGuiMenu.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fGui\CBGuiMenuItem.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fGui\CBGuiObject.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fGui\CBGuiScrollBar.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fGui\CBGuiWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fCamera\CBPivotCamera.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bMaths\collision.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bMaths\collision2D.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\Dbghelp.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bGui\defFont.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\encryption.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fScene\entity.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\event.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\extras.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fCamera.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fGui.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fScene.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bInput\gamesave.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bMaths\geometry.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bGui\gui.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bGui\guiHal.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\heap.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bInput\input.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\lights.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\meshHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcActor.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcAnim.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\pcBkgload.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\pcClock.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\pcDebug.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bDisplay\pcDisplay.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bDisplay\pcDrawprim.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bDisplay\pcDxdebug.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\pcFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bDisplay\pcFont.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bGui\pcGui.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bInput\pcHAL.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\pcKernel.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcLights.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bMaths\pcMaths.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bMaths\pcMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bMaths\pcQuaternion.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bDisplay\pcRender.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bInput\pcsave.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcSet.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bDisplay\pcSplash.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcStockExtras.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bDisplay\pcTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\pcTimer.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bMaths\pcVector.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bDisplay\pcVideo.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bDisplay\pcViewport.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bSound\pcWave.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcxbActor.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcxbAnim.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcxbLights.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcxbSet.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcxbShaderSelector.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\pcxbStockExtras.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\resources.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\fScene\sceneDescriptor.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bActor\set.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bKernel\stringTable.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bMaths\vector2D.h
# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\Include\bSound\wave.h
# End Source File
# End Group
# Begin Group "Babel Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Babel\Pc\lib\debug\bActor\bActor.lib

!IF  "$(CFG)" == "Taz - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\lib\debug\bDisplay\bDisplay.lib

!IF  "$(CFG)" == "Taz - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\lib\debug\bGui\bGui.lib

!IF  "$(CFG)" == "Taz - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\lib\debug\bInput\bInput.lib

!IF  "$(CFG)" == "Taz - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\lib\debug\bKernel\bKernel.lib

!IF  "$(CFG)" == "Taz - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\lib\debug\bMaths\bMaths.lib

!IF  "$(CFG)" == "Taz - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\lib\debug\bSound\bSound.lib

!IF  "$(CFG)" == "Taz - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\lib\debug\fCamera\fCamera.lib

!IF  "$(CFG)" == "Taz - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Babel\Pc\lib\debug\fEffects\fEffects.lib

!IF  "$(CFG)" == "Taz - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Babel Release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Babel\Pc\lib\release\babel_r.lib

!IF  "$(CFG)" == "Taz - Win32 Release"

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Debug Consumer Demo"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Taz - Win32 Release Consumer Demo"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\laval.asp
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\strmiids.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\d3d8.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\d3dx.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\d3dx8.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\d3dx8d.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\d3dx8dt.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\d3dxd.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\d3dxof.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\ddraw.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\dinput.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\dinput8.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\dmoguids.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\dplayx.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\dsetup.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\dsound.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\DxErr8.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\dxguid.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\dxtrans.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\ksproxy.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\ksuser.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\msdmo.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\quartz.lib
# End Source File
# Begin Source File

SOURCE=..\..\Dxsdk\lib\amstrmid.lib
# End Source File
# End Target
# End Project

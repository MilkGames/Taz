#ifndef __H_TAZ
#define __H_TAZ

#include "c:\windows\me.h"

#include "babel.h"
#include "bgui.h"

#if BPLATFORM == PS2
#define BOOL int
#endif

#include "parsemap.h"
#include "collision.h"
#include "physics.h"
#include "splashscreen.h"
#include "main.h"
#include "maths.h"
#include "files.h"
#include "actors.h"
#include "camera.h"
#include "collisioncache.h"
#include "control.h"
#include "events.h"
#include "health.h"
#include "attach.h"
#include "postcard.h"
#include "characters.h"
#include "textures.h"
#include "display.h"
#include "lights.h"
#include "animation.h"
#include "status.h"
#include "scene.h"
#include "tonyscene.h"
#include "claudescene.h"
#include "cutscene.h"
#include "fade.h"
#include "precipitation.h"
#include "collectibles.h"
#include "breakables.h"
#include "holysmoke.h"
#include "debris.h"
#include "font.h"
#include "shadow.h"
#include "thoughtbubble.h"
#include "mapfile.h"
#include "envmap.h"
#include "sfx.h"
#include "timer.h"
#include "paths.h"
#include "objectviewer.h"
#include "projectile.h"
#include "fallingobjects.h"
#include "springyobjects.h"
#include "gui.h"
#include "steppingstone.h"
#include "tiltingobjects.h"
#include "swingingobjects.h"
#include "water.h"
#include "destructibles.h"
#include "genericai.h"
#include "wantedposter.h"
#include "mailbox.h"
#include "playerstats.h"
#include "zookeeper.h"
#include "pneumaticplunger.h"
#include "vehicles.h"
#include "invisibleobjects.h"
#include "trigger.h"
#include "taztoproulette.h"
#include "cartoonscaling.h"
#include "electrocution.h"

#define X (0)
#define Y (1)
#define Z (2)
#define A (3)
#define W (3)

#ifdef	DEBUGGING
void colourmesh(TBSegmentedMesh *mesh);
void addline(TVector pos,TVector dir,float scale,int r1,int g1, int b1,int r2,int g2, int b2);
#endif

#endif
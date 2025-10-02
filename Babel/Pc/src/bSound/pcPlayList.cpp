// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcPlayList.cpp
//   Purpose : Xbox specific playlists
// Component : Xbox playlists (also known as sound tracks)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bPlayListUpdate
   Purpose : Move to next song if we are playing and this one has finished
   Parameters : 
   Returns : 
   Info : Called internally
*/

void bPlayListUpdate()
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListEnum
   Purpose : Establish number of play lists
   Parameters : callback to receive enumeration data, pointer to context data
   Returns : number of play lists found
   Info : callback can be NULL
*/

int bsPlayListEnum(TBPlayListEnumCallback callback, void *context)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListSelect
   Purpose : Select a play list to use
   Parameters : list index, buffer to fill
   Returns : playlist handle 
   Info : 
*/

TBPlayList *bsPlayListSelect(int list, TBPlayList *buffer)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListPlay
   Purpose : Select a song to play from the play list
   Parameters : playlist handle, [song index, flags, volume, pan, frequency]
   Returns : OK/FAIL
   Info : The audiostream properties can be changed but do not delete
          Default song is the last one that was stopped playing
*/

int bsPlayListPlay(TBPlayList *playlist, int song,
				   uint flags, uint volume, int pan, uint frequency)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListStop
   Purpose : stop playing song in play list
   Parameters : playlist handle
   Returns : 
   Info : 
*/
void bsPlayListStop(TBPlayList *playlist)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListDelete
   Purpose : free playlist memory
   Parameters : playlist handle
   Returns : 
   Info : 
*/
void bsPlayListDelete(TBPlayList *playlist)
{
    return;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListSetVolume
   Purpose : set volume of playing and subsequent songs
   Parameters : playlist handle, volume (0-255), panning(+/-255)
   Returns : OK/FAIL
   Info : 
*/

int bsPlayListSetVolume(TBPlayList *playlist, uint volume, int pan)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListSetFrequency
   Purpose : set freqency of playing and subsequent songs
   Parameters : playlist handle, frequency (8000 - 48000, or 0 reset to default)
   Returns : OK/FAIL
   Info : 
*/

int bsPlayListSetFrequency(TBPlayList *playlist, uint frequency)
{
    return 0;
}

/* --------------------------------------------------------------------------------
   Function : bsPlayListFlagsSet
   Purpose : set playlist flags
   Parameters : playlist handle, flags
   Returns :OK/FAIL
   Info : 
*/

int bsPlayListFlagsSet(TBPlayList *playlist, uint flags)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListFlagsClear
   Purpose : clear play list flags
   Parameters : playlist handle, flags
   Returns :OK/FAIL
   Info : 
*/

int bsPlayListFlagsClear(TBPlayList *playlist, uint flags)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListFlagsGet
   Purpose : get play list flags
   Parameters : play list handle, flags
   Returns :flags status
   Info : 
*/

int bsPlayListFlagsGet(TBPlayList *playlist, uint flag)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListGetNumberOfSongs
   Purpose : return the number of songs in the play list
   Parameters : playlist handle
   Returns : number of songs in play list
   Info :
*/

int bsPlayListGetNumberOfSongs(TBPlayList *playlist)
{
    return 0;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListGetSongName
   Purpose : get the name of the current song
   Parameters : playlist handle, [song]
   Returns : 16 bit song name string
   Info : By default this returns the current song name
*/

ushort *bsPlayListGetSongName(TBPlayList *playlist, int song)
{
    return NULL;
}


/* --------------------------------------------------------------------------------
   Function : bsPlayListGetName
   Purpose : get the name of the playlist
   Parameters : playlist handle
   Returns : 16 bit playlist name string
   Info : 
*/

ushort *bsPlayListGetName(TBPlayList *playlist)
{
    return NULL;
}


/*	--------------------------------------------------------------------------------
   Function : bsPlayListGetStatus
   Purpose : retrieve the current status of a play list
   Parameters : playlist handle
 				 ptr for volume, ptr for pan, ptr for current frequency
   Returns : status
   Info : all ptrs except 'playlist' can be NULL
*/

EBPlayListStatus bsPlayListGetStatus(TBPlayList *playlist, int *volume, int *pan, int *frequency)
{
    return BPLAYLISTSTATUS_IDLE;
}
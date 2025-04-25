// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : xbPlayList.h
//   Purpose : Xbox specific playlists
// Component : Xbox playlists (also known as sound tracks)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __BSOUND_XBPLAYLIST_H__
#define __BSOUND_XBPLAYLIST_H__

// ********************************************************************************
// Constants and macros

// ********************************************************************************
// Types, Structures and Classes

// stream status values
enum EBPlayListStatus {
	BPLAYLISTSTATUS_IDLE,						// playlist is idle (the default state)
	BPLAYLISTSTATUS_PLAYING,					// playlist is playing
	BPLAYLISTSTATUS_PAUSED						// playlist is paused
};

#define BPLAYLISTFLAG_AUTOINCREMENT	0x00000001	// Increment to next song once current ends
#define BPLAYLISTFLAG_LOOPING		0x00000002	// Loop playlist
#define BPLAYLISTFLAG_PAUSE			0x00000004	// Pause playback
#define BPLAYLISTFLAG_CLIENTBUFFER	0x00000008	// playlist is using a client supplied buffer

typedef struct _TBPlayList {
	int					playlist;				// index
	ulong				playlistId;				// unique identifiers for playlist
	int					flags;					// play list flags (see BPLAYLISTFLAG_*)
	EBPlayListStatus	status;					// play list status (see BPLAYLISTSTATUS_*)

	int					songAt;					// song index currently playing
	uint				volume;					// song volume (see audio stream)
	int					pan;					// song pan (see audio stream)
	uint				freq;					// song freg (see audio stream)

	HANDLE				file;					// file handle of open/playing song
	TBAudioStream		*stream;				// audiosteam of open/playing song
	int					songs;					// number of songs in playlist
	ulong				*songId;				// list of unique identifiers for each song

	ushort				name[MAX_SOUNDTRACK_NAME]; // play list name
	ushort				*songName;				// all song names MAX_SONG_NAME apart, 16 bit names
} TBPlayList;

enum EBPlayListType {
	EBPLAYLISTTYPE_PLAYLIST,
	EBPLAYLISTTYPE_SONG,
};

// info struct for font callback
typedef struct TBPlayListCallbackInfo {
	int					playlist;				// index
	int					song;					// index
	ushort				*name;					// ptr to 16 bit name
	EBPlayListType		type;					// list or song (EBPLAYLISTTYPE_*)
	int					flags;
} TBPlayListCallbackInfo;

typedef void (*TBPlayListEnumCallback)(TBPlayListCallbackInfo *info, void *context);

// ********************************************************************************
// Prototypes


/* --------------------------------------------------------------------------------
   Function : bPlayListUpdate
   Purpose : Move to next song if we are playing and this one has finished
   Parameters : 
   Returns : 
   Info : Called internally
*/

void bPlayListUpdate();


/* --------------------------------------------------------------------------------
   Function : bsPlayListEnum
   Purpose : Establish number of play lists
   Parameters : callback to receive enumeration data, pointer to context data
   Returns : number of play lists found
   Info : callback can be NULL
*/

int bsPlayListEnum(TBPlayListEnumCallback callback, void *context);


/* --------------------------------------------------------------------------------
   Function : bsPlayListSelect
   Purpose : Select a play list to use
   Parameters : list index, buffer to fill
   Returns : playlist handle 
   Info : 
*/

TBPlayList *bsPlayListSelect(int list=0, TBPlayList *buffer=NULL);


/* --------------------------------------------------------------------------------
   Function : bsPlayListPlay
   Purpose : Select a song to play from the play list
   Parameters : playlist handle, [song index, flags, volume, pan, frequency]
   Returns : OK/FAIL
   Info : The audiostream properties can be changed but do not delete
          Default song is the last one that was stopped playing
*/

int bsPlayListPlay(TBPlayList *playlist, int song=-1,
				   uint flags=(BPLAYLISTFLAG_LOOPING|BPLAYLISTFLAG_AUTOINCREMENT), 
				   uint volume=255, int pan=0, uint frequency=0);


/* --------------------------------------------------------------------------------
   Function : bsPlayListStop
   Purpose : stop playing song in play list
   Parameters : playlist handle
   Returns : 
   Info : 
*/
void bsPlayListStop(TBPlayList *playlist);


/* --------------------------------------------------------------------------------
   Function : bsPlayListDelete
   Purpose : free playlist memory
   Parameters : playlist handle
   Returns : 
   Info : 
*/
void bsPlayListDelete(TBPlayList *playlist);


/* --------------------------------------------------------------------------------
   Function : bsPlayListSetVolume
   Purpose : set volume of playing and subsequent songs
   Parameters : playlist handle, volume (0-255), panning(+/-255)
   Returns : OK/FAIL
   Info : 
*/

int bsPlayListSetVolume(TBPlayList *playlist, uint volume, int pan=0);


/* --------------------------------------------------------------------------------
   Function : bsPlayListSetFrequency
   Purpose : set freqency of playing and subsequent songs
   Parameters : playlist handle, frequency (8000 - 48000, or 0 reset to default)
   Returns : OK/FAIL
   Info : 
*/

int bsPlayListSetFrequency(TBPlayList *playlist, uint frequency);

/* --------------------------------------------------------------------------------
   Function : bsPlayListFlagsSet
   Purpose : set playlist flags
   Parameters : playlist handle, flags
   Returns :OK/FAIL
   Info : 
*/

int bsPlayListFlagsSet(TBPlayList *playlist, uint flags);


/* --------------------------------------------------------------------------------
   Function : bsPlayListFlagsClear
   Purpose : clear play list flags
   Parameters : playlist handle, flags
   Returns :OK/FAIL
   Info : 
*/

int bsPlayListFlagsClear(TBPlayList *playlist, uint flags);


/* --------------------------------------------------------------------------------
   Function : bsPlayListFlagsGet
   Purpose : get play list flags
   Parameters : play list handle, flags
   Returns :flags status
   Info : 
*/

int bsPlayListFlagsGet(TBPlayList *playlist, uint flag);


/* --------------------------------------------------------------------------------
   Function : bsPlayListGetNumberOfSongs
   Purpose : return the number of songs in the play list
   Parameters : playlist handle
   Returns : number of songs in play list
   Info :
*/

int bsPlayListGetNumberOfSongs(TBPlayList *playlist);


/* --------------------------------------------------------------------------------
   Function : bsPlayListGetSongName
   Purpose : get the name of the current song
   Parameters : playlist handle, [song]
   Returns : 16 bit song name string
   Info : By default this returns the current song name
*/

ushort *bsPlayListGetSongName(TBPlayList *playlist, int song=-1);


/* --------------------------------------------------------------------------------
   Function : bsPlayListGetName
   Purpose : get the name of the playlist
   Parameters : playlist handle
   Returns : 16 bit playlist name string
   Info : 
*/

ushort *bsPlayListGetName(TBPlayList *playlist);


/*	--------------------------------------------------------------------------------
   Function : bsPlayListGetStatus
   Purpose : retrieve the current status of a play list
   Parameters : playlist handle
 				 ptr for volume, ptr for pan, ptr for current frequency
   Returns : status
   Info : all ptrs except 'playlist' can be NULL
*/

EBPlayListStatus bsPlayListGetStatus(TBPlayList *playlist,
									 int *volume, int *pan, int *frequency);

#endif //__BSOUND_XBPLAYLIST_H__


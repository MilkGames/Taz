#ifndef __H_MUSIC
#define __H_MUSIC

#include "main.h"

#define ENABLEPRESEEK

#define MAX_STREAMS	(1)
#define MAX_SUBSTREAMS (3)

#define MUSICFLAG_CLOSEAFTERALLFADE	(1<<0)

// PP: TEMP BABEL WORKAROUND
// PP: bsSetGlobalAudioStreamVolume was buggering up
#if(BPLATFORM == XBOX || BPLATFORM == PC)
#define STREAM_VOLUME_WORKAROUND
#endif// PP: platform

#ifdef STREAM_VOLUME_WORKAROUND
// PP: DON'T FORGET ABOUT THIS!!!
#define bsSetGlobalAudioStreamVolume(_vol)		bsSetGlobalAudioStreamVolume(255)
#endif// PP: def STREAM_VOLUME_WORKAROUND

enum EStreamMood
{
	STREAMMOOD_NORMAL,
	STREAMMOOD_SLOW,
	STREAMMOOD_FAST,

	STREAMMOOD_NONE			// PP: this is returned by MUSICSYSTEM::::GetMood if no music pak is loaded
};

enum EMusicType
{
	MUSICTYPE_MAIN,
	MUSICTYPE_FANFAIR,
	MUSICTYPE_LOAD,
	MUSICTYPE_ALL,
};

enum EMusicState
{
	MUSICSTATE_START,
	MUSICSTATE_FADEOUT,
	MUSICSTATE_STOP,
};

typedef struct TAG_STREAMINFO
{
    int		frequency;
    int		noofSamples;
    int		bitDepth;
    int		compressionRatio;
    int		noofMarkers;
    int		*markers;

	int		currentPosition;
	int		closestMarkerOffsetLastFrame;
	int		closestMarkerOffset;
	int		closestMarker;
	int		numSubStreams;
} STREAMINFO, _STREAMINFO BALIGN16;

enum EStreamState
{
	STREAMSTATE_FREE,
	STREAMSTATE_LOADING,
	STREAMSTATE_LOADED,
	STREAMSTATE_PRESEEK,
	STREAMSTATE_PLAYING,
	STREAMSTATE_STOPPING,
};

typedef struct TAG_STREAM
{
private:
	char			streamName[64];						// TP: Debug names of current audio playing
	TBAudioStream	*stream;							// TP: Audio stream pointer

	float			volume[MAX_SUBSTREAMS];				// TP: Current volume levels
	float			destVolume[MAX_SUBSTREAMS];			// TP: Destination volume for fades
	float			volumeFade[MAX_SUBSTREAMS];			// TP: Current fade to be applied
	float			moodVolume[MAX_SUBSTREAMS];			// TP: The maximum volume for each sub stream when using the request mood

	EStreamMood		currentMood;						// TP: Current mode for stream
	EStreamMood		moodRequested;						// TP: Mood to switch to as soon as possible
	float			specialFadeTime;					// PP: forced fade time (see MUSICSYSTEM::RequestMood)
	float			timeSinceLastMoodChange;			// TP: Time since the last mood change took place
	float			timer;								// TP: timer used for stopping and starting music

	EStreamState	state;								// TP: Current state of this audio stream
	STREAMINFO		info;								// TP: stream info
	bool			awaitingFreeAudio;					// TP: set to true when waiting to free audio
	uint32			flags;


	/*	--------------------------------------------------------------------------------
		Function 	: int STREAM::FreeAudioStream
		Purpose 	: Free the audio stream
		Parameters 	: none
		Returns 	: none
		Info 		: 
	*/

	void FreeAudioStream();

public:


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::TAG_STREAM
		Purpose 	: Default constructor
		Parameters 	: none
		Returns 	: none
		Info 		:
	*/

	TAG_STREAM();


	/*	--------------------------------------------------------------------------------
		Function 	: void STREAM::Stop
		Purpose 	: Stop the stream from playing
		Parameters 	: none
		Returns 	: none
		Info 		: 
	*/

	void Stop();


	/*	--------------------------------------------------------------------------------
		Function 	: int STREAM::RequestFreeAudioStream
		Purpose 	: Free the audio stream
		Parameters 	: none
		Returns 	: none
		Info 		: 
	*/

	void RequestFreeAudioStream()
	{
		awaitingFreeAudio = true;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::~TAG_STREAM
		Purpose 	: Default destructor
		Parameters 	: none
		Returns 	: none
		Info 		:
	*/

	~TAG_STREAM();


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::Reset
		Purpose 	: Reset the stream volumes etc.
		Parameters 	: none
		Returns 	: none
		Info 		:
	*/
	void Reset();


	/*  --------------------------------------------------------------------------------
		Function	: CurrentTune
		Purpose		: Returns the name of the current tune playing
		Parameters	: 
		Returns		: 
		Info		: 
	*/

	char *CurrentTune()
	{
		if (state==STREAMSTATE_PLAYING)
		{
			return streamName;
		}
		else
		{
			return NULL;
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::SetVolume
		Purpose 	: Set the current volume for the given sub stream
		Parameters 	: sub stream number, volume 0-255
		Returns 	: none
		Info 		:
	*/

	void SetVolume(int subStream, float volume);


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::SetMoodVolume
		Purpose 	: Set the volume to fade to when using the mood
		Parameters 	: none
		Returns 	: none
		Info 		:
	*/

	void SetMoodVolume(EStreamMood mood, float volume);


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::SetDestVolume
		Purpose 	: Set the destination volume level and a fade time in seconds
		Parameters 	: sub stream number, volume 0-255, fadeTime
		Returns 	: none
		Info 		:
	*/

	void SetDestVolume(int subStream, float destVolume, float fadeTime, int flags=-1);


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::Update
		Purpose 	: Update the current stream
		Parameters 	: 
		Returns 	: 
		Info 		: Applies the current volume and fade parameters
	*/

	void Update();


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::RequestMood
		Purpose 	: Request the mood
		Parameters 	: mood (STREAMMOOD_..), (PP:)forced fade time (see MUSICSYSTEM::RequestMood)
		Returns 	: 
		Info 		: 
	*/

	void RequestMood(EStreamMood mood, const float specialFadeTimeIn);


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::SetMood
		Purpose 	: Set the mood
		Parameters 	: mood (STREAMMOOD_..)
		Returns 	: 
		Info 		: 
	*/

	void SetMood(EStreamMood mood)
	{
		this->moodRequested = this->currentMood = mood;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::GetMood
		Purpose 	: Get the current mood of the stream
		Parameters 	: 
		Returns 	: the current mood of the stream (STREAMMOOD_..)
		Info 		: // PP: 
	*/

	inline EStreamMood GetMood(void) const
	{
		return this->currentMood;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: STREAM::IsFree
		Purpose 	: Returns true/false is the stream is free for use
		Parameters 	: 
		Returns 	: 
		Info 		: 
	*/

	int IsFree()
	{
		if (state==STREAMSTATE_FREE) return TRUE;
		else return FALSE;
	}


	/*	--------------------------------------------------------------------------------
		Function 	: int STREAM::LoadAudioStream(TBPackageIndex *musicPak, char *name)
		Purpose 	: Load the stream
		Parameters 	: music pak, name of stream
		Returns 	: none
		Info 		: 
	*/

	int LoadAudioStream(TBPackageIndex *musicPak, char *name);


	/*	--------------------------------------------------------------------------------
		Function 	: void STREAM::Play()
		Purpose 	: Start the stream playing
		Parameters 	: none
		Returns 	: true/false if successfull
		Info 		: 
	*/

	bool Play();

} STREAM, _STREAM BALIGN16;

typedef struct TAG_MUSICSYSTEM
{
private:

	STREAM			*streams[MAX_STREAMS];		// TP: pointers to the create streams
	int				numStreams;					// TP: number of streams created

	TBPackageIndex	*musicPak;
	char			systemActive;
	uint32			lastTuneCRC;
	uint32			lastScene;					// PP: index of scene for which music was last requested

public:

	struct
	{
		int				stream;
		int				subStream;
		char			name[64];
		float			volume;
		float			fadeTime;
		EStreamMood		mood;
		bool			waiting;
	} queue;

	/*	--------------------------------------------------------------------------------
		Function 	: MUSICSYSTEM::InitialiseMusicSystem
		Purpose 	: Initialises the music system and opens the streams pak file
		Parameters 	: none
		Returns 	: none
		Info 		:
	*/
	int InitialiseMusicSystem();

	/*	--------------------------------------------------------------------------------
		Function 	: MUSICSYSTEM::ShutdownMusicSystem
		Purpose 	: Perform general shutdown of the music system
		Parameters 	: none
		Returns 	: none
		Info 		: Will stop and close any playing audio
	*/
	void ShutdownMusicSystem();

	/*  --------------------------------------------------------------------------------
		Function	: MUSICSYSTEM::ChooseMusic
		Purpose		: Central position for all music calls.
		Parameters	: scene requiring music, EMusicType type of music, EMusicState (start/stop etc)
		Returns		: 
		Info		: 
	*/
	void ChooseMusic(int scene, EMusicType type, EMusicState state, char *name = NULL);

	/*	--------------------------------------------------------------------------------
		Function 	: MUSICSYSTEM::TAG_MUSICSYSTEM
		Purpose 	: Default destructor
		Parameters 	: none
		Returns 	: none
		Info 		:
	*/
	TAG_MUSICSYSTEM()
	{
		musicPak		= NULL;
		systemActive	= FALSE;
	}

	/*	--------------------------------------------------------------------------------
		Function 	: MUSICSYSTEM::TAG_MUSICSYSTEM
		Purpose 	: Default destructor
		Parameters 	: none
		Returns 	: none
		Info 		:
	*/
	~TAG_MUSICSYSTEM(void)
	{
		ShutdownMusicSystem();
	}

	/*  --------------------------------------------------------------------------------
		Function	: CurrentTuneQueued
		Purpose		: Returns the current tune queued
		Parameters	: 
		Returns		: tune ptr if tune is queued and the music is fading down / null if not
		Info		: 
	*/

	char *CurrentTuneQueued(void)
	{
		if (queue.waiting)
		{
			return queue.name;
		}
		return NULL;
	}


	/*  --------------------------------------------------------------------------------
		Function	: IsMusicPlaying
		Purpose		: Check if the named music is currently playing
		Parameters	: name of tune
		Returns		: true/false
		Info		: 
	*/

	bool IsMusicPlaying(char *name);


	/*	--------------------------------------------------------------------------------
		Function 	: ReadyAudio
		Purpose 	: Readys an audio track for playing
		Parameters 	: name of audio track
		Returns 	: handle of stream used or -1 for error
		Info 		:
	*/
	int ReadyAudio(char *name);

	/*	--------------------------------------------------------------------------------
		Function 	: StartAudio
		Purpose 	: Start the audio track playing
		Parameters 	: handle of stream to start playing, OPTIONAL name of stream to ready and then play, volume
		Returns 	: handle of stream used or -1 for error
		Info 		:
	*/
	int StartAudio(int stream, int subStream, char *name, float volume, float fadeTime, EStreamMood mood = STREAMMOOD_NORMAL);

	/*	--------------------------------------------------------------------------------
		Function 	: QueueAudio
		Purpose 	: Queue an audio track to play when the stram is free
		Parameters 	: handle of stream to start playing, OPTIONAL name of stream to ready and then play, volume
		Returns 	: handle of stream used or -1 for error
		Info 		:
	*/
	int QueueAudio(int stream, int subStream, char *name, float volume, float fadeTime, EStreamMood mood = STREAMMOOD_NORMAL);

	/*	--------------------------------------------------------------------------------
		Function 	: FadeAndFreeAll
		Purpose 	: Start fading down all audio streams and then free
		Parameters 	: fadeTime in seconds
		Returns 	: none
		Info 		:
	*/
	void FadeAndFreeAll(float fadeTime);

	/*	--------------------------------------------------------------------------------
		Function 	: FadeDownAudio
		Purpose 	: Start fading down an audio stream and optionaly close it
		Parameters 	: stream handle, fadeTime in seconds, TRUE/FALSE close after fade
		Returns 	: none
		Info 		:
	*/
	void FadeDownAudio(int stream, int subStream, float fadeTime, float volume, int closeAfterFade);

	/*	--------------------------------------------------------------------------------
		Function 	: FadeUpAudio
		Purpose 	: Start fading up an audio stream
		Parameters 	: stream handle, fadeTime in seconds
		Returns 	: none
		Info 		:
	*/
	void FadeUpAudio(int stream, int subStream, float	fadeTime, float volume);

	/*	--------------------------------------------------------------------------------
		Function 	: UpdateMusicSystem
		Purpose 	: Perform general music update
		Parameters 	: none
		Returns 	: none
		Info 		: Called once a frame
	*/
	void UpdateMusicSystem(void);

	/*	--------------------------------------------------------------------------------
		Function 	: RequestMood
		Purpose 	: Choose the new mood for the current stream
		Parameters 	: stream, mood (STREAMMOOD_...), (opt/MUSIC_NO_SPECIAL_FADE_TIME)forced fade time
		Returns 	: none
		Info 		: // PP: use a specialFadeTime (parm3) if you know the duration over which you want the
						mood change to happen.  For example, the length of a transitional
						animation between two major states such as SLEEP and MOVE.
						If you want a normal mood change, don't specify a specialFadeTime!
	*/

	// PP: -1 indicates that the mood change is to happen in the normal way, rather than forcing an immediate fade of specific length - see info in function header.
#define MUSIC_NO_SPECIAL_FADE_TIME		(-1.0f)

	void RequestMood(int stream, EStreamMood mood, const float specialFadeTime=MUSIC_NO_SPECIAL_FADE_TIME)
	{
		if (this->musicPak)
		{
			if(mood != streams[stream]->GetMood())
			{
				bkPrintf("mood %d\n", mood);
			}
			streams[stream]->RequestMood(mood, specialFadeTime);

			// PP: debug...
			/*
			if(mood != streams[stream]->GetMood())
			{
				bkPrintf("MUSICSYSTEM::RequestMood %d on stream %d", mood, stream);

				if(specialFadeTime == MUSIC_NO_SPECIAL_FADE_TIME)
				{
					bkPrintf(", no special fade time\n");
				}
				else
				{
					bkPrintf(", special fade time %5.2f\n", specialFadeTime);
				}
			}
			*/
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: MUSICSYSTEM::::GetMood
		Purpose 	: Get the current music mood
		Parameters 	: 
		Returns 	: the current music mood (STREAMMOOD_..)
		Info 		: // PP: 
	*/
	inline EStreamMood GetMood(void) const
	{
		if (this->musicPak)
		{
			return streams[0]->GetMood();
		}
		else
		{
			return STREAMMOOD_NONE;
		}
	}


	/*	--------------------------------------------------------------------------------
		Function 	: MUSICSYSTEM::SetMoodVolume
		Purpose 	: Set the volume to fade to when using the mood
		Parameters 	: stream to use, mood to set volume for, volume
		Returns 	: none
		Info 		:
	*/
	void SetMoodVolume(int stream, EStreamMood mood, float volume)
	{
		streams[stream]->SetMoodVolume(mood,volume);
	}

} MUSICSYSTEM, _MUSICSYSTEM BALIGN16;

extern MUSICSYSTEM	musicSystem;

#endif
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Taz, (c) 2000 Interactive Studios Ltd.
//
//      File : music.cpp
//   Purpose : functions to play streamed audio
// Component : Taz
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "global.h"

#include "actors.h"
#include "music.h"
#include "control.h"
#include "main.h"

#define MUSIC_MIN_PLAYTIME			0.0f// PP: 
#define MUSIC_SUBSTREAM_FADETIME	0.75f// PP: 

#define INTERACTIVEMUSIC

MUSICSYSTEM		musicSystem;

/*	--------------------------------------------------------------------------------
	Function 	: MUSICSYSTEM::InitialiseMusicSystem
	Purpose 	: Initialises the music system and opens the streams pak file
	Parameters 	: none
	Returns 	: none
	Info 		:
*/
int MUSICSYSTEM::InitialiseMusicSystem()
{
	int i;

	this->musicPak = NULL;

#if((defined CONSUMERDEMO) && (BPLATFORM == PS2))
	#ifdef USE_HOST0
		this->musicPak = bkOpenPackage("streams");
	#else
		#ifdef STANDALONE
			this->musicPak = bkOpenPackage("streams");
		#else
			this->musicPak = bkOpenPackage("tazdemo\\streams");
		#endif
	#endif
#else
		this->musicPak = bkOpenPackage("streams");
#endif

	// TP: create the streams
	for (i=0;i<MAX_STREAMS;i++)
	{
		if (!(this->streams[i] = new STREAM))
		{
			break;
		}
	}
	this->numStreams = i;
	lastTuneCRC = 0;

	this->lastScene=0;

	if (!this->musicPak)
	{
		bkPrintf("*** WARNING *** Could not open streams package.\n");
		return FALSE;
	}

	this->systemActive = TRUE;
	this->queue.waiting = false;
	return TRUE;
}

static char *loadTunes[] =
{
	NULL,									// TP: SCENE_FRONTEND,
	"taz options screen.wav",				// TP: SCENE_FRONTEND,
	NULL,									// TP: SCENE_FRONTEND,

	"zoo hub loop.wav",						// TP: SCENE_ZOOHUB,
	"zoo hub loop.wav",						// TP: SCENE_ICEDOME,
	"zoo hub loop.wav",						// TP: SCENE_SAFARI,
	"zoo hub loop.wav",						// TP: SCENE_AQUA,
	"zoo hub loop.wav",						// TP: SCENE_ZOOBOSS,

	"san fran hub loop.wav",				// TP: SCENE_CITYHUB,
	"san fran hub loop.wav",				// TP: SCENE_DEPTSTR,
	"san fran hub loop.wav",				// TP: SCENE_MUSEUM,
	"san fran hub loop.wav",				// TP: SCENE_CONSTRUCT,
	"san fran hub loop.wav",				// TP: SCENE_CITYBOSS,

	"wild west hub loop.wav",				// TP: SCENE_WILDWESTHUB,
	"wild west hub loop.wav",				// TP: SCENE_GHOSTTOWN,
	"wild west hub loop.wav",				// TP: SCENE_GOLDMINE,
	"wild west hub loop.wav",				// TP: SCENE_GRANDCANYON,
	"wild west hub loop.wav",				// TP: SCENE_WESTBOSS,

	"taz options screen.wav",				// TP: SCENE_TAZHUB,
	"disco volcano normal loop.wav",		// TP: SCENE_TAZPREBOSS,
	"taz vs tweety boss battle loop.wav",	// TP: SCENE_TAZBOSS,

	"san fran hub loop.wav",				// TP: SCENE_MUSEUMVEHICLERACE,
	"wild west hub loop.wav",				// TP: SCENE_GRANDCVEHICLERACE,
	"san fran hub loop.wav",				// TP: SCENE_DEPTSTRVEHICLERACE,

	"zoo hub loop.wav",						// TP: SCENE_DESTRUCTIONAQUA,
	"san fran hub loop.wav",				// TP: SCENE_DESTRUCTIONCONSTRUCT,
	"wild west hub loop.wav",				// TP: SCENE_DESTRUCTIONTASMANIA, // TP: this is actually a west level

	"zoo hub loop.wav",						// TP: SCENE_ICEDOMERACE,
	"wild west hub loop.wav",				// TP: SCENE_GOLDMINERACE,
	"zoo hub loop.wav",						// TP: SCENE_SAFARIRACE,

	NULL,									// PP: dunno
	NULL,									// PP: dunno
	NULL,									// PP: dunno
	NULL,									// PP: dunno

	"tarzan normal loop.wav",				// PP: taz intro?
	NULL,									
	"hiphop normal loop.wav"
};

static char	*mainTunes[] =
{
	NULL,
	"taz options screen.wav",				// TP: SCENE_FRONTEND,
	NULL,

	"zoo hub loop.wav",						// TP: SCENE_ZOOHUB,
	"snowboard normal loop.wav",			// TP: SCENE_ICEDOME,
	"skatepunk normal loop.wav",			// TP: SCENE_SAFARI,
	"surfer normal loop.wav",				// TP: SCENE_AQUA,
	"elephant pong.wav",					// TP: SCENE_ZOOBOSS,

	"san fran hub loop.wav",				// TP: SCENE_CITYHUB,
	"hiphop normal loop.wav",				// TP: SCENE_DEPTSTR,
	"ninja normal loop.wav",				// TP: SCENE_MUSEUM,
	"swat team normal loop.wav",			// TP: SCENE_CONSTRUCT,
	"gladiatoons boss battle loop.wav",		// TP: SCENE_CITYBOSS,

	"wild west hub loop.wav",				// TP: SCENE_WILDWESTHUB,
	"weretaz normal loop.wav",				// TP: SCENE_GHOSTTOWN,
	"explorer normal loop.wav",				// TP: SCENE_GOLDMINE,
	"bandit normal loop.wav",				// TP: SCENE_GRANDCANYON,
	"bomberman normal loop.wav",			// TP: SCENE_WESTBOSS,

	"tarzan normal loop.wav",				// TP: SCENE_TAZHUB,
	"disco volcano normal loop.wav",		// TP: SCENE_TAZPREBOSS,
	"taz vs tweety boss battle loop.wav",	// TP: SCENE_TAZBOSS,

	"ninja normal loop.wav",				// TP: SCENE_MUSEUMVEHICLERACE,
	"bandit normal loop.wav",				// TP: SCENE_GRANDCVEHICLERACE,
	"hiphop normal loop.wav",				// TP: SCENE_DEPTSTRVEHICLERACE,

	"surfer normal loop.wav",				// TP: SCENE_DESTRUCTIONAQUA,
	"swat team normal loop.wav",			// TP: SCENE_DESTRUCTIONCONSTRUCT,
	"weretaz normal loop.wav",				// TP: SCENE_DESTRUCTIONTASMANIA,

	"snowboard normal loop.wav",			// TP: SCENE_ICEDOMERACE,
	"explorer normal loop.wav",				// TP: SCENE_GOLDMINERACE,
	"skatepunk normal loop.wav",			// TP: SCENE_SAFARIRACE,

	NULL,									// PP: dunno
	NULL,									// PP: dunno
	NULL,									// PP: dunno
	NULL,									// PP: dunno

	"tarzan normal loop.wav",				// PP: taz intro?
	NULL,									// PP: demo dept store?
	"hiphop normal loop.wav",
};

/*  --------------------------------------------------------------------------------
	Function	: MUSICSYSTEM::ChooseMusic
	Purpose		: Central position for all music calls.
	Parameters	: scene requiring music, EMusicType type of music, EMusicState (start/stop etc)
	Returns		: 
	Info		: 
*/
void MUSICSYSTEM::ChooseMusic(int scene, EMusicType type, EMusicState state, char *name)
{
	char	*tune=NULL;

	switch(state)
	{
	case MUSICSTATE_STOP:
		if (!streams[0]->IsFree())
		{
			streams[0]->Stop();
			streams[0]->RequestFreeAudioStream();
			lastTuneCRC = 0;
		}
		break;

	case MUSICSTATE_FADEOUT:
		if (!streams[0]->IsFree())
		{
			streams[0]->Stop();
			streams[0]->RequestFreeAudioStream();
			lastTuneCRC = 0;
		}
		break;

	case MUSICSTATE_START:

		// PP: I moved this here, from the beginning of the function
		// PP: It was blocking-out 'stop' messages as well as 'start' ones
		if (!gameStatus.enableMusic) return;

		if (name)
		{
			tune = name;
		}
		else
		{
			switch(type)
			{
			case MUSICTYPE_LOAD:
				tune = loadTunes[scene];
				break;
			case MUSICTYPE_MAIN:
				tune = mainTunes[scene];
				break;
			}
		}

		// PP: record index of scene for which music is being requested
		this->lastScene=scene;

		// TP: Do check to see if new tune if different to current
		if ((tune)&&(lastTuneCRC!=bkCRC32((uchar*)tune,strlen(tune),0)))
		{
			/*if (!streams[0]->IsFree())
			{
				streams[0]->Stop();
				streams[0]->FreeAudioStream();
				lastTuneCRC = 0;
			}*/

			FadeDownAudio(0, -1, 1.0f, 0, MUSICFLAG_CLOSEAFTERALLFADE);
			QueueAudio(0, 0, tune, 255.0f, 2.0f);
			lastTuneCRC = bkCRC32((uchar*)tune,strlen(tune),0);
		}
		else
		{
			bkPrintf("Music %s already playing.\n", tune);
		}
		break;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: int MUSICSYSTEM::ReadyAudio
	Purpose 	: Readys an audio track for playing
	Parameters 	: name of audio track
	Returns 	: handle of stream used or -1 for error
	Info 		:
*/
int MUSICSYSTEM::ReadyAudio(char *name)
{
	int		stream = -1;

	if (!musicPak) return -1;

	// TP: find free stream
	for (int i=0;i<this->numStreams;i++)
	{
		if (this->streams[i]->IsFree())
		{
			stream = i;
			break;
		}
	}

	// TP: Could not find free stream
	if (stream == -1) 
	{
		bkPrintf("*** WARNING *** Could not find free audio stream for %s\n",name);
		return -1;
	}
		
	// TP: prep stream
	if (this->streams[stream]->LoadAudioStream(musicPak, name))
	{
		bkPrintf("Audio stream %s ready on channel %i\n",name,stream);
		return stream;
	}
	else
	{
		bkPrintf("*** WARNING *** Could not read audio %s\n",name);
		return -1;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: int MUSICSYSTEM::StartAudio
	Purpose 	: Start the audio track playing
	Parameters 	: handle of stream to start playing, OPTIONAL name of stream to ready and then play, volume
	Returns 	: handle of stream used or -1 for error
	Info 		:
*/
int MUSICSYSTEM::StartAudio(int stream, int subStream, char *name, float volume, float fadeTime, EStreamMood mood)
{
	if (!this->musicPak) return -1;

	if (name)
	{
		stream = ReadyAudio(name);
		if (stream==-1) return -1;
	}
	else
	{
		return -1;
	}

	this->streams[stream]->Reset();
	this->streams[stream]->SetVolume(subStream,0.0f);
	this->streams[stream]->SetDestVolume(subStream,volume,fadeTime,0);
	this->streams[stream]->SetMood(mood);
	if (!this->streams[stream]->Play())
	{
		// TP: failed to play audio so free audio stream
		this->streams[stream]->RequestFreeAudioStream();
		return -1;
	}

	return stream;
}

/*	--------------------------------------------------------------------------------
	Function 	: QueueAudio
	Purpose 	: Queue an audio track to play when the stram is free
	Parameters 	: handle of stream to start playing, OPTIONAL name of stream to ready and then play, volume
	Returns 	: handle of stream used or -1 for error
	Info 		:
*/
int MUSICSYSTEM::QueueAudio(int stream, int subStream, char *name, float volume, float fadeTime, EStreamMood mood)
{
	queue.stream = stream;
	queue.subStream = subStream;
	strcpy(queue.name, name);
	queue.volume = volume;
	queue.fadeTime = fadeTime;
	queue.mood = mood;

	queue.waiting = true;

	// PP: return handle of stream
	return stream;
}

/*	--------------------------------------------------------------------------------
	Function 	: void MUSICSYSTEM::FadeAndFreeAll
	Purpose 	: Start fading down all audio streams and then free
	Parameters 	: fadeTime in seconds
	Returns 	: none
	Info 		:
*/
void MUSICSYSTEM::FadeAndFreeAll(float fadeTime)
{
	if (!this->musicPak) return;
	for (int i=0;i<this->numStreams;i++)
	{
		if (!this->streams[i]->IsFree())
		{
			for (int j=0;j<MAX_SUBSTREAMS;j++)
			{
				this->streams[i]->SetDestVolume(j,fadeTime,0.0f,TRUE);
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: void MUSICSYSTEM::FadeDownAudio
	Purpose 	: Start fading down an audio stream and optionaly close it
	Parameters 	: stream handle, fadeTime in seconds, TRUE/FALSE close after fade
	Returns 	: none
	Info 		:
*/
void MUSICSYSTEM::FadeDownAudio(int stream, int subStream, float fadeTime, float volume, int closeAfterFade)
{
	int	i;

	if (!this->musicPak) return;

	if (this->streams[stream]->IsFree()) return;

	if (subStream==-1)
	{
		for (i=0;i<MAX_SUBSTREAMS;i++)
		{
			this->streams[stream]->SetDestVolume(i,volume,fadeTime,closeAfterFade);
		}
	}
	else
	{
		this->streams[stream]->SetDestVolume(subStream,volume,fadeTime,closeAfterFade);
	}

	if (fadeTime == 0.0f)
	{
		// TP: Kill it now
		this->streams[stream]->Update();
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: void MUSICSYSTEM::FadeUpAudio
	Purpose 	: Start fading up an audio stream
	Parameters 	: stream handle, fadeTime in seconds
	Returns 	: none
	Info 		:
*/
void MUSICSYSTEM::FadeUpAudio(int stream, int subStream, float	fadeTime, float volume)
{
	if (!this->musicPak) return;

	if (this->streams[stream]->IsFree()) return;

	this->streams[stream]->SetDestVolume(subStream,volume,fadeTime,0);
}

/*	--------------------------------------------------------------------------------
	Function 	: void MUSICSYSTEM::UpdateMusicSystem
	Purpose 	: Perform general music update
	Parameters 	: none
	Returns 	: none
	Info 		: Called once a frame
*/
void MUSICSYSTEM::UpdateMusicSystem()
{
	static	int currTrack = 0;

	if (!this->musicPak) return;

	// TP: process fades
	for (int i=0;i<this->numStreams;i++)
	{
		if (!this->streams[i]->IsFree())
		{
			this->streams[i]->Update();
		}
		else
		{
			if (queue.waiting)
			{
				// TP: tune has been queued
				if (queue.stream == i)
				{
					queue.waiting = false;
					StartAudio(queue.stream, queue.subStream, queue.name, queue.volume, queue.fadeTime, queue.mood);
				}
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: void MUSICSYSTEM::ShutdownMusicSystem
	Purpose 	: Perform general shutdown of the music system
	Parameters 	: none
	Returns 	: none
	Info 		: Will stop and close any playing audio
*/
void MUSICSYSTEM::ShutdownMusicSystem()
{
	if (!this->systemActive) return;
	if (!this->musicPak) return;

	for (int i=0;i < this->numStreams;i++)
	{
		SAFE_DELETE(this->streams[i]);
	}

	bkClosePackage(this->musicPak);
	this->musicPak = NULL;
	this->systemActive = FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: STREAM::STREAM
	Purpose 	: Default constructor
	Parameters 	: none
	Returns 	: none
	Info 		:
*/

STREAM::TAG_STREAM()
{
	stream = NULL;
	for (int i=0;i<MAX_SUBSTREAMS;i++)
	{
		volume[i] = 0.0f;
		destVolume[i] = 0.0f;
		volumeFade[i] = 0.0f;
		SetMoodVolume((EStreamMood)i,255.0f);
	}

	this->timeSinceLastMoodChange=0.0f;// PP: TEST
	currentMood = STREAMMOOD_NORMAL;
	moodRequested = STREAMMOOD_NORMAL;
	specialFadeTime = MUSIC_NO_SPECIAL_FADE_TIME;			// PP: mood-change fading is done according to the defaults, erm, by default
	state = STREAMSTATE_FREE;
	awaitingFreeAudio = false;
}


/*	--------------------------------------------------------------------------------
	Function 	: void STREAM::Stop
	Purpose 	: Stop the stream from playing
	Parameters 	: none
	Returns 	: none
	Info 		: 
*/

void STREAM::Stop()
{
	if (state==STREAMSTATE_PLAYING)
	{
		bkPrintf("bsStopAudioStream start\n");
		bsStopAudioStream(stream);
		state = STREAMSTATE_STOPPING;
		timer = 0.0f;
		bkPrintf("bsStopAudioStream end\n");
		bkPrintf("Audio stream %s stopped.\n", streamName);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: STREAM::~TAG_STREAM
	Purpose 	: Default destructor
	Parameters 	: none
	Returns 	: none
	Info 		:
*/

STREAM::~TAG_STREAM()
{
	if (state==STREAMSTATE_PLAYING)
	{
		Stop();
	}
	state=STREAMSTATE_LOADED;
	FreeAudioStream();
}


/*	--------------------------------------------------------------------------------
	Function 	: STREAM::SetVolume
	Purpose 	: Set the current volume for the given sub stream
	Parameters 	: sub stream number, volume 0-255
	Returns 	: none
	Info 		:
*/

void STREAM::SetVolume(int subStream, float volume)
{
	this->volume[subStream] = volume;

	if ((state==STREAMSTATE_PLAYING)&&(subStream < info.numSubStreams))
	{
		// TP: only set volume if its a valid substream
		bsSetAudioStreamVolume(stream, bmFloatToInt(volume), 0, subStream);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: STREAM::RequestMood
	Purpose 	: Request the mood
	Parameters 	: mood (STREAMMOOD_..), (PP:)forced fade time (see MUSICSYSTEM::RequestMood)
	Returns 	: 
	Info 		: 
*/

void STREAM::RequestMood(EStreamMood mood, const float specialFadeTimeIn)
{
	switch(map.sceneNumber)
	{
	case SCENE_MUSEUMVEHICLERACE:
	case SCENE_GRANDCVEHICLERACE:
	case SCENE_DEPTSTRVEHICLERACE:
	case SCENE_DESTRUCTIONAQUA:
	case SCENE_DESTRUCTIONCONSTRUCT:
	case SCENE_DESTRUCTIONTASMANIA:
	case SCENE_ICEDOMERACE:
	case SCENE_GOLDMINERACE:
	case SCENE_SAFARIRACE:
		this->moodRequested = STREAMMOOD_FAST;
		this->specialFadeTime = specialFadeTimeIn;
		break;
	default:
		this->moodRequested = mood;
		this->specialFadeTime=specialFadeTimeIn;
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: void STREAM::Play()
	Purpose 	: Start the stream playing
	Parameters 	: none
	Returns 	: true/false if successfull
	Info 		: 
*/

bool STREAM::Play()
{
	if (state==STREAMSTATE_LOADED)
	{
#ifdef ENABLEPRESEEK
		bkPrintf("bsPlayAudioStream start\n");
		if (bsPlayAudioStream(this->stream,BSTREAMFLAG_PRESEEK,0,0,0,NULL))
		{
			state = STREAMSTATE_PRESEEK;
#else
		bkPrintf("bsPlayAudioStream start\n");
		if (bsPlayAudioStream(this->stream,0,0,0,0,NULL))
		{
			state = STREAMSTATE_PLAYING;
#endif
			bkPrintf("bsPlayAudioStream end\n");
			bkPrintf("Audio %s started preseek.\n",this->streamName);
			return true;
		}
		else
		{
			bkPrintf("bsPlayAudioStream end\n");
			bkPrintf("*** WARNING *** Audio %s failed to start correctly\n", this->streamName);
			return false;
		}
	}
	return false;
}


/*	--------------------------------------------------------------------------------
	Function 	: STREAM::FreeAudioStream
	Purpose 	: Free the audio stream
	Parameters 	: none
	Returns 	: none
	Info 		: 
*/

void STREAM::FreeAudioStream()
{
	if (state == STREAMSTATE_LOADED)
	{
		bkPrintf("bkDeleteAudioStream start\n");
		if (stream) bkDeleteAudioStream(stream);
		awaitingFreeAudio = false;
		stream = NULL;
		state = STREAMSTATE_FREE;
		bkPrintf("bkDeleteAudioStream end\n");
		bkPrintf("Audio stream %s free'd.\n", streamName);
	}
}


/*	--------------------------------------------------------------------------------
	Function 	: STREAM::SetMoodVolume
	Purpose 	: Set the volume to fade to when using the mood
	Parameters 	: none
	Returns 	: none
	Info 		:
*/
void STREAM::SetMoodVolume(EStreamMood mood, float volume)
{
	if (mood >= MAX_SUBSTREAMS) return;
	moodVolume[mood] = volume;
}

/*	--------------------------------------------------------------------------------
	Function 	: STREAM::Reset
	Purpose 	: Reset the stream volumes etc.
	Parameters 	: none
	Returns 	: none
	Info 		:
*/
void STREAM::Reset()
{
	for (int i=0;i<MAX_SUBSTREAMS;i++)
	{
		volume[i] = 0.0f;
		destVolume[i] = 0.0f;
		volumeFade[i] = 0.0f;
	}
	currentMood = STREAMMOOD_NORMAL;
	moodRequested = STREAMMOOD_NORMAL;
}

/*	--------------------------------------------------------------------------------
	Function 	: void STREAM::Update
	Purpose 	: Perform general update for the stream
	Parameters 	: none
	Returns 	: none
	Info 		: 
*/
void STREAM::Update()
{
	int i,j;

	timer += fTime;		// TP: update general timer

	if (state == STREAMSTATE_STOPPING)
	{
#if BPLATFORM==PS2
		if (timer > 2.0f)
#else
		if (timer > 0.5f)
#endif
		{
			// TP: give it a bit of time to settle down then enter loaded state
			state = STREAMSTATE_LOADED;
		}
	}

	if ((awaitingFreeAudio)&&(state==STREAMSTATE_LOADED))
	{
		FreeAudioStream();
	}

	// TP: check if the audio is paused and preseeking, if finished then unpause
#ifdef ENABLEPRESEEK
	if (state == STREAMSTATE_PRESEEK)
	{
		if (bsGetAudioStreamFlags(stream,BSTREAMFLAG_PAUSE))
		{
			bsClearAudioStreamFlags(stream,BSTREAMFLAG_PAUSE);
			state = STREAMSTATE_PLAYING;
			bkPrintf("Audio %s un-paused.\n",this->streamName);
		}
	}
#endif

	this->timeSinceLastMoodChange += fTime;

	// TP: dont want to do any more work if not playing music
	if (state != STREAMSTATE_PLAYING) return;

	// TP: Check for mood change
#ifndef STREAM_VOLUME_WORKAROUND// PP: TEMP BABEL WORKAROUND
	if (this->currentMood != this->moodRequested)
#endif// PP: ndef STREAM_VOLUME_WORKAROUND
	{
#ifdef INTERACTIVEMUSIC		

		if((this->specialFadeTime != MUSIC_NO_SPECIAL_FADE_TIME) || (timeSinceLastMoodChange > MUSIC_MIN_PLAYTIME))
		{
			if (this->info.numSubStreams > 1)
			{// PP: THIS BLOCK WAS REMMED OUT

				float	fadeTime;

				if(this->specialFadeTime == MUSIC_NO_SPECIAL_FADE_TIME)
				{
					fadeTime=MUSIC_SUBSTREAM_FADETIME;
				}
				else
				{
					fadeTime=this->specialFadeTime;
				}

				/*SetDestVolume(STREAMMOOD_NORMAL, 0.0f, fadeTime);
				SetDestVolume(STREAMMOOD_SLOW, 0.0f, fadeTime);
				SetDestVolume(STREAMMOOD_FAST, 0.0f, fadeTime);*/

				for (int i=0;i<3;i++)
				{
					if (this->moodRequested==i)
					{
#ifdef STREAM_VOLUME_WORKAROUND
						// PP: use special fade time
						SetDestVolume(i, gameStatus.globalSettings.musicVolume, fadeTime);
#else// PP: ifndef STREAM_VOLUME_WORKAROUND
						// PP: use special fade time
						SetDestVolume(i, moodVolume[i],fadeTime);
#endif// PP: ndef STREAM_VOLUME_WORKAROUND
					}
					else
					{
						SetDestVolume(i, 0.0f, fadeTime);
					}
				}
			}
			else
			{
#ifdef STREAM_VOLUME_WORKAROUND// PP: TEMP BABEL WORKAROUND
				// PP: use special fade time
				SetDestVolume(0, gameStatus.globalSettings.musicVolume, 0.1f);
#endif// PP: def STREAM_VOLUME_WORKAROUND
			}

			this->timeSinceLastMoodChange = 0.0f;
			this->currentMood = this->moodRequested;
		}
#endif
	}

#ifdef INTERACTIVEMUSIC
	// NH: Can't define a variable within a loop on PC as it gets declared twice and causes an error! Anyway this is already defined at the top!
	for (j=0;j<MAX_SUBSTREAMS;j++)
#else
	for (j=0;j<1;j++)
#endif
	{
		if (!this->stream) continue;

		if (this->volumeFade[j] != 0.0f)
		{
			this->volume[j] += this->volumeFade[j]*speedRatio;
			if (this->volumeFade[j] < 0.0f)
			{
				// TP: fading down
				if (this->volume[j] <= this->destVolume[j])
				{
					SetVolume(j, this->destVolume[j]);
					this->volumeFade[j] = 0.0f;

					if (this->flags&MUSICFLAG_CLOSEAFTERALLFADE)
					{
						// TP: should wait for all substreams volume to zero
						bool kill = true;
						for (i=0;i<MAX_SUBSTREAMS;i++)
						{
							if (this->volume[i] != 0.0f) kill = false;
						}
						if (kill)
						{
							Stop();
							RequestFreeAudioStream();
							continue;
						}
					}
				}
				else
				{
					SetVolume(j, this->volume[j]);
				}
			}
			else
			{
				if (this->volume[j] > this->destVolume[j])
				{
					this->volumeFade[j] = 0.0f;
					SetVolume(j, this->destVolume[j]);
				}
				else
				{
					SetVolume(j, this->volume[j]);
				}
			}
		}
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: int STREAM::LoadAudioStream(TBPackageIndex *musicPak, char *name)
	Purpose 	: Load the stream
	Parameters 	: music pak, name of stream
	Returns 	: none
	Info 		: 
*/
int STREAM::LoadAudioStream(TBPackageIndex *musicPak, char *name)
{
	bkPrintf("bkLoadAudioStream start\n");
	this->stream = bkLoadAudioStream(musicPak, name, BDEFAULTGROUP);
	bkPrintf("bkLoadAudioStream end\n");

	awaitingFreeAudio = false;

	if (this->stream)
	{
		if (strlen(name) < 64)
		{
			strcpy(this->streamName,name);
		}
		else
		{
			STRNCPY(this->streamName,name,64);
		}
	
		bkPrintf("bsGetAudioStreamInfo start\n");
		bsGetAudioStreamInfo(this->stream,&this->info.frequency,&this->info.noofSamples,&this->info.bitDepth,
			&this->info.compressionRatio,NULL,NULL);
		bkPrintf("bsGetAudioStreamInfo end\n");

		// TP: No current way of obtaining this information through the Babel API
#if((BPLATFORM == XBOX)||(BPLATFORM == GAMECUBE)||(BPLATFORM == PC))
		this->info.numSubStreams = this->stream->noofStreams;
#else
		this->info.numSubStreams = this->stream->noofChannels;
#endif

		state = STREAMSTATE_LOADING;
		// TP: no background load on this so go straight to loaded
		state = STREAMSTATE_LOADED;

		return TRUE;
	}
	else
	{
		bkPrintf("*** WARNING *** Could not read audio %s\n",name);
		return FALSE;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: STREAM::SetDestVolume
	Purpose 	: Set the destination volume level and a fade time in seconds
	Parameters 	: sub stream number, volume 0-255, fadeTime
	Returns 	: none
	Info 		:
*/
void STREAM::SetDestVolume(int subStream, float destVolume, float fadeTime, int flags)
{
	// TP: dont process other request while there is another tune queued
	if (musicSystem.queue.waiting) return;

	this->destVolume[subStream] = destVolume;
	if (fadeTime == 0.0f)
	{
		this->volumeFade[subStream] = destVolume - this->volume[subStream];
	}
	else
	{
		this->volumeFade[subStream] = (destVolume - this->volume[subStream]) / (60.0f * fadeTime);
	}

	// TP: check for possibility of the destVolume and the current volume being the same
	// TP: this is a problem only if the MUSICFLAG_CLOSEAFTERALLFADE is set
	if (flags&MUSICFLAG_CLOSEAFTERALLFADE)
	{
		// TP: make sure the volume fade is set to a negative value to get the MUSICFLAG_CLOSEAFTERALLFADE to have affect
		if (this->volumeFade[subStream]==0.0f) this->volumeFade[subStream] = -1.0f;
	}

	if (flags!=-1)	this->flags = flags;
}


/*  --------------------------------------------------------------------------------
	Function	: MUSICSYSTEM::IsMusicPlaying
	Purpose		: Check if the named music is currently playing
	Parameters	: name of tune
	Returns		: true/false
	Info		: 
*/

bool MUSICSYSTEM::IsMusicPlaying(char *name)
{
	int i;
	char *tune;

	for (i=0;i<MAX_STREAMS;i++)
	{
		if (streams[i])
		{
			if (tune = streams[i]->CurrentTune())
			{
				if (strcmpi(tune, name)==0)
				{
					return true;
				}
			}
		}
	}
	return false;
}


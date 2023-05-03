#include "../tomb5/pch.h"
#include "audio.h"
#include "file.h"
#include "function_stubs.h"
#include "dxshell.h"
#include "winmain.h"
#include "../game/control.h"
#include "LoadSave.h"

const char* TrackFileNames[136] =
{
	"xa1_TL_10B.wav",
	"xa1_Z_10.wav",
	"xa1_TL_05.wav",
	"xa1_TL_08.wav",
	"xa1_TL_11.wav",
	"xa1_ANDYPEW.wav",
	"xa1_SECRET.wav",
	"xa1_TL_02.wav",
	"xa2_HMMM05.wav",
	"xa2_TL_01.wav",
	"xa2_ATTACK04.wav",
	"xa2_UWATER2B.wav",
	"xa2_SPOOKY2A.wav",
	"xa2_TL_10A.wav",
	"xa2_HMMM02.wav",
	"xa2_TOMS01.wav",
	"xa3_Attack03.wav",
	"xa3_Attack02.wav",
	"xa3_Hmmm01.wav",
	"xa3_Stealth1.wav",
	"xa3_Stealth2.wav",
	"xa3_Attack01.wav",
	"xa3_TL_06.wav",
	"xa3_TL_03.wav",
	"xa4_hmmm06.wav",
	"xa4_mil01.wav",
	"xa4_Z_03.wav",
	"xa4_hit01.wav",
	"xa4_spooky05.wav",
	"xa4_drama01.wav",
	"xa4_stealth4.wav",
	"xa4_mil05.wav",
	"xa5_HMMM04.wav",
	"xa5_MIL06.wav",
	"xa5_SPOOKY02.wav",
	"xa5_TL_12.wav",
	"xa5_MIL02A.wav",
	"xa5_HMMM03.wav",
	"xa5_MIL02.wav",
	"xa5_TL_04.wav",
	"xa6_Mil04.wav",
	"xa6_Solo01.wav",
	"xa6_Z12.wav",
	"xa6_Stealth3.wav",
	"xa6_AuthSolo.wav",
	"xa6_Spooky03.wav",
	"xa6_Z13.wav",
	"xa6_Z_04anim.wav",
	"xa7_z_06a.wav",
	"xa7_andyoooh.wav",
	"xa7_andyooer.wav",
	"xa7_tl_07.wav",
	"xa7_z_02.wav",
	"xa7_evibes01.wav",
	"xa7_z_06.wav",
	"xa7_authtr.wav",
	"xa8_mil03.wav",
	"xa8_fightsc.wav",
	"xa8_richcut3.wav",
	"xa8_z_13.wav",
	"xa8_z_08.wav",
	"xa8_uwater2a.wav",
	"xa8_jobyalrm.wav",
	"xa8_mil02b.wav",
	"xa9_swampy.wav",
	"xa9_evibes02.wav",
	"xa9_gods01.wav",
	"xa9_z_03.wav",
	"xa9_richcut4.wav",
	"xa9_title4.wav",
	"xa9_spooky01.wav",
	"xa9_chopin01.wav",
	"xa10_echoir01.wav",
	"xa10_title3.wav",
	"xa10_perc01.wav",
	"xa10_vc01.wav",
	"xa10_title2.wav",
	"xa10_z_09.wav",
	"xa10_spooky04.wav",
	"xa10_z_10.wav",
	"xa11_vc01atv.wav",
	"xa11_andy3.wav",
	"xa11_title1.wav",
	"xa11_flyby1.wav",
	"xa11_monk_2.wav",
	"xa11_andy4.wav",
	"xa11_flyby3.wav",
	"xa11_flyby2.wav",
	"xa12_moses01.wav",
	"xa12_andy4b.wav",
	"xa12_z_10.wav",
	"xa12_flyby4.wav",
	"xa12_richcut1.wav",
	"xa12_andy5.wav",
	"xa12_z_05.wav",
	"xa12_z_01.wav",
	"xa13_Joby3.wav",
	"xa13_Andy7.wav",
	"xa13_Andrea3B.wav",
	"xa13_cossack.wav",
	"xa13_Z_07.wav",
	"xa13_Andy6.wav",
	"xa13_Andrea3.wav",
	"xa13_Joby7.wav",
	"xa14_uwater1.wav",
	"xa14_joby1.wav",
	"xa14_andy10.wav",
	"xa14_richcut2.wav",
	"xa14_andrea1.wav",
	"xa14_andy8.wav",
	"xa14_joby6.wav",
	"xa14_ecredits.wav",
	"xa15_boss_01.wav",
	"xa15_joby2.wav",
	"xa15_joby4.wav",
	"xa15_joby5.wav",
	"xa15_joby9.wav",
	"xa15_a_andy.wav",
	"xa15_a_rome.wav",
	"xa15_andy2.wav",
	"xa16_Joby8.wav",
	"xa16_A_Sub_Amb.wav",
	"xa16_Joby10.wav",
	"xa16_A_Harbour_out.wav",
	"xa16_A_Andy_Out_Norm.wav",
	"xa16_A_Andy_Out_Spooky.wav",
	"xa16_A_Rome_Day.wav",
	"xa16_A_Underwater.wav",
	"xa17_A_Rome_Night.wav",
	"xa17_A_VC_Saga.wav",
	"xa17_A_Industry.wav",
	"xa17_Andrea2.wav",
	"xa17_Andy1.wav",
	"xa17_Andrea4.wav",
	"xa17_Andy9.wav",
	"xa17_Andy11.wav",
};

#pragma warning(push)
#pragma warning(disable : 4838)
#pragma warning(disable : 4309)
static char source_wav_format[50] =
{
	2, 0, 2, 0, 68, 172, 0, 0, 71, 173, 0, 0, 0, 8, 4, 0, 32, 0, 244, 7, 7, 0, 0, 1, 0, 0, 0,
	2, 0, 255, 0, 0, 0, 0, 192, 0, 64, 0, 240, 0, 0, 0, 204, 1, 48, 255, 136, 1, 24, 255
};
#pragma warning(pop)

HACMDRIVER hACMDriver;
uchar* wav_file_buffer;
uchar* ADPCMBuffer;
bool acm_ready;

long XATrack = -1;

static FILE* audio_stream_fp;
static LPDIRECTSOUNDBUFFER DSBuffer;
static LPDIRECTSOUNDNOTIFY DSNotify;
static HACMSTREAM hACMStream;
static HACMDRIVERID hACMDriverID;
static HANDLE NotificationThreadHandle;
static CRITICAL_SECTION audio_cs;
static ACMSTREAMHEADER StreamHeaders[4];
static HANDLE NotifyEventHandles[2];
static uchar* audio_fp_write_ptr;
static uchar* pAudioWrite;
static ulong AudioBytes;
static long audio_play_mode;
static long audio_buffer_size;
static long CurrentNotify;
static long NextWriteOffset;
static long NotifySize;
static long eof_counter;
static volatile bool reading_audio_file;
static volatile bool continue_reading_audio_file;

#define ACM_BUFFER_SIZE		0x5800
#define FILE_BUFFER_SIZE	((ACM_BUFFER_SIZE * 2) * 1)		//WAS * 5
#define EOF_LEEWAY			2								//WAS 8.. every 2 = about 1 extra second of playtime

void S_CDPlay(long track, long mode)
{
	if (acm_ready)
	{
		eof_counter = 0;
		IsAtmospherePlaying = track == CurrentAtmosphere;
		S_CDStop();
		ACMEmulateCDPlay(track, mode);
	}
}

void S_CDStop()
{
	if (acm_ready && audio_stream_fp)
	{
		memset(wav_file_buffer, 0, FILE_BUFFER_SIZE);
		DSBuffer->Stop();
		DSBuffer->SetCurrentPosition(0);
		while (reading_audio_file) {};
		fclose(audio_stream_fp);
		audio_stream_fp = 0;
		eof_counter = 0;
		XATrack = -1;
	}
}

void S_CDFade(long n)
{

}

void S_StartSyncedAudio(long track)
{
	S_CDStop();
	S_CDPlay(track, 2);
}

void ACMSetVolume()
{
	long volume;

	if (!MusicVolume)
		volume = -10000;
	else
		volume = -4000 * (100 - MusicVolume) / 100;

	if (DSBuffer)
		DSBuffer->SetVolume(volume);
}

void OpenStreamFile(char* name)
{
	__try
	{
		EnterCriticalSection(&audio_cs);
	}
	__finally
	{
		LeaveCriticalSection(&audio_cs);
	}

	audio_stream_fp = FileOpen(name);

	if (!audio_stream_fp)
	{
		Log("%s - Not Found", name);
		return;
	}

	fseek(audio_stream_fp, 90, SEEK_SET);
	audio_fp_write_ptr = wav_file_buffer;
	memset(wav_file_buffer, 0, FILE_BUFFER_SIZE);

	if (fread(wav_file_buffer, 1, FILE_BUFFER_SIZE, audio_stream_fp) < FILE_BUFFER_SIZE && audio_play_mode == 1)
	{
		fseek(audio_stream_fp, 90, SEEK_SET);
		Log("FileReset In OpenStreamFile");
	}
}

void GetADPCMData()
{
	if (!audio_stream_fp)
		return;

	memset(audio_fp_write_ptr, 0, ACM_BUFFER_SIZE);

	if (fread(audio_fp_write_ptr, 1, ACM_BUFFER_SIZE, audio_stream_fp) < ACM_BUFFER_SIZE && audio_play_mode == 1)
	{
		Log("FileReset In GetADPCMData");
		fseek(audio_stream_fp, 90, SEEK_SET);
	}

	audio_fp_write_ptr += ACM_BUFFER_SIZE;

	if ((long)audio_fp_write_ptr >= long(wav_file_buffer + FILE_BUFFER_SIZE))
		audio_fp_write_ptr = wav_file_buffer;
}

void ACMEmulateCDPlay(long track, long mode)
{
	char name[256];

	__try
	{
		EnterCriticalSection(&audio_cs);
	}
	__finally
	{
		LeaveCriticalSection(&audio_cs);
	}

	wsprintf(name, "audio\\%s", TrackFileNames[track]);

	if (mode)
		Log("Playing %s %s %d", name, "Looped", track);
	else
		Log("Playing %s %s %d", name, "", track);

	XATrack = track;
	audio_play_mode = mode;
	OpenStreamFile(name);

	if (!audio_stream_fp)
		return;

	memcpy(ADPCMBuffer, audio_fp_write_ptr, ACM_BUFFER_SIZE);
	GetADPCMData();
	DXAttempt(DSBuffer->Lock(0, audio_buffer_size, (LPVOID*)&pAudioWrite, &AudioBytes, 0, 0, 0));
	acmStreamConvert(hACMStream, &StreamHeaders[0], ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_START);
	memcpy(ADPCMBuffer, audio_fp_write_ptr, ACM_BUFFER_SIZE);
	GetADPCMData();
	acmStreamConvert(hACMStream, &StreamHeaders[1], ACM_STREAMCONVERTF_BLOCKALIGN);
	DXAttempt(DSBuffer->Unlock(pAudioWrite, audio_buffer_size, 0, 0));
	CurrentNotify = 2;
	NextWriteOffset = 2 * NotifySize;
	ACMSetVolume();
	DSBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

BOOL __stdcall ACMEnumCallBack(HACMDRIVERID hadid, DWORD_PTR dwInstance, DWORD fdwSupport)
{
	ACMDRIVERDETAILS driver;

	memset(&driver, 0, sizeof(driver));
	driver.cbStruct = sizeof(ACMDRIVERDETAILS);
	acmDriverDetails(hadid, &driver, 0);

	if (strcmp(driver.szShortName, "MS-ADPCM"))
		return 1;

	hACMDriverID = hadid;
	return 0;
}

long ACMSetupNotifications()
{
	DSBPOSITIONNOTIFY posNotif[5];
	ulong ThreadId;
	long result;

	NotifyEventHandles[0] = CreateEvent(0, 0, 0, 0);
	NotifyEventHandles[1] = CreateEvent(0, 0, 0, 0);
	posNotif[0].dwOffset = NotifySize;
	posNotif[0].hEventNotify = NotifyEventHandles[0];
	Log("Set notifies for position %lu", posNotif[0].dwOffset);

	for (int i = 1; i < 4; i++)
	{
		posNotif[i].dwOffset = NotifySize + posNotif[i - 1].dwOffset;
		posNotif[i].hEventNotify = NotifyEventHandles[0];
		Log("Set notifies for positions %lu", posNotif[i].dwOffset);
	}

	posNotif[3].dwOffset--;
	posNotif[4].dwOffset = -1;
	posNotif[4].hEventNotify = NotifyEventHandles[1];
	NotificationThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ACMHandleNotifications, 0, 0, &ThreadId);

	if (!NotificationThreadHandle)
		Log("Create Notification Thread failed");

	result = DSNotify->SetNotificationPositions(5, posNotif);

	if (result != DS_OK)
	{
		CloseHandle(NotifyEventHandles[0]);
		CloseHandle(NotifyEventHandles[1]);
		NotifyEventHandles[0] = 0;
		NotifyEventHandles[1] = 0;
	}
	else
		Log("Setup Notifications OK");

	return result;
}

void FillADPCMBuffer(char* p, long track)
{
	reading_audio_file = 1;

	__try
	{
		EnterCriticalSection(&audio_cs);
	}
	__finally
	{
		LeaveCriticalSection(&audio_cs);
	}

	if (!audio_stream_fp)
	{
		reading_audio_file = 0;
		continue_reading_audio_file = 0;
		return;
	}

	if (track != XATrack || track == -1)
	{
		Log("Not Current Track %d", track);
		reading_audio_file = 0;
		continue_reading_audio_file = 0;
		return;
	}

	memset(p, 0, ACM_BUFFER_SIZE);

	if (!audio_stream_fp)
	{
		reading_audio_file = 0;
		continue_reading_audio_file = 0;
		return;
	}

	fread(p, 1, ACM_BUFFER_SIZE, audio_stream_fp);

	if (audio_stream_fp && feof(audio_stream_fp))
	{
		if (audio_play_mode == 1)
			fseek(audio_stream_fp, 90, SEEK_SET);
		else
		{
			eof_counter++;

			if (eof_counter > EOF_LEEWAY)
			{
				eof_counter = 0;

				if (audio_play_mode == 2)
				{
					reading_audio_file = 0;
					continue_reading_audio_file = 0;
					S_CDStop();
					return;
				}

				if (CurrentAtmosphere && !IsAtmospherePlaying)
				{
					reading_audio_file = 0;
					continue_reading_audio_file = 0;
					S_CDStop();
					S_CDPlay(CurrentAtmosphere, 1);
					return;
				}
			}
		}
	}

	reading_audio_file = 0;
	continue_reading_audio_file = 1;
}

long ACMHandleNotifications()
{
	char* write;
	ulong wait, bytes;

	while ((wait = WaitForMultipleObjects(2, NotifyEventHandles, 0, INFINITE)) != WAIT_FAILED)
	{
		EnterCriticalSection(&audio_cs);

		if (!wait && DSBuffer)
		{
			memcpy(ADPCMBuffer, audio_fp_write_ptr, ACM_BUFFER_SIZE);

			if (XATrack == -1)
				memset(ADPCMBuffer, 0, ACM_BUFFER_SIZE);
			else
				FillADPCMBuffer((char*)audio_fp_write_ptr, XATrack);

			if (continue_reading_audio_file)
			{
				audio_fp_write_ptr += ACM_BUFFER_SIZE;

				if ((long)audio_fp_write_ptr >= long(wav_file_buffer + FILE_BUFFER_SIZE))
					audio_fp_write_ptr = wav_file_buffer;

				DSBuffer->Lock(NextWriteOffset, NotifySize, (LPVOID*)&write, &bytes, 0, 0, 0);
				acmStreamConvert(hACMStream, &StreamHeaders[CurrentNotify], ACM_STREAMCONVERTF_BLOCKALIGN);
				DSBuffer->Unlock(&write, bytes, 0, 0);
				NextWriteOffset += bytes;

				if (NextWriteOffset >= audio_buffer_size)
					NextWriteOffset -= audio_buffer_size;

				CurrentNotify = (CurrentNotify + 1) & 3;
			}
		}

		LeaveCriticalSection(&audio_cs);

		if (!DSBuffer)
			break;
	}

	return DS_OK;
}

bool ACMInit()
{
	DSBUFFERDESC desc;
	static WAVEFORMATEX wav_format;
	static ulong StreamSize;
	ulong version, pMetric;

	version = acmGetVersion();
	InitializeCriticalSection(&audio_cs);
	acm_ready = 0;
	Log("ACM Version %u.%.02u", ((version >> 16) & 0xFFFF) >> 8, (version >> 16) & 0xFF);
	acmDriverEnum(ACMEnumCallBack, 0, 0);

	if (!hACMDriverID)
	{
		Log("*** Unable To Locate MS-ADPCM Driver ***");
		return 0;
	}

	if (acmDriverOpen(&hACMDriver, hACMDriverID, 0))
	{
		Log("*** Failed To Open Driver MS-ADPCM Driver ***");
		return 0;
	}

	ADPCMBuffer = (uchar*)malloc(ACM_BUFFER_SIZE);
	wav_file_buffer = (uchar*)malloc(FILE_BUFFER_SIZE);
	wav_format.wFormatTag = WAVE_FORMAT_PCM;
	acmMetrics(0, ACM_METRIC_MAX_SIZE_FORMAT, &pMetric);
	acmFormatSuggest(hACMDriver, (LPWAVEFORMATEX)&source_wav_format, &wav_format, pMetric, ACM_FORMATSUGGESTF_WFORMATTAG);
	audio_buffer_size = 0x577C0;
	NotifySize = 0x15DF0;

	memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwBufferBytes = 0x577C0;
	desc.dwFlags = DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
	desc.lpwfxFormat = &wav_format;
	App.dx.lpDS->CreateSoundBuffer(&desc, &DSBuffer, 0);
	DSBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&DSNotify);

	ACMSetupNotifications();
	acmStreamOpen(&hACMStream, hACMDriver, (LPWAVEFORMATEX)&source_wav_format, &wav_format, 0, 0, 0, 0);
	acmStreamSize(hACMStream, ACM_BUFFER_SIZE, &StreamSize, 0);
	DXAttempt(DSBuffer->Lock(0, audio_buffer_size, (LPVOID*)&pAudioWrite, &AudioBytes, 0, 0, 0));
	memset(pAudioWrite, 0, audio_buffer_size);

	for (int i = 0; i < 4; i++)
	{
		memset(&StreamHeaders[i], 0, sizeof(ACMSTREAMHEADER));
		StreamHeaders[i].cbStruct = sizeof(ACMSTREAMHEADER);
		StreamHeaders[i].pbSrc = ADPCMBuffer;
		StreamHeaders[i].cbSrcLength = ACM_BUFFER_SIZE;
		StreamHeaders[i].cbDstLength = StreamSize;
		StreamHeaders[i].pbDst = &pAudioWrite[NotifySize * i];
		acmStreamPrepareHeader(hACMStream, &StreamHeaders[i], 0);
	}

	DXAttempt(DSBuffer->Unlock(pAudioWrite, audio_buffer_size, 0, 0));
	acm_ready = 1;
	return 1;
}

void ACMClose()
{
	if (!acm_ready)
		return;

	EnterCriticalSection(&audio_cs);
	S_CDStop();
	CloseHandle(NotifyEventHandles[0]);
	CloseHandle(NotifyEventHandles[1]);
	acmStreamUnprepareHeader(hACMStream, &StreamHeaders[0], 0);
	acmStreamUnprepareHeader(hACMStream, &StreamHeaders[1], 0);
	acmStreamUnprepareHeader(hACMStream, &StreamHeaders[2], 0);
	acmStreamUnprepareHeader(hACMStream, &StreamHeaders[3], 0);
	acmStreamClose(hACMStream, 0);
	acmDriverClose(hACMDriver, 0);

	if (DSNotify)
	{
		Log("Released %s @ %x - RefCnt = %d", "Notification", DSNotify, DSNotify->Release());
		DSNotify = 0;
	}
	else
		Log("%s Attempt To Release NULL Ptr", "Notification");

	if (DSBuffer)
	{
		Log("Released %s @ %x - RefCnt = %d", "Stream Buffer", DSBuffer, DSBuffer->Release());
		DSBuffer = 0;
	}
	else
		Log("%s Attempt To Release NULL Ptr", "Stream Buffer");

	LeaveCriticalSection(&audio_cs);
}

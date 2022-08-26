#include "../tomb5/pch.h"
#include "dxsound.h"
#include "function_stubs.h"
#include "dxshell.h"

#define DSPrimary	VAR_U_(0x0086CAF0, LPDIRECTSOUNDBUFFER)
#define DS_Samples	ARRAY_(0x0086CAF8, DS_SAMPLE, [32])
#define DS_Buffers	ARRAY_(0x0086BEF0, DS_SAMPLE, [256])
#define d_hACMStream	VAR_U_(0x0086BEEC, HACMSTREAM)

#pragma warning(push)
#pragma warning(disable : 4838)
#pragma warning(disable : 4309)
static char source_pcm_format[50] =
{
	2, 0, 1, 0, 34, 86, 0, 0, 147, 43, 0, 0, 0, 2, 4, 0, 32, 0, 244, 3, 7, 0, 0, 1, 0, 0, 0, 2, 0, 255, 0, 0,
	0, 0, 192, 0, 64, 0, 240, 0, 0, 0, 204, 1, 48, 255, 136, 1, 24, 255
};
#pragma warning(pop)

static MMRESULT mmresult;
static WAVEFORMATEX pcm_format;
static ACMSTREAMHEADER ACMStreamHeader;
static char* decompressed_samples_buffer;

bool DXChangeOutputFormat(long nSamplesPerSec, bool force)
{
	WAVEFORMATEX pcfxFormat;
	static long lastSPC;

	if (!force && lastSPC == nSamplesPerSec)
		return 1;

	lastSPC = nSamplesPerSec;
	pcfxFormat.wFormatTag = WAVE_FORMAT_PCM;
	pcfxFormat.nChannels = 2;
	pcfxFormat.nSamplesPerSec = nSamplesPerSec;
	pcfxFormat.nAvgBytesPerSec = 4 * nSamplesPerSec;
	pcfxFormat.nBlockAlign = 4;
	pcfxFormat.wBitsPerSample = 16;
	pcfxFormat.cbSize = 0;
	S_SoundStopAllSamples();

	if (DSPrimary && DXAttempt(DSPrimary->SetFormat(&pcfxFormat)) != DS_OK)
	{
		Log(1, "Can't set sound output format to %d", pcfxFormat.nSamplesPerSec);
		return 0;
	}

	return 1;
}

void DSChangeVolume(long num, long volume)
{
	if (DS_Samples[num].buffer)
		DS_Samples[num].buffer->SetVolume(volume);
}

void DSAdjustPitch(long num, long pitch)
{
	ulong frequency;

	if (DS_Samples[num].buffer)
	{
		frequency = ulong((float)pitch / 65536.0F * 22050.0F);

		if (frequency < 100)
			frequency = 100;
		else if (frequency > 100000)
			frequency = 100000;

		DS_Samples[num].buffer->SetFrequency(frequency);
	}
}

void DSAdjustPan(long num, long pan)
{
	if (DS_Samples[num].buffer)
	{
		if (pan < 0)
		{
			if (pan < -0x4000)
				pan = -0x4000 - pan;
		}
		else if (pan > 0 && pan > 0x4000)
			pan = 0x8000 - pan;

		pan >>= 4;
		DS_Samples[num].buffer->SetPan(pan);
	}
}

bool DXSetOutputFormat()
{
	DSBUFFERDESC desc;

	Log(2, "DXSetOutputFormat");
	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	if (DXAttempt(App.dx.lpDS->CreateSoundBuffer(&desc, &DSPrimary, 0)) == DS_OK)
	{
		DXChangeOutputFormat(sfx_frequencies[SoundQuality], 0);
		DSPrimary->Play(0, 0, DSBPLAY_LOOPING);
		return 1;
	}

	Log(1, "Can't Get Primary Sound Buffer");
	return 0;
}

bool DXDSCreate()
{
	Log(2, "DXDSCreate");
	DXAttempt(DirectSoundCreate(G_dxinfo->DSInfo[G_dxinfo->nDS].lpGuid, &App.dx.lpDS, 0));
	DXAttempt(App.dx.lpDS->SetCooperativeLevel(App.hWnd, DSSCL_EXCLUSIVE));
	DXSetOutputFormat();
	sound_active = 1;
	return 1;
}

bool InitSampleDecompress()
{
	pcm_format.wFormatTag = WAVE_FORMAT_PCM;
	pcm_format.cbSize = 0;
	pcm_format.nChannels = 1;
	pcm_format.nAvgBytesPerSec = 44100;
	pcm_format.nSamplesPerSec = 22050;
	pcm_format.nBlockAlign = 2;
	pcm_format.wBitsPerSample = 16;
	mmresult = acmStreamOpen(&d_hACMStream, hACMDriver, (LPWAVEFORMATEX)source_pcm_format, &pcm_format, 0, 0, 0, 0);

	if (mmresult != DS_OK)
		Log(1, "Stream Open %d", mmresult);

	decompressed_samples_buffer = (char*)MALLOC(0x40000);
	samples_buffer = (char*)MALLOC(0x4005A);
	memset(&ACMStreamHeader, 0, sizeof(ACMSTREAMHEADER));
	ACMStreamHeader.pbSrc = (uchar*)(samples_buffer + 90);
	ACMStreamHeader.cbStruct = sizeof(ACMSTREAMHEADER);
	ACMStreamHeader.cbSrcLength = 0x40000;
	ACMStreamHeader.cbDstLength = 0x40000;
	ACMStreamHeader.pbDst = (uchar*)decompressed_samples_buffer;
	mmresult = acmStreamPrepareHeader(d_hACMStream, &ACMStreamHeader, 0);

	if (mmresult != DS_OK)
		Log(1, "Prepare Stream %d", mmresult);

	return 1;
}

bool FreeSampleDecompress()
{
	ACMStreamHeader.cbSrcLength = 0x40000;
	mmresult = acmStreamUnprepareHeader(d_hACMStream, &ACMStreamHeader, 0);

	if (mmresult != DS_OK)
		Log(1, "UnPrepare Stream %d", mmresult);

	mmresult = acmStreamClose(d_hACMStream, 0);

	if (mmresult != DS_OK)
		Log(1, "Stream Close %d", mmresult);

	FREE(decompressed_samples_buffer);
	FREE(samples_buffer);
	return 1;
}

bool DXCreateSampleADPCM(char* data, long comp_size, long uncomp_size, long num)
{
	LPWAVEFORMATEX format;
	LPDIRECTSOUNDBUFFER buffer;
	LPVOID dest;
	DSBUFFERDESC desc;
	ulong bytes;

	Log(8, "DXCreateSampleADPCM");

	if (!App.dx.lpDS)
		return 0;

	format = (LPWAVEFORMATEX)(data + 20);

	if (format->nSamplesPerSec != 22050)
		Log(1, "Incorrect SamplesPerSec");

	ACMStreamHeader.cbSrcLength = comp_size - (sizeof(WAVEFORMATEX) + format->cbSize + 40);
	mmresult = acmStreamConvert(d_hACMStream, &ACMStreamHeader, ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_START);

	if (mmresult != DS_OK)
		Log(1, "Stream Convert %d", mmresult);

	memset(&desc, 0, sizeof(DSBUFFERDESC));
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwReserved = 0;
	desc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
	desc.dwBufferBytes = uncomp_size - 32;
	desc.lpwfxFormat = &pcm_format;

	if (DXAttempt(App.dx.lpDS->CreateSoundBuffer(&desc, &buffer, 0)) != DS_OK)
	{
		Log(1, "Unable To Create Sound Buffer");
		return 0;
	}

	if (DXAttempt(buffer->Lock(0, uncomp_size - 32, &dest, &bytes, 0, 0, 0)) != DS_OK)
	{
		Log(1, "Unable To Lock Sound Buffer");
		return 0;
	}

	memcpy(dest, decompressed_samples_buffer, uncomp_size - 32);
	DXAttempt(buffer->Unlock(dest, bytes, 0, 0));
	DS_Buffers[num].frequency = pcm_format.nSamplesPerSec;
	DS_Buffers[num].buffer = buffer;
	return 1;
}

void inject_dxsound(bool replace)
{
	INJECT(0x004A2E30, DXChangeOutputFormat, replace);
	INJECT(0x004A2F10, DSChangeVolume, replace);
	INJECT(0x004A2F40, DSAdjustPitch, replace);
	INJECT(0x004A2FB0, DSAdjustPan, replace);
	INJECT(0x004A3030, DXSetOutputFormat, replace);
	INJECT(0x004A3100, DXDSCreate, replace);
	INJECT(0x004A3300, InitSampleDecompress, replace);
	INJECT(0x004A3470, FreeSampleDecompress, replace);
	INJECT(0x004A3510, DXCreateSampleADPCM, replace);
}

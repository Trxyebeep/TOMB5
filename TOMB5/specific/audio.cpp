#include "../tomb5/pch.h"
#include "audio.h"
#include "file.h"
#include "function_stubs.h"

void S_CDPlay(long track, long mode)
{
    if (acm_ready)
    {
        audio_counter = 0;
        IsAtmospherePlaying = track == CurrentAtmosphere;
        S_CDStop();
        ACMEmulateCDPlay(track, mode);
    }
}

void S_CDStop()
{
    if (acm_ready && audio_stream_fp)
    {
        memset(wav_file_buffer, 0, 0x37000);
        DSBuffer->Stop();
        DSBuffer->SetCurrentPosition(0);
        while (reading_audio_file) {};
        CLOSE(audio_stream_fp);
        audio_stream_fp = 0;
        audio_counter = 0;
        XAFlag = 7;
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
        Log(1, "%s - Not Found", name);
        return;
    }

    SEEK(audio_stream_fp, 90, SEEK_SET);
    audio_fp_write_ptr = wav_file_buffer;
    memset(wav_file_buffer, 0, 0x37000);

    if (READ(wav_file_buffer, 1, 0x37000, audio_stream_fp) < 0x37000 && audio_play_mode == 1)
    {
        SEEK(audio_stream_fp, 90, SEEK_SET);
        Log(0, "FileReset In OpenStreamFile");
    }
}

void GetADPCMData()
{
    if (!audio_stream_fp)
        return;

    memset(audio_fp_write_ptr, 0, 0x5800);

    if (READ(audio_fp_write_ptr, 1, 0x5800, audio_stream_fp) < 0x5800 && audio_play_mode == 1)
    {
        Log(0, "FileReset In GetADPCMData");
        SEEK(audio_stream_fp, 90, SEEK_SET);
    }

    audio_fp_write_ptr += 0x5800;

    if ((long)audio_fp_write_ptr >= long(wav_file_buffer + 0x37000))
        audio_fp_write_ptr = wav_file_buffer;
}

void inject_audio(bool replace)
{
    INJECT(0x00492990, S_CDPlay, replace);
    INJECT(0x004929E0, S_CDStop, replace);
    INJECT(0x00492AA0, S_CDFade, replace);
    INJECT(0x00492AC0, S_StartSyncedAudio, replace);
    INJECT(0x00492AF0, ACMSetVolume, replace);
    INJECT(0x00493350, OpenStreamFile, replace);
    INJECT(0x004936A0, GetADPCMData, replace);
}

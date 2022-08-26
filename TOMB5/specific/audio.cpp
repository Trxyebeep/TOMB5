#include "../tomb5/pch.h"
#include "audio.h"
#include "file.h"
#include "function_stubs.h"
#include "dxshell.h"

const char* TrackFileNames[136] =
{
	"xa1_TL_10B.wav",
	"xa1_Z10.wav",
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
        Log(8, "Playing %s %s %d", name, "Looped", track);
    else
        Log(8, "Playing %s %s %d", name, "", track);

    XATrack = track;
    XAReqTrack = track;
    XAFlag = 6;
    audio_play_mode = mode;
    OpenStreamFile(name);

    if (!audio_stream_fp)
        return;

    memcpy(ADPCMBuffer, audio_fp_write_ptr, 0x5800);
    GetADPCMData();
    DXAttempt(DSBuffer->Lock(0, audio_buffer_size, (LPVOID*)&pAudioWrite, &AudioBytes, 0, 0, 0));
    acmStreamConvert(hACMStream, &StreamHeaders[0], ACM_STREAMCONVERTF_BLOCKALIGN | ACM_STREAMCONVERTF_START);
    memcpy(ADPCMBuffer, audio_fp_write_ptr, 0x5800);
    GetADPCMData();
    acmStreamConvert(hACMStream, &StreamHeaders[1], ACM_STREAMCONVERTF_BLOCKALIGN);
    DXAttempt(DSBuffer->Unlock(pAudioWrite, audio_buffer_size, 0, 0));
    CurrentNotify = 2;
    NextWriteOffset = 2 * NotifySize;
    ACMSetVolume();
    DSBuffer->Play(0, 0, DSBPLAY_LOOPING);
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
    INJECT(0x00493760, ACMEmulateCDPlay, replace);
}

/*------------------------------------------------------------------------------
Copyright (c) 2026 Korbosoft
Copyright (c) 2010-2026 The GRRLIB Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
------------------------------------------------------------------------------*/

#include "GRRMOD_internals.h"
#include <xmp.h>
#include <stdlib.h>
#include <string.h>

typedef struct _GRRMOD_DATA {
    char *ModType;    /**< A string representing the MOD type. */
    char *SongTitle;  /**< A string representing the song title. */
    u32 SampleRate;   /**< Sampling rate frequency. */
} GRRMOD_DATA;

static GRRMOD_DATA MusicData = {};
static xmp_context opaque = NULL;   /**< xmp context structure. */

static bool paused = false;
static bool isStereo;

/**
 * Register MOD function list.
 * @param RegFunc The function list to register.
 */
void GRRMOD_MOD_Register(GRRMOD_FuntionsList *RegFunc) {
    RegFunc->Init = GRRMOD_MOD_Init;
    RegFunc->End = GRRMOD_MOD_Unload;
    RegFunc->SetMOD = GRRMOD_MOD_SetMOD;
    RegFunc->Unload = GRRMOD_MOD_Unload;
    RegFunc->SetFrequency = GRRMOD_MOD_SetFrequency;
    RegFunc->GetVoiceFrequency = GRRMOD_MOD_GetVoiceFrequency;
    RegFunc->GetVoiceVolume = GRRMOD_MOD_GetVoiceVolume;
    RegFunc->GetRealVoiceVolume = GRRMOD_MOD_GetRealVoiceVolume;
    RegFunc->Start = GRRMOD_MOD_Start;
    RegFunc->Stop = GRRMOD_MOD_Stop;
    RegFunc->Pause = GRRMOD_MOD_Pause;
    RegFunc->GetSongTitle = GRRMOD_MOD_GetSongTitle;
    RegFunc->GetModType = GRRMOD_MOD_GetModType;
    RegFunc->GetLoopCount = GRRMOD_MOD_GetLoopCount;
    RegFunc->Update = GRRMOD_MOD_Update;
}

/**
 * Initialize MOD library.
 * @param stereo If set to true the music will play in stereo, otherwise it will mono.
 - * @return A number re*presentating a code:
 - *         -     0 : The operation completed successfully.
 - *         -    -1 : Failed to initialize the MOD engine.
 - * @see GRRMOD_MOD_End
 */
s8 GRRMOD_MOD_Init(bool stereo) {
    isStereo = stereo;
    paused = false;
    if (MusicData.SampleRate == 0) {
        MusicData.SampleRate = 48000;
    }

    if (opaque == NULL) {
        opaque = xmp_create_context();
        if (opaque == NULL) return -1;

    }

    return 0;
}

/**
 * Load a MOD file from memory.
 * @param mem Memory to set.
 * @param size Size of the memory to set.
 * @see GRRMOD_MOD_Unload
 */
void GRRMOD_MOD_SetMOD(const void *mem, u64 size) {
    if (opaque == NULL) return;

    GRRMOD_MOD_Unload();

    if (opaque == NULL) {
        opaque = xmp_create_context();
        if (opaque == NULL) return;
    }

    struct xmp_module_info mi;

    if (xmp_load_module_from_memory(opaque, (void *)mem, size) == 0) {
        xmp_get_module_info(opaque, &mi);

        if (mi.mod != NULL) {
            MusicData.SongTitle = strdup(mi.mod->name);
            MusicData.ModType = strdup(mi.mod->type);
        }
    }
}

/**
 * Unload a MOD file.
 * @see GRRMOD_MOD_Unload
 */
void GRRMOD_MOD_Unload(void) {
    if (opaque != NULL) {
        xmp_end_player(opaque);
        xmp_release_module(opaque);
        xmp_free_context(opaque);
        opaque = NULL;
    }
    if (MusicData.ModType != NULL) {
        free(MusicData.ModType);
        MusicData.ModType = NULL;
    }
    if (MusicData.SongTitle != NULL) {
        free(MusicData.SongTitle);
        MusicData.SongTitle = NULL;
    }
}

/**
 * Start the specified module playback.
 * @see GRRMOD_MOD_Stop
 * @see GRRMOD_MOD_Pause
 */
void GRRMOD_MOD_Start(void) {
    if (opaque != NULL) {
        xmp_start_player(opaque, MusicData.SampleRate, isStereo ? 0 : XMP_FORMAT_MONO);
        paused = false;
    }
}

/**
 * Stop the currently playing module.
 * @see GRRMOD_MOD_Start
 * @see GRRMOD_MOD_Pause
 */
void GRRMOD_MOD_Stop(void) {
    if (opaque != NULL) {
        xmp_end_player(opaque);
        paused = true;
    }
}

/**
 * Toggle the playing/paused status of the module.
 * @see GRRMOD_MOD_Start
 * @see GRRMOD_MOD_Stop
 */
void GRRMOD_MOD_Pause(void) {
    if (opaque != NULL) {
        if (paused) {
            xmp_restart_module(opaque);
        } else {
            xmp_stop_module(opaque);
        }
    }
    paused = !paused;
}

char *GRRMOD_MOD_GetSongTitle(void) { return MusicData.SongTitle; }
char *GRRMOD_MOD_GetModType(void) { return MusicData.ModType; }

/**
 * Get the loop count.
 */
u32 GRRMOD_MOD_GetLoopCount(void) {
    if (opaque == NULL) return 0;

    struct xmp_frame_info mi;
    xmp_get_frame_info(opaque, &mi);

    return mi.loop_count;
}

/**
 * Set the frequency.
 * @param freq Frequency to set in Hz.
 */
void GRRMOD_MOD_SetFrequency(u32 freq) {
    MusicData.SampleRate = (freq > 0) ? freq : 48000;
}

u32 GRRMOD_MOD_GetVoiceFrequency(u8 voice) { return 0; }
u32 GRRMOD_MOD_GetVoiceVolume(u8 voice) { return 0; }
u32 GRRMOD_MOD_GetRealVoiceVolume(u8 voice) { return 0; }
/**
 * Set a buffer to update. This routine should be called on a regular basis to update the sound.
 * @param buffer The buffer to update.
 */
void GRRMOD_MOD_Update(u8 *buffer) {
    if (opaque == NULL || buffer == NULL) return;

    if (paused) {
        memset(buffer, 0, SNDBUFFERSIZE);
        return;
    }

    xmp_play_buffer(opaque, buffer, SNDBUFFERSIZE, 0);
}

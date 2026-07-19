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
#include <libopenmpt/libopenmpt.h>
#include <stdlib.h>
#include <string.h>

typedef struct _GRRMOD_DATA {
    char *ModType;    /**< A string representing the MOD type. */
    char *SongTitle;  /**< A string representing the song title. */
    u32 SampleRate;   /**< Sampling rate frequency. */
} GRRMOD_DATA;

static GRRMOD_DATA MusicData = {};
static openmpt_module *mod = NULL;   /**< Module structure. */

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
        MusicData.SampleRate = 48000; // Safe default hardware rate for Wii
    }
    return 0;
}

/**
 * Load a MOD file from memory.
 * @param mem Memory to set.
 * @param size Size of the memory to set.
 */
void GRRMOD_MOD_SetMOD(const void *mem, u64 size) {
    if(mod != NULL) {
        GRRMOD_MOD_Unload();
    }

    mod = openmpt_module_create_from_memory2(mem, size, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    if(mod != NULL) {
        MusicData.SongTitle = strdup(openmpt_module_get_metadata(mod, "title"));
        MusicData.ModType = strdup(openmpt_module_get_metadata(mod, "type"));
        openmpt_module_set_repeat_count(mod, -1);
    }
}

/**
 * Unload a MOD file.
 */
void GRRMOD_MOD_Unload(void) {
    if(mod != NULL) {
        openmpt_module_destroy(mod);
        mod = NULL;
    }
    if(MusicData.ModType != NULL) {
        free(MusicData.ModType);
        MusicData.ModType = NULL;
    }
    if(MusicData.SongTitle != NULL) {
        free(MusicData.SongTitle);
        MusicData.SongTitle = NULL;
    }
}

/**
 * This function starts the specified module playback.
 */
void GRRMOD_MOD_Start(void) {
    if(mod != NULL) {
        openmpt_module_set_position_order_row(mod, 0, 0);
        paused = false;
    }
}

/**
 * This function stops the currently playing module.
 */
void GRRMOD_MOD_Stop(void) {
    if(mod != NULL) {
        openmpt_module_set_position_order_row(mod, 0, 0);
        paused = true;
    }
}

/**
 * This function toggles the playing/paused status of the module.
 */
void GRRMOD_MOD_Pause(void) {
    paused = !paused;
}

char *GRRMOD_MOD_GetSongTitle(void) { return MusicData.SongTitle; }
char *GRRMOD_MOD_GetModType(void) { return MusicData.ModType; }

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
    if (mod == NULL || buffer == NULL) {
        return;
    }

    if (paused) {
        memset(buffer, 0, SNDBUFFERSIZE);
        return;
    }

    static s16 aligned_buffer[SNDBUFFERSIZE / 2];
    size_t frames_rendered = 0;

    if (isStereo) {
        size_t frames_to_render = SNDBUFFERSIZE / 4;

        frames_rendered = openmpt_module_read_interleaved_stereo(mod, MusicData.SampleRate, frames_to_render, aligned_buffer);

        if (frames_rendered < frames_to_render) {
            memset(aligned_buffer + (frames_rendered * 2), 0, (frames_to_render - frames_rendered) * 4);
        }
    } else {
        size_t frames_to_render = SNDBUFFERSIZE / 2;

        frames_rendered = openmpt_module_read_mono(mod, MusicData.SampleRate, frames_to_render, aligned_buffer);

        if (frames_rendered < frames_to_render) {
            memset(aligned_buffer + frames_rendered, 0, (frames_to_render - frames_rendered) * 2);
        }
    }

    memcpy(buffer, aligned_buffer, SNDBUFFERSIZE);
}

/*  Copyright (C) 2018 Aristos Georgiou

    Mixer.c is part of as4/wavengine.

    as4/wavengine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    as4/wavengine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with as4/wavengine.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Definitions.h"

/**
 * @author Aristos Georgiou
 */


/**
 * Create a .wav that plays the left channel of wav_filename1.wav and the right
 * channel of wav_filename2.
 *
 * Works for any combination of stereo and mono inputs.
 *
 * Option ID: 3
 *
 * @param wav_filename1
 * @param wav_filename2
 * @return EXIT CODE
 */
public int mix(char *wav_filename1, char *wav_filename2) {
    int EXIT_CODE;
    Header *wav_header1 = NULL, *wav_header2 = NULL, *wav_header3 = NULL;
    FILE *wav_file1 = NULL, *wav_file2 = NULL, *wav_file3 = NULL;
    char *name = NULL;

    // Initialise wav_header1 from wav_file1
    EXIT_CODE = getHeader(&wav_header1, &wav_file1, wav_filename1);
    if (EXIT_CODE != SUCCESS)
        goto END;

    // Initialise wav_header1 from wav_file2
    EXIT_CODE = getHeader(&wav_header2, &wav_file2, wav_filename2);
    if (EXIT_CODE != SUCCESS)
        goto END;

    // Check for compatibility
    if (wav_header1->bitsPerSample != wav_header2->bitsPerSample) {
        EXIT_CODE = FAILURE;
        printf("Incompatible wav files: %s, %s\n\n", wav_filename1, wav_filename2);
        goto END;
    }

    // Create new file name
    size_t size = strlen(wav_filename1) + strlen(wav_filename2) + 2;
    name = malloc(size);
    if (name == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }
    snprintf(name, size, "mix-%.*s-%s", (int) (strlen(wav_filename1) - 4), wav_filename1,
             wav_filename2);
    {
        // Initialise wav_header3 from the min of header1 and header2
        wav_header3 = malloc(HEADER_SIZE);
        if (wav_header3 == NULL) {
            EXIT_CODE = FAILURE;
            printf("Sorry, program run out of memory.\n\n");
            goto END;
        }
        if (wav_header1->chunkSize <= wav_header2->chunkSize)
            memcpy(wav_header3, wav_header1, HEADER_SIZE);
        else
            memcpy(wav_header3, wav_header2, HEADER_SIZE);

        // Ensure stereo wav_header3
        makeHeaderStereo(wav_header3);

        // Open wav_file3 and write header3 to it
        wav_file3 = fopen(name, "wb");
        fwrite(wav_header3, HEADER_SIZE, 1, wav_file3);

        // Create the buffer of LR samples for wav_file1
        size_t sample_size1 = (size_t) wav_header1->blockAlign;
        u_char sample1[sample_size1];

        // Create the buffer of LR samples for wav_file2
        size_t sample_size2 = (size_t) wav_header2->blockAlign;
        u_char sample2[sample_size2];

        u_int min_sample = min(sample_size1, sample_size2);
        for (register u_int q = 0; q < wav_header3->subchunk2Size / min_sample; q++) {
            // Write left channel of wav_file1 to wav_file3
            if (fread(&sample1, sample_size1, 1, wav_file1) != 1) break;
            fwrite(sample1, sample_size1 / wav_header1->numChannels, 1, wav_file3);

            // Write right channel of wav_file2 to wav_file3
            if (fread(&sample2, sample_size2, 1, wav_file2) != 1) break;
            fwrite(sample2 + (sample_size2 / 2) * (wav_header2->numChannels - 1),
                   sample_size2 / wav_header2->numChannels, 1, wav_file3);
        }
    }

    END:
    freePointer(wav_header1);
    freePointer(wav_header2);
    freePointer(wav_header3);
    freePointer(name);
    closeFile(wav_file1);
    closeFile(wav_file2);
    closeFile(wav_file3);
    return EXIT_CODE;
}

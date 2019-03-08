/*  Copyright (C) 2018 Aristos Georgiou

    Choper.c is part of as4/wavengine.

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
* Chops file from a starting second to an ending second.
* Option ID: 4
*
* @param wav_filename
* @param start_sec
* @param end_second
* @return EXIT CODE
*/
public int chop(char *wav_filename, int start_sec, int end_second) {
    int EXIT_CODE;
    Header *wav_header = NULL;
    FILE *wav_file = NULL, *new_wav_file = NULL;
    char *new_wav_filename = NULL;

    // Initialise wav_header from wav_file
    EXIT_CODE = getHeader(&wav_header, &wav_file, wav_filename);
    if (EXIT_CODE != SUCCESS)
        goto END;

    // Calculate total seconds of file
    int seconds = headerToSeconds(wav_header);

    // Error catching for seconds
    if (start_sec < 0 || end_second < 0 || end_second > seconds || start_sec > end_second) {
        EXIT_CODE = FAILURE;
        printf("Parameters for seconds are invalid.\n\n");
        goto END;
    }

    // Create new file name
    new_wav_filename = malloc(9 + strlen(wav_filename));
    if (new_wav_filename == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }
    snprintf(new_wav_filename, 9 + strlen(wav_filename), "chopped-%s", wav_filename);

    {
        // Create the buffer of LR samples
        size_t sample_size = (size_t) wav_header->blockAlign;
        u_char sample[sample_size];

        // Skip i seconds of samples
        for (register u_int q = 0; q < secondsToSamples(wav_header, start_sec) / sample_size; q++) {
            if (fread(&sample, sample_size, 1, wav_file) != 1) {
                EXIT_CODE = FAILURE;
                printf("Header information mismatch, exiting program.\n\n");
                goto END;
            }
        }
        // Modify header to match a duration of j - i seconds
        changeHeaderDuration(wav_header, end_second - start_sec);

        // Open new_wav_file and write the modified header to it
        new_wav_file = fopen(new_wav_filename, "wb");
        fwrite(wav_header, HEADER_SIZE, 1, new_wav_file);

        // Write rest of the bytes
        for (register u_int q = 0; q < wav_header->subchunk2Size / sample_size; q++) {
            if (fread(&sample, sample_size, 1, wav_file) != 1) {
                EXIT_CODE = FAILURE;
                printf("Header information mismatch, exiting program.\n\n");
                goto END;
            }
            fwrite(&sample, sample_size, 1, new_wav_file);
        }
    }

    END:
    freePointer(wav_header);
    freePointer(new_wav_filename);
    closeFile(wav_file);
    closeFile(new_wav_file);
    return EXIT_CODE;
}


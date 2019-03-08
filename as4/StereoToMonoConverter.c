/*  Copyright (C) 2018 Aristos Georgiou

    StereoToMonoConverter.c is part of as4/wavengine.

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

private int convertToMono(char *wav_filename);


/**
 * Convert .wav files from Stereo to Mono by deleting right channel and changing
 * header information regarding numChannels.
 * Option ID: 2
 *
 * @param files
 * @param number_of_files, number of files
 * @return EXIT CODE
 */
public int convertToMonos(char **files, int number_of_files) {
    int EXIT_CODE = SUCCESS;
    for (int i = 0; i < number_of_files; i++)
        EXIT_CODE += convertToMono(files[i]);

    return EXIT_CODE;
}

/**
 * Convert a .wav file from Stereo to Mono by deleting right channel.
 *
 * @param wav_filename
 * @return EXIT CODE
 */
private int convertToMono(char *wav_filename) {
    int EXIT_CODE;
    Header *wav_header = NULL;
    FILE *wav_file = NULL, *new_wav_file = NULL;
    char *new_wav_filename = NULL;

    EXIT_CODE = getHeader(&wav_header, &wav_file, wav_filename);
    if (EXIT_CODE != SUCCESS)
        goto END;

    EXIT_CODE = makeHeaderMono(wav_header);
    if (EXIT_CODE != SUCCESS) {
        printf("File already mono: %s\n\n", wav_filename);
        goto END;
    }

    // Create new file name
    new_wav_filename = malloc(5 + strlen(wav_filename));
    if (new_wav_filename == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }
    snprintf(new_wav_filename, 5 + strlen(wav_filename), "new-%s", wav_filename);

    {
        // Open new file and write the modified header to it
        new_wav_file = fopen(new_wav_filename, "wb");
        fwrite(wav_header, HEADER_SIZE, 1, new_wav_file);

        // Buffer for half samples
        size_t channel_size = (size_t) wav_header->blockAlign;
        u_char sample[channel_size];

        for (register u_int i = 0; i < wav_header->subchunk2Size / channel_size; i++) {
            // Read and write left channel to new file
            if (fread(sample, channel_size, 1, wav_file) != 1) {
                EXIT_CODE = FAILURE;
                printf("Header information mismatch, exiting program.\n\n");
                goto END;
            }
            fwrite(sample, channel_size, 1, new_wav_file);

            // Skip right channel
            if (fread(sample, channel_size, 1, wav_file) != 1) {
                EXIT_CODE = FAILURE;
                printf("Header information mismatch, exiting program.\n\n");
                goto END;
            }
        }
    }

    END:
    freePointer(wav_header);
    freePointer(new_wav_filename);
    closeFile(wav_file);
    closeFile(new_wav_file);
    return EXIT_CODE;
}

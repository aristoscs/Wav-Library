/*  Copyright (C) 2018 Aristos Georgiou

    Reverser.c is part of as4/wavengine.

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

private int reverseFile(char *wav_filename);


/**
 * Reverses data of given .wav files.
 * Option ID: 5
 *
 * @param files
 * @param number_of_files, number of files
 * @return EXIT CODE
 */
public int reverseFiles(char **files, int number_of_files) {
    int EXIT_CODE = SUCCESS;
    for (int i = 0; i < number_of_files; i++)
        EXIT_CODE += reverseFile(files[i]);

    return EXIT_CODE;
}

/**
 * Reverses data of given .wav file.
 *
 * @param wav_filename
 * @return EXIT_CODE
 */
private int reverseFile(char *wav_filename) {
    int EXIT_CODE;
    Header *wav_header = NULL;
    FILE *wav_file1 = NULL, *wav_file2 = NULL;
    char *new_filename = NULL;

    // Initialise wav_header from wav_file1
    EXIT_CODE = getHeader(&wav_header, &wav_file1, wav_filename);
    if (EXIT_CODE != SUCCESS)
        goto END;

    // Create new file name
    new_filename = malloc(10 + strlen(wav_filename));
    if (new_filename == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }
    snprintf(new_filename, 10 + strlen(wav_filename), "reverse-%s", wav_filename);

    {
        // Open new file and write the original header to it
        wav_file2 = fopen(new_filename, "wb");
        fwrite(wav_header, HEADER_SIZE, 1, wav_file2);

        u_char *data = malloc(wav_header->subchunk2Size);
        if(data == NULL) {
            EXIT_CODE = FAILURE;
            printf("Sorry, program run out of memory.\n\n");
            goto END;
        }
		
        if(fread(data, wav_header->subchunk2Size, 1, wav_file1) != 1) {
            EXIT_CODE = FAILURE;
            printf("Header information mismatch, exiting program.\n\n");
			freePointer(data);
            goto END;
        }

        for(int i = wav_header->subchunk2Size - wav_header->blockAlign; i >= 0; i -= wav_header->blockAlign)
            fwrite(data + i, wav_header->blockAlign, 1, wav_file2);

        freePointer(data);
    }

    END:
    freePointer(wav_header);
    freePointer(new_filename);
    closeFile(wav_file1);
    closeFile(wav_file2);
    return EXIT_CODE;
}

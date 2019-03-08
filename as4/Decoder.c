/*  Copyright (C) 2018 Aristos Georgiou

    Decoder.c is part of as4/wavengine.

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
* Decodes the message that was encoded into a .wav file into an output file.
* Option ID: 8
*
* @param encoded_wav, wav containing the encoded the message
* @param msg_length
* @param output_msg_filename, output file to save the message to.
* @return EXIT CODE
*/
public int decodeFromFile(char *encoded_wav, int msg_length, char *output_msg_filename) {
    int EXIT_CODE;
    Header *wav_header = NULL;
    FILE *encoded_wav_file = NULL, *output_file = NULL;
    u_char *wav_file_data = NULL;
    char *msg = NULL;
    u_int *permutations = NULL;

    // Initialise wav_header from encoded_wav_file
    EXIT_CODE = getHeader(&wav_header, &encoded_wav_file, encoded_wav);
    if (EXIT_CODE != SUCCESS)
        goto END;

    // Initialise wav_file_data from encoded_wav
    wav_file_data = malloc(wav_header->subchunk2Size);
    if (wav_file_data == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }

    if(fread(wav_file_data, wav_header->subchunk2Size, 1, encoded_wav_file) != 1) {
        EXIT_CODE = FAILURE;
        printf("Could not read from file: %s\n\n", encoded_wav);
        goto END;
    }

    // Initialise msg to write to
    msg = calloc((size_t) msg_length + 1, 1);
    if (msg == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }

    // Initialise permutation table
    permutations = createPermutations(msg_length, syskey);
    if (permutations == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }

    // Get the bits back from the data
    for (u_int n = 0; n < (msg_length + 1) * 8; n++) {
        u_int x = permutations[n];
        if (x >= wav_header->subchunk2Size) {
            EXIT_CODE = FAILURE;
            printf("Decoding failed, file should be bigger.\n\n");
            goto END;
        }
        msg[n / 8] |= (*(wav_file_data + x) & 1) << (7 - (n % 8));
    }

    // Write decoded message to output_file
    output_file = fopen(output_msg_filename, "wb");
    fwrite(msg, (size_t) ((msg_length + 1)), 1, output_file);

    END:
    freePointer(wav_header);
    freePointer(wav_file_data);
    freePointer(msg);
    freePointer(permutations);
    closeFile(output_file);
    closeFile(encoded_wav_file);
    return EXIT_CODE;
}

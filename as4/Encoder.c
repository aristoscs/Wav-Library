/*  Copyright (C) 2018 Aristos Georgiou

    Encoder.c is part of as4/wavengine.

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


private int getBit(char *msg, int n);

/**
 * Encodes the bits of a msg contained within the file @param text_filename
 * to the bytes of given .wav file.
 *
 * @param wav_filename
 * @param text_filename
 * @return EXIT CODE
 */
public int encodeToFile(char *wav_filename, char *text_filename) {
    int EXIT_CODE;
    Header *wav_header = NULL;
    FILE *wav_file = NULL, *text_file = NULL;
    u_char *wav_file_data = NULL;
    char *msg_to_encode = NULL, *new_wav_filename = NULL;
    u_int *permutations = NULL;

    // Initialise wav_header from wav_file
    EXIT_CODE = getHeader(&wav_header, &wav_file, wav_filename);
    if (EXIT_CODE != SUCCESS)
        goto END;

    // Initialise wav_file_data from the data of wav_file
    wav_file_data = malloc(wav_header->subchunk2Size);
    if (wav_file_data == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }

    if (fread(wav_file_data, wav_header->subchunk2Size, 1, wav_file) != 1) {
        EXIT_CODE = FAILURE;
        printf("Header information mismatch, exiting program.\n\n");
        goto END;
    }

    // Open file with the text to be encoded
    text_file = fopen(text_filename, "r");
    if (text_file == NULL) {
        EXIT_CODE = FAILURE;
        printf("Error in opening file: %s\n\n", text_filename);
        goto END;
    }

    // Get number of chars in the file
    fseek(text_file, 0, SEEK_END);
    long msg_length = ftell(text_file);
    rewind(text_file);

    // Check if message can fit in file
    if ((msg_length + 1) * 8 >= wav_header->subchunk2Size) {
        EXIT_CODE = FAILURE;
        printf("Message cannot fit in file.\n\n");
        goto END;
    }

    // Initialise msg_to_encode from text_file
    msg_to_encode = calloc((size_t) (msg_length + 1), sizeof(char));
    if (msg_to_encode == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }

    if (fread(msg_to_encode, (size_t) msg_length, 1, text_file) != 1) {
        EXIT_CODE = FAILURE;
        printf("Could not read encoded message from file: %s\n\n", text_filename);
        goto END;
    }

    // Create permutations randomly based on syskey
    permutations = createPermutations(strlen(msg_to_encode), syskey);
    if (permutations == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }

    // Encode bits of the msg based on the permutations
    for (u_int i = 0; i < (msg_length + 1) * 8; i++) {
        int u = getBit(msg_to_encode, i);
        u_int x = permutations[i];
        if (x >= wav_header->subchunk2Size) {
            EXIT_CODE = FAILURE;
            printf("Encoding failed, file should be bigger.\n\n");
            goto END;
        }
        // Delete LSB and add u to it
        u_char *xth = wav_file_data + x;
        *xth &= 0xfe;
        *xth |= u;
    }

    // Create new_wav_filename
    new_wav_filename = malloc(5 + strlen(wav_filename));
    if (new_wav_filename == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }
    snprintf(new_wav_filename, 5 + strlen(wav_filename), "new-%s", wav_filename);

    // Write the new contents of wav_file to new_wav_filename
    FILE *new_file = fopen(new_wav_filename, "wb");
    fwrite(wav_header, HEADER_SIZE, 1, new_file);
    fwrite(wav_file_data, wav_header->subchunk2Size, 1, new_file);
    fclose(new_file);

    END:
    freePointer(wav_header);
    freePointer(wav_file_data);
    freePointer(msg_to_encode);
    freePointer(new_wav_filename);
    freePointer(permutations);
    closeFile(wav_file);
    closeFile(text_file);
    return EXIT_CODE;
}

/**
 * Creates a shuffled sequence of the bits of [0..n - 1].
 *
 * @param msg_length
 * @param key, a seed to root srand() with.
 * @return the helper table with the shuffled sequence [0..n - 1]
 */
public u_int *createPermutations(int msg_length, u_int key) {
    // Root srand with the key
    srand(key);

    u_int n = (u_int) ((msg_length + 1) * 8);
    u_int *perms = malloc(n * sizeof(int));
    if (perms == NULL)
        return NULL;

    // Initialise permutations [0.. n - 1]
    for (u_int i = 0; i < n; i++)
        perms[i] = i;

    // Shuffle permutations
    for (int q = 0; q < n; q++) {
        u_int i = rand() % n;
        u_int j = rand() % n;
        u_int temp = perms[i];
        perms[i] = perms[j];
        perms[j] = temp;
    }
    return perms;
}

/**
 * Calculates the n-th bit of the sequence of bits in the msg.
 *
 * @param msg
 * @param n
 * @return bit, 0 or 1
 */
private int getBit(char *msg, int n) {
    if (n >= 0 && n < strlen(msg) * 8)
        return (msg[n / 8] >> (7 - (n % 8))) & 1;
    return 0;
}

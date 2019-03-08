/*  Copyright (C) 2018 Aristos Georgiou

    HeaderDisplay.c is part of as4/wavengine.

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

private int displayHeader(char *wav_filename);


/**
 * Displays the meta-data(Header) of .wav files.
 * Option ID: 1
 *
 * @param files
 * @param number_of_files, number of files
 * @return EXIT CODE
 */
public int displayHeaders(char **files, int number_of_files) {
    int EXIT_CODE = SUCCESS;
    for (int i = 0; i < number_of_files; i++)
        EXIT_CODE += displayHeader(files[i]);

    return EXIT_CODE;
}

/**
 * Displays the meta-data(Header) of a .wav file.
 *
 * @param wav_filename
 * @return EXIT CODE
 */
private int displayHeader(char *wav_filename) {
    int EXIT_CODE;
    // Initialise wav_header from wav_file
    Header *wav_header = NULL;
    FILE *wav_file = NULL;
    EXIT_CODE = getHeader(&wav_header, &wav_file, wav_filename);
    if (EXIT_CODE != SUCCESS)
        goto END;

    printf("RIFF_CHUNK_HEADER\n");
    printf("=================\n");
    printf("chunkID: %.*s\n", 4, wav_header->chunkID);
    printf("chunkSize: %d\n", wav_header->chunkSize);
    printf("format: %.*s\n\n", 4, wav_header->format);
    printf("FMT_SUBCHUNK_HEADER\n");
    printf("=================\n");
    printf("subChunk1ID: %.*s\n", 3, wav_header->subchunk1ID);
    printf("subChunk1Size: %d\n", wav_header->subchunk1Size);
    printf("audioFormat: %d\n", wav_header->audioFormat);
    printf("numChannels: %d\n", wav_header->numChannels);
    printf("sampleRate: %d\n", wav_header->sampleRate);
    printf("byteRate: %d\n", wav_header->byteRate);
    printf("blockAlign: %d\n", wav_header->blockAlign);
    printf("bitsPerSample: %d\n\n", wav_header->bitsPerSample);
    printf("DATA_SUBCHUNK_HEADER\n");
    printf("=================\n");
    printf("subChunk2ID: %.*s\n", 4, wav_header->subchunk2ID);
    printf("subChunk2Size: %d\n", wav_header->subchunk2Size);
    printf("*************************************\n\n");

    END:
    freePointer(wav_header);
    closeFile(wav_file);
    return EXIT_CODE;
}

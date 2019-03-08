/*  Copyright (C) 2018 Aristos Georgiou

    definitions.c is part of as4/wavengine.

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
* Initialises a Header* with the header of a .wav file.
* Initialises a FILE* with the file called @param wav_filename.
* FILE* will be at 44 bytes where the data section starts at the end.
*
* @param wav_header
* @param wav_file
* @param wav_filename
* @return EXIT_CODE
*/
public int getHeader(Header **wav_header, FILE **wav_file, char *wav_filename) {
    *wav_header = malloc(HEADER_SIZE);
    if (*wav_header == NULL) {
        printf("Sorry, program run out of memory.\n\n");
        return FAILURE;
    }

    *wav_file = fopen(wav_filename, "rb");
    if (*wav_file == NULL) {
        printf("Error in opening file: %s\n\n", wav_filename);
        return FAILURE;
    }

    if (fread(*wav_header, HEADER_SIZE, 1, *wav_file) != 1) {
        printf("File not even 44 bytes: %s\n\n", wav_filename);
        return FAILURE;
    }

    if (wavCheck(*wav_header) == FAILURE) {
        printf("Invalid wav header.\n\n");
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Checks if Header is actually a .wav Header.
 *
 * @param wav_header
 * @return EXIT_CODE
 */
public int wavCheck(Header *wav_header) {
    if (wav_header->chunkID[0] != 'R' || wav_header->chunkID[1] != 'I'
     || wav_header->chunkID[2] != 'F' || wav_header->chunkID[3] != 'F')
        return FAILURE;

    if (wav_header->format[0] != 'W' || wav_header->format[1] != 'A'
     || wav_header->format[2] != 'V' || wav_header->format[3] != 'E')
        return FAILURE;

    if (wav_header->subchunk1ID[0] != 'f' || wav_header->subchunk1ID[1] != 'm'
     || wav_header->subchunk1ID[2] != 't' || wav_header->subchunk1ID[3] != ' ')
        return FAILURE;

    if (wav_header->subchunk1Size != 16 || wav_header->audioFormat != 1)
        return FAILURE;

    if (wav_header->subchunk2ID[0] != 'd' || wav_header->subchunk2ID[1] != 'a'
     || wav_header->subchunk2ID[2] != 't' || wav_header->subchunk2ID[3] != 'a')
        return FAILURE;

    return SUCCESS;
}

/**
 * Closes a FILE* with error checking.
 *
 * @param wav_file
 */
public void closeFile(FILE *wav_file) {
    if (wav_file != NULL)
        fclose(wav_file);
}

/**
 * Frees a pointer with error checking.
 *
 * @param pointer
 */
public void freePointer(void *pointer) {
    if (pointer != NULL)
        free(pointer);
}

/**
 * Converts a Header from stereo to mono.
 *
 * @param wav_header
 * @return EXIT CODE
 */
public int makeHeaderMono(Header *wav_header) {
    if (wav_header->numChannels == 1)
        return FAILURE;

    wav_header->numChannels = 1;
    wav_header->subchunk2Size /= 2;
    wav_header->chunkSize = wav_header->subchunk2Size + 36;
    wav_header->byteRate /= 2;
    wav_header->blockAlign /= 2;
    return SUCCESS;
}

/**
 * Converts a Header from mono to stereo.
 *
 * @param wav_header
 */
public void makeHeaderStereo(Header *wav_header) {
    if (wav_header->numChannels == 2)
        return;

    wav_header->numChannels = 2;
    wav_header->subchunk2Size *= 2;
    wav_header->chunkSize = wav_header->subchunk2Size + 36;
    wav_header->byteRate *= 2;
    wav_header->blockAlign *= 2;
}

/**
 * Changes the header duration to match a given length in seconds.
 *
 * @param wav_header
 * @param seconds
 */
public void changeHeaderDuration(Header *wav_header, int seconds) {
    wav_header->subchunk2Size = secondsToSamples(wav_header, seconds);
    wav_header->chunkSize = wav_header->chunkSize + 36;
}

/**
 * @param wav_header
 * @return the duration of wav file in seconds.
 */
public int headerToSeconds(Header *wav_header) {
    return wav_header->subchunk2Size / wav_header->byteRate;
}

/**
 * @param wav_header
 * @param seconds
 * @return samples per second from this header * @param seconds
 */
public u_int secondsToSamples(Header *wav_header, int seconds) {
    return seconds * wav_header->byteRate;
}

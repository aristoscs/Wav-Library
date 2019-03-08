/*  Copyright (C) 2018 Aristos Georgiou

    Definitions.h is part of as4/wavengine.

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

/**
 * @authors Aristos Georgiou
 */

#ifndef AS4_DEFINITIONS_H
#define AS4_DEFINITIONS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

/**
  * @author Aristos Georgiou
  */

#define public
#define private static

#define HEADER_SIZE 44
#define syskey 77

#define min(a, b) ((a)<(b)?(a):(b))
#define max(a, b) ((a)>(b)?(a):(b))

#define SUCCESS 0
#define FAILURE -1

typedef unsigned char u_char;
typedef unsigned short int s_int;
typedef unsigned int u_int;

/**
 * Represents the Header of a .wav file.
 * Added attribute packed to ensure the size is 44 bytes.
 * This helps with reading and writing of Headers from and to files.
 */
typedef struct Header {

    // RIFF CHUNK
    u_char chunkID[4]; // Contains the letters "RIFF".
    u_int chunkSize;   // Contains the size of file in bytes minus 8 bytes.
    u_char format[4];  // Contains the letters "WAVE".

    // FMT SUB-CHUNK
    u_char subchunk1ID[4]; // Contains the letters "fmt ".
    u_int subchunk1Size;   // 16 for PCM. Size of the rest of the SUB-CHUNK.
    s_int audioFormat;     // PCM = 1, values other than 1 indicate compression.
    s_int numChannels;     // Mono = 1, Stereo = 2.
    u_int sampleRate;      // 8000, 44100.
    u_int byteRate;        // SampleRate * NumChannels * BitsPerSample / 8.
    s_int blockAlign;      // NumChannels * BitsPerSample / 8.
    s_int bitsPerSample;   // 8 bits, 16 bits, etc.

    // DATA SUB-CHUNK
    u_char subchunk2ID[4]; // Contains the letters "data".
    u_int subchunk2Size;   // NumSamples * NumChannels * BitsPerSample / 8.

} __attribute__((__packed__)) Header;

// Definitions.c
public int getHeader(Header **wav_header, FILE **wav_file, char *wav_filename);
public int wavCheck(Header *wav_header);
public void closeFile(FILE *wav_file);
public void freePointer(void *pointer);
public int makeHeaderMono(Header *wav_header);
public void makeHeaderStereo(Header *wav_header);
public void changeHeaderDuration(Header *wav_header, int seconds);
public int headerToSeconds(Header *wav_header);
public u_int secondsToSamples(Header *wav_header, int seconds);

// HeaderDisplay.c
public int displayHeaders(char **files, int number_of_files);

// StereoToMonoConverter.c
public int convertToMonos(char **files, int number_of_files);

// Mixer.c
public int mix(char *wav_filename1, char *wav_filename2);

// Choper.c
public int chop(char *wav_filename, int start_sec, int end_second);

// Reverser.c
public int reverseFiles(char **files, int number_of_files);

// SimilarityCalculator.c
public int calculateDistance(char **files, int number_of_files);

// Encoder.c
public int encodeToFile(char *wav_filename, char *text_filename);
public u_int *createPermutations(int msg_length, u_int key);

// Decoder.c
public int decodeFromFile(char *encoded_wav, int msg_length, char *output_msg_filename);

#endif //AS4_DEFINITIONS_H

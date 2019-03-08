#include "Definitions.h"

/**
  * @author Aristos Georgiou
  */

private double euclidean(u_char *wav_data1, u_char *wav_data2, u_int size1, u_int size2);

private double LCSS(u_char *wav_data1, u_char *wav_data2, u_int size1, u_int size2);


/**
 * Prints euclidean and lcss distances of file[0] in comparison with
 * the rest.
 *
 * @param files
 * @param number_of_files
 * @return EXIT_CODE
 */
public int calculateDistance(char **files, int number_of_files) {
    int EXIT_CODE;
    Header *wav_header1 = NULL;
    FILE *wav_file1 = NULL;
    u_char *wav_file_data1 = NULL;

    // Initialise wav_header1 from first file to be compared with the rest
    EXIT_CODE = getHeader(&wav_header1, &wav_file1, files[0]);
    if (EXIT_CODE != SUCCESS)
        goto END;

    // Initialise wav_file_data1 from the data of first file
    wav_file_data1 = malloc(wav_header1->subchunk2Size);
    if(wav_file_data1 == NULL) {
        EXIT_CODE = FAILURE;
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }

    if (fread(wav_file_data1, wav_header1->subchunk2Size, 1, wav_file1) != 1) {
        EXIT_CODE = FAILURE;
        printf("Header information mismatch, exiting program.\n\n");
        goto END;
    }

    // Read the rest of files and compare with first
    for (int i = 1; i < number_of_files; i++) {
        Header *wav_header2 = NULL;
        FILE *wav_file2 = NULL;
        u_char *wav_file_data2 = NULL;

        // Initialise wav_header2 from wav_file[i]
        EXIT_CODE = getHeader(&wav_header2, &wav_file2, files[i]);
        if (EXIT_CODE != SUCCESS)
            goto LOOP;

        // Compatibility check
        if (wav_header1->bitsPerSample != wav_header2->bitsPerSample
         || wav_header1->numChannels != wav_header2->numChannels) {
            EXIT_CODE = FAILURE;
            printf("Incompatible files: %s, %s\n\n", files[0], files[i]);
            goto LOOP;
        }

        // Initialise wav_file_data2 from the data wav_file[i]
        wav_file_data2 = malloc(wav_header2->subchunk2Size);
        if(wav_file_data2 == NULL) {
            EXIT_CODE = FAILURE;
            printf("Sorry, program run out of memory.\n\n");
            goto LOOP;
        }

        if (fread(wav_file_data2, wav_header2->subchunk2Size, 1, wav_file2) != 1) {
            EXIT_CODE = FAILURE;
            printf("Header information mismatch, exiting loop.\n\n");
            goto LOOP;
        }

        double distance1 = euclidean(wav_file_data1, wav_file_data2,
                                     wav_header1->subchunk2Size, wav_header2->subchunk2Size);
        printf("Euclidean distance: %.3f\n", distance1);

        double distance2 = LCSS(wav_file_data1, wav_file_data2,
                                wav_header1->subchunk2Size, wav_header2->subchunk2Size);
        printf("LCSS distance: %.3f\n\n", distance2);

        LOOP:
        freePointer(wav_header2);
        freePointer(wav_file_data2);
        closeFile(wav_file2);
    }

    END:
    freePointer(wav_header1);
    freePointer(wav_file_data1);
    closeFile(wav_file1);
    return EXIT_CODE;
}

/**
 * Calculates euclidean distance between 2 u_char data buffers.
 *
 * @param wav_data1
 * @param wav_data2
 * @param size1
 * @param size2
 * @return euclidean distance
 */
private double euclidean(u_char *wav_data1, u_char *wav_data2, u_int size1, u_int size2) {
    double euclidean = 0;

    // Compare parallel both data
    for (register u_int i = 0; i < min(size1, size2); i++) {
        int diff = abs(wav_data1[i] - wav_data2[i]);
        euclidean += (diff * diff);
    }
    return sqrt(euclidean);
}

/**
 * Calculates lcss distance between 2 data buffers using DP.
 *
 * @param wav_data1
 * @param wav_data2
 * @param size1
 * @param size2
 * @return lcss distance
 */
private double LCSS(u_char *wav_data1, u_char *wav_data2, u_int size1, u_int size2) {
    double LCSS = -1;

    // Find out which data will represent the columns to save more space
    u_int rows = max(size1, size2);
    u_int cols = min(size1, size2);
    if (size1 > size2) {
        u_char *temp = wav_data1;
        wav_data1 = wav_data2;
        wav_data2 = temp;
    }

    // Create the 2 rows
    u_int *row1 = calloc(cols + 1, sizeof(u_int));
    u_int *row2 = malloc((cols + 1) * sizeof(u_int));
    if (row1 == NULL || row2 == NULL) {
        printf("Sorry, program run out of memory.\n\n");
        goto END;
    }
    row2[0] = 0;

    // Fill in the 2 rows, bottom-up approach with top-down fill
    for (register u_int i = 1; i < rows + 1; i++) {
        for (register u_int j = 1; j < cols + 1; j++) {
            if (wav_data1[j - 1] == wav_data2[i - 1])
                row2[j] = 1 + row1[j - 1];
            else
                row2[j] = max(row1[j], row2[j - 1]);
        }
        u_int *temp = row1;
        row1 = row2;
        row2 = temp;
    }
    // Convert to distance
    LCSS = 1 - ((double) row2[cols] / cols);

    END:
    freePointer(row1);
    freePointer(row2);
    return LCSS;
}
/*  Copyright (C) 2018 Aristos Georgiou

    WavEngine.c is part of as4/wavengine.

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

/*
 * Clean code always looks like it was written by someone who cares.
 * -Robert C. Martin
 */

#include "Definitions.h"

/**
  * @author Aristos Georgiou
  */

private int getOption(int *option, char *argument);

private void showOptions();

private int isNumeric(const char *string);


/**
 * Entry point of our program.
 *
 * @param argc, number of arguments given
 * @param arguments
 * @return EXIT_CODE
 */
public int main(int argc, char *arguments[]) {
    int EXIT_CODE = SUCCESS;

    if (argc <= 1) {
        EXIT_CODE = FAILURE;
        goto END;
    }

    int option;
    getOption(&option, arguments[1]);

    switch (option) {
        case 0:
            if (argc != 2) {
                EXIT_CODE = FAILURE;
                goto END;
            }
            showOptions();
            break;
        case 1:
            if (argc <= 2) {
                EXIT_CODE = FAILURE;
                goto END;
            }
            EXIT_CODE = displayHeaders(&arguments[2], argc - 2);
            break;
        case 2:
            if (argc <= 2) {
                EXIT_CODE = FAILURE;
                goto END;
            }
            EXIT_CODE = convertToMonos(&arguments[2], argc - 2);
            break;
        case 3:
            if (argc != 4) {
                EXIT_CODE = FAILURE;
                goto END;
            }
            EXIT_CODE = mix(arguments[2], arguments[3]);
            break;
        case 4:
            if (argc != 5 || !isNumeric(arguments[3]) || !isNumeric(arguments[4])) {
                EXIT_CODE = FAILURE;
                goto END;
            }
            EXIT_CODE = chop(arguments[2], atoi(arguments[3]), atoi(arguments[4]));
            break;
        case 5:
            if (argc <= 2) {
                EXIT_CODE = FAILURE;
                goto END;
            }
            EXIT_CODE = reverseFiles(&arguments[2], argc - 2);
            break;
        case 6:
            if (argc <= 3) {
                EXIT_CODE = FAILURE;
                goto END;
            }
            EXIT_CODE = calculateDistance(&arguments[2], argc - 2);
            break;
        case 7:
            if (argc != 4) {
                EXIT_CODE = FAILURE;
                goto END;
            }
            EXIT_CODE = encodeToFile(arguments[2], arguments[3]);
            break;
        case 8:
            if (argc != 5 || !isNumeric(arguments[3])) {
                EXIT_CODE = FAILURE;
                goto END;
            }
            EXIT_CODE = decodeFromFile(arguments[2], atoi(arguments[3]), arguments[4]);
            break;
        default:
            EXIT_CODE = FAILURE;
            break;
    }

    END:
    if (EXIT_CODE != SUCCESS) {
        printf("/*  Copyright (C) 2018 Aristos Georgiou, Arsenios Dracoudis.\n"
               "\n"
               "    WavEngine.c is part of as4/wavengine.\n"
               "\n"
               "    as4/wavengine is free software: you can redistribute it and/or modify\n"
               "    it under the terms of the GNU General Public License as published by\n"
               "    the Free Software Foundation, either version 3 of the License, or\n"
               "    (at your option) any later version.\n"
               "\n"
               "    as4/wavengine is distributed in the hope that it will be useful,\n"
               "    but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
               "    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
               "    GNU General Public License for more details.\n"
               "\n"
               "    You should have received a copy of the GNU General Public License\n"
               "    along with as4/wavengine.  If not, see <https://www.gnu.org/licenses/>.\n"
               " */\n\n");
        printf("Use ./wavengine -help ,for options.\n");
    }
    return EXIT_CODE;
}

/**
* Available options and option IDs:
*
* -help, Calls the showOption function.                             ID: 0
* –list (.wav)+, Displays header info for given wav files.          ID: 1
* –mono (.wav)+, Convert given files from stereo to mono.           ID: 2
* –mix  a.wav b.wav, Play a.wav on left and b.wav on right channel. ID: 3
* –chop a.wav 2 4, Chops file from a starting to an ending second.  ID: 4
* –reverse (.wav)+, Reverse data of given files.                    ID: 5
* –similarity (.wav)+, Prints LCSS and Eclidean distance of files.  ID: 6
* –encodeText a.wav text.txt, Encodes text into a.wav file.         ID: 7
* –decodeText a.wav msgLen out.txt, Decodes msg into out.txt        ID: 8
*
* @param option
* @param argument, argument to be parsed as an option
* @return EXIT CODE
*/
private int getOption(int *option, char *argument) {
    if (strcmp(argument, "-help") == 0)
        *option = 0;
    else if (strcmp(argument, "-list") == 0)
        *option = 1;
    else if (strcmp(argument, "-mono") == 0)
        *option = 2;
    else if (strcmp(argument, "-mix") == 0)
        *option = 3;
    else if (strcmp(argument, "-chop") == 0)
        *option = 4;
    else if (strcmp(argument, "-reverse") == 0)
        *option = 5;
    else if (strcmp(argument, "-similarity") == 0)
        *option = 6;
    else if (strcmp(argument, "-encodeText") == 0)
        *option = 7;
    else if (strcmp(argument, "-decodeText") == 0)
        *option = 8;
    else
        *option = -1;

    return SUCCESS;
}

/**
 * Shows the Available options to the client.
 */
private void showOptions() {
    printf("-list (.wav)+ ,for meta-data listing.\n");
    printf("-mono (.wav)+ ,for stereo to mono conversion.\n");
    printf("-mix  file1.wav file2.wav to create a file that plays file1 from left channel and file2 from right channel.\n");
    printf("-chop a.wav 2 4 ,to chop a file from 2s to 4s etc.\n");
    printf("-reverse (.wav)+ to reverse a .wav file.\n");
    printf("-similarity (.wav)+, Prints LCSS and Eclidean distance of files\n");
    printf("-encodeText a.wav text.txt, Encodes text into a.wav file.\n");
    printf("-decodeText a.wav msgLen out.txt, Decodes msg from a.wav into out.txt\n\n");
}

/**
 *
 * @param string
 * @return if string is contains only digits
 */
private int isNumeric(const char *string) {
    while (*string != '\0') {
        if (*string < '0' || *string > '9')
            return 0;
        string++;
    }
    return 1;
}
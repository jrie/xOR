//------------------------------------------------------------------------------
// NOTICE:
//
// Umsetzung von/Created from xOR, made in C by theSplit:
// https://ngb.to/threads/26584-Workshop-Hand-Verschl%C3%BCsselung
//
// Bitte bei Nutzung des/von Teilen des Quellcodes auf dieses Thema hinweisen!
// bzw. auf:
// /Please add a note where the code is from if you use parts or the source itself.
// Refer to this topic:
// https://ngb.to/threads/27001-xORs-Workshop-%28Hand%29verschl%C3%BCsslung-in-Code?p=744503#post744503
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <ctype.h>    // tolower
#include <math.h>     // ceil
#include <time.h>     // time

// Encrypt/Decrypt base
void strip_key_base(const char[], char[]);
void make_polibius_square(char[], const char[], const char[]);

void make_dices(const char[], char[3][11]);
void fill_up_dices(char[3][11], const char[]);

// Flat functions produce a single string versus the usage of an
// two dimensional array in make_dices/fill_up_dices
void make_flat_dices(const char[], char[]);
void fill_up_dices_flat(char[], const char[]);

// Helper function to generate dice order from passwords for the Doppelwürfel
char* generate_double_dice_order(const char[], const char[]);

// Encrypt
void encrypt_decimal_representation(const char[], const char[3][11], char[]);
void encrypt_decimal_representation_flat(const char[], const char[], char[]);
void encrypt_string_representation(char[], const char[], const char[]);
void encrypt_double_dice(const char[], const char[], char[]);

// Decrypt
void decrypt_double_dice(const char[], const char[], char[]);
void decrypt_string_representation(char[], const char[], const char[]);
void decrypt_decimal_representation(const char[], const char[], char[]);

//------------------------------------------------------------------------------
char* encrypt(const char[], const char[], const char[], const char[], const bool, const char*, const char*);
char* decrypt(const char[], const char[], const char[], const char[], const bool, const char*, const char*);
//------------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  bool DOENCRPYT = true;    // Should we encrypt, default true
  bool DODECRYPT = false;   // Should we decrypt, default false
  bool SHOWSTEPS = false;   // Should we show the working steps, default false
  char* inputFileName = NULL;
  char* outputFileName = NULL;

  char* keyBase = NULL;
  char* passwordOne = NULL;
  char* passwordTwo = NULL;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
        case 'e':
          DOENCRPYT = true;
          break;
        case 'd':
          DODECRYPT = true;
          DOENCRPYT = false;
          break;
        case 'v':
          SHOWSTEPS = true;
          break;
        case 'i':
          inputFileName = malloc((strlen(argv[i]) - 1) * sizeof(char));
          strcpy(inputFileName, &argv[i][2]);

          FILE* inputFile = fopen(inputFileName, "r");
          if (inputFile != NULL) {
            fclose(inputFile);
          } else {
            printf("[ERROR] Cannot open input file \"%s\", not present?\n", inputFileName);
            exit(EXIT_FAILURE);
          }

          break;
        case 'o':
          outputFileName = malloc((strlen(argv[i]) - 1) * sizeof(char));
          strcpy(outputFileName, &argv[i][2]);

          FILE *outputFile = fopen(outputFileName, "r");
          if (outputFile != NULL) {
            fclose(outputFile);
            remove(outputFileName);
            printf("[STATUS] Removed output file \"%s\" while beeing present.\n", outputFileName);
          }

          break;
        default:
          break;
      }
    } else {
      if (keyBase == NULL) {
        keyBase = malloc(strlen(argv[i]) * sizeof(char));
        strcpy(keyBase, argv[i]);
      } else if (passwordOne == NULL) {
        passwordOne = malloc(strlen(argv[i]) * sizeof(char));
        strcpy(passwordOne, argv[i]);
      } else if (passwordTwo == NULL) {
        passwordTwo = malloc(strlen(argv[i]) * sizeof(char));
        strcpy(passwordTwo, argv[i]);
      } else {
        printf("[ERROR] Too much unlabeled arguments, need: KeyBase PasswordOne PasswordTwo\n");

        free(keyBase);
        free(passwordOne);
        free(passwordTwo);

        exit(EXIT_FAILURE);
      }
    }
  }

  if (keyBase == NULL) {
    printf("[ERROR] No key base password specified.\n");
    exit(EXIT_FAILURE);
  } else if (passwordOne == NULL) {
    printf("[ERROR] No first dice password specified.\n");

    free(keyBase);
    exit(EXIT_FAILURE);
  } else if (passwordTwo == NULL) {
    printf("[ERROR] No second dice password specified.\n");

    free(passwordOne);
    free(keyBase);
    exit(EXIT_FAILURE);
  }


  if (inputFileName == NULL) {
    printf("[ERROR] No input file name declared.\n");
    exit(EXIT_FAILURE);
  }

  if (!SHOWSTEPS && outputFileName == NULL) {
    printf("[ERROR] No output file name declared using \"-oFILENAME\" and not beeing verbose using \"-v\" switch.\n");
    exit(EXIT_FAILURE);
  }

  //----------------------------------------------------------------------------
  // Generate the alphabet to work with - shared by encryption/decryption and
  // some function do need this as parameter to work with
  char asciiLowercase[27];
  for (int i = 97, j = 0; i < 123; i++) {
      asciiLowercase[j++] = (char) i;
  }
  asciiLowercase[26] = '\0';

  //----------------------------------------------------------------------------
  // Encrypt and decrypt calls
  //----------------------------------------------------------------------------
  if (DOENCRPYT) {
    char *output = encrypt(asciiLowercase, keyBase, passwordOne, passwordTwo, SHOWSTEPS, inputFileName, outputFileName);

    if (output != NULL && outputFileName != NULL) {
      FILE *outputFile = fopen(outputFileName, "w");

      if (outputFile == NULL) {
        printf("[ERROR] Cannot open output file for writing when encrypting.");
      } else {
        char outputChar = '\0';
        int i = 0;

        while ((outputChar = output[i++]) != '\0') {
          fputc(outputChar, outputFile);
        }

        fclose(outputFile);
      }
    }

    if (output != NULL) {
      free(output);
    }
  }

  if (DODECRYPT) {
    char *output = decrypt(asciiLowercase, keyBase, passwordTwo, passwordOne, SHOWSTEPS, inputFileName, outputFileName);

    if (output != NULL && outputFileName != NULL) {
      FILE *outputFile = fopen(outputFileName, "w+");

      if (outputFile == NULL) {
        printf("[ERROR] Cannot open output file for writing. Decrypting.");
      } else {
        char outputChar = '\0';
        int i = 0;

        while ((outputChar = output[i++]) != '\0') {
          fputc(outputChar, outputFile);
        }

        fclose(outputFile);
      }
    }

    if (output != NULL) {
      free(output);
    }
  }

  //----------------------------------------------------------------------------
  if (inputFileName != NULL) {
    free(inputFileName);
  }

  if (outputFileName != NULL) {
    free(outputFileName);
  }

  if (keyBase != NULL) {
    free(keyBase);
  }

  if (passwordOne != NULL) {
    free(passwordOne);
  }

  if (passwordTwo != NULL) {
    free(passwordTwo);
  }

  return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
// Main encrypt and decrypt functions
//------------------------------------------------------------------------------

char* encrypt(const char asciiLowercase[], const char keyBase[], const char passwordOne[], const char passwordTwo[], const bool SHOWSTEPS, const char* inputFileName, const char* outputFileName) {
  //----------------------------------------------------------------------------
  // Encryption
  //----------------------------------------------------------------------------

  printf("\n--------------------------------------------------------------------------------\nENCRYPTION STARTING\n--------------------------------------------------------------------------------\n\n");

  // [STEP 1] - Generate strapped password from input keyBase
  char keyStrapped[strlen(keyBase)];
  strip_key_base(keyBase, keyStrapped);

  if (SHOWSTEPS) {
    printf("[Step 1] Keyphrase generation\nKeyBase...: %s\nKeyStrap..: %s\n\n", keyBase, keyStrapped);
  }

  //----------------------------------------------------------------------------
  // [STEP 2] - Generate polibius_square lowercase
  char polibiusSquare[26];
  make_polibius_square(polibiusSquare, keyStrapped, asciiLowercase);

  if (SHOWSTEPS) {
    printf("[Step 2] Polibius_square\n%s\n\n", polibiusSquare);
  }

  //----------------------------------------------------------------------------
  // [STEP 3] Generate dices
  /*
      12345          12345
    1 pferu        1 pferu        0 pnz iqf dbj
    2 ndsal        2 ndsal        1 tes ckv rag
    3 zbcgh        3 zbcgh    =>  2 mwu lho x__
    4 ijkmo        4 ijkmo
    5 tvwxy        5 qtvwx

    =>

    -1- -2- -3-
    123 123 123
    1 pnzitfdbj
    2 vesckwrag
    3 mxulhoyq_
  */

  char dices[3][11];
  make_dices(polibiusSquare, dices);

  if (SHOWSTEPS) {
    printf("[Step 3] Initial dices\n[1] %s\n[2] %s\n[3] %s\n\n", dices[0], dices[1], dices[2]);
  }

  // Flat dices in one string
  char dicesFlat[strlen(polibiusSquare)+1];
  make_flat_dices(polibiusSquare, dicesFlat);

  if (SHOWSTEPS) {
    // Only output
    printf("[Test 3] Flat dices (one string)\n");
    char fdc = '\0';

    for (int i = 0, k = 0; i < 3; ++i) {
      printf("%d. ", i+1);

      while ((fdc = dicesFlat[k++]) != '\0') {
        printf("%c", fdc);

        if (k % 9 == 0) {
          break;
        }
      }

      printf("\n");
    }

    printf("\n");
    // Output end
  }

  //----------------------------------------------------------------------------

  // [STEP 4] Fill up with alphabet letters [optional?]

  /*
  Dices:
  [1] pnz iqf dbj
  [2] tes ckv rag
  [3] mwu lho x__

  ==>

  [1] pnz iqf dbj
  [2] tes ckv rag
  [3] mwu lho xy_

  */

  fill_up_dices(dices, asciiLowercase);

  if (SHOWSTEPS) {
    printf("[Step 4] Filled dices\n[1] %s\n[2] %s\n[3] %s\n\n", dices[0], dices[1], dices[2]);
  }

  fill_up_dices_flat(dicesFlat, asciiLowercase);

  if (SHOWSTEPS) {
    // Only output
    printf("[Test 4] Filled flat dices (one string)\n");
    char fdc = '\0';

    for (int i = 0, k = 0; i < 3; ++i) {
      printf("%d. ", i+1);

      while ((fdc = dicesFlat[k++]) != '\0') {
        printf("%c", fdc);

        if (k % 9 == 0) {
          break;
        }
      }

      printf("\n");
    }

    printf("\n");
    // Output end
  }

  //----------------------------------------------------------------------------
  // [STEP 5] - Translation to decimal
  //            Example: "no more secrets"

  /*

    -1- -2- -3-
    123 123 123
  1 pnz itf dbj
  2 ves ckw rag
  3 mxu lho yq_

   n   o   _   m   o   r   e   _   s   e   c   r   e   t   s
  112-233-333-131-233-321-122-333-123-122-221-321-122-212-123
  => 112-233-333-131-233-321-122-333-123-122-221-321-122-121-123
  */

  // Read input from file
  FILE* inputFile = fopen(inputFileName, "r");
  char* inputData = malloc(512);
  char inputChar = '\0';
  unsigned int dataIndex = 0;

  while ((inputChar = fgetc(inputFile)) != EOF) {
    inputData[dataIndex++] = inputChar;

    if (dataIndex % 512 == 0) {
      inputData = (char*) realloc(inputData, (dataIndex + 512) * sizeof(char));
    }
  }

  inputData[dataIndex] = '\0';
  //----------------------------------------------------------------------------

  char decimalRepresation[(strlen(inputData)* 3)+1];
  decimalRepresation[0] = '\0';
  encrypt_decimal_representation(inputData, dices, decimalRepresation);

  if (SHOWSTEPS) {
    printf("[Step 5] Decimal representation\nCiphertext: %s\n\n", inputData);
    for (int i = 0, j = strlen(decimalRepresation); i < j; i += 3) {
      if (i != 0) {
        printf("-%c%c%c", decimalRepresation[i], decimalRepresation[i+1], decimalRepresation[i+2]);
      } else {
        printf("%c%c%c", decimalRepresation[i], decimalRepresation[i+1], decimalRepresation[i+2]);
      }
    }

    printf("\n\n");
  }

  // Flat decimal generator
  char decimalRepresation_flat[(strlen(inputData)* 3)+1];
  decimalRepresation_flat[0] = '\0';
  encrypt_decimal_representation_flat(inputData, dicesFlat, decimalRepresation_flat);

  if (SHOWSTEPS) {
    printf("[Test 5] Decimal representation flat\n");
    for (int i = 0, j = strlen(decimalRepresation_flat); i < j; i += 3) {
      if (i != 0) {
        printf("-%c%c%c", decimalRepresation_flat[i], decimalRepresation_flat[i+1], decimalRepresation_flat[i+2]);
      } else {
        printf("%c%c%c", decimalRepresation_flat[i], decimalRepresation_flat[i+1], decimalRepresation_flat[i+2]);
      }
    }

    printf("\n\n");
  }

  //----------------------------------------------------------------------------
  // [STEP 6] Randomizing decimal representation
  /*
    1 2 3
    -----
    a b c
    d e f
    g h i
    j k l
    m n o
    p q r
    s t u
    v w x
    y z
  */

  char stringedRepresentation[strlen(decimalRepresation)+1];
  encrypt_string_representation(stringedRepresentation, asciiLowercase, decimalRepresation);

  if (SHOWSTEPS) {
    printf("[Step 6] Stringed representation\n");
    for (int i = 0, j = strlen(stringedRepresentation); i < j; i += 3) {
      if (i > 0) {
        printf("-%c%c%c", stringedRepresentation[i], stringedRepresentation[i+1], stringedRepresentation[i+2]);
      } else {
        printf("%c%c%c", stringedRepresentation[i], stringedRepresentation[i+1], stringedRepresentation[i+2]);
      }
    }

    printf("\n\n");
  }

  //----------------------------------------------------------------------------
  // [STEP 7] "Doppelwürfel"

  /*
  pyb-woc-uxf-ars-efi-uba-yhk-xor-gtc-mbw-eby-xts-atb-edk-vwx

  =>

  41853791620
  gurkensalat
  -----------
  pybwocuxfar
  sefiubayhkx
  orgtcmbweby
  xtsatbedkvw
  x
  */

  char *passOrderOne = generate_double_dice_order(passwordOne, asciiLowercase);
  char *passOrderTwo = generate_double_dice_order(passwordTwo, asciiLowercase);

  int cipherLength = strlen(stringedRepresentation)+1;

  char tmpData[cipherLength];
  char* encryptedOutput = malloc(cipherLength * sizeof(char));

  if (SHOWSTEPS) {
    printf("[Step 7.1] Generate the \"Doppelwürfel\"\n%s\n%s\n\n", passwordOne, passOrderOne);
    printf("%s\n%s\n\n", passwordTwo, passOrderTwo);
  }

  encrypt_double_dice(stringedRepresentation, passOrderOne, tmpData);
  encrypt_double_dice(tmpData, passOrderTwo, encryptedOutput);

  // Just output
  if (SHOWSTEPS) {
    printf("[Step 7.2] Generate the dice encryption\n");

    char tmpChar = '\0';
    int tmpKey = 0;
    int tmpKeyLength = 0;
    int tmpItems = 0;

    printf("[Round 1]\n%s\n%s\n---------------------------\n", passOrderOne, passwordOne);
    tmpKeyLength = strlen(passwordOne);
    tmpItems = tmpKeyLength;
    for (int i = 0, j = (int) ceil(strlen(stringedRepresentation) / tmpKeyLength); i < j+1; ++i) {
      int tmpKeyEnd = tmpKey + tmpItems;

      while ((tmpChar = stringedRepresentation[(tmpKey++)]) != '\0') {
        printf("%c", tmpChar);
        if (tmpKey == tmpKeyEnd) {
          break;
        }
      }

      printf("\n");
    }

    printf("\n");

    printf("[Round 1] Result\n%s\n\n", tmpData);

    //----------------------------------------------------------------------------

    tmpChar = '\0';
    tmpKey = 0;
    tmpKeyLength = 0;
    tmpItems = 0;

    printf("[Round 2]\n%s\n%s\n---------------------------\n", passOrderTwo, passwordTwo);
    tmpKeyLength = strlen(passwordTwo);
    tmpItems = tmpKeyLength;
    for (int i = 0, j = (int) ceil(strlen(tmpData) / tmpKeyLength); i < j+1; ++i) {
      int tmpKeyEnd = tmpKey + tmpItems;

      while ((tmpChar = tmpData[(tmpKey++)]) != '\0') {
        printf("%c", tmpChar);
        if (tmpKey == tmpKeyEnd) {
          break;
        }
      }

      printf("\n");
    }

    printf("\n");

    printf("\n[Step 7.3] Final output\n%s\n\n\n", encryptedOutput);
  }
  //Output end


  if (passOrderOne != NULL) {
      free(passOrderOne);
  }

  if (passOrderTwo != NULL) {
      free(passOrderTwo);
  }

  return encryptedOutput;
}

char* decrypt(const char asciiLowercase[], const char keyBase[], const char decryptPassOne[], const char decryptPassTwo[], const bool SHOWSTEPS, const char* inputFileName, const char* outputFileName) {
  //----------------------------------------------------------------------------
  // Decryption start
  //----------------------------------------------------------------------------

  printf("\n--------------------------------------------------------------------------------\nDECRYPTION STARTING\n--------------------------------------------------------------------------------\n\n");

  // Read input from file
  FILE* inputFile = fopen(inputFileName, "r");
  char* inputData = malloc(512);
  char inputChar = '\0';
  unsigned int dataIndex = 0;

  while ((inputChar = fgetc(inputFile)) != EOF) {
    inputData[dataIndex++] = inputChar;

    if (dataIndex != 0 && dataIndex % 512 == 0) {
      inputData = (char*) realloc(inputData, ((dataIndex + 512)+1) * sizeof(char));
    }

  }

  inputData[dataIndex] = '\0';
  //----------------------------------------------------------------------------

  char decryptInput[strlen(inputData)+1];
  strcpy(decryptInput, inputData);

  char decryptStageOne[strlen(decryptInput)+1];
  char decryptStageTwo[strlen(decryptInput)+1];

  strcpy(decryptStageOne, decryptInput);
  strcpy(decryptStageTwo, decryptInput);

  //----------------------------------------------------------------------------

  char *decryptOrderOne = generate_double_dice_order(decryptPassOne, asciiLowercase);
  char *decryptOrderTwo = generate_double_dice_order(decryptPassTwo, asciiLowercase);
  char decryptStrapped[strlen(keyBase)+1];
  strip_key_base(keyBase, decryptStrapped);

  if (SHOWSTEPS) {
    printf("[Step 1] Keyphrase generation\nKeyBase...: %s\nKeyStrap..: %s\n\n", keyBase, decryptStrapped);
  }

  char decryptPolibiusSquare[26];
  make_polibius_square(decryptPolibiusSquare, decryptStrapped, asciiLowercase);

  if (SHOWSTEPS) {
    printf("[Step 2] Polibius_square\n%s\n\n", decryptPolibiusSquare);
  }

  char decryptDicesFlat[26];
  make_flat_dices(decryptPolibiusSquare, decryptDicesFlat);
  fill_up_dices_flat(decryptDicesFlat, asciiLowercase);

  if (SHOWSTEPS) {
    printf("[Step 3] Secrets\nRound 1: '%s'\nRound 2: '%s'\n\n", decryptPassOne, decryptPassTwo);
  }

  //----------------------------------------------------------------------------
  if (SHOWSTEPS) {
    printf("[Decrypt 1] Unreveal the dices\n");

    char tmpChar = '\0';
    int tmpKey = 0;
    int tmpKeyLength = 0;
    int tmpItems = 0;

    printf("[Round 1]\n%s\n%s\n---------------------------\n", decryptOrderOne, decryptPassOne);
    tmpKeyLength = strlen(decryptPassOne);
    tmpItems = tmpKeyLength;
    for (int i = 0, j = (int) ceil(strlen(decryptStageOne) / tmpKeyLength); i < j+1; ++i) {
      int tmpKeyEnd = tmpKey + tmpItems;

      while ((tmpChar = decryptStageOne[(tmpKey++)]) != '\0') {
        printf("%c", tmpChar);
        if (tmpKey == tmpKeyEnd) {
          break;
        }
      }

      printf("\n");
    }

    printf("\n");
  }

  //----------------------------------------------------------------------------

  decrypt_double_dice(decryptInput, decryptOrderOne, decryptStageOne);

  if (SHOWSTEPS) {
    printf("[Round 1] Result\n%s\n\n", decryptStageOne);

    char tmpChar = '\0';
    int tmpKey = 0;
    int tmpKeyLength = 0;
    int tmpItems = 0;

    printf("[Round 2]\n%s\n%s\n---------------------------\n", decryptOrderTwo, decryptPassTwo);
    tmpKeyLength = strlen(decryptPassTwo);
    tmpItems = tmpKeyLength;
    for (int i = 0, j = (int) ceil(strlen(decryptStageOne) / tmpKeyLength); i < j+1; ++i) {
      int tmpKeyEnd = tmpKey + tmpItems;

      while ((tmpChar = decryptStageOne[(tmpKey++)]) != '\0') {
        printf("%c", tmpChar);
        if (tmpKey == tmpKeyEnd) {
          break;
        }
      }

      printf("\n");
    }

    printf("\n");
  }


  decrypt_double_dice(decryptStageOne, decryptOrderTwo, decryptStageTwo);

  if (SHOWSTEPS) {
    printf("[Round 2] Result\n");
    for (int i = 0, j = strlen(decryptStageTwo); i < j; i += 3) {
      if (i > 0) {
        printf("-%c%c%c", decryptStageTwo[i], decryptStageTwo[i+1], decryptStageTwo[i+2]);
      } else {
        printf("%c%c%c", decryptStageTwo[i], decryptStageTwo[i+1], decryptStageTwo[i+2]);
      }
    }

    printf("\n\n");
  }

  if (decryptOrderOne != NULL) {
    free(decryptOrderOne);
  }

  if (decryptOrderTwo != NULL) {
    free(decryptOrderTwo);
  }

  //----------------------------------------------------------------------------

  char decryptedDecimalRepresentation[strlen(decryptStageTwo)+1];
  decrypt_string_representation(decryptedDecimalRepresentation, asciiLowercase, decryptStageTwo);

  if (SHOWSTEPS) {
    printf("[Decrypt 2] Recreate decimal representation\n");
    for (int i = 0, j = strlen(decryptedDecimalRepresentation); i < j; i += 3) {
      if (i > 0) {
        printf("-%c%c%c", decryptedDecimalRepresentation[i], decryptedDecimalRepresentation[i+1], decryptedDecimalRepresentation[i+2]);
      } else {
        printf("%c%c%c", decryptedDecimalRepresentation[i], decryptedDecimalRepresentation[i+1], decryptedDecimalRepresentation[i+2]);
      }
    }

    printf("\n\n");
  }

  //----------------------------------------------------------------------------
  char* decryptedOutput = malloc( (int) ceil(strlen(decryptedDecimalRepresentation) / 3)+1 * sizeof(char));
  decrypt_decimal_representation(decryptedDecimalRepresentation, decryptDicesFlat, decryptedOutput);

  if (SHOWSTEPS) {
    printf("[Decrypt 3] Final output\n%s\n\n", decryptedOutput);
  }

  return decryptedOutput;
}


void strip_key_base(const char keyBase[], char keyStrapped[]) {
  keyStrapped[0] = '\0';

  bool isIncluded = false;
  int i = 0, j = 0;
  char kbc = '\0';
  char ksc = '\0';

  while ((kbc = keyBase[i++]) != '\0') {
    j = 0;
    isIncluded = false;

    while ((ksc = keyStrapped[j++]) != '\0') {
      if (ksc == kbc) {
        isIncluded = true;
        break;
      }
    }

    if (!isIncluded) {
      keyStrapped[j-1] = kbc;
      keyStrapped[j] = '\0';
    }
  }

  return;
}

//------------------------------------------------------------------------------

void make_polibius_square(char polibiusSquare[], const char keyStrapped[], const char dictionary[]) {
  int i = 0;
  int cell = 0;
  char ksc = '\0';

  while ((ksc = keyStrapped[i++]) != '\0') {
      polibiusSquare[cell] = tolower(ksc);
      ++cell;
  }

  int initCell = cell;
  bool isIncluded = false;

  while (cell < 25) {
    int j = 0, k = 0, l = 0;

    for (j = 0, l = strlen(dictionary); j < l; ++j) {
      isIncluded = false;

      for (k = 0; k < initCell; ++k) {
        if (dictionary[j] == polibiusSquare[k]) {
          isIncluded = true;
          break;
        }
      }

      if (!isIncluded) {
        polibiusSquare[cell++] = dictionary[j];
      }
    }
  }

  polibiusSquare[25] = '\0';

  return;
}

//------------------------------------------------------------------------------
void make_dices(const char polibiusSquare[], char dices[3][11]) {
  int row = 0;

  for (int i = 0, l = strlen(polibiusSquare); i < l; ++i) {
    int diceNum = 0;

    if (i != 0 && i % 5 == 0) {
      row++;
    }

    int position = (i * 6);

    if (position > 10) {
      diceNum = position % 10;

      if (row < 3) {
        if (diceNum == 2 || diceNum == 8) {
          diceNum = 1;
        } else if (diceNum == 0 || diceNum == 6) {
          diceNum = 0;
        } else {
          diceNum = 2;
        }
      } else if (row < 4) {
        if (diceNum != 6 && diceNum != 0) {
          if (diceNum % 4 == 0) {
            diceNum = 2;
          } else if (diceNum % 2 == 0) {
            diceNum = 1;
          }
        } else {
          diceNum = 0;
        }
      } else {
        if (diceNum == 6 || diceNum == 2) {
          diceNum = 1;
        } else if (diceNum == 8 || diceNum == 4) {
          diceNum = 2;
        } else {
          diceNum = 0;
        }

      }
    } else {
      diceNum = position / 10 % 3;
    }

    //--------------------------------------------------------------------------

    if (diceNum != 0) {
      position -= 10 * diceNum;
    }

    if (row != 0) {
      position -= 29 * row;
    }

    //--------------------------------------------------------------------------

    if (position > 5 && diceNum == 0) {
      position -= 1;
    } else if (diceNum == 1) {
      if (position > 6) {
        position -= 2;
      } else if (position > 0) {
        position -= 1;
      }
    } else if (diceNum == 2) {
      if (position > 2) {
        position -= 2;
      } else {
        position -= 1;
      }
    }

    dices[diceNum][position] = polibiusSquare[i];
  }

  dices[0][9] = '\0';
  dices[1][9] = '\0';
  dices[2][7] = '\0';
  return;
}

//------------------------------------------------------------------------------

void make_flat_dices(const char polibiusSquare[], char dices[]) {
  int diceWriterPos = 0;

  for (int k = 0; k <  5; k++) {
    for (int j = k; j < 25; j += 5) {
      dices[diceWriterPos++] = polibiusSquare[j];
    }
  }
  dices[diceWriterPos] = '\0';

  return;
}

//------------------------------------------------------------------------------

void fill_up_dices(char dices[3][11], const char asciiLowercase[]) {

  char diceChar = '\0';
  char letter = '\0';
  int k = 0;
  bool isIncluded = false;

  while ((letter = asciiLowercase[k++]) != '\0') {
    isIncluded = false;

    for (int i = 0; i < 3; ++i) {
      int j = 0;

      while ((diceChar = dices[i][j++]) != '\0') {
        if (diceChar == letter) {
          isIncluded = true;
          break;
        }
      }

      if (isIncluded) {
        break;
      }
    }

    if (!isIncluded) {
      for (int l = 0; l < 3; ++l) {
        int diceSize = strlen(dices[l]);

        if (diceSize < 9) {
          dices[l][diceSize] = letter;
          dices[l][diceSize+1] = '\0';
          break;
        }
      }
    }

  }

  return;
}

//------------------------------------------------------------------------------

void fill_up_dices_flat(char dices[], const char asciiLowercase[]) {

  int i = 0, j = 0;

  bool isIncluded = false;
  char diceChar = '\0';
  char letter = '\0';

  int diceLength = strlen(dices);

  while ((letter = asciiLowercase[i++]) != '\0') {
    isIncluded = false;
    j = 0;

    while ((diceChar = dices[j++]) != '\0') {
      if (diceChar == letter) {
        isIncluded = true;
        break;
      }
    }

    if (!isIncluded) {
      dices[diceLength++] = letter;
      dices[diceLength] = '\0';
      if (diceLength == 26) {
        break;
      }
    }
  }

  return;
}

char* generate_double_dice_order(const char password[], const char asciiLowercase[]) {
  char *passOrderStore = calloc(strlen(password), sizeof(char));

  char currentLetter = '\0';
  char passLetter = '\0';
  int i = 0, j = 0, pos = 1;

  while ((currentLetter = asciiLowercase[i++]) != '\0') {
    j = 0;

    while ((passLetter = tolower(password[j++])) != '\0') {
      if (passLetter == currentLetter) {
        passOrderStore[j-1] = ((char) pos) + 48;
        ++pos;

        if (pos == 10) {
          pos = 0;
        }
      }
    }
  }

  return passOrderStore;
}


void encrypt_decimal_representation(const char cipherText[], const char dices[3][11], char decimalRepresation[]) {
  int group = 0;
  int row = 0;
  int column = 0;

  int i = 0, j = 0;
  bool isIncluded = false;
  char currentChar = '\0';
  char diceChar = '\0';
  int pos = 0;

  while ((currentChar = tolower(cipherText[i++])) != '\0') {
      if (currentChar == ' ' || currentChar == '\n' || (currentChar < 'a' || currentChar > 'z')) {
        pos = (i-1) * 3;
        sprintf(&decimalRepresation[pos], "%d", 333);
        continue;
      }

      for (int k = 0; k < 3; ++k) {
        j = 0;
        isIncluded = false;


        while ((diceChar = dices[k][j++]) != '\0') {
            if (diceChar == currentChar) {
              row = k+1;

              if (j % 3 == 0) {
                column = 3;
              } else if (j == 4) {
                column = 1;
              } else if (j == 5) {
                column = 2;
              } else if (j % 2 == 0) {
                column = 2;
              } else {
                column = 1;
              }

              if (j < 4) {
                group = 1;
              } else if (j > 6) {
                group = 3;
              } else {
                group = 2;
              }

              pos = (i-1) * 3;
              sprintf(&decimalRepresation[pos], "%d%d%d", group, row, column);
              isIncluded = true;
              break;
            }
        }

        if (isIncluded) {
          break;
        }
      }
  }

  return;
}

//------------------------------------------------------------------------------

void encrypt_decimal_representation_flat(const char cipherText[], const char dices[], char decimalRepresation[]) {

  int group = 0;
  int row = 0;
  int column = 0;

  int i = 0, j = 0;
  char currentChar = '\0';
  char diceChar = '\0';
  int pos = 0;

  while ((currentChar = tolower(cipherText[i++])) != '\0') {
    if (currentChar == ' ' || currentChar == '\n' || (currentChar < 'a' || currentChar > 'z')) {
      pos = (i-1) * 3;
      sprintf(&decimalRepresation[pos], "%d", 333);
      continue;
    }

    j = 0;

    while ((diceChar = dices[j++]) != '\0') {
      if (diceChar == currentChar) {
        int base = j;
        row = ((int) ceil(j) / 9) + 1;
        column = 1;
        group = 1;

        if (base > 9 && base < 19) {
          base -= 9;
        } else if (base > 18) {
          base -= 18;
        }

        if (base != 0) {
          if (base % 3 == 0) {
            column = 3;
          } else if (base % 2 != 0 && base != 7) {
            column = 1;
          } else if (base % 2 == 0 && base != 4) {
            column = 2;
          }
        }

        if (base > 3) {
          if (base > 6) {
            group = 3;
          } else {
            group = 2;
          }
        }

        pos = (i-1) * 3;
        sprintf(&decimalRepresation[pos], "%d%d%d", group, row, column);
      }
    }
  }

  return;
}

//------------------------------------------------------------------------------
void encrypt_string_representation(char stringedRepresentation[], const char asciiLowercase[], const char decimalRepresation[]) {
  char decimalChar = '\0';
  int i = 0, column = 0;

  srand(time(NULL));

  while ((decimalChar = decimalRepresation[i++]) != '\0') {
    column = atoi(&decimalChar);

    int randomNumber = rand() % 9;
    int position = randomNumber < 3 ? column-1 : (randomNumber * 3) + (column-1);
    if (position > 25) {
      position = 25 - (column-1);
    }

    stringedRepresentation[i-1] = asciiLowercase[position];
    stringedRepresentation[i] = '\0';
  }

  return;
}

void encrypt_double_dice(const char inputCipher[], const char blockOrder[], char outputCipher[]) {

  int columns = strlen(blockOrder);
  int cipherLength = strlen(inputCipher);

  char blockShadow[columns];
  strcpy(blockShadow, blockOrder);

  int processedColumns = 0;
  int currentIndex = 1;
  int offset = 0;
  int outputWritePosition = 0;
  char blockChar = '\0';
  char processedSign = (char) 170;

  int i = 0;
  int offsetBase = 0;

  while (processedColumns < columns) {
    i = 0;

    while ((blockChar = blockOrder[i++]) != '\0') {
      offsetBase = i - 1;
      if (blockShadow[offsetBase] == processedSign) {
        continue;
      }

      if ( ((int) blockChar) - 48 == currentIndex) {
        offset = offsetBase;

        while (offset < cipherLength) {
          outputCipher[outputWritePosition++] = inputCipher[offset];
          offset += columns;
        }

        blockShadow[offsetBase] = processedSign;

        // Use this instead of 1133 to go further in line after one item has
        // been processed, comment me out if you use 1133
        if (++currentIndex == 10) {
          currentIndex = 0;
        }

        if (++processedColumns == columns) {
          break;
        }
      }
    }

    /*
    // Use this instead to go further in line only after each index has been
    // processed
    if (++currentIndex == 10) {
      currentIndex = 0;
    }*/

  }

  outputCipher[outputWritePosition] = '\0';
  return;
}

//------------------------------------------------------------------------------
// Decrypt
void decrypt_decimal_representation(const char decimalRepresentation[], const char dices[], char decryptedOutput[]) {

  char decimalChar = '\0';
  int group = 0, row = 0, column = 0;

  int decryptedOutputWriterPos = 0;
  int diceReadPos = 0;

  int i = 0;
  while ((decimalChar = decimalRepresentation[i]) != '\0') {
    group = (int) decimalRepresentation[i] - 48;
    row = (int) decimalRepresentation[i+1] - 48;
    column = (int) decimalRepresentation[i+2] - 48;

    if (group == 3 && row == 3 && column == 3) {
        decryptedOutput[decryptedOutputWriterPos++] = ' ';
        i += 3;
        continue;
    }

    diceReadPos = ((group-1) * 3) + ((row-1) * 9) + (column-1);
    decryptedOutput[decryptedOutputWriterPos++] = dices[diceReadPos];

    i += 3;
  }

  decryptedOutput[decryptedOutputWriterPos] = '\0';

  return;
}

void decrypt_double_dice(const char decryptInput[], const char blockOrder[], char outputCipher[]) {

  char internalBlockOrder[strlen(blockOrder)];
  strcpy(internalBlockOrder, blockOrder);

  int columns = strlen(internalBlockOrder);
  int processedColumns = 0;
  int outputWritePosition = 0;
  int currentIndex = 1;
  int offset = 0;

  int cipherLength = strlen(decryptInput);
  char processedSign = (char) 170;

  int i = 0;
  char currentChar = '\0';

  while (processedColumns < columns) {
    i = 0;

    while ((currentChar = internalBlockOrder[i++]) != '\0') {
      if (currentChar == processedSign) {
        continue;
      }

      if ( (int) currentChar - 48 == currentIndex) {
        offset = i - 1;

        while (offset < cipherLength) {
          outputCipher[offset] = decryptInput[outputWritePosition++];
          offset += columns;
        }

        internalBlockOrder[i-1] = processedSign;

        if (++currentIndex == 10) {
          currentIndex = 0;
        }

        if (++processedColumns == columns) {
          break;
        }
      }
    }
  }

  return;
}

//------------------------------------------------------------------------------

void decrypt_string_representation(char decimalRepresation[], const char asciiLowercase[], const char stringedRepresentation[]) {

  char asciiChar = '\0';
  char stringedChar = '\0';
  int intVal = 0;

  int i = 0, j = 0;

  while((stringedChar = stringedRepresentation[i++]) != '\0') {
    j = 0;

    while((asciiChar = asciiLowercase[j++]) != '\0') {
      if (asciiChar == stringedChar) {
        intVal = (int) asciiChar % 3;

        if (intVal == 0) {
          intVal = 3;
        }

        decimalRepresation[i-1] = (char) intVal+48;
        decimalRepresation[i] = '\0';
        break;
      }
    }
  }

  return;
}

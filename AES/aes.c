#include <stdio.h>
#include <stdlib.h>

// Rcon[] is 1-based, so the first entry is just a place holder 
static const uint32_t Rcon[52] = { 0x00000000, 
           0x01000000, 0x02000000, 0x04000000, 0x08000000, 
           0x10000000, 0x20000000, 0x40000000, 0x80000000, 
           0x1B000000, 0x36000000, 0x6C000000, 0xD8000000, 
           0xAB000000, 0x4D000000, 0x9A000000, 0x2F000000, 
           0x5E000000, 0xBC000000, 0x63000000, 0xC6000000, 
           0x97000000, 0x35000000, 0x6A000000, 0xD4000000, 
           0xB3000000, 0x7D000000, 0xFA000000, 0xEF000000, 
           0xC5000000, 0x91000000, 0x39000000, 0x72000000, 
           0xE4000000, 0xD3000000, 0xBD000000, 0x61000000, 
           0xC2000000, 0x9F000000, 0x25000000, 0x4A000000, 
           0x94000000, 0x33000000, 0x66000000, 0xCC000000, 
           0x83000000, 0x1D000000, 0x3A000000, 0x74000000, 
           0xE8000000, 0xCB000000, 0x8D000000};

// S-box
static const uint8_t Sbox[256] = {
    // 0      1     2     3     4     5     6    7     8     9     A     B     C     D     E     F
     0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
     0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
     0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
     0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
     0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
     0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
     0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
     0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
     0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
     0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
     0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
     0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
     0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
     0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
     0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
     0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
    };

// Inverse S-Box
static const uint8_t InvSbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
    };

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t lword;
typedef uint8_t state_t[4][4];
int Nb = 4; // number of columns in the state

// Finite Field Arithmetic
uint8_t ffAdd(uint8_t x, uint8_t y);
uint8_t xtime(uint8_t x);
uint8_t ffMultiply(uint8_t x, uint8_t y);

// Key Expansion
void rotword(uint32_t* w);
void subword(uint32_t* w);

// Cipher
void subBytes(state_t* state);
void shiftRows(state_t* state);
void mixColumns(state_t* state);
void addRoundKey(state_t* state, uint32_t* w, int round);

// Inverse Cipher
void invSubBytes(state_t* state);
void invShiftRows(state_t* state);
void invMixColumns(state_t* state);



// FINITE FIELD ARITHMETIC //
// Adds two finite fields
uint8_t ffAdd(uint8_t x, uint8_t y) {  
    return x ^ y;
}

// Multiplies a finite field by x
uint8_t xtime(uint8_t x) {  
    uint8_t result = x << 1; // multiply by 2
    if (x & 0x80) {  // if high bit is set
        result ^= 0x1b;
    }
    return result;
}

// Uses xtime to multiply any finite field by any other finite field
uint8_t ffMultiply(uint8_t x, uint8_t y) {  
    uint8_t result = 0;
    uint8_t mask = 0x01;
    int i;
    for (i = 0; i < 8; i++) {
        if (y & mask) {
            result ^= x;
        }
        x = xtime(x);
        mask <<= 1; // shift mask left by 1
    }
    return result;
    printf("result = %x\n", result);
}


// KEY EXPANSION // 
uint8_t getSbox(uint8_t n) {
    return Sbox[n];
} 

// Performs a cyclic permutation on its input word
void rotword(uint32_t* w) {
    // printf("\nrotword ");
    uint8_t temp = *w >> 24; // assign the first byte to temp
    *w = (*w << 8) | temp; // shift the word left by 8 bits and add the first byte to the end
    // printf(" %x\n", *w);
}

// Takes a 4-byte input word and substitutes each byte in that word with its appropriate Sbox value
void subword(uint32_t* w) {
    // printf("\nsubword ");
    int i;
    byte temp[4];

    for (i = 0; i < 4; i++) {
        temp[i] = getSbox(*w >> (24 - 8*i));
    }
    *w = temp[0] << 24 | temp[1] << 16 | temp[2] << 8 | temp[3]; // 
    // printf(" %x\n", *w);
}

// Takes the cipher key and generates a key schedule
void keyExpansion(uint8_t* key, uint32_t* w, int Nk, int Nr) {
    int i;
    uint32_t temp;

    // First round key is the key itself
    for (i = 0; i < Nk; i++) {
        w[i] = key[4*i] << 24 | key[4*i+1] << 16 | key[4*i+2] << 8 | key[4*i+3];
    }

    // All other round keys are found from the previous round keys
    for (i = Nk; i < Nb * (Nr + 1); i++) {

        temp = w[i-1];
        if (i % Nk == 0) {
            rotword(&temp);
            subword(&temp);
            temp ^= Rcon[i/Nk];
            // printf("\nAfter XOR w Rcon = %x", temp);
        } else if (Nk > 6 && i % Nk == 4) {
            subword(&temp);
        }
        w[i] = w[i-Nk] ^ temp;
        // printf("\nw[i] = %x", w[i]);
    }
}


// CIPHER //
// Applies the Sbox to each byte of the State 
void subBytes(state_t* state){
    printf("\nsubBytes ");
    // state at i,j becomes Sbox[state at i,j]
    int i, j;
    for (i = 0; i < 4; i++) {
        printf("\n");
        for (j = 0; j < Nb; j++) {
            (*state)[i][j] = Sbox[(*state)[i][j]];
            printf("  %x ", (*state)[i][j]);
        }
    }
}

// Cyclically shifts the last three rows in the State
void shiftRows(state_t* state){
    printf("\nshiftRows ");
    int i, j;
    uint8_t temp;

    for(i = 1; i < 4; i++) {
        for(j = 0; j < i; j++) {
            temp = (*state)[i][0];
            (*state)[i][0] = (*state)[i][1];
            (*state)[i][1] = (*state)[i][2];
            (*state)[i][2] = (*state)[i][3];
            (*state)[i][3] = temp;
        }
    }
    for (i = 0; i < 4; i++) {
        printf("\n");
        for (j = 0; j < Nb; j++) {
            printf("  %x ", (*state)[i][j]);
        }
    }
}

// This transformation treats each column in the state as a four-term polynomial. 
// This polynomial is multiplied (modulo another polynomial) by a fixed polynomial with coefficients
void mixColumns(state_t* state){
    printf("\nmixColumns ");
    int c;
    for (c = 0; c < Nb; c++) {
        uint8_t s0 = (*state)[0][c];
        uint8_t s1 = (*state)[1][c];
        uint8_t s2 = (*state)[2][c];
        uint8_t s3 = (*state)[3][c];

        (*state)[0][c] = ffAdd(ffAdd(ffAdd(ffMultiply(0x02, s0),ffMultiply(0x03, s1)),s2),s3);
        (*state)[1][c] = ffAdd(ffAdd(ffAdd(s0,ffMultiply(0x02, s1)),ffMultiply(0x03, s2)),s3);
        (*state)[2][c] = ffAdd(ffAdd(ffAdd(s0,s1),ffMultiply(0x02, s2)),ffMultiply(0x03, s3));
        (*state)[3][c] = ffAdd(ffAdd(ffAdd(ffMultiply(0x03, s0),s1),s2),ffMultiply(0x02, s3));
    }

    for (int i = 0; i < 4; i++) {
        printf("\n");
        for (int j = 0; j < Nb; j++) {
            printf("  %x,", (*state)[i][j]);
        }
    }
}

// XORs each column of the State with a word from the key schedule
// The length of a Round Key equals the size of the State 
// (i.e., for Nb = 4, the Round Key length equals 128 bits/16 bytes)
void addRoundKey(state_t* state, uint32_t* w, int round){
    

    printf("\nround KEY VALUE ");
    for (int i = 0; i < Nb; i++) {
        printf("  %x", w[round * Nb + i]);
    }

    for (int c = 0; c < Nb; c++) {
        (*state)[0][c] ^= (w[round * Nb + c] >> 24) & 0xff;
        (*state)[1][c] ^= (w[round * Nb + c] >> 16) & 0xff;
        (*state)[2][c] ^= (w[round * Nb + c] >> 8) & 0xff;
        (*state)[3][c] ^= (w[round * Nb + c]) & 0xff;
    }    
}

void cipher(state_t* state, uint32_t* w, int Nr){
    printf("\nCIPHER ");

    int round = 0;
    // Add the First round key to the state before starting the rounds
    addRoundKey(state, w, round);

    for (round = 1; round < Nr; round++) {


        printf("\nstart of round ");
        for (int i = 0; i < 4; i++) {
            printf("\n");
            for (int j = 0; j < Nb; j++) {
                printf("  %x,", (*state)[i][j]);
            }
        }


        subBytes(state);
        shiftRows(state);
        mixColumns(state);  
        addRoundKey(state, w, round);
    }
    // Last round 
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, w, round);

    printf("\noutput = ");
    for (int i = 0; i < 4; i++) {
        printf("\n");
        for (int j = 0; j < Nb; j++) {
            printf("  %x,", (*state)[i][j]);
        }
    }

}


// INVERSE CIPHER //
void invSubBytes(state_t* state){}
void invShiftRows(state_t* state){}
void invMixColumns(state_t* state){}



int main() {
    
    uint8_t key[16] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,
                       0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
    uint8_t in[16]  = { 0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
                        0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34 };
    uint8_t out[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    

    int Nk = 0;
    int Nr = 0;
    // if size of Key is 16 bytes (128 bits) -> Nk = 4, Nr = 10
    // if size of Key is 24 bytes (192 bits) -> Nk = 6, Nr = 12
    // if size of Key is 32 bytes (256 bits) -> Nk = 8, Nr = 14
    if (sizeof(in) == 16) {
        Nk = 4;
        Nr = 10;
    } else if (sizeof(in) == 24) {
        Nk = 6;
        Nr = 12;
    } else if (sizeof(in) == 32) {
        Nk = 8;
        Nr = 14;
    } else {
        printf("Invalid key size. Must be 16, 24, or 32 bytes.\n");
        return 1;
    }

    uint32_t* w = (uint32_t*)malloc(Nb * (Nr + 1) * sizeof(uint32_t));
    state_t* state = (state_t*)malloc(sizeof(state_t));

    for (int i = 0; i < 4; i++) {
        printf("\n");
        for (int j = 0; j < Nb; j++) {
            (*state)[i][j] = in[i + 4 * j];
            printf("  %x,", (*state)[i][j]);
        }
    }
    
    keyExpansion(key, w, Nk, Nr);
    cipher(state, w, Nr);

    free(w);
    free(state);
    return 0;
}





// **************** TESTING **************** //

// TEST: Finite Field Arithmetic //
void TEST_ffAdd(uint8_t x, uint8_t y) {
    printf("Testing FFADD\n");
    printf("  ffAdd(%x, %x) = %x\n", x, y, ffAdd(x, y));
}

void TEST_xtime(uint8_t x) {
    printf("Testing XTIME\n");
    printf("  xtime(0x57) = %x\n", xtime(0x57));
    printf("  xtime(0xae) = %x\n", xtime(0xae));
    printf("  xtime(0x47) = %x\n", xtime(0x47));
    printf("  xtime(0x8e) = %x\n", xtime(0x8e));
}

void TEST_ffMultiply(uint8_t x, uint8_t y) {
    printf("Testing FFMULTIPLY\n");
    printf("  ffMultiply(0x57, 0x13) = %x\n", ffMultiply(0x57, 0x13));
}

// TEST: Key Expansion //
void TEST_rotWord() {
    uint32_t r1 = 0x09cf4f3c;
    uint32_t r2 = 0x2a6c7605;
    printf("Testing ROTWORD\n");
    printf("  rotWord(0x09cf4f3c) = ");
    rotword(&r1);
    printf("  rotWord(0x2a6c7605) = ");
    rotword(&r2);
}

void TEST_subWord() {
    uint32_t s1 = 0x00102030;
    uint32_t s2 = 0x40506070;
    uint32_t s3 = 0x8090a0b0;
    uint32_t s4 = 0xc0d0e0f0;
    printf("Testing SUBWORD\n");
    printf("  subWord(0x00102030) = ");
    subword(&s1);
    printf("  subWord(0x40506070) = ");
    subword(&s2);
    printf("  subWord(0x8090a0b0) = ");
    subword(&s3);
    printf("  subWord(0xc0d0e0f0) = ");
    subword(&s4);
}

void TEST_keyExpansion(uint8_t key[16], int Nk, int Nr){
    // printf("Testing KEY EXPANSION\n");
    
    // uint32_t w[44];
    // keyExpansion(key, w, Nk, Nr);

    // printf("  expanded key = \n");
    // for (int i = 0; i < 44; i++) {
    //     printf("%x ", w[i]);
    //     if (i % 4 == 3) {
    //         printf("\n");
    //     }
    // }
}

// TEST: Cipher //
void TEST_cipher(state_t* state, uint32_t* w, int Nr) {
    printf("Testing CIPHER\n");
    cipher(state, w, Nr);
    
}

// TEST: Inverse Cipher //


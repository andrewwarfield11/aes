﻿/* Your project should ultimately provide a class with two functions: encrypt() and decrypt(). 
Each function should take one argument, the plaintext or ciphertext, respectively, as a byte[] and return the result in a new byte[]. 
The encryption key should be provided as the only argument to the class constructor. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// important starting stuff

#define Nb 4 // Block Size ()
#define MAX_KEY_EXP 256

unsigned int Nk; // Key Length
unsigned int Nr; // Number of rounds
unsigned int k;  // cipher key

int init(const uint8_t *key, const size_t len);

void KeyExpansion(uint8_t *key, uint8_t schedule[], unsigned int Nk, unsigned int Nr);

void SubBytes(uint8_t **state);

void InvSubBytes(uint8_t **state);

void ShiftRows(uint8_t **state);

void InvShiftRows(uint8_t **state);

void MixColumns(uint8_t **state);

void InvMixColumns(uint8_t **state);

void AddRoundKey(uint8_t **state, int round);

void RotWord(uint8_t *word);

void SubWord(uint8_t *word);

void encrypt(const uint8_t *in, uint8_t *out);

void decrypt(const uint8_t *in, uint8_t *out);

static const uint8_t sbox[256] =
    {
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
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};
static const uint8_t inverseSbox[256] =
    {
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
        0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};

static const uint8_t Rcon[255] = {

    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d,
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab,
    0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d,
    0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25,
    0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01,
    0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d,
    0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa,
    0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a,
    0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02,
    0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f,
    0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5,
    0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33,
    0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb};

uint8_t MixCol9[256] = {
    0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f, 0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
    0x90, 0x99, 0x82, 0x8b, 0xb4, 0xbd, 0xa6, 0xaf, 0xd8, 0xd1, 0xca, 0xc3, 0xfc, 0xf5, 0xee, 0xe7,
    0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04, 0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
    0xab, 0xa2, 0xb9, 0xb0, 0x8f, 0x86, 0x9d, 0x94, 0xe3, 0xea, 0xf1, 0xf8, 0xc7, 0xce, 0xd5, 0xdc,
    0x76, 0x7f, 0x64, 0x6d, 0x52, 0x5b, 0x40, 0x49, 0x3e, 0x37, 0x2c, 0x25, 0x1a, 0x13, 0x08, 0x01,
    0xe6, 0xef, 0xf4, 0xfd, 0xc2, 0xcb, 0xd0, 0xd9, 0xae, 0xa7, 0xbc, 0xb5, 0x8a, 0x83, 0x98, 0x91,
    0x4d, 0x44, 0x5f, 0x56, 0x69, 0x60, 0x7b, 0x72, 0x05, 0x0c, 0x17, 0x1e, 0x21, 0x28, 0x33, 0x3a,
    0xdd, 0xd4, 0xcf, 0xc6, 0xf9, 0xf0, 0xeb, 0xe2, 0x95, 0x9c, 0x87, 0x8e, 0xb1, 0xb8, 0xa3, 0xaa,
    0xec, 0xe5, 0xfe, 0xf7, 0xc8, 0xc1, 0xda, 0xd3, 0xa4, 0xad, 0xb6, 0xbf, 0x80, 0x89, 0x92, 0x9b,
    0x7c, 0x75, 0x6e, 0x67, 0x58, 0x51, 0x4a, 0x43, 0x34, 0x3d, 0x26, 0x2f, 0x10, 0x19, 0x02, 0x0b,
    0xd7, 0xde, 0xc5, 0xcc, 0xf3, 0xfa, 0xe1, 0xe8, 0x9f, 0x96, 0x8d, 0x84, 0xbb, 0xb2, 0xa9, 0xa0,
    0x47, 0x4e, 0x55, 0x5c, 0x63, 0x6a, 0x71, 0x78, 0x0f, 0x06, 0x1d, 0x14, 0x2b, 0x22, 0x39, 0x30,
    0x9a, 0x93, 0x88, 0x81, 0xbe, 0xb7, 0xac, 0xa5, 0xd2, 0xdb, 0xc0, 0xc9, 0xf6, 0xff, 0xe4, 0xed,
    0x0a, 0x03, 0x18, 0x11, 0x2e, 0x27, 0x3c, 0x35, 0x42, 0x4b, 0x50, 0x59, 0x66, 0x6f, 0x74, 0x7d,
    0xa1, 0xa8, 0xb3, 0xba, 0x85, 0x8c, 0x97, 0x9e, 0xe9, 0xe0, 0xfb, 0xf2, 0xcd, 0xc4, 0xdf, 0xd6,
    0x31, 0x38, 0x23, 0x2a, 0x15, 0x1c, 0x07, 0x0e, 0x79, 0x70, 0x6b, 0x62, 0x5d, 0x54, 0x4f, 0x46};

uint8_t MixCol11[256] = {
    0x00, 0x0b, 0x16, 0x1d, 0x2c, 0x27, 0x3a, 0x31, 0x58, 0x53, 0x4e, 0x45, 0x74, 0x7f, 0x62, 0x69,
    0xb0, 0xbb, 0xa6, 0xad, 0x9c, 0x97, 0x8a, 0x81, 0xe8, 0xe3, 0xfe, 0xf5, 0xc4, 0xcf, 0xd2, 0xd9,
    0x7b, 0x70, 0x6d, 0x66, 0x57, 0x5c, 0x41, 0x4a, 0x23, 0x28, 0x35, 0x3e, 0x0f, 0x04, 0x19, 0x12,
    0xcb, 0xc0, 0xdd, 0xd6, 0xe7, 0xec, 0xf1, 0xfa, 0x93, 0x98, 0x85, 0x8e, 0xbf, 0xb4, 0xa9, 0xa2,
    0xf6, 0xfd, 0xe0, 0xeb, 0xda, 0xd1, 0xcc, 0xc7, 0xae, 0xa5, 0xb8, 0xb3, 0x82, 0x89, 0x94, 0x9f,
    0x46, 0x4d, 0x50, 0x5b, 0x6a, 0x61, 0x7c, 0x77, 0x1e, 0x15, 0x08, 0x03, 0x32, 0x39, 0x24, 0x2f,
    0x8d, 0x86, 0x9b, 0x90, 0xa1, 0xaa, 0xb7, 0xbc, 0xd5, 0xde, 0xc3, 0xc8, 0xf9, 0xf2, 0xef, 0xe4,
    0x3d, 0x36, 0x2b, 0x20, 0x11, 0x1a, 0x07, 0x0c, 0x65, 0x6e, 0x73, 0x78, 0x49, 0x42, 0x5f, 0x54,
    0xf7, 0xfc, 0xe1, 0xea, 0xdb, 0xd0, 0xcd, 0xc6, 0xaf, 0xa4, 0xb9, 0xb2, 0x83, 0x88, 0x95, 0x9e,
    0x47, 0x4c, 0x51, 0x5a, 0x6b, 0x60, 0x7d, 0x76, 0x1f, 0x14, 0x09, 0x02, 0x33, 0x38, 0x25, 0x2e,
    0x8c, 0x87, 0x9a, 0x91, 0xa0, 0xab, 0xb6, 0xbd, 0xd4, 0xdf, 0xc2, 0xc9, 0xf8, 0xf3, 0xee, 0xe5,
    0x3c, 0x37, 0x2a, 0x21, 0x10, 0x1b, 0x06, 0x0d, 0x64, 0x6f, 0x72, 0x79, 0x48, 0x43, 0x5e, 0x55,
    0x01, 0x0a, 0x17, 0x1c, 0x2d, 0x26, 0x3b, 0x30, 0x59, 0x52, 0x4f, 0x44, 0x75, 0x7e, 0x63, 0x68,
    0xb1, 0xba, 0xa7, 0xac, 0x9d, 0x96, 0x8b, 0x80, 0xe9, 0xe2, 0xff, 0xf4, 0xc5, 0xce, 0xd3, 0xd8,
    0x7a, 0x71, 0x6c, 0x67, 0x56, 0x5d, 0x40, 0x4b, 0x22, 0x29, 0x34, 0x3f, 0x0e, 0x05, 0x18, 0x13,
    0xca, 0xc1, 0xdc, 0xd7, 0xe6, 0xed, 0xf0, 0xfb, 0x92, 0x99, 0x84, 0x8f, 0xbe, 0xb5, 0xa8, 0xa3};

uint8_t MixCol13[256] = {
    0x00, 0x0d, 0x1a, 0x17, 0x34, 0x39, 0x2e, 0x23, 0x68, 0x65, 0x72, 0x7f, 0x5c, 0x51, 0x46, 0x4b,
    0xd0, 0xdd, 0xca, 0xc7, 0xe4, 0xe9, 0xfe, 0xf3, 0xb8, 0xb5, 0xa2, 0xaf, 0x8c, 0x81, 0x96, 0x9b,
    0xbb, 0xb6, 0xa1, 0xac, 0x8f, 0x82, 0x95, 0x98, 0xd3, 0xde, 0xc9, 0xc4, 0xe7, 0xea, 0xfd, 0xf0,
    0x6b, 0x66, 0x71, 0x7c, 0x5f, 0x52, 0x45, 0x48, 0x03, 0x0e, 0x19, 0x14, 0x37, 0x3a, 0x2d, 0x20,
    0x6d, 0x60, 0x77, 0x7a, 0x59, 0x54, 0x43, 0x4e, 0x05, 0x08, 0x1f, 0x12, 0x31, 0x3c, 0x2b, 0x26,
    0xbd, 0xb0, 0xa7, 0xaa, 0x89, 0x84, 0x93, 0x9e, 0xd5, 0xd8, 0xcf, 0xc2, 0xe1, 0xec, 0xfb, 0xf6,
    0xd6, 0xdb, 0xcc, 0xc1, 0xe2, 0xef, 0xf8, 0xf5, 0xbe, 0xb3, 0xa4, 0xa9, 0x8a, 0x87, 0x90, 0x9d,
    0x06, 0x0b, 0x1c, 0x11, 0x32, 0x3f, 0x28, 0x25, 0x6e, 0x63, 0x74, 0x79, 0x5a, 0x57, 0x40, 0x4d,
    0xda, 0xd7, 0xc0, 0xcd, 0xee, 0xe3, 0xf4, 0xf9, 0xb2, 0xbf, 0xa8, 0xa5, 0x86, 0x8b, 0x9c, 0x91,
    0x0a, 0x07, 0x10, 0x1d, 0x3e, 0x33, 0x24, 0x29, 0x62, 0x6f, 0x78, 0x75, 0x56, 0x5b, 0x4c, 0x41,
    0x61, 0x6c, 0x7b, 0x76, 0x55, 0x58, 0x4f, 0x42, 0x09, 0x04, 0x13, 0x1e, 0x3d, 0x30, 0x27, 0x2a,
    0xb1, 0xbc, 0xab, 0xa6, 0x85, 0x88, 0x9f, 0x92, 0xd9, 0xd4, 0xc3, 0xce, 0xed, 0xe0, 0xf7, 0xfa,
    0xb7, 0xba, 0xad, 0xa0, 0x83, 0x8e, 0x99, 0x94, 0xdf, 0xd2, 0xc5, 0xc8, 0xeb, 0xe6, 0xf1, 0xfc,
    0x67, 0x6a, 0x7d, 0x70, 0x53, 0x5e, 0x49, 0x44, 0x0f, 0x02, 0x15, 0x18, 0x3b, 0x36, 0x21, 0x2c,
    0x0c, 0x01, 0x16, 0x1b, 0x38, 0x35, 0x22, 0x2f, 0x64, 0x69, 0x7e, 0x73, 0x50, 0x5d, 0x4a, 0x47,
    0xdc, 0xd1, 0xc6, 0xcb, 0xe8, 0xe5, 0xf2, 0xff, 0xb4, 0xb9, 0xae, 0xa3, 0x80, 0x8d, 0x9a, 0x97};

uint8_t MixCol14[256] = {
    0x00, 0x0e, 0x1c, 0x12, 0x38, 0x36, 0x24, 0x2a, 0x70, 0x7e, 0x6c, 0x62, 0x48, 0x46, 0x54, 0x5a,
    0xe0, 0xee, 0xfc, 0xf2, 0xd8, 0xd6, 0xc4, 0xca, 0x90, 0x9e, 0x8c, 0x82, 0xa8, 0xa6, 0xb4, 0xba,
    0xdb, 0xd5, 0xc7, 0xc9, 0xe3, 0xed, 0xff, 0xf1, 0xab, 0xa5, 0xb7, 0xb9, 0x93, 0x9d, 0x8f, 0x81,
    0x3b, 0x35, 0x27, 0x29, 0x03, 0x0d, 0x1f, 0x11, 0x4b, 0x45, 0x57, 0x59, 0x73, 0x7d, 0x6f, 0x61,
    0xad, 0xa3, 0xb1, 0xbf, 0x95, 0x9b, 0x89, 0x87, 0xdd, 0xd3, 0xc1, 0xcf, 0xe5, 0xeb, 0xf9, 0xf7,
    0x4d, 0x43, 0x51, 0x5f, 0x75, 0x7b, 0x69, 0x67, 0x3d, 0x33, 0x21, 0x2f, 0x05, 0x0b, 0x19, 0x17,
    0x76, 0x78, 0x6a, 0x64, 0x4e, 0x40, 0x52, 0x5c, 0x06, 0x08, 0x1a, 0x14, 0x3e, 0x30, 0x22, 0x2c,
    0x96, 0x98, 0x8a, 0x84, 0xae, 0xa0, 0xb2, 0xbc, 0xe6, 0xe8, 0xfa, 0xf4, 0xde, 0xd0, 0xc2, 0xcc,
    0x41, 0x4f, 0x5d, 0x53, 0x79, 0x77, 0x65, 0x6b, 0x31, 0x3f, 0x2d, 0x23, 0x09, 0x07, 0x15, 0x1b,
    0xa1, 0xaf, 0xbd, 0xb3, 0x99, 0x97, 0x85, 0x8b, 0xd1, 0xdf, 0xcd, 0xc3, 0xe9, 0xe7, 0xf5, 0xfb,
    0x9a, 0x94, 0x86, 0x88, 0xa2, 0xac, 0xbe, 0xb0, 0xea, 0xe4, 0xf6, 0xf8, 0xd2, 0xdc, 0xce, 0xc0,
    0x7a, 0x74, 0x66, 0x68, 0x42, 0x4c, 0x5e, 0x50, 0x0a, 0x04, 0x16, 0x18, 0x32, 0x3c, 0x2e, 0x20,
    0xec, 0xe2, 0xf0, 0xfe, 0xd4, 0xda, 0xc8, 0xc6, 0x9c, 0x92, 0x80, 0x8e, 0xa4, 0xaa, 0xb8, 0xb6,
    0x0c, 0x02, 0x10, 0x1e, 0x34, 0x3a, 0x28, 0x26, 0x7c, 0x72, 0x60, 0x6e, 0x44, 0x4a, 0x58, 0x56,
    0x37, 0x39, 0x2b, 0x25, 0x0f, 0x01, 0x13, 0x1d, 0x47, 0x49, 0x5b, 0x55, 0x7f, 0x71, 0x63, 0x6d,
    0xd7, 0xd9, 0xcb, 0xc5, 0xef, 0xe1, 0xf3, 0xfd, 0xa7, 0xa9, 0xbb, 0xb5, 0x9f, 0x91, 0x83, 0x8d};

/*void printState(uint8_t **state)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%x", state[i][j]);
        }
        printf(" ");
    }
    printf("\n");
}*/

struct aes_ctx
{
    unsigned int Nk; // Key Length (32b words)
    unsigned int Nr; // Number of rounds
    uint8_t schedule[MAX_KEY_EXP];
} ctx;

int init(const uint8_t *key, const size_t len)
{
    ctx.Nk = len / 4;
    /*switch (len)
    {
    case 16:
        ctx.Nr = 10;
    case 24:
        ctx.Nr = 12;
    case 32:
        ctx.Nr = 14;
    default:
        printf("Invalid key length\n");
        return 0;
    }*/
    if (len == 16) {
        ctx.Nr = 10;
    }
    else if (len == 24) {
        ctx.Nr = 12;
    }
    else if (len == 32) {
        ctx.Nr = 14;
    }
    else {
        printf("Invalid key length\n");
        return 0;
    }
    KeyExpansion(key, ctx.schedule, ctx.Nk, ctx.Nr);
    return 1;
}

// Key Scheduler
// THIS IS ALL PSUDO CODE
void KeyExpansion(uint8_t* key, uint8_t schedule[], unsigned int Nk, unsigned int Nr)
{

    int i = 0;
    uint8_t temp[4];

    for (i = 0; i < Nk; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            schedule[4*i+j] = key[4*i+j];
        }
    }

    for (i = Nk; i < Nb * (Nr + 1); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp[j] = schedule[4*(i-1)+j];
        }

        if (i % Nk == 0)
        {
            RotWord(temp);
            SubWord(temp);
            temp[0] = temp[0] ^ Rcon[i / Nk];
        }
        else if (Nk > 6 && i % Nk == 4)
        {
            SubWord(temp);
        }
        for (int j = 0; j < 4; j++)
        {
            schedule[4*i+j] = schedule[4*(i-Nk)+j] ^ temp[j];
        }
    }
}

// Individual Transformations

/* Transformation in the Cipher that processes the State using
a nonlinear byte substitution table (S-box) that operates on
each of the State bytes independently. */
void SubBytes(uint8_t** state)
{ //ben
    // byte wise substition
    uint8_t i, j;
    for (i = 0; i < 4; i++) // for each row
    {
        for (j = 0; j < 4; j++) // for each column
        {
            state[i][j] = sbox[state[i][j]]; // Replaces the byte with the byte in the sub table
        }
    }
}

    /* Transformation in the Inverse Cipher that is the inverse of
SubBytes(). */
void InvSubBytes(uint8_t** state)
{
    // Same as sub bytes, just use a different table
    uint8_t i, j;
    for (i = 0; i < 4; i++) // for each row
    {
        for (j = 0; j < 4; j++) // for each column
        {
            state[i][j] = inverseSbox[state[i][j]]; // Replaces the byte with the byte in the inverse sub table
        }
    }
}

    /* Transformation in the Cipher that processes the State by cyclically
shifting the last three rows of the State by different offsets. */
void ShiftRows(uint8_t** state)
{ //ben
    // first row is not shifted
    // row 2 shifts one byte
    uint8_t val0, val1, val2;

    val0 = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = state[1][2];
    state[1][2] = state[1][3];
    state[1][3] = val0;

    // row 3 shifts 2 bytes over
    val0 = state[2][0];
    val1 = state[2][1];
    state[2][0] = state[2][2];
    state[2][1] = state[2][3];
    state[2][2] = val0;
    state[2][3] = val1;

    // row 4 shifts 3 bytes over
    val0 = state[3][0];
    val1 = state[3][1];
    val2 = state[3][2];
    state[3][0] = state[3][3];
    state[3][1] = val0;
    state[3][2] = val1;
    state[3][3] = val2;
}

    /* Transformation in the Inverse Cipher that is the inverse of
Shiftcols(). */
void InvShiftRows(uint8_t ** state)
{ //Andrew
    uint8_t temp1 = state[1][0];
    uint8_t temp2 = state[2][0];
    uint8_t temp3 = state[3][0];
    uint8_t temp4 = state[2][1];

    state[1][0] = state[1][3];
    state[1][3] = state[1][2];
    state[1][2] = state[1][1];
    state[1][1] = temp1;

    state[2][0] = state[2][2];
    state[2][1] = state[2][3];
    state[2][2] = temp2;
    state[2][3] = temp4;

    state[3][0] = state[3][1];
    state[3][1] = state[3][2];
    state[3][2] = state[3][3];
    state[3][3] = temp3;
}

uint8_t mixCol2(uint8_t b)
{
    // get the first bit
    uint8_t first = b >> 7 & 0x01;
    b = b << 1;
    if (first)
        return b;
    else
        return b ^ 0x1B; // xor with 0001 1011 (0x1B) if first bit was 1 before shift1
}

uint8_t mixCol3(uint8_t b)
{
    return mixCol2(b) ^ b;
}

/* Transformation in the Cipher that takes all of the columns of the
State and mixes their data (independently of one another) to
produce new columns. */
void MixColumns(uint8_t** state)
{ //andrew

    /*
|  2  3  1  1  |
|  1  2  3  1  |
|  1  1  2  3  |
|  3  1  1  2  |
*/
    for (int i = 0; i < 4; i++)
    {
        uint8_t col[4] = {state[0][i], state[1][i], state[2][i], state[3][i]};

        state[0][i] = mixCol2(col[0]) ^ mixCol3(col[1]) ^ col[2] ^ col[3];
        state[1][i] = col[0] ^ mixCol2(col[1]) ^ mixCol3(col[2]) ^ col[3];
        state[2][i] = col[0] ^ col[1] ^ mixCol2(col[2]) ^ mixCol3(col[3]);
        state[3][i] = mixCol3(col[0]) ^ col[1] ^ col[2] ^ mixCol2(col[3]);
    }
}

/* Transformation in the Inverse Cipher that is the inverse of
MixColumns(). */
void InvMixColumns(uint8_t** state)
{ //andrew

    /*
|  14  11  13  9   |
|  9   14  11  13  |
|  13  9   14  11  |
|  11  13  9   14  |
*/
    for (int i = 0; i < 4; i++)
    {
        uint8_t col[4] = {state[0][i], state[1][i], state[2][i], state[3][i]};

        state[0][i] = MixCol14[col[0]] ^ MixCol11[col[1]] ^ MixCol13[col[2]] ^ MixCol9[col[3]];
        state[1][i] = MixCol9[col[0]] ^ MixCol14[col[1]] ^ MixCol11[col[2]] ^ MixCol13[col[3]];
        state[2][i] = MixCol13[col[0]] ^ MixCol9[col[1]] ^ MixCol14[col[2]] ^ MixCol11[col[3]];
        state[3][i] = MixCol11[col[0]] ^ MixCol13[col[1]] ^ MixCol9[col[2]] ^ MixCol14[col[3]];
    }
}

/* Transformation in the Cipher and Inverse Cipher in which a Round
Key is added to the State using an XOR operation. The length of a
Round Key equals the size of the State (i.e., for Nb = 4, the Round
Key length equals 128 bits/16 bytes). */
void AddRoundKey(uint8_t** state, int round)
{ //abraham
    int i, j;
    //printf("\nround key using sub key\n");
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            //state[i][j] = state[i][j] ^ ctx.schedule[4 * (4 * round + j) + i];
            uint8_t sched = ctx.schedule[4 * (4 * round + j) + i];
            state[i][j] = state[i][j] ^ sched;
            //state[i][j] = state[i][j] ^ ctx.schedule[4 * (4 * round + i) + j];
            //printf("%x", sched);
        }
        //printf(" ");
    }
    //printf("\n");
}

/* Function used in the Key Expansion routine that takes a four-byte
word and performs a cyclic permutation. */
void RotWord(uint8_t* word)
{ //abraham
    uint8_t c = word[0];
    int i;
    for (i = 0; i < 3; i++)
    {
        word[i] = word[i + 1];
    }
    word[3] = c;
}

/* Function used in the Key Expansion routine that takes a four-byte
input word and applies an S-box to each of the four bytes to
produce an output word. */
void SubWord(uint8_t* word)
{ //abraham
    int i;
    for (i = 0; i < 4; i++)
    {
        word[i] = sbox[word[i]];
    }
}

// ----------------------------------------------------------------------------------------------------------------------------------------------
/* Each function should take one argument, the plaintext or ciphertext, respectively, as a byte[] and return the result in a new byte[].
The encryption key should be provided as the only argument to the class constructor. */

// Encryption Routine
void encrypt(const uint8_t *in, uint8_t *out)
{
    //Get the key and length, confirm that it's of correct length.
    uint8_t **state = malloc(4 * sizeof(uint8_t *));
    for (int i = 0; i < 4; i++)
    {
        state[i] = malloc(4 * sizeof(uint8_t));
    }
    //state = in;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            state[i][j] = in[4*j+i];
        }
    }
    //printf("\nstate after first round key:\n");
    AddRoundKey(state, 0);
    //printState(state);
    int i;
    for (i = 1; i < ctx.Nr; i++)
    {
        //printf("\nstate after sub bytes:\n");
        SubBytes(state);
        //printState(state);

        //printf("\nstate after shift rows:\n");
        ShiftRows(state);
        //printState(state);

        //printf("\nstate after mix columns:\n");
        MixColumns(state);
        //printState(state);

        //printf("\nstate after add round key:\n");
        AddRoundKey(state, i);
        //printState(state);

    }
    //printf("\nstate after sub bytes:\n");
    SubBytes(state);
    //printState(state);

    //printf("\nstate after shift rows:\n");
    ShiftRows(state);
    //printState(state);

    //printf("\nstate after add round key:\n");
    AddRoundKey(state, i);
    //printState(state);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            out[4*j+i] = state[i][j];
        }
    }
}

    /*
//PUSDO CODE
//FOR ENCRYPTION
Cipher(byte in[4*Nb], byte out[4*Nb], word w[Nb*(Nr+1)]) {
begin
    byte state[4,Nb]

    state = in

    AddRoundKey(state, w[0, Nb-1]) // See Sec. 5.1.4

    for round = 1 step 1 to Nr–1
        SubBytes(state) // See Sec. 5.1.1
        Shiftcols(state) // See Sec. 5.1.2
        MixColumns(state) // See Sec. 5.1.3
        AddRoundKey(state, w[round*Nb, (round+1)*Nb-1])
    end for

    SubBytes(state)
    Shiftcols(state)
    AddRoundKey(state, w[Nr*Nb, (Nr+1)*Nb-1])

    out = state
end
}
*/

    // Decryption Routine
void decrypt(const uint8_t *in, uint8_t *out)
{
    uint8_t **state = malloc(4 * sizeof(uint8_t *));
    for (int i = 0; i < 4; i++)
    {
        state[i] = malloc(4 * sizeof(uint8_t));
    }

    //set state equal to in
    //state = in;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            state[i][j] = in[4 * j + i];
        }
    }

    // perform decryption
    AddRoundKey(state, ctx.Nr);
    //printf("\nstate after first round key:\n");
    //printState(state);
    for (int i = ctx.Nr - 1; i > 0; i--)
    {
        //printf("\nstate after inv shift rows:\n");
        InvShiftRows(state);
        //printState(state);

        //printf("\nstate after inv subbytes:\n");
        InvSubBytes(state);
        //printState(state);

        AddRoundKey(state, i);
        //printf("\nstate after %dth round key:\n", i);
        //printState(state);

        //printf("\nstate after inv mix columns:\n");
        InvMixColumns(state);
        //printState(state);
    }
    //printf("\nstate after inv shift rows:\n");
    InvShiftRows(state);
    //printState(state);

    //printf("\nstate after inv subbytes:\n");
    InvSubBytes(state);
    //printState(state);

    AddRoundKey(state, 0);
    //printf("\nstate after last round key:\n");
    //printState(state);

    // set out equal to state
    //out = state;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            out[4 * j + i] = state[i][j];
        }
    }

}
int main()
{
    uint8_t **state = malloc(4 * sizeof(uint8_t *));
    uint8_t *out = malloc(16 * sizeof(uint8_t *));
    uint8_t *final = malloc(16 * sizeof(uint8_t *));
    uint8_t *in = malloc(16 * sizeof(uint8_t *));
    uint8_t *key = malloc(32 * sizeof(uint8_t *));
    for (int i = 0; i < 4; i++)
    {
        state[i] = malloc(4 * sizeof(uint8_t));
    }
    uint8_t arr[4][4] = {{0x00, 0x01, 0x02, 0x03}, {0x04, 0x05, 0x06, 0x07}, {0x08, 0x09, 0x0A, 0x0B}, {0x0C, 0x0D, 0x0E, 0x0F}};
    uint8_t k[8][4] = {
        {0x00, 0x01, 0x02, 0x03},
        {0x04, 0x05, 0x06, 0x07},
        {0x08, 0x09, 0x0A, 0x0B},
        {0x0C, 0x0D, 0x0E, 0x0F},
        {0x10, 0x11, 0x12, 0x13},
        {0x14, 0x15, 0x16, 0x17},
        {0x18, 0x19, 0x1A, 0x1B},
        {0x1C, 0x1D, 0x1E, 0x1F}};
    uint8_t inarr[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    for (int i = 0; i < 16; i++)
    {
        in[i] = inarr[i];
    }
    printf("\nencrypting\n-------------------------------\n\nstarting text\n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf(" %x ", in[4 * i + j]);
        }
        printf("\n");
    }

    printf("Key:\n");
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            key[4 * i + j] = k[i][j];
            printf(" %x ", key[4 * i + j]);
        }
        printf("\n");
    }
    size_t keysize = sizeof(key);
    keysize *= 4;
    //printf("\nk: %zu\n", keysize);
    if (!init(key, keysize))
        return 0;
    //KeyExpansion(key, ctx.schedule, ctx.Nk, ctx.Nr);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            state[i][j] = arr[i][j];
        }
    }

    encrypt(in, out);
    printf("\nout\n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf(" %x ", out[4 * i + j]);
        }
        printf("\n");
    }
    printf("\nDecrypting\n----------------------------------\n");
    printf("\nstarting text\n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf(" %x ", out[4 * i + j]);
        }
        printf("\n");
    }
    decrypt(out, final);
    printf("\nDecrypted:\n");
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf(" %x ", final[4 * i + j]);
        }
        printf("\n");
    }

    for (int i = 0; i < 4; i++)
    {
        free(state[i]);
    }
    free(state);
    free(out);
    free(in);
    free(final);
    free(key);
    return 0;
}

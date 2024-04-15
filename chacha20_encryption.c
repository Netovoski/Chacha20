#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INT_BITS 32 

unsigned int constant[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};  // "expand 32-byte k"
unsigned int key[8] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c}; // password
unsigned int state[4][4]; // 4x4 matrix
unsigned int *pointer_state = &state[0][0];
unsigned int nonce[3] = {0x09000000, 0x4a000000, 0x00000000};
unsigned int counter = 0x01;
char block[64];
void generate_nonce();

 
void inner_block(unsigned int *state);
void quarter_round(unsigned int *state, unsigned int a, unsigned int b, unsigned int c, unsigned int d);
unsigned int left_rotate(unsigned int value, unsigned short int num_shifted);
unsigned int *chacha20_block(unsigned int *key, unsigned int counter, unsigned int *nonce);
void xor_message(char *message, char *encrypted_message);


int main(){  

    char message[] = "oi, bebe";
    char *encrypted_message = calloc((strlen(message)+1) * sizeof(char), sizeof(char));
    
    // Encrpyt message, xor with state 
    printf("Original message: %s\n", message);
    xor_message(message, encrypted_message); // encrypt
    printf("\nEncrypted_message Text: %s\n", encrypted_message);
    xor_message(encrypted_message, message); // decrypt
    printf("\nDecrypted Text: %s\n", message);

    return 0;    
}

void inner_block(unsigned int *state){
    quarter_round(state, 0, 4, 8, 12);
    quarter_round(state, 1, 5, 9, 13);
    quarter_round(state, 2, 6, 10, 14);
    quarter_round(state, 3, 7, 11, 15);
    quarter_round(state, 0, 5, 10, 15);
    quarter_round(state, 1, 6, 11, 12);
    quarter_round(state, 2, 7, 8, 13);
    quarter_round(state, 3, 4, 9, 14);
}

void quarter_round(unsigned int *state, unsigned int a, unsigned int b, unsigned int c, unsigned int d){

    unsigned int *p = state;

    *(p + a) += *(p + b);
    *(p + d) ^= *(p + a);
    *(p + d) = left_rotate(*(p + d), 16);
    *(p + c) += *(p + d); 
    *(p + b) ^= *(p + c); 
    *(p + b) = left_rotate(*(p + b), 12);
    *(p + a) += *(p + b);
    *(p + d) ^= *(p + a); 
    *(p + d) = left_rotate(*(p + d), 8);
    *(p + c) += *(p + d); 
    *(p + b) ^= *(p + c); 
    *(p + b) = left_rotate(*(p + b), 7);
}

unsigned int left_rotate(unsigned int value, unsigned short int num_shifted){
    return (value << num_shifted) | (value >> (INT_BITS - num_shifted));
}

unsigned int* chacha20_block(unsigned int *key, unsigned int counter, unsigned int *nonce){
    unsigned short int cont = 0;

    // Attributing value on matrix
    for (unsigned short int i = 0; i < 16; i++)
    {
        if(i <= 3){ 
            *(pointer_state + (i)) = constant[cont];
            cont++;
            if(i == 3) cont = 0;
        }
        else if((i >= 4) && (i <= 11)){ 
            *(pointer_state + (i)) = key[cont];
            cont++;
            if(i == 11) cont = 0;
        }
        else if(i == 12){ 
            *(pointer_state + (i)) = counter;
        }
        else{ 
            *(pointer_state + (i)) = nonce[cont];
            cont++;
        }
    }

    unsigned int initial_state[4][4];
    unsigned int *pointer_initial = &initial_state[0][0];

    for (unsigned short int i = 0; i < 16; i++)
    {
        *(pointer_initial + i) = *(pointer_state + i);
    }
  
    for (unsigned short int i = 0; i < 10; i++)
    {
        inner_block(&state[0][0]);
    }

    for (unsigned short int i = 0; i < 16; i++)
    {
        *(pointer_state + i) = *(pointer_state + i) + *(pointer_initial + i);
    }
    
    return pointer_state;
}

// XOR message with state matrix
void xor_message(char *message, char *encrypted_message) {

    generate_nonce();
    unsigned int len_text = strlen(message);
    unsigned int *key_stream;
    unsigned int parts = len_text / 64 + 1;

    for (int part = 0; part < parts; part++)
    {
        key_stream = chacha20_block(&key[0], counter + part, &nonce[0]);
        unsigned int last_i = part * 64 + 64;
        if (part == parts - 1) {
            last_i = len_text;
        }
        for (int j = part * 64; j < last_i; j++)
        {
            // get ith byte of the state, with 4 bytes per word. 4x4
            unsigned int state_word = *(key_stream + (j / 16));
            block[j] = (state_word >> (8 * (j % 4))) & 0xff;
            *(encrypted_message + j) = *(message + j) ^ block[j];
        }

        
    }

}

void generate_nonce() {
    int seed;
    // Método 1: Gerar com uma seed fixa.
    seed = 5;

    // Método 2: Gerar usando a chave para a seed
    for (int i = 0; i < 8; i++) {
        seed |= (key[i] & 0xF) << (i * 4);
    }

    srand(seed);
    nonce[0] = rand();
    nonce[1] = rand();
    nonce[2] = rand();
}

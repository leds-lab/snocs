#include "Simon.h"

// Cipher Operation Macros
#define shift_one(x_word) (((x_word) << 1) | ((x_word) >> (word_size - 1)))
#define shift_eight(x_word) (((x_word) << 8) | ((x_word) >> (word_size - 8)))
#define shift_two(x_word) (((x_word) << 2) | ((x_word) >> (word_size - 2)))

#define rshift_three(x) (((x) >> 3) | (((x) & 0x7) << (word_size - 3)))
#define rshift_one(x)   (((x) >> 1) | (((x) & 0x1) << (word_size - 1)))

uint64_t z_arrays[5] = {0b0001100111000011010100100010111110110011100001101010010001011111,
                        0b0001011010000110010011111011100010101101000011001001111101110001,
                        0b0011001101101001111110001000010100011001001011000000111011110101,
                        0b0011110000101100111001010001001000000111101001100011010111011011,
                        0b0011110111001001010011000011101000000100011011010110011110001011};

uint8_t initials_key[9][8] = {{0x00, 0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x19},
                          {0x00, 0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x19},
                          {0x00, 0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x19},
                          {0x00, 0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x19},
                          {0x00, 0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x19},
                          {0x00, 0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x19},
                          {0x00, 0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x19},
                          {0x00, 0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x19},
                          {0x00, 0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x19}
                         };

// Valid Cipher Parameters
const uint8_t simon_rounds[] = {32, 36, 36, 42, 44, 52, 54, 68, 69, 72};
const uint8_t simon_block_sizes[] = {32, 48, 48, 64, 64, 96, 96, 128, 128, 128};
const uint16_t simon_key_sizes[] = {64, 72, 96, 96, 128, 96, 144, 128, 192, 256};
const uint8_t  z_assign[] = {0, 0, 1, 2, 3, 2, 3, 2, 3, 4};


SIMON::SIMON(sc_module_name nm,
             unsigned short int SIMON_ID)
    :sc_module(nm),
     SIMON_ID(SIMON_ID)
{
    // Distribui as chaves iniciais
    if(SIMON_ID == DISTRIBUTOR_KEY_POS){
        for(unsigned short x = 0; x < 9; x++) {
            for(unsigned short y = 0; y < 8; y++) {
                w_KEY[x][y] = initials_key[x][y];
            }
        }
    }else{
        for(unsigned short y = 0; y < 8; y++) {
            w_KEY[DISTRIBUTOR_KEY_POS-1][y] = initials_key[SIMON_ID-1][y];
        }
    }

    SC_METHOD(Simon_EDI);
    sensitive << i_DATA;
}


void SIMON::Simon_Encrypt_32(const uint8_t *key_schedule, const uint8_t *plaintext, uint8_t *ciphertext) {

    const uint8_t word_size = 16;
    uint16_t *y_word = (uint16_t *)ciphertext;
    uint16_t *x_word = (((uint16_t *)ciphertext) + 1);

    *y_word = *(uint16_t *)plaintext;
    *x_word = *(((uint16_t *)plaintext) + 1);

    uint16_t *round_key_ptr = (uint16_t *)key_schedule;
    int i =0;
    for( i = 0; i < 32; i++) {  // Block size 32 has only one round number option

        // Shift, AND , XOR ops
        uint16_t temp = (shift_one(*x_word) & shift_eight(*x_word)) ^ *y_word ^ shift_two(*x_word);

        // Feistel Cross
        *y_word = *x_word;

        // XOR with Round Key
        *x_word = temp ^ *(round_key_ptr + i);
    }
}

void SIMON::Simon_Decrypt_32(const uint8_t *key_schedule, const uint8_t *ciphertext, uint8_t *plaintext) {

    const uint8_t word_size = 16;
    uint16_t *x_word = (uint16_t *)plaintext;
    uint16_t *y_word = ((uint16_t *)plaintext) + 1;
    uint16_t *round_key_ptr = (uint16_t *)key_schedule;

    *x_word = *(uint16_t *)ciphertext;
    *y_word = *(((uint16_t *)ciphertext) + 1);
    int i =0;
    for( i = 31; i >= 0; i--) {  // Block size 32 has only one round number option

        // Shift, AND , XOR ops
        uint16_t temp = (shift_one(*x_word) & shift_eight(*x_word)) ^ *y_word ^ shift_two(*x_word);

        // Feistel Cross
        *y_word = *x_word;

        // XOR with Round Key
        *x_word = temp ^ *(round_key_ptr + i);
    }
}

void SIMON::Simon_Init(Simon_Cipher *cipher_object, void *key) {

    cipher_object->block_size = 32 ;//simon_block_sizes[cipher_cfg];
    cipher_object->key_size = 64; //simon_key_sizes[cipher_cfg];
    cipher_object->round_limit = 32; //simon_rounds[cipher_cfg];
    cipher_object->z_seq = 0 ; //z_assign[cipher_cfg];

    uint8_t word_size = 32 >> 1;//simon_block_sizes[cipher_cfg] >> 1;
    uint8_t word_bytes = word_size >> 3;
    uint16_t key_words =  64 / word_size; //simon_key_sizes[cipher_cfg] / word_size;
    uint64_t sub_keys[4] = {};
    uint64_t mod_mask = ULLONG_MAX >> (64 - word_size);

    int i = 0;
    // Setup
    for( i = 0; i < key_words; i++) {
        memcpy(&sub_keys[i], key + (word_bytes * i), word_bytes);
    }

    uint64_t tmp1,tmp2;
    uint64_t c = 0xFFFFFFFFFFFFFFFC;

    // Store First Key Schedule Entry
    memcpy(cipher_object->key_schedule, &sub_keys[0], word_bytes);

    for(i = 0; i < simon_rounds[0] - 1; i++){
        tmp1 = rshift_three(sub_keys[key_words - 1]);

        if(key_words == 4) {
            tmp1 ^= sub_keys[1];
        }

        tmp2  = rshift_one(tmp1);
        tmp1 ^= sub_keys[0];
        tmp1 ^= tmp2;

        tmp2 = c ^ ((z_arrays[cipher_object->z_seq] >> (i % 62)) & 1);

        tmp1 ^= tmp2;

        int j = 0;
        // Shift Sub Words
        for( j = 0; j < (key_words - 1); j++){
            sub_keys[j] = sub_keys[j+1];
        }
        sub_keys[key_words - 1] = tmp1 & mod_mask;

        // Append sub key to key schedule
        memcpy(cipher_object->key_schedule + (word_bytes * (i+1)), &sub_keys[0], word_bytes);
    }

    //return 0;
}



void SIMON::Simon_EDI(){

    // Create generic tmp variables
    uint8_t ciphertext_buffer[4];
    uint8_t simon64_32_data[4];
    uint8_t cipher_buffer[4];

    //uint8_t simon64_32_key[] = {0x00, 0x01, 0x08, 0x09, 0x10, 0x11, 0x18, 0x19}; //Pegar Chave
    //uint8_t simon64_32_plain[] = {0x77, 0x68, 0x65, 0x65};
    //uint8_t simon64_32_cipher[]= {0xBB,0xE9, 0x9B, 0xC6};

    //Get FLit
    Flit f = i_DATA.read();
    bool isHeader = false;

    if( f.data[FLIT_WIDTH-2] == 1) {
        isHeader = true;
        // Verifica se criptografa ou descriptografa
        if(f.data[23] == 1 ){
            w_TYPE.write(true);
            f.data[23] = 0; // Altera para o simon descriptografar o pacote no destino
        }else {
            w_TYPE.write(false);
        }

        // Pega Chave Correta
        // Coordenadas Origem
        xSrc = f.range(RIB_WIDTH*2-1,RIB_WIDTH*2-RIB_WIDTH/2);
        ySrc = f.range(RIB_WIDTH*2-RIB_WIDTH/2-1,RIB_WIDTH);
        // Coordenadas Destino
        xDst = f.range(RIB_WIDTH-1,RIB_WIDTH/2);
        yDst = f.range(RIB_WIDTH/2-1,0);

        // Envia
        if( COORDINATE_2D_TO_ID(xDst,yDst) == DISTRIBUTOR_KEY_POS){
            for(unsigned short i = 0; i < 8; i++) {
                cipher_buffer[i] = w_KEY[DISTRIBUTOR_KEY_POS -1][i];
            }
        }else{
            for(unsigned short i = 0; i < 8; i++) {
                cipher_buffer[i] = w_KEY[COORDINATE_2D_TO_ID(xDst,yDst) -1][i];
            }
        }

        // Recebe
        if( COORDINATE_2D_TO_ID(xSrc,ySrc) == DISTRIBUTOR_KEY_POS){
            // Guardar id do destinatario da chave (pegar do cabeçalho)
            for(unsigned short i = 0; i < 8; i++) {
                cipher_buffer[i] = w_KEY[DISTRIBUTOR_KEY_POS -1][i];
            }
        }else{
            for(unsigned short i = 0; i < 8; i++) {
                cipher_buffer[i] = w_KEY[COORDINATE_2D_TO_ID(xSrc,ySrc) -1][i];
            }
        }
        //Inicia o SIMON
        Simon_Init(&s_cipher_object, &cipher_buffer);
    }

     // Receber e armazenar chave
    if( !isHeader && COORDINATE_2D_TO_ID(xSrc,ySrc) != DISTRIBUTOR_KEY_POS) {

        // Desmenbra data do flit de 8 em 8 bits
        simon64_32_data[3]=  f.data.range(31,24);
        simon64_32_data[2]=  f.data.range(23,16);
        simon64_32_data[1]=  f.data.range(15,8);
        simon64_32_data[0]=  f.data.range(7,0);

        if(w_TYPE.read()){
            Simon_Encrypt_32(&s_cipher_object.key_schedule, &simon64_32_data, &ciphertext_buffer);
        }else{
            Simon_Decrypt_32(&s_cipher_object.key_schedule, &simon64_32_data, &ciphertext_buffer);
        }

        // Remonta data do flit
        f.data.range(31,24) = ciphertext_buffer[3];
        f.data.range(23,16) = ciphertext_buffer[2];
        f.data.range(15,8) = ciphertext_buffer[1];
        f.data.range(7,0) = ciphertext_buffer[0];

    }else if(!isHeader && COORDINATE_2D_TO_ID(xSrc,ySrc) == DISTRIBUTOR_KEY_POS){

        // Desmenbra data do flit de 8 em 8 bits
        simon64_32_data[3]=  f.data.range(31,24);
        simon64_32_data[2]=  f.data.range(23,16);
        simon64_32_data[1]=  f.data.range(15,8);
        simon64_32_data[0]=  f.data.range(7,0);

        if(w_TYPE.read()){
            Simon_Encrypt_32(&s_cipher_object.key_schedule, &simon64_32_data, &ciphertext_buffer);
        }else{
            Simon_Decrypt_32(&s_cipher_object.key_schedule, &simon64_32_data, &ciphertext_buffer);
        }

        // Remonta data do flit
        f.data.range(31,24) = ciphertext_buffer[3];
        f.data.range(23,16) = ciphertext_buffer[2];
        f.data.range(15,8) = ciphertext_buffer[1];
        f.data.range(7,0) = ciphertext_buffer[0];

        // GUARDAR Chave w_KEY[][]

    }

    o_DATA.write(f);
}

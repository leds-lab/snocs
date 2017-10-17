#ifndef SIMON_H
#define SIMON_H

#include <systemc>
#include "../SoCINDefines.h"

// Defines SIMON

#define BLOCK_SIZE 32
#define KEY_SIZE 64
#define ROUND_LIMIT 32
#define WORD_SIZE 16
#define K_SEGMENTS 4
#define Z 0

class SIMON:public sc_module
{
public:

    struct Simon_Cipher{
      uint16_t key_size;
      uint8_t block_size;
      uint8_t round_limit;
      uint8_t init_vector[16];
      uint8_t counter[16];
      uint8_t key_schedule[576];
      uint8_t z_seq;
    };

    sc_in<Flit> i_DATA_SEND;
    sc_out<Flit> o_DATA_SEND;
    sc_in<Flit> i_DATA_RECEIVE;
    sc_out<Flit> o_DATA_RECEIVE;


    sc_signal<uint8_t> w_KEY[9][8]; // Matriz para armazenar chaves
    sc_signal<bool> w_TYPE_SEND;  // Tipo de operação
    sc_signal<bool> w_TYPE_RECEIVE;  // Tipo de operação

    Simon_Cipher s_cipher_object_send;
    Simon_Cipher s_cipher_object_receive;

    //Destinatario da chave
    unsigned short destination_key_send;
    unsigned short destination_key_receive;
    unsigned short ctr_packet_key_send;
    unsigned short ctr_packet_key_receive;

    //Coordenadas de destino do pacote recebido
    sc_unsigned xSrc_send;
    sc_unsigned ySrc_send;
    sc_unsigned xSrc_receive;
    sc_unsigned ySrc_receive;

    //Coordenadas de destino do pacote recebido
    sc_unsigned yDst_send;
    sc_unsigned xDst_send;
    sc_unsigned yDst_receive;
    sc_unsigned xDst_receive;

    void Simon_Encrypt_32(const uint8_t *key_schedule, const uint8_t *plaintext, uint8_t *ciphertext);
    void Simon_Decrypt_32(const uint8_t *key_schedule, const uint8_t *ciphertext, uint8_t *plaintext);
    void Simon_Init(Simon_Cipher *cipher_object, void *key);
    void Simon_EDI_SEND();
    void Simon_EDI_RECEIVE();

    SC_HAS_PROCESS(SIMON);
    SIMON(sc_module_name nm,unsigned short int SIMON_ID);

private:
    unsigned short SIMON_ID;
};

#endif // SIMON_H

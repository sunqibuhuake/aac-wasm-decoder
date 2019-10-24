/*
 * @Author: Sunqi
 * @Date: 2019-09-09 16:08:48
 * @LastEditTime: 2019-10-24 16:11:26
 * @Description: Decoder Header File
 * @FilePath: /aac-wasm-decoder/decoder.h
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "faad2/include/neaacdec.h"

 
 // 1024 * 1024
 #define MAX_OUTPUT_BYTES  1048576
 
typedef struct
{
    NeAACDecHandle aac_handle;
    NeAACDecConfigurationPtr config;
    int config_ready;
    unsigned char data[1024 * 1024];
} AAC_ADTS_DECODER;

/**
 * @description: init decoder
 * @param {integer} input_sample_rate 
 * @param {integer} input_type 
 *                  Check all options in neaacdec.h
 * @return: AAC_ADTS_DECODER *
 */
AAC_ADTS_DECODER *aac_adts_decoder_create();


void aac_adts_decoder_free(AAC_ADTS_DECODER *);

/**
 * @description: decode input aac adts
 * @param {AAC_ADTS_DECODER *} decoder  pointer of decoder
 * @param {unsigned char *} data        pointer of input data
 * @param {size_t} data_size            input size
 * @param {unsigned char *} pcm_out     pointer of output data
 * @return: {Integer}                   -1 : fail 
 *                                      >0 : decode success
 */
int aac_adts_decoder_decode(AAC_ADTS_DECODER * decoder, unsigned char *data, size_t data_size, unsigned char *pcm_out);


int main();

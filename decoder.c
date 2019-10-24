/*
 * @Author: your name
 * @Date: 2019-10-24 13:39:42
 * @LastEditTime: 2019-10-24 16:11:16
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /aac-wasm-decoder/decoder.c
 */

#include "decoder.h"

AAC_ADTS_DECODER *aac_adts_decoder_create()
{

    AAC_ADTS_DECODER decoder;

    decoder.aac_handle = NeAACDecOpen();
    decoder.config = NeAACDecGetCurrentConfiguration(decoder.aac_handle);

    // 会影响输出时 memcpy
    // decoder.config->defSampleRate = 44100;
    // decoder.config->defObjectType = LC;

    // decoder.config->defSampleRate = input_sample_rate;
    // decoder.config->defObjectType = input_type;

    // need config
    decoder.config_ready = 0;
    NeAACDecSetConfiguration(decoder.aac_handle, decoder.config);

    AAC_ADTS_DECODER *ptr = malloc(sizeof(decoder));
    *ptr = decoder;

    return ptr;
}

int aac_adts_decoder_decode(AAC_ADTS_DECODER *decoder, unsigned char *data, size_t data_size, unsigned char *pcm_out)
{
    // 配置的必要性有待考证
    if (decoder->config_ready == 0)
    {
        size_t samplerate = 0;
        unsigned char channels = 0;
        int ret = NeAACDecInit(decoder->aac_handle, data, 4, &samplerate, &channels);

        if (ret < 0) {
            printf("Error: decoder init failed\n");
            return ret;
        }
        decoder->config_ready = 1;
    }

    NeAACDecFrameInfo frame_info;
    
    
    
    size_t cur_bytes = 0;
    size_t left_bytes = data_size;
    size_t pcm_buffer_bytes = 0;
    void * pcm_data = NULL;

    do {
        pcm_data = NeAACDecDecode(decoder->aac_handle, &frame_info, data + cur_bytes, data_size);
        if (frame_info.error > 0) {
                printf("Error: %s\n", NeAACDecGetErrorMessage(frame_info.error));
                return -1;
        } else {
            cur_bytes += frame_info.bytesconsumed;
            left_bytes -= frame_info.bytesconsumed;
            int sampleBytes = frame_info.samples << 1;


//    printf("decode one frame ok: sam:%ld, chn=%d, samplecount=%ld, obj_type=%d, header_type=%d, consumed=%ld\n",
//                               frame_info.samplerate, frame_info.channels, frame_info.samples, frame_info.object_type,
//                               frame_info.header_type, frame_info.bytesconsumed);


            // todo 修复优化，这样会造成音频丢失
            // 但是在当前场景下基本上不会出现问题
            // 因为输入是单个 ADTS
            if (pcm_buffer_bytes + sampleBytes > MAX_OUTPUT_BYTES) {
                return pcm_buffer_bytes;
            }
            memcpy(pcm_out + pcm_buffer_bytes, pcm_data, sampleBytes);
            pcm_buffer_bytes += sampleBytes;
        }
    } while(pcm_data && left_bytes > 0 && pcm_buffer_bytes < MAX_OUTPUT_BYTES);


    return pcm_buffer_bytes;
    
}

void aac_adts_decoder_free(AAC_ADTS_DECODER *decoder)
{
    free(decoder);
}

char *aac_adts_decoder_version()
{
    return "1.0.0";
}

int main()
{
    printf("=======\nAAC MPEG2 DECODER\nAuthor: suNqi\nVersion: 1.0.0\n=======\n");
    return 1;
}
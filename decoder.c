#include "decoder.h"


AdtsQueue gen_adts_queue() {
    AdtsQueue queue;
    queue.cur_index = 0;
    queue.total = 0;
    queue.last_index = QUEUE_SIZE - 1;
    return queue;
}


AAC_ADTS_DECODER *aac_adts_decoder_create() {

    AAC_ADTS_DECODER decoder;
    AdtsQueue adts_queue = gen_adts_queue();

    decoder.adts_queue = adts_queue;

    decoder.aac_handle = NeAACDecOpen();
    decoder.config = NeAACDecGetCurrentConfiguration(decoder.aac_handle);

    // 会影响输出时 memcpy
    decoder.config->defSampleRate = 44100;
    decoder.config->defObjectType = LC;
    
    // need config 
    decoder.config_ready = 0;
    NeAACDecSetConfiguration(decoder.aac_handle, decoder.config);


    AAC_ADTS_DECODER *ptr = malloc(sizeof(decoder));
    *ptr = decoder;

    return ptr;

}


// todo 处理队列
// 写入速度和解码速度有待测试，测试环境下解码速度快于写入，故队列没有实际作用
int aac_adts_decoder_enqueue(AAC_ADTS_DECODER *decoder, unsigned char *data, size_t size) {

//    for (int i = 0; i < size; i++) {
//        printf("%x", data[i]);
//    }
//    printf("\n");

    int last_index = decoder->adts_queue.last_index;
    int store_index;

    if (last_index == (QUEUE_SIZE - 1)) {
        store_index = 0;
    } else {
        store_index = last_index + 1;
    }
    decoder->adts_queue.last_index = store_index;


//    if (store_index == decoder->adts_queue.cur_index) {
//        if (decoder->adts_queue.total > 0) {
//            printf("cur_index:%d\nstore_index:%d\n", decoder->adts_queue.cur_index, store_index);
//            return 0;
//        }
//    }

    OneSegment new_adts;
    new_adts.bytes = size;
    memcpy(new_adts.data, data, size);
    decoder->adts_queue.queue[store_index] = new_adts;
    decoder->adts_queue.total += 1;


    return 1;
}


OneSegment *get_one_segment(AAC_ADTS_DECODER *decoder, int index) {
    return &decoder->adts_queue.queue[index];
}

int aac_adts_decoder_decode(AAC_ADTS_DECODER *decoder, unsigned char *pcm_out) {


    int cur_index = decoder->adts_queue.cur_index;
//    printf("read from index:%d\n", cur_index);

    // ❌
    // unsigned char *data = (unsigned char *) &decoder->adts_queue.queue[0].data;
    // int bytes = (int) decoder->adts_queue.queue[0].bytes;

    // ✅
    OneSegment *cur_seg = get_one_segment(decoder, cur_index);
    unsigned char *data = cur_seg->data;
    int bytes = cur_seg->bytes;

    //  「ADTS requires a header for each frame, not just one per file.」
    //  from : https://hydrogenaud.io/index.php/topic,52126.0.html

    // 配置的必要性有待考证
    if (!decoder->config_ready) {
        unsigned long samplerate = 0;
        unsigned char channels = 0;
        int ret = NeAACDecInit(decoder->aac_handle, data, 4, &samplerate, &channels);
        decoder->config_ready = 1;
    }

    NeAACDecFrameInfo frame_info;

    unsigned char *pcm_data = NeAACDecDecode(decoder->aac_handle, &frame_info, data, bytes);
    // 不管是否解码成功，移动指针
    decoder->adts_queue.cur_index += 1;

    if (decoder->adts_queue.cur_index == QUEUE_SIZE) {
        decoder->adts_queue.cur_index = 0;
    }

    if (frame_info.error > 0) {
//        printf("FAAD DECODE ERROR:\n%s\n", NeAACDecGetErrorMessage(frame_info.error));
        return -1;

    } else {
//        printf("解码成功！");
        int samplesBytes = frame_info.samples << 1;
//        printf("decode one frame ok: sam:%ld, chn=%d, samplecount=%ld, obj_type=%d, header_type=%d, consumed=%ld\npcm_bytes=%d\n",
//               frame_info.samplerate, frame_info.channels, frame_info.samples, frame_info.object_type,
//               frame_info.header_type, frame_info.bytesconsumed, samplesBytes);


        memcpy(pcm_out, pcm_data, samplesBytes);


        return samplesBytes;
    }
}


void aac_adts_decoder_free(AAC_ADTS_DECODER *decoder) {
    free(decoder);
}


char *aac_adts_decoder_version() {
    printf("powered by faad");
    return "aac adts decoder 0.0.1\n";
}
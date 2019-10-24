/*
 * @Author: Sunqi
 * @Date: 2019-09-10 14:51:34
 * @LastEditTime: 2019-10-24 16:10:54
 * @FilePath: /aac-wasm-decoder/js/post.js
 */


// output size: 4096/per

Module['AAC_ADTS_DECODER'] = AAC_ADTS_DECODER;


// for node
// if ('undefined' !== typeof global && exports) {
//     // var {performance} = require('perf_hooks');
//     // global.performance = performance;
// }


function AdtsDecoded({ pcm }) {
    // 采样频率暂时写死
    this.pcm = pcm;
    this.sampleRate = 44100;
}

/*
*   @param {Function} onDecode 
* */
function AAC_ADTS_DECODER(options) {
    this.options = options || {}

    if ('function' !== typeof options.onDecode)
        throw Error('onDecode callback is required.');

    // set as read-only
    Object.defineProperty(this, 'onDecode', { value: options.onDecode });
}

// 初始化完成后返回resolve
AAC_ADTS_DECODER.prototype.ready = new Promise(function (resolve, reject) {
    // queue the promise to resolve within Emscripten's init loop
    addOnPreMain(function () {
        var api = {
            malloc: cwrap('malloc', 'number', ['number']),
            free: cwrap('free', null, ['number']),
            HEAPU8: HEAPU8,
            HEAPF32: HEAPF32,
            getDecoderVersion: cwrap('aac_adts_decoder_version', 'string', []),
            // 音频赫兹  类型
            // 44100 2 (LC)
            createDecoder: cwrap('aac_adts_decoder_create', null, []),
            welcome: cwrap('main', 'Number', []),
            // decoder_ptr
            freeDecoder: cwrap('aac_adts_decoder_free', null, ['number']),
            // decoder_ptr , input_ptr, input_size, output_ptr
            // 当前返回数据为4096 字节（16bit : samplebytes * 2）
            decode: cwrap('aac_adts_decoder_decode', 'number', ['number', 'number', 'number', 'number']),
        }

        // 只读限制
        Object.freeze(api);
        Object.defineProperty(AAC_ADTS_DECODER.prototype, 'api', { value: api });

        resolve(this);
    });

    // 用 AAC_ADTS_DECODER.ready.catch() 捕捉错误
    // 重写了emscripten 的错误抛出，版本更新后检查
    var origAbort = this.abort;
    this.abort = function (what) {
        console.log('abort')
        reject(Error(what));
        origAbort.call(this, what);
    }
});


/*
* @param {Uint8Array} uint8array adts buffer
* */
AAC_ADTS_DECODER.prototype.decode = function (audio_buffer) {
    if (!(audio_buffer instanceof Uint8Array))
        throw Error('Data to decode must be Uint8Array');

    if (!this._decoderPointer) {
        // ⚠️⚠️⚠️ Tips ⚠️⚠️⚠️
        // params are not required cause decoder will detect audio format autoly
        this._decoderPointer = this.api.createDecoder();
        this.api.welcome()
    }

    var input_ptr, output_ptr, output_array;

    try {
        // 44100 Hz 单帧
        // 4096字节  2048 * 2 16bit
        // 输出bytes
        var output_pcm_bytes = 1024 * 1024;

        // 输出 指针，数组  (U8 / F32 却别待研究)
        [output_ptr, output_array] = this.createOutputArray(output_pcm_bytes);

        // 传入字节数
        var input_size = audio_buffer.byteLength;

        // console.log('SEND IN :' + sendSize + ' bytes')

        var input_bytes = input_size * audio_buffer.BYTES_PER_ELEMENT

        input_ptr = this.api.malloc(input_bytes);

        // 移动（复制？）输入数据到指针位置
        this.api.HEAPU8.set(audio_buffer, input_ptr);

        let output_bytes = this.api.decode(this._decoderPointer, input_ptr, input_bytes, output_ptr)

        if (output_bytes < 0) {
            throw Error('decode failed')
        }

        this.onDecode(new AdtsDecoded(
            { pcm: output_array.slice(0, output_bytes) }
        ));


    } catch (e) {
        throw e;
    } finally {
        this.api.free(input_ptr);
        this.api.free(output_ptr);
    }
}

AAC_ADTS_DECODER.prototype.free = function () {
    if (this._decoderPointer) {
        this.api.freeDecoder(this._decoderPointer);
    }
}

// 指针使用完毕后free
Object.defineProperty(AAC_ADTS_DECODER.prototype, 'createOutputArray', {
    value: function (length) {
        var pointer = this.api.malloc(Uint8Array.BYTES_PER_ELEMENT * length);
        var array = new Uint8Array(this.api.HEAPU8.buffer, pointer, length);
        return [pointer, array];
    }
});
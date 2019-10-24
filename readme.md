<!--
 * @Author: Sunqi
 * @Date: 2019-09-09 18:47:41
 * @LastEditTime: 2019-10-24 16:45:28
 -->
# MPEG-2 AAC ADTS DECODER

This is a web aac decoder based on [Faad2](https://sourceforge.net/projects/faac/files/faad2-src/).


I made this decoder to convert aac **mpeg-2** bitstream to pcm data, that can be played with AudioContext. 

If you need handling aac **mpeg-4** audio, you need to modify a bit of code manually.

[Difference between **AAC MPEG2** and **AAC MEPG4**](https://stackoverflow.com/questions/49189337/what-is-the-difference-between-aac-mpeg2-and-aac-mpeg4-encode-and-how-to-extra)

---


> Install  Emscripten before running the commands below.

## Compile Faad2

- unix: `./dl.sh`


## Build

- unix: `./compile.sh`


## Useage Example

```

function onDecode({pcm}) {
    // play pcm 
}

let decoder = new AAC_ADTS_DECODER({
        onDecode: onDecode
})

decoder.ready.then(() => {
    decoder.decode(aac_stream_buffer)
})
.catch(e) {
    // handle error
}

```

## TODO

- support AAC MPEG4
- and more
- 减肥



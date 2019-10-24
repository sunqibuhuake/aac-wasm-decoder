<!--
 * @Author: Sunqi
 * @Date: 2019-09-09 18:47:41
 * @LastEditTime: 2019-10-24 16:34:37
 -->
# MPEG-2 AAC ADTS DECODER

> Base on [Faad2](https://sourceforge.net/projects/faac/files/faad2-src/)


This decoder is made to decode **mpeg-2** aac bitstream to pcm data. You need to modify a bit of code in `decoder.c` manually if you wanna handle **mpeg-4** aac.

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



add_library(thirdparty INTERFACE)

target_include_directories(thirdparty INTERFACE
    ../thirdparty
)

add_library(sljit STATIC
    sljit/sljit_src/sljitLir.c
)

target_include_directories(sljit PUBLIC ../thirdparty)
target_compile_definitions(sljit PUBLIC SLJIT_HAVE_CONFIG_PRE=1 SLJIT_HAVE_CONFIG_POST=1)

add_library(libchdr STATIC
    libchdr/src/libchdr_bitstream.c
    libchdr/src/libchdr_cdrom.c
    libchdr/src/libchdr_chd.c
    libchdr/src/libchdr_flac.c
    libchdr/src/libchdr_huffman.c

    libchdr/deps/lzma-19.00/src/Alloc.c
    libchdr/deps/lzma-19.00/src/Bra86.c
    libchdr/deps/lzma-19.00/src/BraIA64.c
    libchdr/deps/lzma-19.00/src/CpuArch.c
    libchdr/deps/lzma-19.00/src/Delta.c
    libchdr/deps/lzma-19.00/src/LzFind.c
    libchdr/deps/lzma-19.00/src/Lzma86Dec.c
    libchdr/deps/lzma-19.00/src/LzmaDec.c
    libchdr/deps/lzma-19.00/src/LzmaEnc.c
    libchdr/deps/lzma-19.00/src/Sort.c

    libchdr/deps/zlib-1.2.13/adler32.c
    libchdr/deps/zlib-1.2.13/compress.c
    libchdr/deps/zlib-1.2.13/crc32.c
    libchdr/deps/zlib-1.2.13/deflate.c
    libchdr/deps/zlib-1.2.13/infback.c
    libchdr/deps/zlib-1.2.13/inffast.c
    libchdr/deps/zlib-1.2.13/inflate.c
    libchdr/deps/zlib-1.2.13/inftrees.c
    libchdr/deps/zlib-1.2.13/trees.c
    libchdr/deps/zlib-1.2.13/uncompr.c
    libchdr/deps/zlib-1.2.13/zutil.c
)

target_include_directories(libchdr
    PUBLIC ../thirdparty/libchdr/include
    PRIVATE
    ../thirdparty/libchdr/deps/lzma-19.00/include
    ../thirdparty/libchdr/deps/zlib-1.2.13
)

target_compile_definitions(libchdr PRIVATE
    _7ZIP_ST
    # instruct glibc to declare fseeko/ftello
    _LARGEFILE_SOURCE
)

add_library(tzxfile STATIC
    TZXFile/TZXAudioGenerator.cpp
    TZXFile/TZXBlock.cpp
    TZXFile/TZXBlockArchiveInfo.cpp
    TZXFile/TZXBlockCustomInfo.cpp
    TZXFile/TZXBlockGroupEnd.cpp
    TZXFile/TZXBlockGroupStart.cpp
    TZXFile/TZXBlockHardwareType.cpp
    TZXFile/TZXBlockLoopEnd.cpp
    TZXFile/TZXBlockLoopStart.cpp
    TZXFile/TZXBlockMessage.cpp
    TZXFile/TZXBlockPause.cpp
    TZXFile/TZXBlockPulseSequence.cpp
    TZXFile/TZXBlockPureData.cpp
    TZXFile/TZXBlockPureTone.cpp
    TZXFile/TZXBlockStandardSpeedData.cpp
    TZXFile/TZXBlockStopTheTape48K.cpp
    TZXFile/TZXBlockTextDescription.cpp
    TZXFile/TZXBlockTurboSpeedData.cpp
    TZXFile/TZXFile.cpp
)

target_include_directories(tzxfile PUBLIC
    ../thirdparty/TZXFile
    )

add_library(ymfm STATIC
    ymfm/src/ymfm_adpcm.cpp
    ymfm/src/ymfm_misc.cpp
    ymfm/src/ymfm_opl.cpp
    ymfm/src/ymfm_opm.cpp
    ymfm/src/ymfm_opn.cpp
    ymfm/src/ymfm_opq.cpp
    ymfm/src/ymfm_opz.cpp
    ymfm/src/ymfm_pcm.cpp
    ymfm/src/ymfm_ssg.cpp
)

target_include_directories(ymfm PUBLIC
    ../thirdparty/ymfm/src
)

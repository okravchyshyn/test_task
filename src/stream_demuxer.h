#if !defined (_stream_demuxer_h__)
#define _stream_demuxer_h__

#include <stdio.h>
#include <assert.h>
//#include "ffmpeg.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

typedef int (*demux_read_data)(void *opaque, unsigned char *buf, 
                       int buf_size);

int demux_stream(int videoIdx, int audioIdx, 
                       demux_read_data pReadData, void* socket);

#endif

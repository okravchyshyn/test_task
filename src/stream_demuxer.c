#include <stdio.h>
#include <assert.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "stream_demuxer.h"
#include "reciever.h"
#include "log.h"

#define MKV_FILE "../demo_content/sample.mkv"
#define VIDEO_BIN "video.bin"
#define AUDIO_BIN "audio.bin"
#define BUFFER_SIZE 32768


int demux_stream(int videoIdx, int audioIdx, 
       demux_read_data pReadData, void* socket ) {

  int ret;
  AVFormatContext* pFormatCtx;
  AVProbeData pd;
  AVIOContext *pb;
  AVDictionaryEntry *tag = NULL;
  RECIVER_HANDLER video_handler = NULL;
  RECIVER_HANDLER audio_handler = NULL;

  const int ioBufferSize = 32768;
  unsigned char * ioBuffer = (unsigned char *)av_malloc(ioBufferSize + FF_INPUT_BUFFER_PADDING_SIZE); // can get av_free()ed by libav

  av_register_all();
  avcodec_register_all();

  pFormatCtx = avformat_alloc_context();
  pb = avio_alloc_context(ioBuffer, ioBufferSize, 0, socket, pReadData, NULL, NULL);

  pFormatCtx->pb = pb;
  int ires = avformat_open_input(&pFormatCtx, NULL, NULL, NULL);
  if(ires < 0)
  {
    printf("Open input stream not successful %d\n",ires);
  }
  else
  {
    printf("Open input stream successfull %d\n",ires);
  }

  if(audioIdx >= pFormatCtx->nb_streams) { 
    fprintf(stderr, "Index of audio substream is bigger then number streams into container\n");
    ret = -1;
    goto fail;
  }
  if(videoIdx >= pFormatCtx->nb_streams) { 
    fprintf(stderr, "Index of video substream is bigger then number streams into container\n");
    ret = -1;
    goto fail;
  }

  DEBUG_PRINT(("create reciever \n"));
  video_handler = create_reciever(VIDEO_BIN);
  if(!video_handler) {
    fprintf(stderr, "Failed during creating video handler\n");
    ret = -1;
    goto fail;
  }
  audio_handler = create_reciever(AUDIO_BIN);
  if(!audio_handler) {
    fprintf(stderr, "Failed during creating audio handler\n");
    ret = -1;
    goto fail;
  }

  DEBUG_PRINT(("start reading file %p \n", video_handler));
  do {
      AVPacket *pkt = (AVPacket*) malloc(sizeof(AVPacket));
      av_new_packet(pkt, 0x2048);
      av_init_packet(pkt);
      ret = av_read_frame(pFormatCtx, pkt);
      if(ret < 0) {
        fprintf(stderr, "Failed read frame\n");
        av_free_packet(pkt);
        free(pkt);
        break;
      } 
    
      if( pkt->stream_index == videoIdx ) {
          ret = push_packet(video_handler, pkt);
      } else if( pkt->stream_index == audioIdx ) {
          ret = push_packet(audio_handler, pkt);
      } else {
        av_free_packet(pkt);
        free(pkt);
      }
      if( ret < 0 ) {
          fprintf(stderr, "Failed push frame\n");
          av_free_packet(pkt);
          free(pkt);
          break;
      }
 
 } while(1);


fail:

  if( video_handler ) {
     stop_reciever(video_handler);
     destroy_reciever(video_handler);
  }

  if( audio_handler ) {
     stop_reciever(audio_handler);
     destroy_reciever(audio_handler);
  }

  if(pFormatCtx) avformat_close_input(&pFormatCtx);
 // av_free(ioBuffer);

  puts("Demuxed finished work. OK");
  return ret;
}


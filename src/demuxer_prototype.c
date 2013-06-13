#include <stdio.h>
#include <assert.h>
//#include "ffmpeg.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

#define MKV_FILE "../demo_content/sample.mkv"

int read_data(void *opaque, unsigned char *buf, int buf_size) {
   FILE *f = (FILE *) opaque;
   int sz = fread(buf, 1, buf_size, f);
   printf("sizeof(%d) sz(%d) \n", buf_size, sz );

   return sz;
}  

int64_t seek_data(void *opaque, int64_t offset, int whence)
{
  printf("SEEK DATA\n");
  return -1;
}

static void display_stream_attributes(AVStream *stream) {
     int idx = stream->index;
     int id = stream->id;
     AVCodecContext *codec = stream->codec;
     assert(codec);
     printf("stream idx(%d) codecId(%d) %s -> %s \n",
       idx, id,
       codec->codec_type == AVMEDIA_TYPE_VIDEO ? "video" : "audio",
       avcodec_get_name(id)
      );

//AVMEDIA_TYPE_VIDEO,AVMEDIA_TYPE_AUDIO, 
}

static void display_file_attributes(AVFormatContext* ctx) {
  int ret;
  int i;
  AVDictionaryEntry *tag = NULL;
  for(i = 0; i < ctx->nb_streams; i++ )
  {
      /* find decoder for the stream */
        AVStream *st = ctx->streams[i];

        while ((tag = av_dict_get(st->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
           printf("%s=%s\n", tag->key, tag->value);

        AVCodecContext *dec_ctx = st->codec;
        AVCodec *dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(dec_ctx->codec_type));
            return;
        }

        if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
            fprintf(stderr, "Failed to open %s codec\n",
                    av_get_media_type_string(dec_ctx->codec_type));
            return;
        }
        display_stream_attributes(ctx->streams[i]);
  }
}


int main() {

  AVFormatContext* pFormatCtx;
  AVProbeData pd;
  AVIOContext *pb;
  AVDictionaryEntry *tag = NULL;

  const int ioBufferSize = 32768;
  unsigned char * ioBuffer = (unsigned char *)av_malloc(ioBufferSize + FF_INPUT_BUFFER_PADDING_SIZE); // can get av_free()ed by libav

  av_register_all();
  avcodec_register_all();

  FILE *f = fopen(MKV_FILE, "rb");
  if( !f ) {
	perror ("Error opening file");
 	exit(-1);
  } 

  pFormatCtx = avformat_alloc_context();
  pb = avio_alloc_context(ioBuffer, ioBufferSize, 0, f, read_data, NULL, NULL);


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

  if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
  }

  while ((tag = av_dict_get(pFormatCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
        printf("%s=%s\n", tag->key, tag->value);

  display_file_attributes(pFormatCtx);


  AVPacket pkt;
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;
  while (av_read_frame(pFormatCtx, &pkt) >= 0) {
        //decode_packet(&got_frame, 0);
        av_free_packet(&pkt);
  }


  fclose(f);
  puts("OK");
  return 0;
}


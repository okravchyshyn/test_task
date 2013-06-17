#include <stdio.h>
#include <assert.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"



/* The name of this program. */
static const char* program_name;
/* Prints usage information for this program to STREAM (typically
stdout or stderr), and exit the program with EXIT_CODE. Does not
return. */
static void print_usage (int exit_code) {
  printf( "Usage: %s [inputfile]\n" , program_name);
  exit (exit_code);
}


int main(int argc, char* args[]) {
  program_name = args[0];
  if( argc != 2 ) print_usage(-1);
   
  AVFormatContext* pFormatCtx = NULL;
  AVDictionaryEntry *tag = NULL;

  av_register_all();

  const char* input_file = args[1];

  int ires = avformat_open_input(&pFormatCtx, input_file, NULL, NULL);
  if(ires < 0 ) {
       printf("Failed to open %s file\n", input_file );
  }

  //while ((tag = av_dict_get(pFormatCtx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
  //      printf("%s=%s\n", tag->key, tag->value);

  int i;
  for(i = 0; i < pFormatCtx->nb_streams; i++ )
  {
      /* find decoder for the stream */
        AVStream *st = pFormatCtx->streams[i];
        
        AVCodecContext *dec_ctx = st->codec;
        AVCodec *dec = avcodec_find_decoder(dec_ctx->codec_id);
        if (!dec) {
            fprintf(stderr, "Failed to find %s codec\n",
                    av_get_media_type_string(dec_ctx->codec_type));
           exit(-1);
        }

        printf("stream idx(%d) codecId(%d) %s -> %s \n",
           st->index, st->codec->codec_id,
           dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO ? "video" : "audio",
           avcodec_get_name(st->codec->codec_id)
        );
  
        while ((tag = av_dict_get(st->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
           printf("%s=%s\n", tag->key, tag->value);
       
        printf("\n");
   }

   return 0;
}


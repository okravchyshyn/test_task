#if !defined (_reciever_h__)
#define _reciever_h__

#include <stdio.h>
#include <assert.h>
//#include "ffmpeg.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

typedef  void* RECIVER_HANDLER;

RECIVER_HANDLER create_reciever(const char* dest_file_name);
int start_reciever( RECIVER_HANDLER hrcv);
int stop_reciever( RECIVER_HANDLER hrcv);
int push_packet( RECIVER_HANDLER hrcv, AVPacket* packet);  
void destroy_reciever( RECIVER_HANDLER hrcv);

#endif


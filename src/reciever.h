#if defined (_reciever_h__)
#define _reciever_h__

#include <stdio.h>
#include <assert.h>
//#include "ffmpeg.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

typedef struct reciever* ptr_reciever;

ptr_reciever create_reciever(const char* dest_file_name);
int start_reciever(ptr_reciever rcv);
int stop_reciever(ptr_reciever rcv);
int push_packet(ptr_reciever rcv, AVPacket* packet);  


#endif

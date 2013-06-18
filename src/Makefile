CFLAGS = -g -DDEBUG

#SRCS=$(wildcard *.c)
SRCS=fifo.c reciever.c stream_demuxer.c
OBJS=$(SRCS:.c=.o)
SPATH=../3rparty/ffmpeg/
LIBS=-L $(SPATH)libavformat -lavformat -L $(SPATH)libavcodec -lavcodec
LIBS+= -L $(SPATH)libavutil -lavutil -lpthread -lm -lz
LIBS+= -L $(SPATH)swscale -lswscale 
LIBS+= -L $(SPATH)avdevice -lavdevice
LIBS+= -L $(SPATH)avfilter -lavfilter
#LIBS+= -L $(SPATH)avresample -lavresample
LIBS+= -L $(SPATH)libswresample -lswresample


STREAM_INFO_OBJS=$(OBJS) stream_info.o 
DEMUXER_PROTOTYPE_OBJS=$(OBJS) demuxer_prototype.o 
MINI_STREAMER_OBJS=$(OBJS) mini_streamer.o 
MKV_CLIENT_OBJS=$(OBJS) mkv_client.o 

TARGETS= stream_info demuxer_prototype mini_streamer mkv_client 


all:$(TARGETS) 
stream_info: $(STREAM_INFO_OBJS)
	gcc $(CFLAGS) -o stream_info $(STREAM_INFO_OBJS) $(LIBS)

demuxer_prototype: $(DEMUXER_PROTOTYPE_OBJS)
	gcc $(CFLAGS) -o demuxer_prototype $(DEMUXER_PROTOTYPE_OBJS) $(LIBS)

mini_streamer: $(MINI_STREAMER_OBJS)
	gcc $(CFLAGS) -o mini_streamer $(MINI_STREAMER_OBJS) $(LIBS)

mkv_client: $(MKV_CLIENT_OBJS)
	gcc $(CFLAGS) -o mkv_client $(MKV_CLIENT_OBJS) $(LIBS)

%.o : %.c
	echo "SRCS" $(SRCS)
	gcc -c $(CFLAGS) -I $(SPATH) $< -o $@


clean:
	-rm *.o $(TARGETS) 

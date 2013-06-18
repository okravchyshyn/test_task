#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "log.h"

#define DEFAULT_PORT 1234

static int video_idx = -1;
static int audio_idx = -1;
static char ip_address[INET_ADDRSTRLEN];
static int port = DEFAULT_PORT;

struct sockaddr_in antelope;
static const char* program_name;


static void print_usage (FILE* stream, int exit_code)
{
  fprintf (stream, "Usage: %s [options mandatory] \n", program_name);
  fprintf (stream,
         " -h --help Display this usage information.\n"
         " -p --port number Port for streaming. Default is %d.\n"
         " -v --video number. Index of video stream.\n"
         " -a --audio number. Index of audio stream.\n"
         " -i --ip string.    Pere IP address.\n"
      , DEFAULT_PORT);
  exit (exit_code);
}


static int parse_input_parameters(int argc, char* args[]) {

  int err_happened = 0;
  if(argc < 2) print_usage(stderr, -1);

  static struct option long_options[] = {
      { "help",  0, NULL, 'h' },
      { "port",  1, NULL, 'p' },
      { "video", 1, NULL, 'v' },
      { "audio", 1, NULL, 'a' },
      { "ip",    1, NULL, 'i' },
      {  NULL,   0, NULL,  0  } 
  };
 
  const char* short_options = "hp:a:v:i:";
 
  int next_option;
  do {
     next_option = getopt_long (argc, args, short_options,
              long_options, NULL);
     switch (next_option)
     {
        case 'h': 
          print_usage (stdout, 0);
          break;
        case 'p': /*port*/
          port = atoi(optarg);
          break;
        case 'a': 
          audio_idx = atoi(optarg);
          break;
        case 'v': 
          video_idx = atoi(optarg);
          break;
        case 'i': 
          strncpy( ip_address, optarg, sizeof(ip_address)-1);
          ip_address[sizeof(ip_address)-1] = 0;
          break;
        case -1: /* Done with options. */
          break;
     }
  } while (next_option != -1);

  if(video_idx < 0) {
    fprintf(stderr , "Index of video stream is not specified\n");
    err_happened = 1;
  }
   
  if(audio_idx < 0) {
    fprintf(stderr , "Index of audio stream is not specified\n");
    err_happened = 1;
  }

  if(ip_address[0] == 0 ) {
    fprintf(stderr , "IP address of peer is not specified\n");
    err_happened = 1;
  }

  if(err_happened) { 
     exit(-1);
  }

  return 0;
}

static int socket_fd;
static int conn_fd;

#define BUFFER_SIZE 0x1000
unsigned char buffer[BUFFER_SIZE];
static FILE* input_file;

int stream() {
  
   while(1) {
      size_t sz = read(socket_fd, buffer, BUFFER_SIZE);
      DEBUG_PRINT(("read from socket %d bytes \n", (int)sz));
      if(sz == 0) break;
      if(sz == -1) return -1;
   }
   
   return 0; 
}

int read_data(void *opaque, unsigned char *buf, int buf_size) {
   int socket_fd  = *((int *) opaque);
   int sz = read( socket_fd, buf, buf_size);
   DEBUG_PRINT(("read from socket %d bytes \n", (int)sz));
   
   return sz;                                                              }  


int main(int argc, char* args[]) {
 
  program_name = args[0];

  if( parse_input_parameters(argc, args) == 0 )
  {
      printf("IP address (%s:%d)\n"
          "video_idx(%d)\n"
          "audio_idx(%d)\n" ,
          ip_address, port, video_idx, audio_idx);
  }
 
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) 
  {
        perror("ERROR opening socket");
        exit(1);
  }
 
  struct sockaddr_in name = {0};
  inet_aton(ip_address, &name.sin_addr);
  name.sin_port = htons (port);
  name.sin_family = AF_INET;
/* Connect to the Web server */
  if (connect (socket_fd, &name, sizeof (struct sockaddr_in)) == -1) {
      perror ("connect");
      return -1;
  }
  
  demux_stream(video_idx, audio_idx, read_data, &socket_fd);
  //close(socket_fd);
  //fclose(input_file);

  return 0;
}


void cntr_c_handler( sig_t s){
  printf("Close socket\n");
  close(socket_fd);
  fclose(input_file);
  exit(1); 
}


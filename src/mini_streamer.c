#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "log.h"

#define DEFAULT_PORT 1234

static const char* program_name;
static const char* file_for_streaming;
static int port = DEFAULT_PORT;



static void print_usage (FILE* stream, int exit_code)
{
  fprintf (stream, "Usage: %s options [ inputfile ... ]\n", program_name);
  fprintf (stream,
         " -h --help Display this usage information.\n"
         " -p --port number Port for streaming. Default is %d.\n", 
            DEFAULT_PORT);
  exit (exit_code);
}

static const char* find_file_for_streaming(int argc, char* args[]) {
   int i;
   for( i = argc - 1; i > 0 ; i-- ) {
      if( args[i][0] != '-' ) /*assume that the last parameter without mines is file */
        return args[i];
   }

   return NULL;
}

static int parse_input_parameters(int argc, char* args[]) {
    
  if(argc < 2) print_usage(stderr, -1);

  file_for_streaming = find_file_for_streaming(argc, args);
  if(file_for_streaming == NULL) print_usage(stderr, -1);

  static struct option long_options[] = {
      { "help", 0, NULL, 'h' },
      { "port", 1, NULL, 'o' },
      {  NULL,  0, NULL,  0  } 
  };
 
  const char* short_options = "hp:";
 
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
        case -1: /* Done with options. */
          break;
     }
  } while (next_option != -1);

  return 0;
}

static int socket_fd;
static int conn_fd;

#define BUFFER_SIZE 0x1000
unsigned char buffer[BUFFER_SIZE];
static FILE* input_file;

int stream(int conn_fd) {
 
   while( 1 ) {
   
      int read_bytes = fread(buffer, 1, BUFFER_SIZE, input_file);  
      DEBUG_PRINT(("read %d bytes from file\n", read_bytes));
      if(read_bytes <= 0) break;   
 
      int sz = write(conn_fd, buffer, read_bytes);
      DEBUG_PRINT(("write to socket %d bytes \n", sz));
      if( sz == -1 )
      {
          perror ("The following error occurred");
          return -1;
      }
   }
   
   return 0; 
}


int main(int argc, char* args[]) {

  if( parse_input_parameters(argc, args) == 0 )
  {
      printf("File for streaming (%s)\nPort(%d)\n",
          file_for_streaming, port);
  }

  input_file = fopen(file_for_streaming, "rb");
  if(input_file == NULL) {
     fprintf(stderr, "Invalid path to %s file \n", file_for_streaming);
     exit(-1);
  }

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr; 
  memset(&serv_addr, '0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port); 
  bind(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
  listen(socket_fd, 1); 

  printf("Server is listening. Use Cntr-C for exit\n");
  void cntr_c_handler( sig_t s);
  signal (SIGINT,cntr_c_handler);
 
  while(1)
  {
        conn_fd = accept(socket_fd, (struct sockaddr*)NULL, NULL); 
        int ret = stream(conn_fd);
        if(ret == 0 ) {
           printf("file was transfered\n");
        }
        close(conn_fd);
        fseek(input_file,SEEK_SET, 0);
  }

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


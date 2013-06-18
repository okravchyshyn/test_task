#include "reciever.h"
#include <pthread.h>

#include "fifo.h"
#include "log.h"


typedef struct {
  FILE *dest_file;

  int abort_request;

  pthread_t thread; 
  pthread_mutex_t  mutex;
  pthread_cond_t cond;

  fifo_t *queue;

} reciever;

static void* routine(void *ctx) {

  reciever *rcv = (reciever*) ctx;
  if( !rcv ) return NULL;

  DEBUG_PRINT(("Enter into routine\n")); 
  while(!rcv->abort_request) {
      pthread_mutex_lock(&rcv->mutex);

      if(fifo_empty(rcv->queue))
          pthread_cond_wait(&rcv->cond, &rcv->mutex);
    
      if(rcv->abort_request) { 
           pthread_mutex_unlock(&rcv->mutex);
           break;
      }


      AVPacket *pkt = (AVPacket*) fifo_remove(rcv->queue);

      DEBUG_PRINT(("Wake up  packet(%p) data(%p) size(%d) \n", 
              pkt, pkt->data, pkt->size));

      fwrite(pkt->data, pkt->size, 1, rcv->dest_file);
      av_free_packet(pkt);
      free(pkt);

      pthread_mutex_unlock(&rcv->mutex);
  }
  DEBUG_PRINT(("Leave routine\n")); 
}

static void destroy_all_reciever_objects(RECIVER_HANDLER hrcv) {
   
  reciever* rcv = (reciever*) hrcv;  
  if( !rcv ) return NULL;
  
  DEBUG_PRINT(("Destroy all reciever %p\n", rcv));
  pthread_cond_destroy(&(rcv->cond));
  pthread_mutex_destroy(&(rcv->mutex));

  fifo_free(rcv->queue, NULL);

  fclose(rcv->dest_file);

  free(rcv);
}

RECIVER_HANDLER create_reciever(const char* dest_file_name) {
   int res;

   reciever *r = (reciever*) malloc(sizeof(reciever));
   memset(r, 0, sizeof(reciever));

   r->queue = fifo_new();
   r->abort_request = 0;

   r->dest_file = fopen(dest_file_name, "wb");
   
   res = pthread_create (&r->thread, NULL, routine, r);
   if(res != 0 ) goto fail;  

   res = pthread_mutex_init(&(r->mutex), NULL);
   if(res != 0 ) goto fail;  

   res = pthread_cond_init(&(r->cond), NULL); 
   if(res != 0 ) goto fail;  
 
   return r;
fail:
   
   destroy_all_reciever_objects(r);

   return NULL;
}

void destroy_reciever(RECIVER_HANDLER hrcv) {
   DEBUG_PRINT(("Destroy reciever \n"));
   reciever* rcv = (reciever*) hrcv;  
   if( !rcv ) return;

   destroy_all_reciever_objects(rcv);
}

int push_packet( RECIVER_HANDLER hrcv, AVPacket* packet) {

    reciever* rcv = (reciever*) hrcv;  
    if( !rcv ) return -1;

    pthread_mutex_lock(&rcv->mutex);

    DEBUG_PRINT(("Push Packet packet(%p) data(%p) size(%d) \n", packet, packet->data, packet->size));
    fifo_add(rcv->queue, packet);

    pthread_cond_signal(&rcv->cond);
    pthread_mutex_unlock(&rcv->mutex);
}

int start_reciever(RECIVER_HANDLER rcv) {


    return 0;
}

int stop_reciever(RECIVER_HANDLER hrcv) {
    DEBUG_PRINT(("Stop reciever \n"));
    reciever* rcv = (reciever*) hrcv;  
    if( !rcv ) return -1;
    
    pthread_mutex_lock(&rcv->mutex);
    rcv->abort_request = 1;
    pthread_cond_signal(&(rcv->cond));
    pthread_mutex_unlock(&rcv->mutex);

    pthread_join (rcv->thread, NULL);
    //close(rcv->dest_file);
    return 0;
}


//ptr_reciever create_reciever(const char* dest_file_name);
//int start_reciever(ptr_reciever rcv);
//int stop_reciever(ptr_reciever rcv);
//int push_packet(ptr_reciever rcv, AVPacket* packet);  
//void destroy_reciever(ptr_reciever rcv);


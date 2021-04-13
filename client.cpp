
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <cstdio>
#include <list>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include <sys/mman.h>

#define SEM_ID	1234   
#define SHM_ID	2345     
#define PERMS	0777      


#define MSG_TYPE_EMPTY  0 
#define MSG_TYPE_STRING 1        
#define MSG_TYPE_FINISH 2 
#define MSG_TYPE_START 3 
                          
#define MAX_STRING 120

typedef struct
{
  int type;
  char string [MAX_STRING];
} message_t;

void sys_error (const char *msg)
{
  puts (msg);
  exit (1);
}

using namespace std;
int main (int argc, char** argv){
    int shmid = 0;
    int semid;
    message_t* msg;

    if((semid = semget(SEM_ID, 1,PERMS | IPC_CREAT)) < 0){
        sys_error("semget()");
    }
      if((shmid = shmget(SHM_ID, sizeof(MAX_STRING),PERMS | IPC_CREAT)) < 0){
        sys_error("shmget()");
    }
    if((msg = (message_t*)shmat(shmid, 0, 0)) == NULL){
        sys_error("shmat()");
    }

    msg->type = MSG_TYPE_EMPTY;
    if (semctl(semid, 0, SETVAL, 0) < 0) {
      perror("semctl");
      cout << errno << endl;
      sys_error("Client: comand SETVAL not fulfilled");
    }

    cout << "\nSHMID " << shmid << "\nSEMID " << semid << endl;

    while (true)
    {
        if(semctl(semid, 0 ,GETVAL, 0) || msg->type != MSG_TYPE_EMPTY){
          if (semctl(semid, 0, SETVAL, 1) < 0) {
                sys_error("Client: comand SETVAL not fulfilled.");
            }
          if(msg->type == MSG_TYPE_STRING){
            cout << "\nRESULT STRING: ";
            printf("%s %c", msg->string, '\n');
          }
          if(msg->type == MSG_TYPE_FINISH){
            break;
          }
          msg->type = MSG_TYPE_EMPTY;
          semctl(semid, 0, SETVAL, 0);
        }
    }
    shmdt(msg);
    return 0;
}
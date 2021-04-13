
#include <cstring>

#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <cstdio>
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
} message;

void sys_error (const char *msg)
{
  puts (msg);
  exit (1);
}

using namespace std;

int main(int argc, char** argv){
    char line[MAX_STRING];
    char buffer[256];
    int shmid = 0;
    int semid;
    message* msg = nullptr;

    if((semid = semget(SEM_ID, 1, PERMS | IPC_CREAT)) < 0){
        perror("semget");
        cout << errno << endl;
        sys_error("semget()");
    }
    if((shmid = shmget(SHM_ID, sizeof(MAX_STRING), PERMS | IPC_CREAT)) < 0){
        sys_error("shmget()");
    }
    if((msg = (message*)shmat(shmid, 0, 0)) == NULL){
        sys_error("shmat()");
    }

    cout << "\nSHMID " << shmid << "\nSEMID " << semid << endl;
    char flg;
    msg->type = MSG_TYPE_START;

    while (true)
    {
        while (semctl(semid, 0, GETVAL, 0)){};
        while (msg->type != MSG_TYPE_EMPTY){};
        semctl(semid, 0, SETVAL, 1);
        cout << "S: send string | Q: exit\n";
        cin >> flg;
        switch (flg)
        {
        case('s'):
            cin.clear();
            fgets(buffer, sizeof(buffer) -1, stdin);
            cout << "SERVER ...\n" << "Entee the string" << endl;
            cin >> line ;
            fflush(stdin);
            strcpy(msg->string, line);
            msg->type = MSG_TYPE_STRING;
            semctl(semid, 0, SETVAL, 0);
            break;
        case('q'):
            cout << "Exit" << endl;
            if (semctl(semid, 0, IPC_RMID, 0) < 0) {
            sys_error("Server: Semaphore not delete.");
            }
            shmdt(msg);
            if (shmctl(shmid, IPC_RMID, 0) < 0) {
            sys_error("Server: Segment not delete.");
            }
            msg->type == MSG_TYPE_FINISH;
            semctl(semid, 0, SETVAL, 0);
            cout << "SERVER WAS DELETE" << endl;
            return 0;
            break;
        default:
        cout << "Default" << endl;
        msg->type = MSG_TYPE_EMPTY;
        cin.clear();
        cout << "q or s" <<  endl;
        fgets(buffer, sizeof(buffer) -1, stdin);
        break;
        } 
        semctl(semid, 0 ,SETVAL ,0);  
    }
    return 0;
}
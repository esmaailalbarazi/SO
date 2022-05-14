//? @authors: Esmaail Albarazi
//? compile with: gcc -Wall -pthread projeto.c -o projeto
//! Olá Daviiiiiii <3 <3 <3 <3 <3 <3 <3 <3 <3 <3 <3 <3 <3 <3 <3 <3 <3 <3

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#define DEBUG        // remove this line to remove debug messages
#define MAX_CHAR 100 // maximum

typedef struct Tarefa
{
    int id;
    int prioridade; // 1-3
} Tarefa;

typedef struct MobileNode
{
    int id;
    // lista de tarefas ?? //TODO

} MobileNode;

typedef struct vCPU
{               // Struct vCPU para gerir as terfas
    bool state; // TRUE -> ACTIVE ,  FALSE -> NOT ACTIVE
    // Esse estado vai determinar  o nível de performance do Edge Server ..
    int PROCESSING_CAPACITY;
    pthread_t vCPU_thread; // vCPU Thread
} vCPU;

typedef struct EdgeServer
{
    char SERVER_NAME[50];
    vCPU vCPUs[2]; // cada Edge server tem dois vCPUs ,

} EdgeServer;

typedef struct // Struct para a memoria partilhada
{
    // Code to add here ...
    EdgeServer *edgeServer_list; // lista de edge servers .. versão /SHM/
    int working_servers_num;     // numero de Edge servers a funcionar
} mem_structure;

EdgeServer *edgeServer_list; // lista de edge servers para uso estático ..

// Static configuration variables
int QUEUE_POS, MAX_WAIT, EDGE_SERVER_NUMBER;

// Shared Memory
int shmid;
mem_structure *statistic;

// Processes
pid_t Task_Manager, Monitor, Maintenance_Manager;

// Semaphores
sem_t *LOG_SEM, *STAT_SEM;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex

void *vCPU_thread(void *p)
{
    int c_id = *((int *)p);
    pthread_t id = pthread_self();

#ifdef DEBUG
    sleep(10);
    printf("Thread %ld: vCPU %d starting..\n", id, c_id);
#endif

    pthread_mutex_lock(&mutex);
    // Do somthing here ...
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void edge_server_manager(EdgeServer es)
{
    pthread_t threads_vCPU[2];

#ifdef DEBUG
    printf("Processo EdgeServer #%s# a correr\n", es.SERVER_NAME);
#endif

    // Criação das threads que simulam os vCPUs
    for (int i = 0; i < 2; i++) // Percorremos a lista de threads
    {
        //es.vCPUs.
    }
}

void task_manager()
{
    pid_t edge_servers[EDGE_SERVER_NUMBER];

    // Criar os	processos Edge Server
    for (int i = 0; i < EDGE_SERVER_NUMBER; ++i)
    {
        // Criação do processo Monitor
        if ((edge_servers[i] = fork()) == 0)
        {
            sem_wait(STAT_SEM);
            edge_server_manager(statistic->edgeServer_list[i]);
            sem_post(STAT_SEM);
            exit(0);
        }
    }

    // TODO Criação da thread scheduler e gestão	do escalonamento das tarefas
}

void log_output(char *txt)
{
    FILE *f;

    // Get current time
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sem_wait(LOG_SEM);

    f = fopen("log.txt", "a");

    fprintf(f, "%d:%d:%d %s\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, txt);
    printf("%d:%d:%d %s\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, txt);

    fclose(f);

    sem_post(LOG_SEM);
}

void monitor()
{
}

void maintenance_manager()
{
}

void print_variables()
{
    printf("QUEUE_POS:%d\nMAX_WAIT:%d Sec\nEDGE_SERVER_NUMBER:%d\n", QUEUE_POS, MAX_WAIT, EDGE_SERVER_NUMBER);
    for (int i = 0; i < EDGE_SERVER_NUMBER; ++i)
        printf("\nSERVER #%d: \nSERVER NAME: %s\nvCPU 1 PROCESSING CAPACITY: %d\nvCPU 2 PROCESSING CAPACITY:%d", i + 1, edgeServer_list[i].SERVER_NAME, edgeServer_list[i].vCPUs[0].PROCESSING_CAPACITY,edgeServer_list[i].vCPUs[1].PROCESSING_CAPACITY);
}

void load_config()
{
    FILE *f;
    char name[100];
    f = fopen("config.txt", "r");

    fscanf(f, "%d", &QUEUE_POS);          // QUEUE_POS - número de slots na fila interna do Task Manager
    fscanf(f, "%d", &MAX_WAIT);           // MAX_WAIT - tempo máximo para que o processo Monitor eleve o nível de performance dos Edge Servers (em segundos)
    fscanf(f, "%d", &EDGE_SERVER_NUMBER); // EDGE_SERVER_NUMBER - número de edge servers (>=2)

    edgeServer_list = (EdgeServer *)malloc(sizeof(EdgeServer) * EDGE_SERVER_NUMBER); // alocação de memória para a lista de edge servers

    fgets(name, 100, f); // dá Segmention fault sem o fgets() aqui ..

    for (int i = 0; i < EDGE_SERVER_NUMBER; ++i)
    {
        fgets(name, 100, f);
        // Returns first token
        // edgeServer_list[i].SERVER_NAME = strtok(name, ",");

        strcpy(edgeServer_list[i].SERVER_NAME, strtok(name, ","));

        if (edgeServer_list[i].SERVER_NAME != NULL)
        {
            edgeServer_list[i].vCPUs[0].state = false;
            edgeServer_list[i].vCPUs[0].PROCESSING_CAPACITY = atoi(strtok(NULL, ","));

            edgeServer_list[i].vCPUs[1].state = false;
            edgeServer_list[i].vCPUs[1].PROCESSING_CAPACITY = atoi(strtok(NULL, ","));
        }
    }

    fclose(f);
}

int main(void)
{ // System Manager

    // Leitura do ficheiro de configurações
    load_config();

    print_variables(); // View config Variables
/*
    // Validação dos dados
    if (EDGE_SERVER_NUMBER < 2)
    {
        log_output("MINIMUM NUMBER REQUIRED OF EDGE SERVERS INEXICITENT! ");
        sem_unlink("LOG_SEM");
        exit(0);
    }

    // Criação da memória partilhada
    shmid = shmget(IPC_PRIVATE, sizeof(mem_structure), IPC_CREAT | 0700); // Create shared memory
    statistic = (mem_structure *)shmat(shmid, NULL, 0);                   // Attach shared memory

    statistic->edgeServer_list = edgeServer_list;

    // TODO Criação do named pipe

    // Criação do processo Task	Manager
    if ((Task_Manager = fork()) == 0) // child process
    {
        task_manager();
        exit(0);
    }

    // Criação do processo Monitor
    if ((Monitor = fork()) == 0)
    {
        monitor();
        exit(0);
    }

    // Criação do processo Maintenance Manager
    if ((Maintenance_Manager = fork()) == 0) // child process
    {
        maintenance_manager();
        exit(0);
    }

    while (1)
        ;

        // Create semaphore for log_output
        sem_unlink("LOG_SEM");
        LOG_SEM = sem_open("LOG_SEM", O_CREAT | O_EXCL, 0700, 1);
        sem_unlink("STAT_SEM");
        STAT_SEM = sem_open("STAT_SEM", O_CREAT | O_EXCL, 0700, 1);

    log_output("OFFLOAD SIMULATOR STARTING");

    // remove resources (shared memory and semaphore)
    // semaphore
    sem_close(LOG_SEM);    // fecha o semaforo
    sem_unlink("LOG_SEM"); // apaga o semaforo
    sem_close(STAT_SEM);
    sem_unlink("STAT_SEM");
    */

    // shared memory
    shmctl(shmid, IPC_RMID, NULL);

    log_output("SIMULATOR CLOSING");

    exit(0);
}

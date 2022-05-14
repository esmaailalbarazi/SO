//? @authors: Esmaail Albarazi
// Mobile Node ...
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

pid_t Mobile_Node;

void mobileNode()
{
}

int main(void)
{
    // TODO leitura da linha de comando

    // TODO Verifica se o input é válido

    // Criação do processo mobile node
    if ((Mobile_Node = fork()) == 0) // child process
    {
        mobileNode();
    }
}
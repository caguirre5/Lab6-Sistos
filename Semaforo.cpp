// UNIVERSIDAD DEL VALLE DE GUATEMALA
// SISTEMAS OPERATIVOS
// CATEDRATICO: JUAN LUIS GARCIA

// CRISTIAN EDUARDO AGUIRRE DUARTE     20231


#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

#define IDC 100
#define ITERS 10
#define THREADS 10

using namespace std;

sem_t semaphore, write_to_file_sem;
ofstream output_file;

void write_to_file(string log) {
    sem_wait(&write_to_file_sem);
    output_file.open("BitacoraSemaforo.txt", ios::app | ios::out);
    if (output_file.is_open()) {
        output_file << log << endl;
        output_file.close();
    }
    sem_post(&write_to_file_sem);
}

void* consume(void* arg) {
    int id = *((int *) arg);
    free(arg);
    string log;
    for (int i = 0; i < ITERS; i++) {
        sem_wait(&semaphore);
        log =  "Iniciando iteracion " + to_string(i);
        write_to_file(log);
        log = to_string(id) + " - Semaforo abierto con exito.";
        write_to_file(log);
        float sleep_time = (rand() % 10) * 0.1;
        sleep(sleep_time);
        log =  to_string(id) + " - Buenos dias! Recurso usado";
        write_to_file(log);
        sem_post(&semaphore);
    }
    pthread_exit(NULL);
}


int main() {
    sem_init(&semaphore, 0, IDC);
    sem_init(&write_to_file_sem, 0, 1);
    output_file.open("BitacoraSemaforo.txt", ios::out);
    output_file << "";
    output_file.close();
    pthread_t threads[THREADS];
    // Creacion de threads.
    for (int i = 0; i < THREADS; i++) {
        string log = "Iniciando thread " + to_string(i);
        write_to_file(log);
        int* index = (int *) malloc(sizeof(int));
        *index = i;
        pthread_create(&threads[i], NULL, &consume, (void *) index);
    }
    for(int i = 0; i < THREADS;i++) {
        pthread_join(threads[i], NULL);
    }
    sem_destroy(&semaphore);
    sem_destroy(&write_to_file_sem);
}
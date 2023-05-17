// UNIVERSIDAD DEL VALLE DE GUATEMALA
// SISTEMAS OPERATIVOS
// CATEDRATICO: JUAN LUIS GARCIA

// CRISTIAN EDUARDO AGUIRRE DUARTE     20231

#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <semaphore.h>
#include <unistd.h>

#define CANT_ID 100
#define ITERS 10
#define THREADS 10

using namespace std;

ofstream output_file;

int available_resources = CANT_ID;

sem_t write_to_file_sem;

pthread_cond_t condition;
pthread_mutex_t mutex;


// Función que permite escribir registros en el archivo.

void write_to_file(string log) {
    sem_wait(&write_to_file_sem);// Espera a adquirir el semáforo de escritura
    output_file.open("BitacoraMonitor.txt", ios::app | ios::out);
    if (output_file.is_open()) {
        output_file << log << endl;
        output_file.close();
    }
    sem_post(&write_to_file_sem);// Libera el semáforo de escritura
}


// Función de consumo realizada por cada hilo
int decrease_count(int count) {
    string log;
    log = "Iniciando increase_count";
    write_to_file(log);
    pthread_mutex_lock(&mutex);
    if (available_resources < count) {
        log = "Se necesitan " + to_string(count) + " recursos pero solo nos rolaron " + to_string(available_resources);
        write_to_file(log);
    }
    while (available_resources < count) {
        pthread_cond_wait(&condition, &mutex);
    }

    available_resources -= count;
    pthread_mutex_unlock(&mutex);
    log = "Terminando increase_count";
    write_to_file(log);
    return 0;
}

int increase_count(int count) {
    pthread_mutex_lock(&mutex);
    available_resources += count;
    pthread_cond_broadcast(&condition);
    pthread_mutex_unlock(&mutex);
    return 0;
}

void* consume(void* arg) {
    int id = *((int *) arg);
    free(arg);
    string log;
    for (int i = 0; i < ITERS; i++) {
        log = "Iniciando iteracion " + to_string(i);
        write_to_file(log);
        int resources = rand() % CANT_ID;
        log = to_string(id) + " - Se consumiran" + to_string(resources) + " recursos.";
        write_to_file(log);
        decrease_count(resources);
        log = to_string(id) + " - Recurso tomado.";
        write_to_file(log);
        float sleep_time = (rand() % 10) * 0.1;
        sleep(sleep_time);
        increase_count(resources);
        log = to_string(id) + " - BUenos dias! Recurso usado";
        write_to_file(log);
    }
    pthread_exit(NULL);
}


int main() {
    sem_init(&write_to_file_sem, 0, 1);
    condition = PTHREAD_COND_INITIALIZER;
    mutex = PTHREAD_MUTEX_INITIALIZER;
    output_file.open("BitacoraMonitor.txt", ios::out);
    output_file << "";
    output_file.close();
    pthread_t threads[THREADS];

    // Creacion de threads
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
    sem_destroy(&write_to_file_sem);
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdarg.h>
#include <unistd.h>

static int N = 5; // Buffer size
static int M = 10; // Number of items to produce

static int* buffer;
static int produced_count = 0;
static int consumed_count = 0;
static int head = 0;
static int tail = 0;
static sem_t empty;
static sem_t full;
static pthread_mutex_t mutex;
static pthread_mutex_t print_mutex;

static int item = 1; // Item to produce
static pthread_mutex_t item_mutex = PTHREAD_MUTEX_INITIALIZER;

void sync_printf(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    pthread_mutex_lock(&print_mutex);
    vprintf(msg, args);
    pthread_mutex_unlock(&print_mutex);
    va_end(args);
}

int prod_next_item() {
    pthread_mutex_lock(&item_mutex);
    int next_item = item++;
    pthread_mutex_unlock(&item_mutex);
    return next_item;
}

int put_item(int x) {
    sem_wait(&empty); // Wait for an empty slot
    pthread_mutex_lock(&mutex); // Lock the buffer

    if (produced_count >= M) {
        pthread_mutex_unlock(&mutex);
        sem_post(&empty); // Revert the wait
        return -1;
    }

    buffer[head] = x;
    head = (head + 1) % N;
    produced_count++;

    pthread_mutex_unlock(&mutex); // Unlock the buffer
    sem_post(&full); // Signal that a new item is available
    return 0;
}

// 2 producer threads will call this function, combined
// they will produce M items
void* producer(void* arg) {
    int id = (int)(size_t)arg;

    while (1) {
        int item = prod_next_item();

        // Add item to buffer
        if (put_item(item) == -1) {
            sync_printf("[%d] Producer finished.\n", id);
            break;
        }
        sync_printf("[%d] Produced: %d\n", id, item);
    }

    return NULL;
}

int get_item() {
    sem_wait(&full); // Wait for a full slot
    pthread_mutex_lock(&mutex); // Lock the buffer

    if (consumed_count >= M) {
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        return -1;
    }

    int item = buffer[tail];
    tail = (tail + 1) % N;
    consumed_count++;
    int finished = (consumed_count >= M);

    pthread_mutex_unlock(&mutex); // Unlock the buffer
    sem_post(&empty); // Signal that an empty slot is available
    
    if (finished) {
        sem_post(&full); // Wake up other consumers
    }
    return item;
}


void* consumer(void* arg) {
    int id = (int)(size_t)arg;

    while (1) {
        int item;
        // Remove item from buffer
        if ((item = get_item()) == -1) {
            sync_printf("[%d] Consumer finished.\n", id);
            break;
        }

        // Process the item (here we just print it)
        sync_printf("[%d] Consumed: %d\n", id, item);
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <buffer_size> <num_items>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);

    buffer = (int*)calloc(N, sizeof(int));
    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&print_mutex, NULL);

    srand(10);

    // Create 4 threads: 2 producers and 2 consumers
    pthread_t producers[2], consumers[2];
    for (int i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, producer, (void*)i);
        // sleep(1); 
        pthread_create(&consumers[i], NULL, consumer, (void*)i);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    free(buffer);
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&print_mutex);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_INVENTORY 100
#define MIN_INVENTORY 20

int inventory[MAX_INVENTORY];
int inventory_size = 0;
pthread_mutex_t inventory_lock;

void* producer(void* arg) {
    char* name = (char*)arg;
    while (1) {
        pthread_mutex_lock(&inventory_lock);
        if (inventory_size < MAX_INVENTORY) {
            int items_to_produce = rand() % 10 + 1;
            for (int i = 0; i < items_to_produce; i++) {
                inventory[inventory_size++] = i + 1;
            }
            printf("%s produced %d items. Current inventory: %d\n", name, items_to_produce, inventory_size);
        } else {
            printf("%s cannot produce more items. Inventory is full.\n", name);
        }
        pthread_mutex_unlock(&inventory_lock);
        sleep(rand() % 3 + 1);
    }
    return NULL;
}

void* consumer(void* arg) {
    char* name = (char*)arg;
    while (1) {
        pthread_mutex_lock(&inventory_lock);
        if (inventory_size > MIN_INVENTORY) {
            int items_to_consume = rand() % 5 + 1;
            if (items_to_consume > inventory_size) {
                items_to_consume = inventory_size;
            }
            printf("%s consumed %d items. Current inventory: %d\n", name, items_to_consume, inventory_size - items_to_consume);
            inventory_size -= items_to_consume;
        } else {
            printf("%s cannot consume more items. Inventory is low.\n", name);
        }
        pthread_mutex_unlock(&inventory_lock);
        sleep(rand() % 3 + 1);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_mutex_init(&inventory_lock, NULL);

    pthread_t producer_threads[3];
    pthread_t consumer_threads[3];

    // Create producer threads
    for (int i = 0; i < 3; i++) {
        char* name = malloc(sizeof(char) * 20);
        sprintf(name, "Producer %d", i + 1);
        pthread_create(&producer_threads[i], NULL, producer, name);
    }

    // Create consumer threads
    for (int i = 0; i < 3; i++) {
        char* name = malloc(sizeof(char) * 20);
        sprintf(name, "Consumer %d", i + 1);
        pthread_create(&consumer_threads[i], NULL, consumer, name);
    }

    // Wait for all threads to finish
    for (int i = 0; i < 3; i++) {
        pthread_join(producer_threads[i], NULL);
        pthread_join(consumer_threads[i], NULL);
    }

    pthread_mutex_destroy(&inventory_lock);
    return 0;
}

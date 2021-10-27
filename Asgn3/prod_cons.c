#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 

pthread_t threadID[2];
pthread_mutex_t lock;
int count;

void* thing(void* arg) {
    pthread_mutex_lock(&lock);

    count = count + 1;
    printf("job %d has started\n", count);
    printf("job %d has finished\n", count);
    
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main() {
    
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex lock failed to initialize.\n");
        return 0;
    }
    for (int i = 0; i < 2; i++) {
        if (pthread_create(&threadID[i], NULL, &thing, NULL)) {
            printf("Thread failed to initialize.\n");
        }
    }
    
    pthread_join(&threadID[0] NULL);
    pthread_join(&threadID[1] NULL);
    pthread_mutex_destroy(&lock);
    
    return 0;
}
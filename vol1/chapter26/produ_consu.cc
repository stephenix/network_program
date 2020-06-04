#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

static int my_count = 0;
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t my_cond = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    cout << "Begin producer." << endl;
    pthread_mutex_lock(&my_mutex);
    ++ my_count;
    if (my_count > 0) {
        pthread_cond_signal(&my_cond);
    }
    pthread_mutex_unlock(&my_mutex);
    cout << "End producer." << endl;
    return NULL;
}

void *consumer(void *arg) {
    cout << "Begin consumer." << endl;
    pthread_mutex_lock(&my_mutex);
    // must be while loop
    // because fake wakeup
    while (my_count <= 0) {
        pthread_cond_wait(&my_cond, &my_mutex);
    }
    -- my_count;
    pthread_mutex_unlock(&my_mutex);
    cout << "End consumer." << endl;
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    sleep(1);
    pthread_create(&producer_thread, NULL, producer, NULL);

    pthread_join(consumer_thread, NULL);
    pthread_join(producer_thread, NULL);

    return 0;
}

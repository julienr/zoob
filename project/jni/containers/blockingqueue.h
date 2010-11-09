#ifndef _BLOCKINGQUEUE_H
#define	_BLOCKINGQUEUE_H

#include "vector.h"
#include "list.h"
#include <pthread.h>

/**
 * Implementation of a SINGLE writer multiple reader queue
 * http://stackoverflow.com/questions/1212623/multiple-writer-thread-safe-queue-in-c
 */
template <class T>
class blockingqueue {
  public:
    blockingqueue ()
      : cancel(false), writeFinished(false) {
        if (pthread_mutex_init(&mutex, NULL) != 0)
          LOGE("Error initializing mutex");
        if (pthread_cond_init(&cond, NULL) != 0)
          LOGE("Error initializing condition variable");

    }

    void append (const T& val) {
      pthread_mutex_lock(&mutex);
      content.append(val);
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&mutex);
    }

    //Gets and remove the top value from the queue. NON-BLOCKING
    //Returns false if the queue is empty, true otherwise
    bool pop (T* val) {
      pthread_mutex_lock(&mutex);
      bool canPop = !content.empty();
      if (canPop) {
        *val = content.removeFirst();
        //special termination case
        if (content.empty() && writeFinished)
          pthread_cond_broadcast(&cond);
      }
      pthread_mutex_unlock(&mutex);
      return canPop;
    }

    //Gets and remove the top value from the queue. BLOCKING
    //The value will be stored in the given pointer
    //true is returned if an actual value could be read
    //false is returned if the blockingqueue is destroyed
    //while the current thread is waiting on it
    bool blockingPop (T* val) {
      pthread_mutex_lock(&mutex);

      //We have to wait if the queue is empty.
      while (content.empty() && !cancel) {
        //special case : write thread has finished (we ASSUME there is only ONE writing thread)
        if (writeFinished) {
          pthread_mutex_unlock(&mutex);
          return false;
        }
        pthread_cond_wait(&cond, &mutex);
      }
      if (!cancel) {
        *val = content.removeFirst();
        //special termination case : writer has finished and we have read the last value.
        //we should notify the other reader so they can see that there is nothing left to read
        if (content.empty() && writeFinished)
          pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
        return true;
      } else {
        pthread_mutex_unlock(&mutex);
        return false;
      }
    }

    void finishWriting () {
      //just wake up all thread waiting on condition
      pthread_mutex_lock(&mutex);
      writeFinished = true;
      pthread_cond_broadcast(&cond);
      pthread_mutex_unlock(&mutex);
    }

    ~blockingqueue () {
      _cancel();
      
      pthread_cond_destroy(&cond);
      pthread_mutex_destroy(&mutex);
    }
  private:
    //Will cancel all threads currently waiting on this queue
    //(the pop() will return false) and clear the data contained
    //in this queue
    void _cancel () {
      pthread_mutex_lock(&mutex);
      cancel = true;
      content.clear();
      pthread_cond_broadcast(&cond);
      pthread_mutex_unlock(&mutex);
    }

    list<T> content;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    bool cancel;
    bool writeFinished;
};


#endif	/* BLOCKINGQUEUE_H */


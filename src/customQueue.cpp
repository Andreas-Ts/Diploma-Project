
#include "constants.h"

MessageQueue* createMessageQueue(int capacity) {
    MessageQueue *queue = (MessageQueue*)malloc(sizeof(MessageQueue));
    if (queue == NULL) {
      return NULL;  // Σφάλμα στη δεσμεύση μνήμης
    }
  
    queue->messages = (SensorMessage*)malloc(capacity * sizeof(SensorMessage));
    if (queue->messages == NULL) {
      free(queue);  // Απελευθέρωσε τη μνήμη του struct αν αποτύχει
      return NULL;
    }
  
    queue->capacity = capacity;
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
  
    return queue;
  }
#define DECL_QUEUE(name, type, len) \
  typedef struct {\
    size_t head; \
    size_t tail; \
    type data[len]; \
  } name; \

#ifndef queue_enqueue
#define queue_enqueue(queue, x) (queue).data[(queue).tail++] = (x);
#endif

#ifndef queue_dequeue
#define queue_dequeue(queue) (queue).data[(queue).head++];
#endif

#ifndef queue_is_empty
#define queue_is_empty(queue) ((queue).head == (queue).tail)
#endif

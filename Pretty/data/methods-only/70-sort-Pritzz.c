/* Initialize merge QUEUE, allocating space suitable for a maximum of
   NTHREADS threads.  */

static void
queue_init (struct merge_node_queue *queue, size_t nthreads)
{
  /* Though it's highly unlikely all nodes are in the heap at the same
     time, the heap should accommodate all of them.  Counting a NULL
     dummy head for the heap, reserve 2 * NTHREADS nodes.  */
  queue->priority_queue = heap_alloc (compare_nodes, 2 * nthreads);
  pthread_mutex_init (&queue->mutex, NULL);
  pthread_cond_init (&queue->cond, NULL);
}


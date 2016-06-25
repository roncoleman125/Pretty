/* Insert NODE into QUEUE.  The caller either holds a lock on NODE, or
   does not need to lock NODE.  */

static void
queue_insert (struct merge_node_queue *queue, struct merge_node *node)
{
  pthread_mutex_lock (&queue->mutex);
  heap_insert (queue->priority_queue, node);
  node->queued = true;
  pthread_mutex_unlock (&queue->mutex);
  pthread_cond_signal (&queue->cond);
}


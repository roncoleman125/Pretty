/* Lock a merge tree NODE.  */

static inline void
lock_node (struct merge_node *node)
{
  pthread_mutex_lock (&node->lock);
}


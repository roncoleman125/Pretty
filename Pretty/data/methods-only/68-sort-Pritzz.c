/* Unlock a merge tree NODE. */

static inline void
unlock_node (struct merge_node *node)
{
  pthread_mutex_unlock (&node->lock);
}


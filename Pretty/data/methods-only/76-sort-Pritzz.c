/* Into QUEUE, insert NODE's parent if the parent can now be worked on.  */

static void
queue_check_insert_parent (struct merge_node_queue *queue,
                           struct merge_node *node)
{
  if (node->level > MERGE_ROOT)
    {
      lock_node (node->parent);
      queue_check_insert (queue, node->parent);
      unlock_node (node->parent);
    }
  else if (node->nlo + node->nhi == 0)
    {
      /* If the MERGE_ROOT NODE has finished merging, insert the
         MERGE_END node.  */
      queue_insert (queue, node->parent);
    }
}


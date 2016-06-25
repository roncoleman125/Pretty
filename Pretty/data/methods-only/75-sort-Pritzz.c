/* Into QUEUE, insert NODE if it is not already queued, and if one of
   NODE's children has available lines and the other either has
   available lines or has exhausted its lines.  */

static void
queue_check_insert (struct merge_node_queue *queue, struct merge_node *node)
{
  if (! node->queued)
    {
      bool lo_avail = (node->lo - node->end_lo) != 0;
      bool hi_avail = (node->hi - node->end_hi) != 0;
      if (lo_avail ? hi_avail || ! node->nhi : hi_avail && ! node->nlo)
        queue_insert (queue, node);
    }
}


/* Like sortlines, except with a signature acceptable to pthread_create.  */

static void *
sortlines_thread (void *data)
{
  struct thread_args const *args = data;
  sortlines (args->lines, args->nthreads, args->total_lines,
             args->node, args->is_lo_child, args->queue, args->tfp,
             args->output_temp);
  return NULL;
}


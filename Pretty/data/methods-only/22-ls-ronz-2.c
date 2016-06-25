/* Add `pattern' to the list of patterns for which files that match are
   not listed.  */

static void
add_ignore_pattern (const char *pattern)
{
  struct ignore_pattern *ignore;

  ignore = xmalloc (sizeof *ignore);
  ignore->pattern = pattern;
  /* Add it to the head of the linked list.  */
  ignore->next = ignore_patterns;
  ignore_patterns = ignore;
}


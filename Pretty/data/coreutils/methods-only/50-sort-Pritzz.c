/* Debug LINE by underlining its keys.  */

static void
debug_line (struct line const *line)
{
  struct keyfield const *key = keylist;

  do
    debug_key (line, key);
  while (key && ((key = key->next) || ! (unique || stable)));
}


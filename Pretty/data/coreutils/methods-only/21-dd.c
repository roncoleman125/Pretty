/* Apply the character-set translations specified by the user
   to the NREAD bytes in BUF.  */

static void
translate_buffer (char *buf, size_t nread)
{
  char *cp;
  size_t i;

  for (i = nread, cp = buf; i; i--, cp++)
    *cp = trans_table[to_uchar (*cp)];
}

/* If true, the last char from the previous call to `swab_buffer'
   is saved in `saved_char'.  */
static bool char_is_saved = false;

/* Odd char from previous call.  */
static char saved_char;


/* Split the checksum string S (of length S_LEN) from a BSD 'md5' or
   'sha1' command into two parts: a hexadecimal digest, and the file
   name.  S is modified.  Return true if successful.  */

static bool
bsd_split_3 (char *s, size_t s_len, unsigned char **hex_digest,
             char **file_name)
{
  size_t i;

  if (s_len == 0)
    return false;

  *file_name = s;

  /* Find end of filename. The BSD 'md5' and 'sha1' commands do not escape
     filenames, so search backwards for the last ')'. */
  i = s_len - 1;
  while (i && s[i] != ')')
    i--;

  if (s[i] != ')')
    return false;

  s[i++] = '\0';

  while (ISWHITE (s[i]))
    i++;

  if (s[i] != '=')
    return false;

  i++;

  while (ISWHITE (s[i]))
    i++;

  *hex_digest = (unsigned char *) &s[i];
  return true;
}


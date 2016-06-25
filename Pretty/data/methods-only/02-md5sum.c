/* Split the string S (of length S_LEN) into three parts:
   a hexadecimal digest, binary flag, and the file name.
   S is modified.  Return true if successful.  */

static bool
split_3 (char *s, size_t s_len,
         unsigned char **hex_digest, int *binary, char **file_name)
{
  bool escaped_filename = false;
  size_t algo_name_len;

  size_t i = 0;
  while (ISWHITE (s[i]))
    ++i;

  /* Check for BSD-style checksum line. */
  algo_name_len = strlen (DIGEST_TYPE_STRING);
  if (strncmp (s + i, DIGEST_TYPE_STRING, algo_name_len) == 0)
    {
      if (s[i + algo_name_len] == ' ')
        ++i;
      if (s[i + algo_name_len] == '(')
        {
          *binary = 0;
          return bsd_split_3 (s +      i + algo_name_len + 1,
                              s_len - (i + algo_name_len + 1),
                              hex_digest, file_name);
        }
    }

  /* Ignore this line if it is too short.
     Each line must have at least `min_digest_line_length - 1' (or one more, if
     the first is a backslash) more characters to contain correct message digest
     information.  */
  if (s_len - i < min_digest_line_length + (s[i] == '\\'))
    return false;

  if (s[i] == '\\')
    {
      ++i;
      escaped_filename = true;
    }
  *hex_digest = (unsigned char *) &s[i];

  /* The first field has to be the n-character hexadecimal
     representation of the message digest.  If it is not followed
     immediately by a white space it's an error.  */
  i += digest_hex_bytes;
  if (!ISWHITE (s[i]))
    return false;

  s[i++] = '\0';

  if (s[i] != ' ' && s[i] != '*')
    return false;
  *binary = (s[i++] == '*');

  /* All characters between the type indicator and end of line are
     significant -- that includes leading and trailing white space.  */
  *file_name = &s[i];

  if (escaped_filename)
    {
      /* Translate each `\n' string in the file name to a NEWLINE,
         and each `\\' string to a backslash.  */

      char *dst = &s[i];

      while (i < s_len)
        {
          switch (s[i])
            {
            case '\\':
              if (i == s_len - 1)
                {
                  /* A valid line does not end with a backslash.  */
                  return false;
                }
              ++i;
              switch (s[i++])
                {
                case 'n':
                  *dst++ = '\n';
                  break;
                case '\\':
                  *dst++ = '\\';
                  break;
                default:
                  /* Only `\' or `n' may follow a backslash.  */
                  return false;
                }
              break;

            case '\0':
              /* The file name may not contain a NUL.  */
              return false;
              break;

            default:
              *dst++ = s[i++];
              break;
            }
        }
      *dst = '\0';
    }
  return true;
}


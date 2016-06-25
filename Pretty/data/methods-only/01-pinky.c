/* Create a string (via xmalloc) which contains a full name by substituting
   for each ampersand in GECOS_NAME the USER_NAME string with its first
   character capitalized.  The caller must ensure that GECOS_NAME contains
   no `,'s.  The caller also is responsible for free'ing the return value of
   this function.  */

static char *
create_fullname (const char *gecos_name, const char *user_name)
{
  size_t rsize = strlen (gecos_name) + 1;
  char *result;
  char *r;
  size_t ampersands = count_ampersands (gecos_name);

  if (ampersands != 0)
    {
      size_t ulen = strlen (user_name);
      size_t product = ampersands * ulen;
      rsize += product - ampersands;
      if (xalloc_oversized (ulen, ampersands) || rsize < product)
        xalloc_die ();
    }

  r = result = xmalloc (rsize);

  while (*gecos_name)
    {
      if (*gecos_name == '&')
        {
          const char *uname = user_name;
          if (islower (to_uchar (*uname)))
            *r++ = toupper (to_uchar (*uname++));
          while (*uname)
            *r++ = *uname++;
        }
      else
        {
          *r++ = *gecos_name;
        }

      gecos_name++;
    }
  *r = 0;

  return result;
}


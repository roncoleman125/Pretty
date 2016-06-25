/* Compare the keys TEXTA (of length LENA) and TEXTB (of length LENB)
   using one or more random hash functions.  TEXTA[LENA] and
   TEXTB[LENB] must be zero.  */

static int
compare_random (char *restrict texta, size_t lena,
                char *restrict textb, size_t lenb)
{
  /* XFRM_DIFF records the equivalent of memcmp on the transformed
     data.  This is used to break ties if there is an checksum
     collision, and this is good enough given the astronomically low
     probability of a collision.  */
  int xfrm_diff = 0;

  char stackbuf[4000];
  char *buf = stackbuf;
  size_t bufsize = sizeof stackbuf;
  void *allocated = NULL;
  uint32_t dig[2][MD5_DIGEST_SIZE / sizeof (uint32_t)];
  struct md5_ctx s[2];
  s[0] = s[1] = random_md5_state;

  if (hard_LC_COLLATE)
    {
      char const *lima = texta + lena;
      char const *limb = textb + lenb;

      while (true)
        {
          /* Transform the text into the basis of comparison, so that byte
             strings that would otherwise considered to be equal are
             considered equal here even if their bytes differ.

             Each time through this loop, transform one
             null-terminated string's worth from TEXTA or from TEXTB
             or both.  That way, there's no need to store the
             transformation of the whole line, if it contains many
             null-terminated strings.  */

          /* Store the transformed data into a big-enough buffer.  */

          /* A 3X size guess avoids the overhead of calling strxfrm
             twice on typical implementations.  Don't worry about
             size_t overflow, as the guess need not be correct.  */
          size_t guess_bufsize = 3 * (lena + lenb) + 2;
          if (bufsize < guess_bufsize)
            {
              bufsize = MAX (guess_bufsize, bufsize * 3 / 2);
              free (allocated);
              buf = allocated = malloc (bufsize);
              if (! buf)
                {
                  buf = stackbuf;
                  bufsize = sizeof stackbuf;
                }
            }

          size_t sizea =
            (texta < lima ? xstrxfrm (buf, texta, bufsize) + 1 : 0);
          bool a_fits = sizea <= bufsize;
          size_t sizeb =
            (textb < limb
             ? (xstrxfrm ((a_fits ? buf + sizea : NULL), textb,
                          (a_fits ? bufsize - sizea : 0))
                + 1)
             : 0);

          if (! (a_fits && sizea + sizeb <= bufsize))
            {
              bufsize = sizea + sizeb;
              if (bufsize < SIZE_MAX / 3)
                bufsize = bufsize * 3 / 2;
              free (allocated);
              buf = allocated = xmalloc (bufsize);
              if (texta < lima)
                strxfrm (buf, texta, sizea);
              if (textb < limb)
                strxfrm (buf + sizea, textb, sizeb);
            }

          /* Advance past NULs to the next part of each input string,
             exiting the loop if both strings are exhausted.  When
             exiting the loop, prepare to finish off the tiebreaker
             comparison properly.  */
          if (texta < lima)
            texta += strlen (texta) + 1;
          if (textb < limb)
            textb += strlen (textb) + 1;
          if (! (texta < lima || textb < limb))
            {
              lena = sizea; texta = buf;
              lenb = sizeb; textb = buf + sizea;
              break;
            }

          /* Accumulate the transformed data in the corresponding
             checksums.  */
          md5_process_bytes (buf, sizea, &s[0]);
          md5_process_bytes (buf + sizea, sizeb, &s[1]);

          /* Update the tiebreaker comparison of the transformed data.  */
          if (! xfrm_diff)
            {
              xfrm_diff = memcmp (buf, buf + sizea, MIN (sizea, sizeb));
              if (! xfrm_diff)
                xfrm_diff = (sizea > sizeb) - (sizea < sizeb);
            }
        }
    }

  /* Compute and compare the checksums.  */
  md5_process_bytes (texta, lena, &s[0]); md5_finish_ctx (&s[0], dig[0]);
  md5_process_bytes (textb, lenb, &s[1]); md5_finish_ctx (&s[1], dig[1]);
  int diff = memcmp (dig[0], dig[1], sizeof dig[0]);

  /* Fall back on the tiebreaker if the checksums collide.  */
  if (! diff)
    {
      if (! xfrm_diff)
        {
          xfrm_diff = memcmp (texta, textb, MIN (lena, lenb));
          if (! xfrm_diff)
            xfrm_diff = (lena > lenb) - (lena < lenb);
        }

      diff = xfrm_diff;
    }

  free (allocated);

  return diff;
}


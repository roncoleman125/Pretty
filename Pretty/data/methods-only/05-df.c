/* Add integral value while using uintmax_t for value part and separate
   negation flag. It adds value of SRC and SRC_NEG to DEST and DEST_NEG.
   The result will be in DEST and DEST_NEG.  See df_readable to understand
   how the negation flag is used.  */
static void
add_uint_with_neg_flag (uintmax_t *dest, bool *dest_neg,
                        uintmax_t src, bool src_neg)
{
  if (LOG_EQ (*dest_neg, src_neg))
    {
      *dest += src;
      return;
    }

  if (*dest_neg)
    *dest = -*dest;

  if (src_neg)
    src = -src;

  if (src < *dest)
    *dest -= src;
  else
    {
      *dest = src - *dest;
      *dest_neg = src_neg;
    }

  if (*dest_neg)
    *dest = -*dest;
}


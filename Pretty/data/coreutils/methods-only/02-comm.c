/* Check that successive input lines PREV and CURRENT from input file
   WHATFILE are presented in order.

   If the user specified --nocheck-order, the check is not made.
   If the user specified --check-order, the problem is fatal.
   Otherwise (the default), the message is simply a warning.

   A message is printed at most once per input file.

   This funtion was copied (nearly) verbatim from `src/join.c'. */

static void
check_order (struct linebuffer const *prev,
             struct linebuffer const *current,
             int whatfile)
{

  if (check_input_order != CHECK_ORDER_DISABLED
      && ((check_input_order == CHECK_ORDER_ENABLED) || seen_unpairable))
    {
      if (!issued_disorder_warning[whatfile - 1])
        {
          int order;

          if (hard_LC_COLLATE)
            order = xmemcoll (prev->buffer, prev->length - 1,
                              current->buffer, current->length - 1);
          else
            order = memcmp2 (prev->buffer, prev->length - 1,
                             current->buffer, current->length - 1);

          if (0 < order)
            {
              error ((check_input_order == CHECK_ORDER_ENABLED
                      ? EXIT_FAILURE : 0),
                     0, _("file %d is not in sorted order"), whatfile);

              /* If we get to here, the message was just a warning, but we
                 want only to issue it once. */
              issued_disorder_warning[whatfile - 1] = true;
            }
        }
    }
}


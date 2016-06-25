/*---------------------------------------------------------------------.
| Sort the whole occurs table in memory.  Presumably, `qsort' does not |
| take intermediate copies or table elements, so the sort will be      |
| stabilized throughout the comparison routine.			       |
`---------------------------------------------------------------------*/

static void
sort_found_occurs (void)
{

  /* Only one language for the time being.  */

  qsort (occurs_table[0], number_of_occurs[0], sizeof **occurs_table,
         compare_occurs);
}


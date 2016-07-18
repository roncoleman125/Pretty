/* Read and process the file pointed to by FP. */

static void
process_file (FILE *fp)
{
  while (readlinebuffer (&line_buf, fp))
    {
      switch (check_section ())
        {
        case Header:
          proc_header ();
          break;
        case Body:
          proc_body ();
          break;
        case Footer:
          proc_footer ();
          break;
        case Text:
          proc_text ();
          break;
        }
    }
}


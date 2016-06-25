/*------------------------------------------------------------------------.
| This routine will attempt to swallow a whole file name FILE_NAME into a |
| contiguous region of memory and return a description of it into BLOCK.  |
| Standard input is assumed whenever FILE_NAME is NULL, empty or "-".	  |
|									  |
| Previously, in some cases, white space compression was attempted while  |
| inputting text.  This was defeating some regexps like default end of	  |
| sentence, which checks for two consecutive spaces.  If white space	  |
| compression is ever reinstated, it should be in output routines.	  |
`------------------------------------------------------------------------*/

static void
swallow_file_in_memory (const char *file_name, BLOCK *block)
{
  size_t used_length;		/* used length in memory buffer */

  /* As special cases, a file name which is NULL or "-" indicates standard
     input, which is already opened.  In all other cases, open the file from
     its name.  */
  bool using_stdin = !file_name || !*file_name || STREQ (file_name, "-");
  if (using_stdin)
    block->start = fread_file (stdin, &used_length);
  else
    block->start = read_file (file_name, &used_length);

  if (!block->start)
    error (EXIT_FAILURE, errno, "%s", quote (using_stdin ? "-" : file_name));

  block->end = block->start + used_length;
}


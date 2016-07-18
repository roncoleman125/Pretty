/*------------------------------------------------------------------------.
| This will initialize various tables for pattern match and compiles some |
| regexps.								  |
`------------------------------------------------------------------------*/

static void
initialize_regex (void)
{
  int character;		/* character value */

  /* Initialize the case folding table.  */

  if (ignore_case)
    for (character = 0; character < CHAR_SET_SIZE; character++)
      folded_chars[character] = toupper (character);

  /* Unless the user already provided a description of the end of line or
     end of sentence sequence, select an end of line sequence to compile.
     If the user provided an empty definition, thus disabling end of line
     or sentence feature, make it NULL to speed up tests.  If GNU
     extensions are enabled, use end of sentence like in GNU emacs.  If
     disabled, use end of lines.  */

  if (context_regex.string)
    {
      if (!*context_regex.string)
        context_regex.string = NULL;
    }
  else if (gnu_extensions && !input_reference)
    context_regex.string = "[.?!][]\"')}]*\\($\\|\t\\|  \\)[ \t\n]*";
  else
    context_regex.string = "\n";

  if (context_regex.string)
    compile_regex (&context_regex);

  /* If the user has already provided a non-empty regexp to describe
     words, compile it.  Else, unless this has already been done through
     a user provided Break character file, construct a fastmap of
     characters that may appear in a word.  If GNU extensions enabled,
     include only letters of the underlying character set.  If disabled,
     include almost everything, even punctuations; stop only on white
     space.  */

  if (word_regex.string)
    compile_regex (&word_regex);
  else if (!break_file)
    {
      if (gnu_extensions)
        {

          /* Simulate \w+.  */

          for (character = 0; character < CHAR_SET_SIZE; character++)
            word_fastmap[character] = !! isalpha (character);
        }
      else
        {

          /* Simulate [^ \t\n]+.  */

          memset (word_fastmap, 1, CHAR_SET_SIZE);
          word_fastmap[' '] = 0;
          word_fastmap['\t'] = 0;
          word_fastmap['\n'] = 0;
        }
    }
}


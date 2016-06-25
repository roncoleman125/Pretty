/* Merge lines from FILES onto OFP.  NTEMPS is the number of temporary
   files (all of which are at the start of the FILES array), and
   NFILES is the number of files; 0 <= NTEMPS <= NFILES <= NMERGE.
   Close input and output files before returning.
   OUTPUT_FILE gives the name of the output file.

   Return the number of files successfully merged.  This number can be
   less than NFILES if we ran low on file descriptors, but in this
   case it is never less than 2.  */

static size_t
mergefiles (struct sortfile *files, size_t ntemps, size_t nfiles,
            FILE *ofp, char const *output_file)
{
  FILE **fps;
  size_t nopened = open_input_files (files, nfiles, &fps);
  if (nopened < nfiles && nopened < 2)
    die (_("open failed"), files[nopened].name);
  mergefps (files, ntemps, nopened, ofp, output_file, fps);
  return nopened;
}


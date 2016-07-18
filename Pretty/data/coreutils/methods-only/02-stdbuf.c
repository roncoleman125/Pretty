/* argv[0] can be anything really, but generally it contains
   the path to the executable or just a name if it was executed
   using $PATH. In the latter case to get the path we can:
   search getenv("PATH"), readlink("/prof/self/exe"), getenv("_"),
   dladdr(), pstat_getpathname(), etc.  */

static void
set_program_path (const char *arg)
{
  if (strchr (arg, '/'))        /* Use absolute or relative paths directly.  */
    {
      program_path = dir_name (arg);
    }
  else
    {
      char *path = xreadlink ("/proc/self/exe");
      if (path)
        program_path = dir_name (path);
      else if ((path = getenv ("PATH")))
        {
          char *dir;
          path = xstrdup (path);
          for (dir = strtok (path, ":"); dir != NULL; dir = strtok (NULL, ":"))
            {
              char *candidate = file_name_concat (dir, arg, NULL);
              if (access (candidate, X_OK) == 0)
                {
                  program_path = dir_name (candidate);
                  free (candidate);
                  break;
                }
              free (candidate);
            }
        }
      free (path);
    }
}


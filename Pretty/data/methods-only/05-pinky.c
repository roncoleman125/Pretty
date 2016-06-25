/* Display a verbose line of information about UTMP_ENT. */

static void
print_long_entry (const char name[])
{
  struct passwd *pw;

  pw = getpwnam (name);

  printf (_("Login name: "));
  printf ("%-28s", name);

  printf (_("In real life: "));
  if (pw == NULL)
    {
      /* TRANSLATORS: Real name is unknown; no hard limit. */
      printf (" %s", _("???\n"));
      return;
    }
  else
    {
      char *const comma = strchr (pw->pw_gecos, ',');
      char *result;

      if (comma)
        *comma = '\0';

      result = create_fullname (pw->pw_gecos, pw->pw_name);
      printf (" %s", result);
      free (result);
    }

  putchar ('\n');

  if (include_home_and_shell)
    {
      printf (_("Directory: "));
      printf ("%-29s", pw->pw_dir);
      printf (_("Shell: "));
      printf (" %s", pw->pw_shell);
      putchar ('\n');
    }

  if (include_project)
    {
      FILE *stream;
      char buf[1024];
      const char *const baseproject = "/.project";
      char *const project =
        xmalloc (strlen (pw->pw_dir) + strlen (baseproject) + 1);
      stpcpy (stpcpy (project, pw->pw_dir), baseproject);

      stream = fopen (project, "r");
      if (stream)
        {
          size_t bytes;

          printf (_("Project: "));

          while ((bytes = fread (buf, 1, sizeof (buf), stream)) > 0)
            fwrite (buf, 1, bytes, stdout);
          fclose (stream);
        }

      free (project);
    }

  if (include_plan)
    {
      FILE *stream;
      char buf[1024];
      const char *const baseplan = "/.plan";
      char *const plan =
        xmalloc (strlen (pw->pw_dir) + strlen (baseplan) + 1);
      stpcpy (stpcpy (plan, pw->pw_dir), baseplan);

      stream = fopen (plan, "r");
      if (stream)
        {
          size_t bytes;

          printf (_("Plan:\n"));

          while ((bytes = fread (buf, 1, sizeof (buf), stream)) > 0)
            fwrite (buf, 1, bytes, stdout);
          fclose (stream);
        }

      free (plan);
    }

  putchar ('\n');
}


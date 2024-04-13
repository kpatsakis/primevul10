parse_dotla_file(FILE *file, char **dlname, char **libdir, char **deplibs,
    char **old_name, int *installed)
{
  int		errors = 0;
  size_t	line_len = LT_FILENAME_MAX;
  char *	line = MALLOC (char, line_len);

  if (!line)
    {
      LT__SETERROR (FILE_NOT_FOUND);
      return 1;
    }

  while (!feof (file))
    {
      line[line_len-2] = '\0';
      if (!fgets (line, (int) line_len, file))
	{
	  break;
	}

      /* Handle the case where we occasionally need to read a line
	 that is longer than the initial buffer size.
	 Behave even if the file contains NUL bytes due to corruption. */
      while (line[line_len-2] != '\0' && line[line_len-2] != '\n' && !feof (file))
	{
	  line = REALLOC (char, line, line_len *2);
	  if (!line)
	    {
	      ++errors;
	      goto cleanup;
	    }
	  line[line_len * 2 - 2] = '\0';
	  if (!fgets (&line[line_len -1], (int) line_len +1, file))
	    {
	      break;
	    }
	  line_len *= 2;
	}

      if (line[0] == '\n' || line[0] == '#')
	{
	  continue;
	}

#undef  STR_DLNAME
#define STR_DLNAME	"dlname="
      if (strncmp (line, STR_DLNAME, sizeof (STR_DLNAME) - 1) == 0)
	{
	  errors += trim (dlname, &line[sizeof (STR_DLNAME) - 1]);
	}

#undef  STR_OLD_LIBRARY
#define STR_OLD_LIBRARY	"old_library="
      else if (strncmp (line, STR_OLD_LIBRARY,
	    sizeof (STR_OLD_LIBRARY) - 1) == 0)
	{
	  errors += trim (old_name, &line[sizeof (STR_OLD_LIBRARY) - 1]);
	}
#undef  STR_LIBDIR
#define STR_LIBDIR	"libdir="
      else if (strncmp (line, STR_LIBDIR, sizeof (STR_LIBDIR) - 1) == 0)
	{
	  errors += trim (libdir, &line[sizeof(STR_LIBDIR) - 1]);
	}

#undef  STR_DL_DEPLIBS
#define STR_DL_DEPLIBS	"dependency_libs="
      else if (strncmp (line, STR_DL_DEPLIBS,
	    sizeof (STR_DL_DEPLIBS) - 1) == 0)
	{
	  errors += trim (deplibs, &line[sizeof (STR_DL_DEPLIBS) - 1]);
	}
      else if (streq (line, "installed=yes\n"))
	{
	  *installed = 1;
	}
      else if (streq (line, "installed=no\n"))
	{
	  *installed = 0;
	}

#undef  STR_LIBRARY_NAMES
#define STR_LIBRARY_NAMES "library_names="
      else if (!*dlname && strncmp (line, STR_LIBRARY_NAMES,
	    sizeof (STR_LIBRARY_NAMES) - 1) == 0)
	{
	  char *last_libname;
	  errors += trim (dlname, &line[sizeof (STR_LIBRARY_NAMES) - 1]);
	  if (!errors
	      && *dlname
	      && (last_libname = strrchr (*dlname, ' ')) != 0)
	    {
	      last_libname = lt__strdup (last_libname + 1);
	      if (!last_libname)
		{
		  ++errors;
		  goto cleanup;
		}
	      MEMREASSIGN (*dlname, last_libname);
	    }
	}

      if (errors)
	break;
    }
cleanup:
  FREE (line);
  return errors;
}
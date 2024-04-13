int main (int argc, char **argv)
#endif
{
  static const char *flags =
    "0123456789cdefhnqvDFJNORTVs:m:B:C:E:I:L:O:M:P:W:A::S::";
  xd3_cmd cmd;
  main_file ifile;
  main_file ofile;
  main_file sfile;
  main_merge_list merge_order;
  main_merge *merge;
  int my_optind;
  const char *my_optarg;
  const char *my_optstr;
  const char *sfilename;
  int env_argc;
  char **env_argv;
  char **free_argv;  /* malloc() in setup_environment() */
  char *free_value;  /* malloc() in setup_environment() */
  int ret;

#ifdef _WIN32
  GetStartupInfo(&winStartupInfo);
  setvbuf(stderr, NULL, _IONBF, 0);  /* Do not buffer stderr */
#endif

  main_file_init (& ifile);
  main_file_init (& ofile);
  main_file_init (& sfile);
  main_merge_list_init (& merge_order);

  reset_defaults();

  free_argv = NULL;
  free_value = NULL;
  setup_environment(argc, argv, &env_argc, &env_argv,
		    &free_argv, &free_value);
  cmd = CMD_NONE;
  sfilename = NULL;
  my_optind = 1;
  argv = env_argv;
  argc = env_argc;
  program_name = env_argv[0];

 takearg:
  my_optarg = NULL;
  my_optstr = argv[my_optind];

  /* This doesn't use getopt() because it makes trouble for -P & python which
   * reenter main() and thus care about freeing all memory.  I never had much
   * trust for getopt anyway, it's too opaque.  This implements a fairly
   * standard non-long-option getopt with support for named operations (e.g.,
   * "xdelta3 [encode|decode|printhdr...] < in > out"). */
  if (my_optstr)
    {
      if (*my_optstr == '-')    { my_optstr += 1; }
      else if (cmd == CMD_NONE) { goto nonflag; }
      else                      { my_optstr = NULL; }
    }
  while (my_optstr)
    {
      const char *s;
      my_optarg = NULL;
      if ((ret = *my_optstr++) == 0) { my_optind += 1; goto takearg; }

      /* Option handling: first check for one ':' following the option in
       * flags, then check for two.  The syntax allows:
       *
       * 1. -Afoo                   defines optarg="foo"
       * 2. -A foo                  defines optarg="foo"
       * 3. -A ""                   defines optarg="" (allows empty-string)
       * 4. -A [EOA or -moreargs]   error (mandatory case)
       * 5. -A [EOA -moreargs]      defines optarg=NULL (optional case)
       * 6. -A=foo                  defines optarg="foo"
       * 7. -A=                     defines optarg="" (mandatory case)
       * 8. -A=                     defines optarg=NULL (optional case)
       *
       * See tests in test_command_line_arguments().
       */
      s = strchr (flags, ret);
      if (s && s[1] && s[1] == ':')
	{
	  int option = s[2] && s[2] == ':';

	  /* Case 1, set optarg to the remaining characters. */
	  my_optarg = my_optstr;
	  my_optstr = "";

	  /* Case 2-5 */
	  if (*my_optarg == 0)
	    {
	      /* Condition 4-5 */
	      int have_arg = (my_optind < (argc - 1) &&
			      *argv[my_optind+1] != '-');

	      if (! have_arg)
		{
		  if (! option)
		  {
		    /* Case 4 */
		    XPR(NT "-%c: requires an argument\n", ret);
		    ret = EXIT_FAILURE;
		    goto cleanup;
		  }
		  /* Case 5. */
		  my_optarg = NULL;
		}
	      else
		{
		  /* Case 2-3. */
		  my_optarg = argv[++my_optind];
		}
	    }
	  /* Case 6-8. */
	  else if (*my_optarg == '=')
	    {
	      /* Remove the = in all cases. */
	      my_optarg += 1;

	      if (option && *my_optarg == 0)
		{
		  /* Case 8. */
		  my_optarg = NULL;
		}
	    }
	}

      switch (ret)
	{
	/* case: if no '-' was found, maybe check for a command name. */
	nonflag:
	       if (strcmp (my_optstr, "decode") == 0) { cmd = CMD_DECODE; }
	  else if (strcmp (my_optstr, "encode") == 0)
	    {
#if XD3_ENCODER
	      cmd = CMD_ENCODE;
#else
	      XPR(NT "encoder support not compiled\n");
	      return EXIT_FAILURE;
#endif
	    }
	  else if (strcmp (my_optstr, "config") == 0) { cmd = CMD_CONFIG; }
#if REGRESSION_TEST
	  else if (strcmp (my_optstr, "test") == 0) { cmd = CMD_TEST; }
#endif
#if VCDIFF_TOOLS
	  else if (strcmp (my_optstr, "printhdr") == 0) { cmd = CMD_PRINTHDR; }
	  else if (strcmp (my_optstr, "printhdrs") == 0)
	    { cmd = CMD_PRINTHDRS; }
	  else if (strcmp (my_optstr, "printdelta") == 0)
	    { cmd = CMD_PRINTDELTA; }
	  else if (strcmp (my_optstr, "recode") == 0) { cmd = CMD_RECODE; }
	  else if (strcmp (my_optstr, "merge") == 0) { cmd = CMD_MERGE; }
#endif

	  /* If no option was found and still no command, let the default
	   * command be encode.  The remaining args are treated as
	   * filenames. */
	  if (cmd == CMD_NONE)
	    {
	      cmd = CMD_DEFAULT;
	      my_optstr = NULL;
	      break;
	    }
	  else
	    {
	      /* But if we find a command name, continue the getopt loop. */
	      my_optind += 1;
	      goto takearg;
	    }

	  /* gzip-like options */
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	  option_level = ret - '0';
	  break;
	case 'f': option_force = 1; break;
	case 'F':
#if EXTERNAL_COMPRESSION
	  option_force2 = 1;
#else
	  XPR(NT "warning: -F option ignored, "
	      "external compression support was not compiled\n");
	  break;
#endif
	case 'v': option_verbose += 1; option_quiet = 0; break;
	case 'q': option_quiet = 1; option_verbose = 0; break;
	case 'c': option_stdout = 1; break;
	case 'd':
	  if (cmd == CMD_NONE) { cmd = CMD_DECODE; }
	  else { ret = main_help (); goto exit; }
	  break;
	case 'e':
#if XD3_ENCODER
	  if (cmd == CMD_NONE) { cmd = CMD_ENCODE; }
	  else { ret = main_help (); goto exit; }
	  break;
#else
	  XPR(NT "encoder support not compiled\n");
	  return EXIT_FAILURE;
#endif

	case 'n': option_use_checksum = 0; break;
	case 'N': option_no_compress = 1; break;
	case 'T': option_use_altcodetable = 1; break;
	case 'C': option_smatch_config = my_optarg; break;
	case 'J': option_no_output = 1; break;
	case 'S': if (my_optarg == NULL)
	    {
	      option_use_secondary = 1;
	      option_secondary = "none";
	    }
	  else
	    {
	      option_use_secondary = 1;
	      option_secondary = my_optarg;
	    }
	  break;
	case 'A': if (my_optarg == NULL) { option_use_appheader = 0; }
	          else { option_appheader = (uint8_t*) my_optarg; } break;
	case 'B': {
	  xoff_t bsize;
	  if ((ret = main_atoux (my_optarg, & bsize,
				 XD3_MINSRCWINSZ, XD3_MAXSRCWINSZ, 'B')))
	    {
	      goto exit;
	    }
	  option_srcwinsz = bsize;
	  break;
	}
	case 'I':
	  if ((ret = main_atou (my_optarg, & option_iopt_size, 0,
				0, 'I')))
	    {
	      goto exit;
	    }
	  break;
	case 'P':
	  if ((ret = main_atou (my_optarg, & option_sprevsz, 0,
				0, 'P')))
	    {
	      goto exit;
	    }
	  break;
	case 'W':
	  if ((ret = main_atou (my_optarg, & option_winsize, XD3_ALLOCSIZE,
				XD3_HARDMAXWINSIZE, 'W')))
	  {
	    goto exit;
	  }
	  break;
	case 'D':
#if EXTERNAL_COMPRESSION == 0
	  if (option_verbose > 0)
	    {
	      XPR(NT "warning: -D option ignored, "
		  "external compression support was not compiled\n");
	    }
#else
	  option_decompress_inputs  = 0;
#endif
	  break;
	case 'R':
#if EXTERNAL_COMPRESSION == 0
	  if (option_verbose > 0)
	    {
	      XPR(NT "warning: -R option ignored, "
		  "external compression support was not compiled\n");
	    }
#else
	  option_recompress_outputs = 0;
#endif
	  break;
	case 's':
	  if (sfilename != NULL)
	    {
	      XPR(NT "specify only one source file\n");
	      goto cleanup;
	    }

	  sfilename = my_optarg;
	  break;
	case 'm':
	  if ((merge = (main_merge*)
	       main_malloc (sizeof (main_merge))) == NULL)
	    {
	      goto cleanup;
	    }
	  main_merge_list_push_back (& merge_order, merge);
	  merge->filename = my_optarg;
	  break;
	case 'V':
	  ret = main_version (); goto exit;
	default:
	  ret = main_help (); goto exit;
	}
    }

  option_source_filename = sfilename;

  /* In case there were no arguments, set the default command. */
  if (cmd == CMD_NONE) { cmd = CMD_DEFAULT; }

  argc -= my_optind;
  argv += my_optind;

  /* There may be up to two more arguments. */
  if (argc > 2)
    {
      XPR(NT "too many filenames: %s ...\n", argv[2]);
      goto cleanup;
    }

  ifile.flags    = RD_FIRST | RD_MAININPUT;
  sfile.flags    = RD_FIRST;
  sfile.filename = option_source_filename;

  /* The infile takes the next argument, if there is one.  But if not, infile
   * is set to stdin. */
  if (argc > 0)
    {
      ifile.filename = argv[0];

      if ((ret = main_file_open (& ifile, ifile.filename, XO_READ)))
	{
	  goto cleanup;
	}
    }
  else
    {
      XSTDIN_XF (& ifile);
    }

  /* The ofile takes the following argument, if there is one.  But if not, it
   * is left NULL until the application header is processed.  It will be set
   * in main_open_output. */
  if (argc > 1)
    {
      /* Check for conflicting arguments. */
      if (option_stdout && ! option_quiet)
	{
	  XPR(NT "warning: -c option overrides output filename: %s\n",
	      argv[1]);
	}

      if (! option_stdout) { ofile.filename = argv[1]; }
    }

#if VCDIFF_TOOLS
  if (cmd == CMD_MERGE &&
      (ret = main_merge_arguments (&merge_order)))
    {
      goto cleanup;
    }
#endif /* VCDIFF_TOOLS */

  switch (cmd)
    {
    case CMD_PRINTHDR:
    case CMD_PRINTHDRS:
    case CMD_PRINTDELTA:
#if XD3_ENCODER
    case CMD_ENCODE:
    case CMD_RECODE:
    case CMD_MERGE:
#endif
    case CMD_DECODE:
      ret = main_input (cmd, & ifile, & ofile, & sfile);
      break;

#if REGRESSION_TEST
    case CMD_TEST:
      main_config ();
      ret = xd3_selftest ();
      break;
#endif

    case CMD_CONFIG:
      ret = main_config ();
      break;

    default:
      ret = main_help ();
      break;
    }

  if (0)
    {
    cleanup:
      ret = EXIT_FAILURE;
    exit:
      (void)0;
    }

#if EXTERNAL_COMPRESSION
  main_external_compression_cleanup ();
#endif

  main_file_cleanup (& ifile);
  main_file_cleanup (& ofile);
  main_file_cleanup (& sfile);

  while (! main_merge_list_empty (& merge_order))
    {
      merge = main_merge_list_pop_front (& merge_order);
      main_free (merge);
    }

  main_free (free_argv);
  main_free (free_value);

  main_cleanup ();

  fflush (stdout);
  fflush (stderr);
  return ret;
}
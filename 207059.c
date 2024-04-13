mail_send (int argc, char **argv)
{
  compose_env_t env;
  int status;
  int save_to = mu_isupper (argv[0][0]);

  compose_init (&env);

  if (argc < 2)
    {
      if (interactive)
	compose_header_set (&env, MU_HEADER_TO, ml_readline_with_intr ("To: "),
			    COMPOSE_REPLACE);
      else if (mailvar_is_true (mailvar_name_editheaders))
	{
	  if (parse_headers (mu_strin, &env) != parse_headers_ok)
	    {
	      mu_error ("%s", _("Errors parsing message"));
	      exit (EXIT_FAILURE);
	    }
	  if (add_header_list)
	    {
	      mu_iterator_t itr;
	      mu_list_get_iterator (add_header_list, &itr);
	      for (mu_iterator_first (itr); !mu_iterator_is_done (itr);
		   mu_iterator_next (itr))
		{
		  struct add_header *hp;
		  int mode;
		  if (mu_iterator_current (itr, (void**) &hp))
		    break; 
		  mode = hp->mode;
		  if (mu_header_sget_value (env.header, hp->name, NULL) == 0)
		    mode = COMPOSE_REPLACE;
		  compose_header_set (&env, hp->name, hp->value, mode);
		}
	      mu_iterator_destroy (&itr);
	    }
	}
      else
	{
	  mu_error ("%s", _("No recipients specified"));
	  exit (EXIT_FAILURE);
	}
    }
  else
    {
      while (--argc)
	{
	  char *p = *++argv;
	  if (isfilename (p))
	    {
	      env.outfiles = realloc (env.outfiles,
				      (env.nfiles + 1) *
				      sizeof (*(env.outfiles)));
	      if (env.outfiles)
		{
		  env.outfiles[env.nfiles] = p;
		  env.nfiles++;
		}
	    }
	  else
	    compose_header_set (&env, MU_HEADER_TO, p, COMPOSE_SINGLE_LINE);
	}
    }

  if (interactive)
    {
      if (!mailvar_is_true (mailvar_name_mailx))
	read_cc_bcc (&env);

      if (mailvar_is_true (mailvar_name_asksub))
	compose_header_set (&env, MU_HEADER_SUBJECT,
			    ml_readline_with_intr ("Subject: "),
			    COMPOSE_REPLACE);
    }
  
  status = mail_compose_send (&env, save_to);
  compose_destroy (&env);
  return status;
}
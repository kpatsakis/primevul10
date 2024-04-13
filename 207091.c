escape_list_attachments (int argc, char **argv, compose_env_t *env)
{
  mu_iterator_t itr;
  int i;
  
  report_multipart_type (env);

  if (mu_list_is_empty (env->attlist) ||
      mu_list_get_iterator (env->attlist, &itr))
    {
      mu_printf ("%s\n", _("No attachments"));
      return 0;
    }
  
  for (mu_iterator_first (itr), i = 1; !mu_iterator_is_done (itr);
       mu_iterator_next (itr), i++)
    {
      struct atchinfo *aptr;
      if (mu_iterator_current (itr, (void**)&aptr))
	continue;
	  
      mu_printf ("%3d %-12s %-30s %-s\n",
		 i, aptr->id, aptr->content_type, aptr->encoding);
    }
  mu_iterator_destroy (&itr);

  return 0;
}
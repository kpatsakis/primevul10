add_header (char *name, char *value, int mode)
{
  struct add_header *hp;
  
  if (!add_header_list)
    {
      int rc = mu_list_create (&add_header_list);
      if (rc)
	{
	  mu_error (_("Cannot create header list: %s"), mu_strerror (rc));
	  exit (1);
	}
    }

  hp = mu_alloc (sizeof (*hp));
  hp->mode = mode;
  hp->name = name;
  hp->value = value;
  mu_list_append (add_header_list, hp);
}
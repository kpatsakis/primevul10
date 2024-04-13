compose_header_set (compose_env_t *env, const char *name,
		    const char *value, int mode)
{
  int status;
  char *expansion = NULL;
    
  if (!value || value[0] == 0)
    return EINVAL;

  if (is_address_field (name)
      && mailvar_is_true (mailvar_name_inplacealiases))
    {
      struct mu_address hint = MU_ADDRESS_HINT_INITIALIZER;
      mu_address_t a;
      
      status = mu_address_create_hint (&a, value, &hint, 0);
      if (status)
	{
	  mu_error (_("Cannot parse address `%s': %s"),
		    value, mu_strerror (status));
	  return status;
	}
      util_address_expand_aliases (&a);
      status = mu_address_aget_printable (a, &expansion);
      mu_address_destroy (&a);
      if (status)
	{
	  mu_diag_funcall (MU_DIAG_ERROR, "mu_address_aget_printable",
			   NULL, status);
	  return status;
	}
      value = expansion;
    }
  
  if (!env->header
      && (status = mu_header_create (&env->header, NULL, 0)) != 0)
    {
      mu_error (_("Cannot create header: %s"), mu_strerror (status));
    }
  else
    {
      switch (mode)
	{
	case COMPOSE_REPLACE:
	  status = mu_header_set_value (env->header, name, value, 1);
	  break;

	case COMPOSE_APPEND:
	  status = mu_header_append (env->header, name, value);
	  break;
      
	case COMPOSE_SINGLE_LINE:
	  {
	    char *old_value;
	    
	    if (mu_header_aget_value (env->header, name, &old_value) == 0
		&& old_value[0])
	      {
		if (is_address_field (name))
		  {
		    status = util_merge_addresses (&old_value, value);
		    if (status == 0)
		      status = mu_header_set_value (env->header, name,
						    old_value, 1);
		  }
		else
		  {
		    size_t size = strlen (old_value) + strlen (value) + 2;
		    char *p = realloc (old_value, size);
		    if (!p)
		      status = ENOMEM;
		    else
		      {
			old_value = p;
			strcat (old_value, ",");
			strcat (old_value, value);
			status = mu_header_set_value (env->header,
						      name, old_value,
						      1);
		      }
		  }
		free (old_value);
	      }
	    else
	      status = mu_header_set_value (env->header, name, value, 1);
	  }
	}
    }
  free (expansion);
  return status;
}
lt_dlsym (lt_dlhandle place, const char *symbol)
{
  size_t lensym;
  char	lsym[LT_SYMBOL_LENGTH];
  char	*sym;
  void *address;
  lt_user_data data;
  lt_dlhandle handle;

  if (!place)
    {
      LT__SETERROR (INVALID_HANDLE);
      return 0;
    }

  handle = place;

  if (!symbol)
    {
      LT__SETERROR (SYMBOL_NOT_FOUND);
      return 0;
    }

  lensym = LT_STRLEN (symbol) + LT_STRLEN (handle->vtable->sym_prefix)
					+ LT_STRLEN (handle->info.name);

  if (lensym + LT_SYMBOL_OVERHEAD < LT_SYMBOL_LENGTH)
    {
      sym = lsym;
    }
  else
    {
      sym = MALLOC (char, lensym + LT_SYMBOL_OVERHEAD + 1);
      if (!sym)
	{
	  LT__SETERROR (BUFFER_OVERFLOW);
	  return 0;
	}
    }

  data = handle->vtable->dlloader_data;
  if (handle->info.name)
    {
      const char *saved_error;

      LT__GETERROR (saved_error);

      /* this is a libtool module */
      if (handle->vtable->sym_prefix)
	{
	  strcpy(sym, handle->vtable->sym_prefix);
	  strcat(sym, handle->info.name);
	}
      else
	{
	  strcpy(sym, handle->info.name);
	}

      strcat(sym, "_LTX_");
      strcat(sym, symbol);

      /* try "modulename_LTX_symbol" */
      address = handle->vtable->find_sym (data, handle->module, sym);
      if (address)
	{
	  if (sym != lsym)
	    {
	      FREE (sym);
	    }
	  return address;
	}
      LT__SETERRORSTR (saved_error);
    }

  /* otherwise try "symbol" */
  if (handle->vtable->sym_prefix)
    {
      strcpy(sym, handle->vtable->sym_prefix);
      strcat(sym, symbol);
    }
  else
    {
      strcpy(sym, symbol);
    }

  address = handle->vtable->find_sym (data, handle->module, sym);
  if (sym != lsym)
    {
      FREE (sym);
    }

  return address;
}
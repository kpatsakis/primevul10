main_get_compressor (const char *ident)
{
  const main_extcomp *ext = main_ident_compressor (ident);

  if (ext == NULL)
    {
      if (! option_quiet)
	{
	  XPR(NT "warning: cannot recompress output: "
		   "unrecognized external compression ID: %s\n", ident);
	}
      return NULL;
    }
  else if (! EXTERNAL_COMPRESSION)
    {
      if (! option_quiet)
	{
	  XPR(NT "warning: external support not compiled: "
		   "original input was compressed: %s\n", ext->recomp_cmdname);
	}
      return NULL;
    }
  else
    {
      return ext;
    }
}
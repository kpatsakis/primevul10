main_set_secondary_flags (xd3_config *config)
{
  int ret;
  if (option_use_secondary)
    {
      /* The default secondary compressor is DJW, if it's compiled. */
      if (option_secondary == NULL)
	{
	  if (SECONDARY_DJW)
	    {
	      config->flags |= XD3_SEC_DJW;
	    }
	}
      else
	{
	  if (strcmp (option_secondary, "fgk") == 0 && SECONDARY_FGK)
	    {
	      config->flags |= XD3_SEC_FGK;
	    }
	  else if (strcmp (option_secondary, "lzma") == 0 && SECONDARY_LZMA)
	    {
	      config->flags |= XD3_SEC_LZMA;
	    }
	  else if (strncmp (option_secondary, "djw", 3) == 0 && SECONDARY_DJW)
	    {
	      usize_t level = XD3_DEFAULT_SECONDARY_LEVEL;

	      config->flags |= XD3_SEC_DJW;

	      if (strlen (option_secondary) > 3 &&
		  (ret = main_atou (option_secondary + 3,
				    &level,
				    0, 9, 'S')) != 0 &&
		  !option_quiet)
		{
		  return XD3_INVALID;
		}

	      /* XD3_SEC_NOXXXX flags disable secondary compression on
	       * a per-section basis.  For djw, ngroups=1 indicates
	       * minimum work, ngroups=0 uses default settings, which
	       * is > 1 groups by default. */
	      if (level < 1) { config->flags |= XD3_SEC_NODATA; }
	      if (level < 7) { config->sec_data.ngroups = 1; }
	      else { config->sec_data.ngroups = 0; }

	      if (level < 3) { config->flags |= XD3_SEC_NOINST; }
	      if (level < 8) { config->sec_inst.ngroups = 1; }
	      else { config->sec_inst.ngroups = 0; }

	      if (level < 5) { config->flags |= XD3_SEC_NOADDR; }
	      if (level < 9) { config->sec_addr.ngroups = 1; }
	      else { config->sec_addr.ngroups = 0; }
	    }
	  else if (strcmp (option_secondary, "none") == 0 && SECONDARY_DJW)
	    {
	      /* No secondary */
	    }
	  else
	    {
	      if (!option_quiet)
		{
		  XPR(NT "unrecognized secondary compressor type: %s\n",
		      option_secondary);
		  return XD3_INVALID;
		}
	    }
	}
    }

  return 0;
}
bool DTCollation::aggregate(DTCollation &dt, uint flags)
{
  if (!my_charset_same(collation, dt.collation))
  {
    /* 
       We do allow to use binary strings (like BLOBS)
       together with character strings.
       Binaries have more precedence than a character
       string of the same derivation.
    */
    if (collation == &my_charset_bin)
    {
      if (derivation <= dt.derivation)
      {
	/* Do nothing */
      }
      else
      {
	set(dt); 
      }
    }
    else if (dt.collation == &my_charset_bin)
    {
      if (dt.derivation <= derivation)
      {
        set(dt);
      }
    }
    else if ((flags & MY_COLL_ALLOW_SUPERSET_CONV) &&
             left_is_superset(this, &dt))
    {
      /* Do nothing */
    }
    else if ((flags & MY_COLL_ALLOW_SUPERSET_CONV) &&
             left_is_superset(&dt, this))
    {
      set(dt);
    }
    else if ((flags & MY_COLL_ALLOW_COERCIBLE_CONV) &&
             derivation < dt.derivation &&
             dt.derivation >= DERIVATION_SYSCONST)
    {
      /* Do nothing */
    }
    else if ((flags & MY_COLL_ALLOW_COERCIBLE_CONV) &&
             dt.derivation < derivation &&
             derivation >= DERIVATION_SYSCONST)
    {
      set(dt);
    }
    else
    {
      // Cannot apply conversion
      set(&my_charset_bin, DERIVATION_NONE,
          (dt.repertoire|repertoire));
      return 1;
    }
  }
  else if (derivation < dt.derivation)
  {
    /* Do nothing */
  }
  else if (dt.derivation < derivation)
  {
    set(dt);
  }
  else
  { 
    if (collation == dt.collation)
    {
      /* Do nothing */
    }
    else 
    {
      if (derivation == DERIVATION_EXPLICIT)
      {
        set(0, DERIVATION_NONE, 0);
        return 1;
      }
      if (collation->state & MY_CS_BINSORT)
        return 0;
      if (dt.collation->state & MY_CS_BINSORT)
      {
        set(dt);
        return 0;
      }
      CHARSET_INFO *bin= get_charset_by_csname(collation->csname, 
                                               MY_CS_BINSORT,MYF(0));
      set(bin, DERIVATION_NONE);
    }
  }
  repertoire|= dt.repertoire;
  return 0;
}
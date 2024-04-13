bool Item_type_holder::join_types(THD *thd, Item *item)
{
  uint max_length_orig= max_length;
  uint decimals_orig= decimals;
  DBUG_ENTER("Item_type_holder::join_types");
  DBUG_PRINT("info:", ("was type %d len %d, dec %d name %s",
                       fld_type, max_length, decimals,
                       (name ? name : "<NULL>")));
  DBUG_PRINT("info:", ("in type %d len %d, dec %d",
                       get_real_type(item),
                       item->max_length, item->decimals));
  fld_type= Field::field_type_merge(fld_type, get_real_type(item));
  {
    uint item_decimals= item->decimals;
    /* fix variable decimals which always is NOT_FIXED_DEC */
    if (Field::result_merge_type(fld_type) == INT_RESULT)
      item_decimals= 0;
    decimals= max(decimals, item_decimals);
  }

  if (fld_type == FIELD_TYPE_GEOMETRY)
    geometry_type=
      Field_geom::geometry_type_merge(geometry_type, item->get_geometry_type());

  if (Field::result_merge_type(fld_type) == DECIMAL_RESULT)
  {
    decimals= min(max(decimals, item->decimals), DECIMAL_MAX_SCALE);
    int item_int_part= item->decimal_int_part();
    int item_prec = max(prev_decimal_int_part, item_int_part) + decimals;
    int precision= min(item_prec, DECIMAL_MAX_PRECISION);
    unsigned_flag&= item->unsigned_flag;
    max_length= my_decimal_precision_to_length_no_truncation(precision,
                                                             decimals,
                                                             unsigned_flag);
  }

  switch (Field::result_merge_type(fld_type))
  {
  case STRING_RESULT:
  {
    const char *old_cs, *old_derivation;
    uint32 old_max_chars= max_length / collation.collation->mbmaxlen;
    old_cs= collation.collation->name;
    old_derivation= collation.derivation_name();
    if (collation.aggregate(item->collation, MY_COLL_ALLOW_CONV))
    {
      my_error(ER_CANT_AGGREGATE_2COLLATIONS, MYF(0),
	       old_cs, old_derivation,
	       item->collation.collation->name,
	       item->collation.derivation_name(),
	       "UNION");
      DBUG_RETURN(TRUE);
    }
    /*
      To figure out max_length, we have to take into account possible
      expansion of the size of the values because of character set
      conversions.
     */
    if (collation.collation != &my_charset_bin)
    {
      max_length= max(old_max_chars * collation.collation->mbmaxlen,
                      display_length(item) /
                      item->collation.collation->mbmaxlen *
                      collation.collation->mbmaxlen);
    }
    else
      set_if_bigger(max_length, display_length(item));
    break;
  }
  case REAL_RESULT:
  {
    if (decimals != NOT_FIXED_DEC)
    {
      /*
        For FLOAT(M,D)/DOUBLE(M,D) do not change precision
         if both fields have the same M and D
      */
      if (item->max_length != max_length_orig ||
          item->decimals != decimals_orig)
      {
        int delta1= max_length_orig - decimals_orig;
        int delta2= item->max_length - item->decimals;
        max_length= max(delta1, delta2) + decimals;
        if (fld_type == MYSQL_TYPE_FLOAT && max_length > FLT_DIG + 2)
        {
          max_length= MAX_FLOAT_STR_LENGTH;
          decimals= NOT_FIXED_DEC;
        } 
        else if (fld_type == MYSQL_TYPE_DOUBLE && max_length > DBL_DIG + 2)
        {
          max_length= MAX_DOUBLE_STR_LENGTH;
          decimals= NOT_FIXED_DEC;
        }
      }
    }
    else
      max_length= (fld_type == MYSQL_TYPE_FLOAT) ? FLT_DIG+6 : DBL_DIG+7;
    break;
  }
  default:
    max_length= max(max_length, display_length(item));
  };
  maybe_null|= item->maybe_null;
  get_full_info(item);

  /* Remember decimal integer part to be used in DECIMAL_RESULT handleng */
  prev_decimal_int_part= decimal_int_part();
  DBUG_PRINT("info", ("become type: %d  len: %u  dec: %u",
                      (int) fld_type, max_length, (uint) decimals));
  DBUG_RETURN(FALSE);
}
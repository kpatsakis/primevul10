adjust_max_effective_column_length(Field *field_par, uint32 max_length)
{
  uint32 new_max_length= field_par->max_display_length();
  uint32 sign_length= (field_par->flags & UNSIGNED_FLAG) ? 0 : 1;

  switch (field_par->type())
  {
  case MYSQL_TYPE_INT24:
    /*
      Compensate for MAX_MEDIUMINT_WIDTH being 1 too long (8)
      compared to the actual number of digits that can fit into
      the column.
    */
    new_max_length+= 1;
    /* fall through */
  case MYSQL_TYPE_LONG:
  case MYSQL_TYPE_TINY:
  case MYSQL_TYPE_SHORT:

    /* Take out the sign and add a conditional sign */
    new_max_length= new_max_length - 1 + sign_length;
    break;

  /* BINGINT is always 20 no matter the sign */
  case MYSQL_TYPE_LONGLONG:
  /* make gcc happy */
  default:
    break;
  }

  /* Adjust only if the actual precision based one is bigger than specified */
  return new_max_length > max_length ? new_max_length : max_length;
}
static int save_field_in_field(Field *from, bool *null_value,
                               Field *to, bool no_conversions)
{
  int res;
  DBUG_ENTER("save_field_in_field");
  if (from->is_null())
  {
    (*null_value)= 1;
    DBUG_RETURN(set_field_to_null_with_conversions(to, no_conversions));
  }
  to->set_notnull();

  /*
    If we're setting the same field as the one we're reading from there's 
    nothing to do. This can happen in 'SET x = x' type of scenarios.
  */
  if (to == from)
  {
    (*null_value)= 0;
    DBUG_RETURN(0);
  }

  res= field_conv(to, from);
  (*null_value)= 0;
  DBUG_RETURN(res);
}
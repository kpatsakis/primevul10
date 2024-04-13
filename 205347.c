uint get_errcode_from_name(const char *error_name, const char *error_end,
                            st_error *e)
{
  DBUG_ENTER("get_errcode_from_name");
  DBUG_PRINT("enter", ("error_name: %s", error_name));

  /* Loop through the array of known error names */
  for (; e->name; e++)
  {
    /*
      If we get a match, we need to check the length of the name we
      matched against in case it was longer than what we are checking
      (as in ER_WRONG_VALUE vs. ER_WRONG_VALUE_COUNT).
    */
    if (!strncmp(error_name, e->name, (int) (error_end - error_name)) &&
        (uint) strlen(e->name) == (uint) (error_end - error_name))
    {
      DBUG_RETURN(e->code);
    }
  }
  DBUG_RETURN(0);
}
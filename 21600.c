variable_completion_function (text, state)
     const char *text;
     int state;
{
  static char **varlist = (char **)NULL;
  static int varlist_index;
  static char *varname = (char *)NULL;
  static int first_char, first_char_loc;

  if (!state)
    {
      if (varname)
	free (varname);

      first_char_loc = 0;
      first_char = text[0];

      if (first_char == '$')
	first_char_loc++;

      if (text[first_char_loc] == '{')
	first_char_loc++;

      varname = savestring (text + first_char_loc);

      if (varlist)
	strvec_dispose (varlist);

      varlist = all_variables_matching_prefix (varname);
      varlist_index = 0;
    }

  if (!varlist || !varlist[varlist_index])
    {
      return ((char *)NULL);
    }
  else
    {
      char *value;

      value = (char *)xmalloc (4 + strlen (varlist[varlist_index]));

      if (first_char_loc)
	{
	  value[0] = first_char;
	  if (first_char_loc == 2)
	    value[1] = '{';
	}

      strcpy (value + first_char_loc, varlist[varlist_index]);
      if (first_char_loc == 2)
	strcat (value, "}");

      varlist_index++;
      return (value);
    }
}
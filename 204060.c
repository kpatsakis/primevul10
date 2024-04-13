modify_param_value (param_token *value, int encoding_type )
{
  if (encoding_type == RFC2231_ENCODING)
    {
      const char *delim = memrchr (value->b, '\'', value->e - value->b);
      if (delim != NULL)
        {
          value->b = (delim+1);
        }
    }
}
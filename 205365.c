const char *get_errname_from_code(uint error_code)
{
  const char *name;
  if ((name= get_errname_from_code(error_code, global_error_names)) !=
      unknown_error)
    return name;
  return get_errname_from_code(error_code, handler_error_names);
}
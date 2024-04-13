Item_param::clone_item()
{
  /* see comments in the header file */
  switch (state) {
  case NULL_VALUE:
    return new Item_null(name);
  case INT_VALUE:
    return (unsigned_flag ?
            new Item_uint(name, value.integer, max_length) :
            new Item_int(name, value.integer, max_length));
  case REAL_VALUE:
    return new Item_float(name, value.real, decimals, max_length);
  case STRING_VALUE:
  case LONG_DATA_VALUE:
    return new Item_string(name, str_value.c_ptr_quick(), str_value.length(),
                           str_value.charset());
  case TIME_VALUE:
    break;
  case NO_VALUE:
  default:
    DBUG_ASSERT(0);
  };
  return 0;
}
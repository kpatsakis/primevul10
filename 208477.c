int get_shada_parameter(int type)
{
  char_u  *p;

  p = find_shada_parameter(type);
  if (p != NULL && ascii_isdigit(*p))
    return atoi((char *)p);
  return -1;
}
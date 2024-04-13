hash(XML_Parser parser, KEY s)
{
  unsigned long h = get_hash_secret_salt(parser);
  while (*s)
    h = CHAR_HASH(h, *s++);
  return h;
}
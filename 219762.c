XML_MemRealloc(XML_Parser parser, void *ptr, size_t size)
{
  if (parser == NULL)
    return NULL;
  return REALLOC(ptr, size);
}
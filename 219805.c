XML_GetInputContext(XML_Parser parser, int *offset, int *size)
{
#ifdef XML_CONTEXT_BYTES
  if (parser == NULL)
    return NULL;
  if (eventPtr && buffer) {
    if (offset != NULL)
      *offset = (int)(eventPtr - buffer);
    if (size != NULL)
      *size   = (int)(bufferEnd - buffer);
    return buffer;
  }
#else
  (void)parser;
  (void)offset;
  (void)size;
#endif /* defined XML_CONTEXT_BYTES */
  return (char *) 0;
}
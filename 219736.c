errorProcessor(XML_Parser parser,
               const char *UNUSED_P(s),
               const char *UNUSED_P(end),
               const char **UNUSED_P(nextPtr))
{
  return errorCode;
}
XML_SetCdataSectionHandler(XML_Parser parser,
                           XML_StartCdataSectionHandler start,
                           XML_EndCdataSectionHandler end)
{
  if (parser == NULL)
    return;
  startCdataSectionHandler = start;
  endCdataSectionHandler = end;
}
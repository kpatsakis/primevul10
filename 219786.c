XML_SetUserData(XML_Parser parser, void *p)
{
  if (parser == NULL)
    return;
  if (handlerArg == userData)
    handlerArg = userData = p;
  else
    userData = p;
}
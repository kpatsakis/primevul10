xmlEntitiesErr(xmlParserErrors code, const char *msg)
{
    __xmlSimpleError(XML_FROM_TREE, code, NULL, msg, NULL);
}
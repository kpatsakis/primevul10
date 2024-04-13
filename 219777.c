XML_SetCharacterDataHandler(XML_Parser parser,
                            XML_CharacterDataHandler handler)
{
  if (parser != NULL)
    characterDataHandler = handler;
}
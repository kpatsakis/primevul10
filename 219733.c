XML_SetProcessingInstructionHandler(XML_Parser parser,
                                    XML_ProcessingInstructionHandler handler)
{
  if (parser != NULL)
    processingInstructionHandler = handler;
}
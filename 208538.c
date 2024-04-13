void Parser::OutOfMemory()
{
    throw ParseExceptionObject(ERRnoMemory);
}
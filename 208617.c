void Parser::ValidateSourceElementList()
{
    ParseStmtList<false>(nullptr, nullptr, SM_NotUsed, true);
}
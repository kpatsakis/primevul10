void Parser::AddToNodeListEscapedUse(ParseNode ** ppnodeList, ParseNode *** pppnodeLast,
                           ParseNode * pnodeAdd)
{
    AddToNodeList(ppnodeList, pppnodeLast, pnodeAdd);
    pnodeAdd->SetIsInList();
}
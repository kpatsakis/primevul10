void Parser::AddToNodeList(ParseNode ** ppnodeList, ParseNode *** pppnodeLast,
                           ParseNode * pnodeAdd)
{
    Assert(!this->m_deferringAST);
    if (nullptr == *pppnodeLast)
    {
        // should be an empty list
        Assert(nullptr == *ppnodeList);

        *ppnodeList = pnodeAdd;
        *pppnodeLast = ppnodeList;
    }
    else
    {
        //
        AssertNodeMem(*ppnodeList);
        AssertNodeMem(**pppnodeLast);

        ParseNode *pnodeT = CreateBinNode(knopList, **pppnodeLast, pnodeAdd);
        **pppnodeLast = pnodeT;
        *pppnodeLast = &pnodeT->sxBin.pnode2;
    }
}
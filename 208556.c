void Parser::AddFastScannedRegExpNode(ParseNodePtr const pnode)
{
    Assert(!IsBackgroundParser());
    Assert(m_doingFastScan);

    if (fastScannedRegExpNodes == nullptr)
    {
        fastScannedRegExpNodes = Anew(&m_nodeAllocator, NodeDList, &m_nodeAllocator);
    }
    fastScannedRegExpNodes->Append(pnode);
}
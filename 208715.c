void Parser::AddBackgroundRegExpNode(ParseNodePtr const pnode)
{
    Assert(IsBackgroundParser());
    Assert(currBackgroundParseItem != nullptr);

    currBackgroundParseItem->AddRegExpNode(pnode, &m_nodeAllocator);
}
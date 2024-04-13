void Parser::Error(HRESULT hr, ParseNodePtr pnode)
{
    if (pnode && pnode->ichLim)
    {
        Error(hr, pnode->ichMin, pnode->ichLim);
    }
    else
    {
        Error(hr);
    }
}
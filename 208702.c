void Parser::AddBackgroundParseItem(BackgroundParseItem *const item)
{
    if (currBackgroundParseItem == nullptr)
    {
        backgroundParseItems = item;
    }
    else
    {
        currBackgroundParseItem->SetNext(item);
    }
    currBackgroundParseItem = item;
}
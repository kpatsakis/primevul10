ParseNodePtr Parser::ParseArrayLiteral()
{
    ParseNodePtr pnode = nullptr;
    bool arrayOfTaggedInts = false;
    bool arrayOfInts = false;
    bool arrayOfNumbers = false;
    bool hasMissingValues = false;
    uint count = 0;
    uint spreadCount = 0;

    ParseNodePtr pnode1 = ParseArrayList<buildAST>(&arrayOfTaggedInts, &arrayOfInts, &arrayOfNumbers, &hasMissingValues, &count, &spreadCount);

    if (buildAST)
    {
        pnode = CreateNodeWithScanner<knopArray>();
        pnode->sxArrLit.pnode1 = pnode1;
        pnode->sxArrLit.arrayOfTaggedInts = arrayOfTaggedInts;
        pnode->sxArrLit.arrayOfInts = arrayOfInts;
        pnode->sxArrLit.arrayOfNumbers = arrayOfNumbers;
        pnode->sxArrLit.hasMissingValues = hasMissingValues;
        pnode->sxArrLit.count = count;
        pnode->sxArrLit.spreadCount = spreadCount;

        if (pnode->sxArrLit.pnode1)
        {
            this->CheckArguments(pnode->sxArrLit.pnode1);
        }
    }

    return pnode;
}
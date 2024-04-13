void Parser::IdentifierExpectedError(const Token& token)
{
    Assert(token.tk != tkID);

    HRESULT hr;
    if (token.IsReservedWord())
    {
        if (token.IsKeyword())
        {
            hr = ERRKeywordNotId;
        }
        else
        {
            Assert(token.IsFutureReservedWord(true));
            if (token.IsFutureReservedWord(false))
            {
                // Future reserved word in strict and non-strict modes
                hr = ERRFutureReservedWordNotId;
            }
            else
            {
                // Future reserved word only in strict mode. The token would have been converted to tkID by the scanner if not
                // in strict mode.
                Assert(IsStrictMode());
                hr = ERRFutureReservedWordInStrictModeNotId;
            }
        }
    }
    else
    {
        hr = ERRnoIdent;
    }

    Error(hr);
}
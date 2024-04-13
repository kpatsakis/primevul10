bool Parser::ScanAheadToFunctionEnd(uint count)
{
    bool found = false;
    uint curlyDepth = 0;

    RestorePoint funcStart;
    m_pscan->Capture(&funcStart);

    for (uint i = 0; i < count; i++)
    {
        switch (m_token.tk)
        {
            case tkStrTmplBegin:
            case tkStrTmplMid:
            case tkStrTmplEnd:
            case tkDiv:
            case tkAsgDiv:
            case tkScanError:
            case tkEOF:
                goto LEnd;

            case tkLCurly:
                UInt32Math::Inc(curlyDepth, Parser::OutOfMemory);
                break;

            case tkRCurly:
                if (curlyDepth == 1)
                {
                    found = true;
                    goto LEnd;
                }
                if (curlyDepth == 0)
                {
                    goto LEnd;
                }
                curlyDepth--;
                break;
        }

        m_pscan->ScanAhead();
    }

 LEnd:
    m_pscan->SeekTo(funcStart);
    return found;
}
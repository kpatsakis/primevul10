void Parser::CaptureContext(ParseContext *parseContext) const
{
    parseContext->pszSrc = m_pscan->PchBase();
    parseContext->length = this->m_originalLength;
    parseContext->characterOffset = m_pscan->IchMinTok();
    parseContext->offset = parseContext->characterOffset + m_pscan->m_cMultiUnits;
    parseContext->grfscr = this->m_grfscr;
    parseContext->lineNumber = m_pscan->LineCur();

    parseContext->pnodeProg = this->m_currentNodeProg;
    parseContext->fromExternal = m_pscan->IsFromExternalSource();
    parseContext->strictMode = this->IsStrictMode();
    parseContext->sourceContextInfo = this->m_sourceContextInfo;
    parseContext->currentBlockInfo = this->m_currentBlockInfo;
    parseContext->nextBlockId = this->m_nextBlockId;
}
void Parser::Error(HRESULT hr, charcount_t ichMin, charcount_t ichLim)
{
    m_pscan->SetErrorPosition(ichMin, ichLim);
    Error(hr);
}
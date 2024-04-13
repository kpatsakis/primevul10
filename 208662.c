LabelId* Parser::CreateLabelId(IdentToken* pToken)
{
    LabelId* pLabelId;

    pLabelId = (LabelId*)m_nodeAllocator.Alloc(sizeof(LabelId));
    if (NULL == pLabelId)
        Error(ERRnoMemory);
    pLabelId->pid = pToken->pid;
    pLabelId->next = NULL;

    return pLabelId;
}
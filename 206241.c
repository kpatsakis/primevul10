static int edipartyname_cmp(const EDIPARTYNAME *a, const EDIPARTYNAME *b)
{
    int res;

    if (a == NULL || b == NULL) {
        /*
         * Shouldn't be possible in a valid GENERAL_NAME, but we handle it
         * anyway. OTHERNAME_cmp treats NULL != NULL so we do the same here
         */
        return -1;
    }
    if (a->nameAssigner == NULL && b->nameAssigner != NULL)
        return -1;
    if (a->nameAssigner != NULL && b->nameAssigner == NULL)
        return 1;
    /* If we get here then both have nameAssigner set, or both unset */
    if (a->nameAssigner != NULL) {
        res = ASN1_STRING_cmp(a->nameAssigner, b->nameAssigner);
        if (res != 0)
            return res;
    }
    /*
     * partyName is required, so these should never be NULL. We treat it in
     * the same way as the a == NULL || b == NULL case above
     */
    if (a->partyName == NULL || b->partyName == NULL)
        return -1;

    return ASN1_STRING_cmp(a->partyName, b->partyName);
}
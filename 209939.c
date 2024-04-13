static void jsi_push_try(Jsi_Interp* interp, jsi_TryList **head, jsi_TryList *n)
{
    interp->tryList = n;
    interp->framePtr->tryDepth++;
    n->next = *head;
    *head = n;
}
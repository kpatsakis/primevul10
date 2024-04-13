static void jsi_pop_try(Jsi_Interp* interp, jsi_TryList **head)
{
    interp->framePtr->tryDepth--;
    jsi_TryList *t = (*head)->next;
    Jsi_Free(*head);
    *head = t;
    interp->tryList = t;
}
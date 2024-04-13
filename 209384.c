Jsi_ListEntry* Jsi_ListPop(Jsi_List *list, Jsi_ListEntry *item)
{
    Assert(item && list->head && list->tail && item->list);
    SIGASSERT(list, LIST);
    SIGASSERT(item, LISTENTRY);
    if (item == list->head) {
        if (list->head == list->tail)
            list->head = list->tail = NULL;
        else
            list->head = list->head->next;
    }
    else if (item == list->tail) {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
    } else {
        item->prev->next = item->next;
        if (item->next)
            item->next->prev = item->prev;
    }
    list->numEntries--;
    item->next = item->prev = NULL;
    return item;
}
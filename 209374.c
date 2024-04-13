Jsi_ListEntry* Jsi_ListPush(Jsi_List *list, Jsi_ListEntry *item, Jsi_ListEntry *before)
{
    Assert(item && list);
    if (item->list && (item->list->head == item || item->prev || item->next)) {
        Assert(list->opts.freeListProc == item->list->opts.freeListProc);
        Jsi_ListPop(item->list, item);
    }
        
    if (!item->list)
         item->list = list;
    else if (list != item->list) {
        Assert(list->opts.freeListProc == item->list->opts.freeListProc);
        item->list = list;
    }
    if (!list->head) {
        list->head = list->tail = item;
    } else if (item == list->head) {
        assert(0);
    } else if (before == NULL) {
        item->prev = list->tail;
        list->tail->next = item;
        list->tail = item;
    } else if (before == list->head) {
        item->next = list->head;
        list->head->prev = item;
        list->head = item;
    } else {
        item->next = before;
        item->prev = before->prev;
        before->prev->next = item;
        before->prev = item;
    }
    list->numEntries++;
    item->list = list;
    return item;
}
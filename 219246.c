    yaffscache_object_find(YAFFSFS_INFO *yfs, uint32_t obj_id, YaffsCacheObject **obj)
{
    YaffsCacheObject *curr, *prev;
    curr = yfs->cache_objects;
    prev = NULL;

    if (obj == NULL) {
        return TSK_ERR;
    }

    while(curr != NULL) {
        if (curr->yco_obj_id == obj_id) {
            *obj = curr;
            return TSK_OK;
        }
        else if (curr->yco_obj_id > obj_id) {
            *obj = prev;
            return TSK_STOP;
        }

        prev = curr;
        curr = curr->yco_next;
    }

    *obj = prev;
    return TSK_STOP;
}
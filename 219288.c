    yaffscache_object_find_or_add(YAFFSFS_INFO *yfs, uint32_t obj_id, YaffsCacheObject **obj)
{
    YaffsCacheObject *prev;
    TSK_RETVAL_ENUM result;

    if (obj == NULL) {
        return TSK_ERR;
    }

    // Look for this obj_id in yfs->cache_objects
    // If not found, add it in the correct spot
    // yaffscache_object_find returns the last object with obj_id less than the one
    // we were searching for, so use that to insert the new one in the list
    result = yaffscache_object_find(yfs, obj_id, &prev);
    if (result == TSK_OK) {
        *obj = prev;
        return TSK_OK;
    }
    else if (result == TSK_STOP) {
        *obj = (YaffsCacheObject *) tsk_malloc(sizeof(YaffsCacheObject));
        (*obj)->yco_obj_id = obj_id;
        if (prev == NULL) {
            (*obj)->yco_next = yfs->cache_objects;
            yfs->cache_objects = *obj;
        }
        else {
            (*obj)->yco_next = prev->yco_next;
            prev->yco_next = (*obj);
        }
        return TSK_OK;
    }
    else {
        *obj = NULL;
        return TSK_ERR;
    }
}
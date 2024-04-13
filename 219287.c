    yaffscache_chunk_compare(YaffsCacheChunk *curr, uint32_t addee_obj_id, TSK_OFF_T addee_offset, uint32_t addee_seq_number)
{
    if (curr->ycc_obj_id == addee_obj_id) {
        if (curr->ycc_seq_number == addee_seq_number) {
            if (curr->ycc_offset == addee_offset) {
                return 0;
            }
            else if (curr->ycc_offset < addee_offset) {
                return -1;
            }
            else {
                return 1;
            }
        }
        else if (curr->ycc_seq_number < addee_seq_number) {
            return -1;
        }
        else {
            return 1;
        }
    }
    else if (curr->ycc_obj_id < addee_obj_id) {
        return -1;
    }
    else {
        return 1;
    }
}
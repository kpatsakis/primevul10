MOBI_RET mobi_load_file(MOBIData *m, FILE *file) {
    MOBI_RET ret;
    if (m == NULL) {
        debug_print("%s", "Mobi structure not initialized\n");
        return MOBI_INIT_FAILED;
    }
    ret = mobi_load_pdbheader(m, file);
    if (ret != MOBI_SUCCESS) {
        return ret;
    }
    if (strcmp(m->ph->type, "BOOK") != 0 && strcmp(m->ph->type, "TEXt") != 0) {
        debug_print("Unsupported file type: %s\n", m->ph->type);
        return MOBI_FILE_UNSUPPORTED;
    }
    if (m->ph->rec_count == 0) {
        debug_print("%s", "No records found\n");
        return MOBI_DATA_CORRUPT;
    }
    ret = mobi_load_reclist(m, file);
    if (ret != MOBI_SUCCESS) {
        return ret;
    }
    ret = mobi_load_rec(m, file);
    if (ret != MOBI_SUCCESS) {
        return ret;
    }
    ret = mobi_parse_record0(m, 0);
    if (ret != MOBI_SUCCESS) {
        return ret;
    }
    if (m->rh && m->rh->encryption_type == RECORD0_OLD_ENCRYPTION) {
        /* try to set key for encryption type 1 */
        debug_print("Trying to set key for encryption type 1%s", "\n")
        mobi_drm_setkey(m, NULL);
    }
    /* if EXTH is loaded parse KF8 record0 for hybrid KF7/KF8 file */
    if (m->eh) {
        const size_t boundary_rec_number = mobi_get_kf8boundary_seqnumber(m);
        if (boundary_rec_number != MOBI_NOTSET && boundary_rec_number < UINT32_MAX) {
            /* it is a hybrid KF7/KF8 file */
            m->kf8_boundary_offset = (uint32_t) boundary_rec_number;
            m->next = mobi_init();
            /* link pdb header and records data to KF8data structure */
            m->next->ph = m->ph;
            m->next->rec = m->rec;
            m->next->drm_key = m->drm_key;
            /* close next loop */
            m->next->next = m;
            ret = mobi_parse_record0(m->next, boundary_rec_number + 1);
            if (ret != MOBI_SUCCESS) {
                return ret;
            }
            /* swap to kf8 part if use_kf8 flag is set */
            if (m->use_kf8) {
                mobi_swap_mobidata(m);
            }
        }
    }
    return MOBI_SUCCESS;
}
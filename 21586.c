MOBI_RET mobi_parse_mobiheader(MOBIData *m, MOBIBuffer *buf) {
    int isKF8 = 0;
    if (m == NULL) {
        debug_print("%s", "Mobi structure not initialized\n");
        return MOBI_INIT_FAILED;
    }
    m->mh = calloc(1, sizeof(MOBIMobiHeader));
    if (m->mh == NULL) {
        debug_print("%s", "Memory allocation for MOBI header failed\n");
        return MOBI_MALLOC_FAILED;
    }
    mobi_buffer_getstring(m->mh->mobi_magic, buf, 4);
    mobi_buffer_dup32(&m->mh->header_length, buf);
    if (strcmp(m->mh->mobi_magic, MOBI_MAGIC) != 0 || m->mh->header_length == NULL) {
        debug_print("%s", "MOBI header not found\n");
        mobi_free_mh(m->mh);
        m->mh = NULL;
        return MOBI_DATA_CORRUPT;
    }
    const size_t saved_maxlen = buf->maxlen;
    /* some old files declare zero length mobi header, try to read first 24 bytes anyway */
    uint32_t header_length = (*m->mh->header_length > 0) ? *m->mh->header_length : 24;
    /* read only declared MOBI header length (curr offset minus 8 already read bytes) */
    const size_t left_length = header_length + buf->offset - 8;
    buf->maxlen = saved_maxlen < left_length ? saved_maxlen : left_length;
    mobi_buffer_dup32(&m->mh->mobi_type, buf);
    uint32_t encoding = mobi_buffer_get32(buf);
    if (encoding == 1252) {
        m->mh->text_encoding = malloc(sizeof(MOBIEncoding));
        if (m->mh->text_encoding == NULL) {
            debug_print("%s", "Memory allocation for MOBI header failed\n");
            return MOBI_MALLOC_FAILED;
        }
        *m->mh->text_encoding = MOBI_CP1252;
    }
    else if (encoding == 65001) {
        m->mh->text_encoding = malloc(sizeof(MOBIEncoding));
        if (m->mh->text_encoding == NULL) {
            debug_print("%s", "Memory allocation for MOBI header failed\n");
            return MOBI_MALLOC_FAILED;
        }
        *m->mh->text_encoding = MOBI_UTF8;
    } else {
        debug_print("Unknown encoding in mobi header: %i\n", encoding);
    }
    mobi_buffer_dup32(&m->mh->uid, buf);
    mobi_buffer_dup32(&m->mh->version, buf);
    if (header_length >= MOBI_HEADER_V7_SIZE
        && m->mh->version && *m->mh->version == 8) {
        isKF8 = 1;
    }
    mobi_buffer_dup32(&m->mh->orth_index, buf);
    mobi_buffer_dup32(&m->mh->infl_index, buf);
    mobi_buffer_dup32(&m->mh->names_index, buf);
    mobi_buffer_dup32(&m->mh->keys_index, buf);
    mobi_buffer_dup32(&m->mh->extra0_index, buf);
    mobi_buffer_dup32(&m->mh->extra1_index, buf);
    mobi_buffer_dup32(&m->mh->extra2_index, buf);
    mobi_buffer_dup32(&m->mh->extra3_index, buf);
    mobi_buffer_dup32(&m->mh->extra4_index, buf);
    mobi_buffer_dup32(&m->mh->extra5_index, buf);
    mobi_buffer_dup32(&m->mh->non_text_index, buf);
    mobi_buffer_dup32(&m->mh->full_name_offset, buf);
    mobi_buffer_dup32(&m->mh->full_name_length, buf);
    mobi_buffer_dup32(&m->mh->locale, buf);
    mobi_buffer_dup32(&m->mh->dict_input_lang, buf);
    mobi_buffer_dup32(&m->mh->dict_output_lang, buf);
    mobi_buffer_dup32(&m->mh->min_version, buf);
    mobi_buffer_dup32(&m->mh->image_index, buf);
    mobi_buffer_dup32(&m->mh->huff_rec_index, buf);
    mobi_buffer_dup32(&m->mh->huff_rec_count, buf);
    mobi_buffer_dup32(&m->mh->datp_rec_index, buf);
    mobi_buffer_dup32(&m->mh->datp_rec_count, buf);
    mobi_buffer_dup32(&m->mh->exth_flags, buf);
    mobi_buffer_seek(buf, 32); /* 32 unknown bytes */
    mobi_buffer_dup32(&m->mh->unknown6, buf);
    mobi_buffer_dup32(&m->mh->drm_offset, buf);
    mobi_buffer_dup32(&m->mh->drm_count, buf);
    mobi_buffer_dup32(&m->mh->drm_size, buf);
    mobi_buffer_dup32(&m->mh->drm_flags, buf);
    mobi_buffer_seek(buf, 8); /* 8 unknown bytes */
    if (isKF8) {
        mobi_buffer_dup32(&m->mh->fdst_index, buf);
    } else {
        mobi_buffer_dup16(&m->mh->first_text_index, buf);
        mobi_buffer_dup16(&m->mh->last_text_index, buf);
    }
    mobi_buffer_dup32(&m->mh->fdst_section_count, buf);
    mobi_buffer_dup32(&m->mh->fcis_index, buf);
    mobi_buffer_dup32(&m->mh->fcis_count, buf);
    mobi_buffer_dup32(&m->mh->flis_index, buf);
    mobi_buffer_dup32(&m->mh->flis_count, buf);
    mobi_buffer_dup32(&m->mh->unknown10, buf);
    mobi_buffer_dup32(&m->mh->unknown11, buf);
    mobi_buffer_dup32(&m->mh->srcs_index, buf);
    mobi_buffer_dup32(&m->mh->srcs_count, buf);
    mobi_buffer_dup32(&m->mh->unknown12, buf);
    mobi_buffer_dup32(&m->mh->unknown13, buf);
    mobi_buffer_seek(buf, 2); /* 2 byte fill */
    mobi_buffer_dup16(&m->mh->extra_flags, buf);
    mobi_buffer_dup32(&m->mh->ncx_index, buf);
    if (isKF8) {
        mobi_buffer_dup32(&m->mh->fragment_index, buf);
        mobi_buffer_dup32(&m->mh->skeleton_index, buf);
    } else {
        mobi_buffer_dup32(&m->mh->unknown14, buf);
        mobi_buffer_dup32(&m->mh->unknown15, buf);
    }
    mobi_buffer_dup32(&m->mh->datp_index, buf);
    if (isKF8) {
        mobi_buffer_dup32(&m->mh->guide_index, buf);
    } else {
        mobi_buffer_dup32(&m->mh->unknown16, buf);
    }
    mobi_buffer_dup32(&m->mh->unknown17, buf);
    mobi_buffer_dup32(&m->mh->unknown18, buf);
    mobi_buffer_dup32(&m->mh->unknown19, buf);
    mobi_buffer_dup32(&m->mh->unknown20, buf);
    if (buf->maxlen > buf->offset) {
        debug_print("Skipping %zu unknown bytes in MOBI header\n", (buf->maxlen - buf->offset));
        mobi_buffer_setpos(buf, buf->maxlen);
    }
    buf->maxlen = saved_maxlen;
    /* get full name stored at m->mh->full_name_offset */
    if (m->mh->full_name_offset && m->mh->full_name_length) {
        const size_t saved_offset = buf->offset;
        const uint32_t full_name_length = min(*m->mh->full_name_length, MOBI_TITLE_SIZEMAX);
        mobi_buffer_setpos(buf, *m->mh->full_name_offset);
        m->mh->full_name = malloc(full_name_length + 1);
        if (m->mh->full_name == NULL) {
            debug_print("%s", "Memory allocation for full name failed\n");
            return MOBI_MALLOC_FAILED;
        }
        if (full_name_length) {
            mobi_buffer_getstring(m->mh->full_name, buf, full_name_length);
        } else {
            m->mh->full_name[0] = '\0';
        }
        mobi_buffer_setpos(buf, saved_offset);
    }
    return MOBI_SUCCESS;
}
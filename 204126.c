decode_pe_resource_id (WinLibrary *fi, WinResource *wr, uint32_t value)
{
    if (value & IMAGE_RESOURCE_NAME_IS_STRING) {	/* numeric id */
        int c, len;
        uint16_t *mem = (uint16_t *)
          (fi->first_resource + (value & ~IMAGE_RESOURCE_NAME_IS_STRING));

        /* copy each char of the string, and terminate it */
        RETURN_IF_BAD_POINTER(false, *mem);
        len = mem[0];
        RETURN_IF_BAD_OFFSET(false, &mem[1], sizeof(uint16_t) * len);

        len = MIN(mem[0], WINRES_ID_MAXLEN);
        for (c = 0 ; c < len ; c++)
            wr->id[c] = mem[c+1] & 0x00FF;
        wr->id[len] = '\0';
    } else {					/* Unicode string id */
        /* translate id into a string */
        snprintf(wr->id, WINRES_ID_MAXLEN, "%" PRIu32, value);
    }

    wr->numeric_id = (value & IMAGE_RESOURCE_NAME_IS_STRING ? false:true);
    return true;
}
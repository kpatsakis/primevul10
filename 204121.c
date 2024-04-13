decode_ne_resource_id (WinLibrary *fi, WinResource *wr, uint16_t value)
{
    if (value & NE_RESOURCE_NAME_IS_NUMERIC) {		/* numeric id */
        /* translate id into a string */
        snprintf(wr->id, WINRES_ID_MAXLEN, "%d", value & ~NE_RESOURCE_NAME_IS_NUMERIC);
    } else {					/* ASCII string id */
        unsigned char len;
        char *mem = (char *) NE_HEADER(fi->memory)
                             + NE_HEADER(fi->memory)->rsrctab
                             + value;

        /* copy each char of the string, and terminate it */
        RETURN_IF_BAD_POINTER(false, *mem);
        len = mem[0];
        RETURN_IF_BAD_OFFSET(false, &mem[1], sizeof(char) * len);
        memcpy(wr->id, &mem[1], len);
        wr->id[len] = '\0';
    }

    wr->numeric_id = (value & NE_RESOURCE_NAME_IS_NUMERIC ? true:false);
    return true;
}
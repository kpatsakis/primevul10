static bool checkreturn decode_callback_field(pb_istream_t *stream, pb_wire_type_t wire_type, pb_field_iter_t *iter)
{
    pb_callback_t *pCallback = (pb_callback_t*)iter->pData;
#ifdef PB_OLD_CALLBACK_STYLE
    void *arg;
#else
    void **arg;
#endif
    
    if (pCallback == NULL || pCallback->funcs.decode == NULL)
        return pb_skip_field(stream, wire_type);

#ifdef PB_OLD_CALLBACK_STYLE
    arg = pCallback->arg;
#else
    arg = &(pCallback->arg);
#endif
    
    if (wire_type == PB_WT_STRING)
    {
        pb_istream_t substream;
        
        if (!pb_make_string_substream(stream, &substream))
            return false;
        
        do
        {
            if (!pCallback->funcs.decode(&substream, iter->pos, arg))
                PB_RETURN_ERROR(stream, "callback failed");
        } while (substream.bytes_left);
        
        if (!pb_close_string_substream(stream, &substream))
            return false;

        return true;
    }
    else
    {
        /* Copy the single scalar value to stack.
         * This is required so that we can limit the stream length,
         * which in turn allows to use same callback for packed and
         * not-packed fields. */
        pb_istream_t substream;
        pb_byte_t buffer[10];
        size_t size = sizeof(buffer);
        
        if (!read_raw_value(stream, wire_type, buffer, &size))
            return false;
        substream = pb_istream_from_buffer(buffer, size);
        
        return pCallback->funcs.decode(&substream, iter->pos, arg);
    }
}
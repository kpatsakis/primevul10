static void pb_message_set_to_defaults(const pb_field_t fields[], void *dest_struct)
{
    pb_field_iter_t iter;

    if (!pb_field_iter_begin(&iter, fields, dest_struct))
        return; /* Empty message type */
    
    do
    {
        pb_field_set_to_default(&iter);
    } while (pb_field_iter_next(&iter));
}
cr_input_get_cur_pos (CRInput const * a_this, CRInputPos * a_pos)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this) && a_pos,
                              CR_BAD_PARAM_ERROR);

        a_pos->next_byte_index = PRIVATE (a_this)->next_byte_index;
        a_pos->line = PRIVATE (a_this)->line;
        a_pos->col = PRIVATE (a_this)->col;
        a_pos->end_of_line = PRIVATE (a_this)->end_of_line;
        a_pos->end_of_file = PRIVATE (a_this)->end_of_input;

        return CR_OK;
}
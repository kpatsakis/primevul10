cr_input_set_cur_pos (CRInput * a_this, CRInputPos const * a_pos)
{
        g_return_val_if_fail (a_this && PRIVATE (a_this) && a_pos,
                              CR_BAD_PARAM_ERROR);

        cr_input_set_column_num (a_this, a_pos->col);
        cr_input_set_line_num (a_this, a_pos->line);
        cr_input_set_cur_index (a_this, a_pos->next_byte_index);
        cr_input_set_end_of_line (a_this, a_pos->end_of_line);
        cr_input_set_end_of_file (a_this, a_pos->end_of_file);

        return CR_OK;
}
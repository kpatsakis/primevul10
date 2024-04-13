ins_compl_mode(void)
{
    if (ctrl_x_mode_not_defined_yet() || ctrl_x_mode_scroll() || compl_started)
	return (char_u *)ctrl_x_mode_names[ctrl_x_mode & ~CTRL_X_WANT_IDENT];

    return (char_u *)"";
}
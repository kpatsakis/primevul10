corruption_error(char *mesg, char_u *file_name)
{
    EMSG3(_("E825: Corrupted undo file (%s): %s"), mesg, file_name);
}
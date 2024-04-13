parse_file_access_string(const ref *op, char file_access[4])
{
    const byte *astr;

    check_read_type(*op, t_string);
    astr = op->value.const_bytes;
    switch (r_size(op)) {
        case 2:
            if (astr[1] != '+')
                return_error(gs_error_invalidfileaccess);
            file_access[1] = '+';
            file_access[2] = 0;
            break;
        case 1:
            file_access[1] = 0;
            break;
        default:
            return_error(gs_error_invalidfileaccess);
    }
    switch (astr[0]) {
        case 'r':
        case 'w':
        case 'a':
            break;
        default:
            return_error(gs_error_invalidfileaccess);
    }
    file_access[0] = astr[0];
    return 0;
}
vpnprintf(char *string, int size_in, const char *f, va_list args)
{
    int f_idx = 0;
    int s_idx = 0;
    int f_len = strlen_sigsafe(f);
    char *string_arg;
    char number[21];
    int p_len;
    int i;
    uint64_t ui;
    int64_t si;
    size_t size = size_in;
    int precision;

    for (; f_idx < f_len && s_idx < size - 1; f_idx++) {
        int length_modifier = 0;
        if (f[f_idx] != '%') {
            string[s_idx++] = f[f_idx];
            continue;
        }

        f_idx++;

        /* silently swallow minimum field width */
        if (f[f_idx] == '*') {
            f_idx++;
            va_arg(args, int);
        } else {
            while (f_idx < f_len && ((f[f_idx] >= '0' && f[f_idx] <= '9')))
                f_idx++;
        }

        /* is there a precision? */
        precision = size;
        if (f[f_idx] == '.') {
            f_idx++;
            if (f[f_idx] == '*') {
                f_idx++;
                /* precision is supplied in an int argument */
                precision = va_arg(args, int);
            } else {
                /* silently swallow precision digits */
                while (f_idx < f_len && ((f[f_idx] >= '0' && f[f_idx] <= '9')))
                    f_idx++;
            }
        }

        /* non-digit length modifiers */
        if (f_idx < f_len) {
            int parsed_bytes = parse_length_modifier(&f[f_idx], f_len - f_idx, &length_modifier);
            if (parsed_bytes < 0)
                return 0;
            f_idx += parsed_bytes;
        }

        if (f_idx >= f_len)
            break;

        switch (f[f_idx]) {
        case 's':
            string_arg = va_arg(args, char*);

            for (i = 0; string_arg[i] != 0 && s_idx < size - 1 && s_idx < precision; i++)
                string[s_idx++] = string_arg[i];
            break;

        case 'u':
            if (length_modifier & LMOD_LONGLONG)
                ui = va_arg(args, unsigned long long);
            else if (length_modifier & LMOD_LONG)
                ui = va_arg(args, unsigned long);
            else if (length_modifier & LMOD_SIZET)
                ui = va_arg(args, size_t);
            else
                ui = va_arg(args, unsigned);

            FormatUInt64(ui, number);
            p_len = strlen_sigsafe(number);

            for (i = 0; i < p_len && s_idx < size - 1; i++)
                string[s_idx++] = number[i];
            break;
        case 'i':
        case 'd':
            if (length_modifier & LMOD_LONGLONG)
                si = va_arg(args, long long);
            else if (length_modifier & LMOD_LONG)
                si = va_arg(args, long);
            else if (length_modifier & LMOD_SIZET)
                si = va_arg(args, ssize_t);
            else
                si = va_arg(args, int);

            FormatInt64(si, number);
            p_len = strlen_sigsafe(number);

            for (i = 0; i < p_len && s_idx < size - 1; i++)
                string[s_idx++] = number[i];
            break;

        case 'p':
            string[s_idx++] = '0';
            if (s_idx < size - 1)
                string[s_idx++] = 'x';
            ui = (uintptr_t)va_arg(args, void*);
            FormatUInt64Hex(ui, number);
            p_len = strlen_sigsafe(number);

            for (i = 0; i < p_len && s_idx < size - 1; i++)
                string[s_idx++] = number[i];
            break;

        case 'x':
            if (length_modifier & LMOD_LONGLONG)
                ui = va_arg(args, unsigned long long);
            else if (length_modifier & LMOD_LONG)
                ui = va_arg(args, unsigned long);
            else if (length_modifier & LMOD_SIZET)
                ui = va_arg(args, size_t);
            else
                ui = va_arg(args, unsigned);

            FormatUInt64Hex(ui, number);
            p_len = strlen_sigsafe(number);

            for (i = 0; i < p_len && s_idx < size - 1; i++)
                string[s_idx++] = number[i];
            break;
        case 'f':
            {
                double d = va_arg(args, double);
                FormatDouble(d, number);
                p_len = strlen_sigsafe(number);

                for (i = 0; i < p_len && s_idx < size - 1; i++)
                    string[s_idx++] = number[i];
            }
            break;
        case 'c':
            {
                char c = va_arg(args, int);
                if (s_idx < size - 1)
                    string[s_idx++] = c;
            }
            break;
        case '%':
            string[s_idx++] = '%';
            break;
        default:
            BUG_WARN_MSG(f[f_idx], "Unsupported printf directive '%c'\n", f[f_idx]);
            va_arg(args, char*);
            string[s_idx++] = '%';
            if (s_idx < size - 1)
                string[s_idx++] = f[f_idx];
            break;
        }
    }

    string[s_idx] = '\0';

    return s_idx;
}
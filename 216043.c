R_API int r_str_word_set0_stack(char *str) {
	int i;
	char *p, *q;
	RStack *s;
	void *pop;
	if (!str || !*str) {
		return 0;
	}
	for (i = 0; str[i] && str[i+1]; i++) {
		if (i > 0 && str[i - 1] == ' ' && str[i] == ' ') {
			memmove (str + i, str + i + 1, strlen (str + i));
			i--;
		}
		if (i == 0 && str[i] == ' ') {
			memmove (str + i, str + i + 1, strlen (str + i));
		}
	}
	if (str[i] == ' ') {
		str[i] = 0;
	}
	s = r_stack_new (5); //Some random number
	for (i = 1, p = str; *p; p++) {
		q = p - 1;
		if (p > str && (*q == '\\')) {
			memmove (q, p, strlen (p) + 1);
			p--;
			continue;
		}
		switch (*p) {
		case '(':
		case '{':
		case '[':
			r_stack_push (s, (void *)p);
			continue;
		case '\'':
		case '"':
			pop = r_stack_pop (s);
			if (pop && *(char *)pop != *p) {
				r_stack_push (s, pop);
				r_stack_push (s, (void *)p);
			} else if (!pop) {
				r_stack_push (s, (void *)p);
			}
			continue;
		case ')':
		case '}':
		case ']':
			pop = r_stack_pop (s);
			if (pop) {
				if ((*(char *)pop == '(' && *p == ')') ||
					(*(char *)pop == '{' && *p == '}') ||
					(*(char *)pop == '[' && *p == ']')) {
					continue;
				}
			}
			break;
		case ' ':
			if (p > str && !*q) {
				memmove (p, p+1, strlen (p + 1) + 1);
				if (*q == '\\') {
					*q = ' ';
					continue;
				}
				p--;
			}
			if (r_stack_is_empty (s)) {
				i++;
				*p = '\0';
			}
		default:
			break;
		}
	}
	r_stack_free (s);
	return i;
}
trim(char *str)
{
	int i = 0;
	int j = 0;

	/* Remove spaces and tabs from beginning */
	while ( (str[i] == ' ') || (str[i] == '\t') ) {
		i++;
	}
	if (i > 0) {
		for (j = i; j < strlen(str); j++) {
			str[j-i] = str[j];
		}
		str[j-i] = '\0';
	}

	/* Now remove spaces and tabs from end */
	i = strlen(str) - 1;
	while ( (str[i] == ' ') || (str[i] == '\t')) {
		i--;
	}
	if ( i < (strlen(str) - 1) ) {
		str[i+1] = '\0';
	}
}
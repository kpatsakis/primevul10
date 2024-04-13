convert_arg_to_entry (const char *set_value, ExifEntry *e, ExifByteOrder o, ExifLog *log)
{
	unsigned int i, numcomponents;
	char *value_p, *buf;

	 /*
	 * ASCII strings are handled separately,
	 * since they don't require any conversion.
	 */
        if (e->format == EXIF_FORMAT_ASCII ||
	    e->tag == EXIF_TAG_USER_COMMENT) {
		if (e->data) free (e->data);
		e->components = strlen (set_value) + 1;
		if (e->tag == EXIF_TAG_USER_COMMENT)
			e->components += 8 - 1;
		e->size = sizeof (char) * e->components;
		e->data = malloc (e->size);
                if (!e->data) {
                        fprintf (stderr, _("Not enough memory."));
                        fputc ('\n', stderr);
                        exit (1);
                }
		if (e->tag == EXIF_TAG_USER_COMMENT) {
			/* assume ASCII charset */
			/* TODO: get this from the current locale */
			memcpy ((char *) e->data, "ASCII\0\0\0", 8);
			memcpy ((char *) e->data + 8, set_value, 
				strlen (set_value));
		} else
			strcpy ((char *) e->data, set_value);
                return;
	}

	/*
	 * Make sure we can handle this entry
	 */
	if ((e->components == 0) && *set_value) {
		fprintf (stderr, _("Setting a value for this tag "
				   "is unsupported!"));
		fputc ('\n', stderr);
		exit (1);
	}

	/* Copy the string so we can modify it */
	buf = strdup(set_value);
	if (!buf) exit(1);
	value_p = strtok(buf, SEP);
	numcomponents = e->components;
	for (i = 0; i < numcomponents; ++i) {
		unsigned char s;

		if (!value_p) {
				fprintf (stderr, _("Too few components specified "
					  "(need %d, found %d)\n"), numcomponents, i);
				exit (1);
		}
		if (!isdigit(*value_p) && (*value_p != '+') && (*value_p != '-')) {
				fprintf (stderr, _("Numeric value expected\n"));
				exit (1);
		}

		s = exif_format_get_size (e->format);
		switch (e->format) {
		case EXIF_FORMAT_ASCII:
			exif_log (log, -1, "exif", _("Internal error. "
				"Please contact <%s>."), PACKAGE_BUGREPORT);
			break;
		case EXIF_FORMAT_SHORT:
			exif_set_short (e->data + (s * i), o, atoi (value_p));
			break;
		case EXIF_FORMAT_SSHORT:
			exif_set_sshort (e->data + (s * i), o, atoi (value_p));
			break;
		case EXIF_FORMAT_RATIONAL:
			/*
			 * Hack to simplify the loop for rational numbers.
			 * Should really be using exif_set_rational instead
			 */
			if (i == 0) numcomponents *= 2;
			s /= 2;
			/* Fall through to LONG handler */
		case EXIF_FORMAT_LONG:
			exif_set_long (e->data + (s * i), o, atol (value_p));
			break;
		case EXIF_FORMAT_SRATIONAL:
			/*
			 * Hack to simplify the loop for rational numbers.
			 * Should really be using exif_set_srational instead
			 */
			if (i == 0) numcomponents *= 2;
			s /= 2;
			/* Fall through to SLONG handler */
		case EXIF_FORMAT_SLONG:
			exif_set_slong (e->data + (s * i), o, atol (value_p));
			break;
		case EXIF_FORMAT_BYTE:
		case EXIF_FORMAT_SBYTE:
		case EXIF_FORMAT_UNDEFINED: /* treat as byte array */
			e->data[s * i] = atoi (value_p);
			break;
		case EXIF_FORMAT_FLOAT:
		case EXIF_FORMAT_DOUBLE:
		default:
			fprintf (stderr, _("Not yet implemented!"));
			fputc ('\n', stderr);
			exit (1);
		}
		value_p = strtok(NULL, SEP);
	}
	free(buf);
	if (value_p) {
		fprintf (stderr, _("Warning; Too many components specified!"));
		fputc ('\n', stderr);
	}
}
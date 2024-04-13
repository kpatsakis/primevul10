_g_mime_type_get_from_content (char  *buffer,
			       gsize  buffer_size)
{
#if ENABLE_MAGIC

	static magic_t magic = NULL;

	if (magic == NULL) {
		magic = magic_open (MAGIC_MIME_TYPE);
		if (magic != NULL)
			magic_load (magic, NULL);
		else
			g_warning ("unable to open magic database");
	}

	if (magic != NULL) {
		const char * mime_type;

		mime_type = magic_buffer (magic, buffer, buffer_size);
		if (mime_type)
			return mime_type;

		g_warning ("unable to detect filetype from magic: %s", magic_error (magic));
	}

#else

	static const struct magic {
		const unsigned int off;
		const unsigned int len;
		const char * const id;
		const char * const mime_type;
	}
	magic_ids [] = {
		/* magic ids taken from magic/Magdir/archive from the file-4.21 tarball */
		{ 0,  6, "7z\274\257\047\034",                   "application/x-7z-compressed" },
		{ 7,  7, "**ACE**",                              "application/x-ace"           },
		{ 0,  2, "\x60\xea",                             "application/x-arj"           },
		{ 0,  3, "BZh",                                  "application/x-bzip2"         },
		{ 0,  2, "\037\213",                             "application/x-gzip"          },
		{ 0,  4, "LZIP",                                 "application/x-lzip"          },
		{ 0,  9, "\x89\x4c\x5a\x4f\x00\x0d\x0a\x1a\x0a", "application/x-lzop",         },
		{ 0,  4, "Rar!",                                 "application/x-rar"           },
		{ 0,  4, "RZIP",                                 "application/x-rzip"          },
		{ 0,  6, "\3757zXZ\000",                         "application/x-xz"            },
		{ 20, 4, "\xdc\xa7\xc4\xfd",                     "application/x-zoo",          },
		{ 0,  4, "PK\003\004",                           "application/zip"             },
		{ 0,  8, "PK00PK\003\004",                       "application/zip"             },
		{ 0,  4, "LRZI",                                 "application/x-lrzip"         },
	};

	int  i;

	for (i = 0; i < G_N_ELEMENTS (magic_ids); i++) {
		const struct magic * const magic = &magic_ids[i];

		if ((magic->off + magic->len) > buffer_size)
			g_warning ("buffer underrun for mime-type '%s' magic", magic->mime_type);
		else if (! memcmp (buffer + magic->off, magic->id, magic->len))
			return magic->mime_type;
	}

#endif

	return NULL;
}
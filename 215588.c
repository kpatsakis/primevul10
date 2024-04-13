GF_Err gf_isom_set_handler_name(GF_ISOFile *the_file, u32 trackNumber, const char *nameUTF8)
{
	GF_TrackBox *trak;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	if (trak->Media->handler->nameUTF8) gf_free(trak->Media->handler->nameUTF8);
	trak->Media->handler->nameUTF8 = NULL;

	if (!nameUTF8) return GF_OK;

	if (!strnicmp(nameUTF8, "file://", 7)) {
		u8 BOM[4];
		FILE *f = gf_fopen(nameUTF8+7, "rb");
		u64 size;
		if (!f) return GF_URL_ERROR;
		size = gf_fsize(f);
		if (3!=gf_fread(BOM, 3, f)) {
			gf_fclose(f);
			return GF_CORRUPTED_DATA;
		}
		/*skip BOM if any*/
		if ((BOM[0]==0xEF) && (BOM[1]==0xBB) && (BOM[2]==0xBF)) size -= 3;
		else if ((BOM[0]==0xEF) || (BOM[0]==0xFF)) {
			gf_fclose(f);
			return GF_BAD_PARAM;
		}
		else gf_fseek(f, 0, SEEK_SET);
		trak->Media->handler->nameUTF8 = (char*)gf_malloc(sizeof(char)*(size_t)(size+1));
		if (!trak->Media->handler->nameUTF8) {
			gf_fclose(f);
			return GF_OUT_OF_MEM;
		}
		size = gf_fread(trak->Media->handler->nameUTF8, (size_t)size, f);
		trak->Media->handler->nameUTF8[size] = 0;
		gf_fclose(f);
	} else {
		u32 i, j, len;
		char szOrig[1024], szLine[1024];
		strcpy(szOrig, nameUTF8);
		j=0;
		len = (u32) strlen(szOrig);
		for (i=0; i<len; i++) {
			if (szOrig[i] & 0x80) {
				/*non UTF8 (likely some win-CP)*/
				if ( (szOrig[i+1] & 0xc0) != 0x80) {
					szLine[j] = 0xc0 | ( (szOrig[i] >> 6) & 0x3 );
					j++;
					szOrig[i] &= 0xbf;
				}
				/*UTF8 2 bytes char */
				else if ( (szOrig[i] & 0xe0) == 0xc0) {
					szLine[j] = szOrig[i];
					i++;
					j++;
				}
				/*UTF8 3 bytes char */
				else if ( (szOrig[i] & 0xf0) == 0xe0) {
					szLine[j] = szOrig[i];
					i++;
					j++;
					szLine[j] = szOrig[i];
					i++;
					j++;
				}
				/*UTF8 4 bytes char */
				else if ( (szOrig[i] & 0xf8) == 0xf0) {
					szLine[j] = szOrig[i];
					i++;
					j++;
					szLine[j] = szOrig[i];
					i++;
					j++;
					szLine[j] = szOrig[i];
					i++;
					j++;
				}
			}
			szLine[j] = szOrig[i];
			j++;
		}
		szLine[j] = 0;
		trak->Media->handler->nameUTF8 = gf_strdup(szLine);
	}
	return GF_OK;
}
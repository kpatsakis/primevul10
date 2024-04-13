static void tnt_add_checksum(char *sentence)
/* add NMEA-style CRC checksum to a command */
{
    unsigned char sum = '\0';
    char c, *p = sentence;

    if (*p == '@') {
	p++;
    } else {
	gpsd_report(LOG_ERROR, "Bad TNT sentence: '%s'\n", sentence);
    }
    while (((c = *p) != '\0')) {
	sum ^= c;
	p++;
    }
    (void)snprintf(p, 6, "*%02X\r\n", (unsigned int)sum);
}
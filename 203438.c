static size_t oncore_payload_cksum_length(unsigned char id1, unsigned char id2)
{
    size_t l;

    /* For the packet sniffer to not terminate the message due to
     * payload data looking like a trailer, the known payload lengths
     * including the checksum are given.  Return -1 for unknown IDs.
     */

#define ONCTYPE(id2,id3) ((((unsigned int)id2)<<8)|(id3))

    /* *INDENT-OFF* */
    switch (ONCTYPE(id1,id2)) {
    case ONCTYPE('A','b'): l = 10; break; /* GMT offset */
    case ONCTYPE('A','w'): l =  8; break; /* time mode */
    case ONCTYPE('A','c'): l = 11; break; /* date */
    case ONCTYPE('A','a'): l = 10; break; /* time of day */
    case ONCTYPE('A','d'): l = 11; break; /* latitude */
    case ONCTYPE('A','e'): l = 11; break; /* longitude */
    case ONCTYPE('A','f'): l = 15; break; /* height */
    case ONCTYPE('E','a'): l = 76; break; /* position/status/data */
    case ONCTYPE('A','g'): l =  8; break; /* satellite mask angle */
    case ONCTYPE('B','b'): l = 92; break; /* visible satellites status */
    case ONCTYPE('B','j'): l =  8; break; /* leap seconds pending */
    case ONCTYPE('A','q'): l =  8; break; /* atmospheric correction mode */
    case ONCTYPE('A','p'): l = 25; break; /* set user datum / select datum */
    /* Command "Ao" gives "Ap" response   (select datum) */
    case ONCTYPE('C','h'): l =  9; break; /* almanac input ("Cb" response) */
    case ONCTYPE('C','b'): l = 33; break; /* almanac output ("Be" response) */
    case ONCTYPE('S','z'): l =  8; break; /* system power-on failure */
    case ONCTYPE('C','j'): l = 294; break; /* receiver ID */
    case ONCTYPE('F','a'): l =  9; break; /* self-test */
    case ONCTYPE('C','f'): l =  7; break; /* set-to-defaults */
    case ONCTYPE('E','q'): l = 96; break; /* ASCII position */
    case ONCTYPE('A','u'): l = 12; break; /* altitide hold height */
    case ONCTYPE('A','v'): l =  8; break; /* altitude hold mode */
    case ONCTYPE('A','N'): l =  8; break; /* velocity filter */
    case ONCTYPE('A','O'): l =  8; break; /* RTCM report mode */
    case ONCTYPE('C','c'): l = 80; break; /* ephemeris data input ("Bf") */
    case ONCTYPE('C','k'): l =  7; break; /* pseudorng correction inp. ("Ce")*/
    /* Command "Ci" (switch to NMEA, GT versions only) has no response */
    case ONCTYPE('B','o'): l =  8; break; /* UTC offset status */
    case ONCTYPE('A','z'): l = 11; break; /* 1PPS cable delay */
    case ONCTYPE('A','y'): l = 11; break; /* 1PPS offset */
    case ONCTYPE('A','P'): l =  8; break; /* pulse mode */
    case ONCTYPE('A','s'): l = 20; break; /* position-hold position */
    case ONCTYPE('A','t'): l =  8; break; /* position-hold mode */
    case ONCTYPE('E','n'): l = 69; break; /* time RAIM setup and status */
    default:
	return 0;
    }
    /* *INDENT-ON* */

    return l - 6;		/* Subtract header and trailer. */
}
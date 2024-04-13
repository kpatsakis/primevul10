#include <gpac/mpd.h>

void get_file_callback(void *usr_cbk, GF_NETIO_Parameter *parameter)
{
	if (parameter->msg_type==GF_NETIO_DATA_EXCHANGE) {
		u64 tot_size, done, max;
		u32 bps;
		gf_dm_sess_get_stats(parameter->sess, NULL, NULL, &tot_size, &done, &bps, NULL);
		if (tot_size) {
			max = done;
			max *= 100;
			max /= tot_size;
			fprintf(stderr, "download %02d %% at %05d kpbs\r", (u32) max, bps*8/1000);
		}
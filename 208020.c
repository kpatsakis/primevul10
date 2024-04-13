int squidclamav_io(char *wbuf, int *wlen, char *rbuf, int *rlen, int iseof, ci_request_t * req)
{
     int ret = CI_OK;

     if (rbuf && rlen) {
           *rlen = squidclamav_read_from_net(rbuf, *rlen, iseof, req);
	   if (*rlen == CI_ERROR)
	      return CI_ERROR;
           else if (*rlen < 0)
	      ret = CI_OK;
     } else if (iseof) {
	   if (squidclamav_read_from_net(NULL, 0, iseof, req) == CI_ERROR)
	      return CI_ERROR;
     }
     if (wbuf && wlen) {
          *wlen = squidclamav_write_to_net(wbuf, *wlen, req);
     }
     return CI_OK;
}
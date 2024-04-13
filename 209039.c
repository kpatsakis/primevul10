void flush_return_buffer() {
#ifdef HAVE_SSL
   send_with_timeout(_ssl_con, _sock, return_buffer, strlen_retbuf);
#else
   send_with_timeout(NULL, _sock, return_buffer, strlen_retbuf);
#endif
   memset(return_buffer, 0, return_buffer_size);
   strlen_retbuf = 0;
}
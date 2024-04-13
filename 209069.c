int send_with_timeout(void *p, int sock, char *buf, int buf_size) {
   int status, sent, send_size, send_packet;
   time_t start, now;
   char *pbuf;
#ifdef HAVE_SSL
   SSL *ssl;
#endif

   time(&start);
   sent = 0;
   send_size = buf_size;
   pbuf = p; // fix compiler warning
   pbuf = buf;

   do {
      if (send_size > 65536)
         send_packet = 65536;
      else
         send_packet = send_size;

#ifdef HAVE_SSL
      ssl = (SSL *) p;
      if (ssl)
         status = SSL_write(ssl, pbuf, send_packet);
      else
#endif
         status = send(sock, pbuf, send_packet, 0);

      // abort after 30 seconds
      time(&now);
      if (now > start + 30) {
         printf("Timeout after 30 seconds\n");
         break;
      }

      // repeat if we were interrupted by alarm() signal
      if (status == -1 && errno == EINTR) {
         continue;
      }

      if (status == -1)
         break;

      if (status > 0)
         sent += status;

      if (status > 0 && sent < buf_size) {
         pbuf += status;
         send_size -= status;
      }

   } while (sent < buf_size);

   return sent;
}
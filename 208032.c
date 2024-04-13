int squidclamav_end_of_data_handler(ci_request_t * req)
{
     av_req_data_t *data = ci_service_data(req);
     ci_simple_file_t *body;
     char cbuff[MAX_URL_SIZE];
     char clbuf[SMALL_BUFF];

     ssize_t ret;
     int nbread = 0;
     int loopw = 60;
     uint16_t port;
     struct sockaddr_in server;
     struct sockaddr_in peer;
     size_t peer_size;
     char *pt = NULL;
     int sockd;
     int wsockd; 
     unsigned long total_read;

     ci_debug_printf(2, "DEBUG squidclamav_end_of_data_handler: ending request data handler.\n");

     /* Nothing more to scan */
     if (!data || !data->body)
          return CI_MOD_DONE;

     if (data->blocked == 1) {
        ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: blocked content, sending redirection header + error page.\n");
	return CI_MOD_DONE;
     }

     body = data->body;
     if (data->no_more_scan == 1) {
        ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: no more data to scan, sending content.\n");
	ci_simple_file_unlock_all(body);
	return CI_MOD_DONE;
     }

     /* SCAN DATA HERE */
     if ((sockd = dconnect ()) < 0) {
	ci_debug_printf(0, "ERROR squidclamav_end_of_data_handler: Can't connect to Clamd daemon.\n");
	return CI_MOD_ALLOW204;
     }
     ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: Sending STREAM command to clamd.\n");

     if (write(sockd, "STREAM", 6) <= 0) {
	ci_debug_printf(0, "ERROR squidclamav_end_of_data_handler: Can't write to Clamd socket.\n");
	close(sockd);
	return CI_MOD_ALLOW204;
     }

     while (loopw > 0) {
	memset (cbuff, 0, sizeof(cbuff));
	ret = read (sockd, cbuff, MAX_URL_SIZE);
	if ((ret > -1) && (pt = strstr (cbuff, "PORT"))) {
	   pt += 5;
	   sscanf(pt, "%d", (int *) &port);
	   break;
	}
	loopw--;
     }
     if (loopw == 0) {
	ci_debug_printf(0, "ERROR squidclamav_end_of_data_handler: Clamd daemon not ready for stream scanning.\n");
	close(sockd);
	return CI_MOD_ALLOW204;
     }

     ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: Received port %d from clamd.\n", port);

     /* connect to clamd given port */
     if ((wsockd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
	ci_debug_printf(0, "ERROR squidclamav_end_of_data_handler: Can't create the Clamd socket.\n");
	close(sockd);
	return CI_MOD_ALLOW204;
     }

     server.sin_family = AF_INET;
     server.sin_port = htons (port);
     peer_size = sizeof (peer);

     if (getpeername(sockd, (struct sockaddr *) &peer, (socklen_t *) &peer_size) < 0) {
	ci_debug_printf(0, "ERROR squidclamav_end_of_data_handler: Can't get socket peer name.\n");
	close(sockd);
	return CI_MOD_ALLOW204;
     }
     switch (peer.sin_family) {
	case AF_UNIX:
	server.sin_addr.s_addr = inet_addr ("127.0.0.1");
	break;
	case AF_INET:
	server.sin_addr.s_addr = peer.sin_addr.s_addr;
	break;
	default:
	ci_debug_printf(0, "ERROR squidclamav_end_of_data_handler: Unexpected socket type: %d.\n", peer.sin_family);
	close(sockd);
	return CI_MOD_ALLOW204;
     }

     ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: Trying to connect to clamd [port: %d].\n", port);

     if (connect (wsockd, (struct sockaddr *) &server, sizeof (struct sockaddr_in)) < 0) {
	close(wsockd);
	ci_debug_printf(0, "ERROR squidclamav_end_of_data_handler: Can't connect to clamd [port: %d].\n", port);
	return CI_MOD_ALLOW204;
     }
     ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: Ok connected to clamd on port: %d.\n", port);

/*-----------------------------------------------------*/

     ci_debug_printf(1, "DEBUG: squidclamav_end_of_data_handler: Scanning data now\n");
     lseek(body->fd, 0, SEEK_SET);
     memset(cbuff, 0, sizeof(cbuff));
     total_read = 0;
     while (data->virus == 0 && (nbread = read(body->fd, cbuff, MAX_URL_SIZE)) > 0) {
	    total_read += nbread;
	    ret = write(wsockd, cbuff, nbread);
	    if ( (ret <= 0) && (total_read > 0) ) {
		ci_debug_printf(3, "ERROR squidclamav_end_of_data_handler: Can't write to clamd socket (maybe we reach clamd StreamMaxLength, total read: %ld).\n", total_read);
		break;
	    } else if ( ret <= 0 ) {
		ci_debug_printf(0, "ERROR squidclamav_end_of_data_handler: Can't write to clamd socket.\n");
		break;
	    } else {
		ci_debug_printf(3, "DEBUG squidclamav_end_of_data_handler: Write %d bytes on %d to socket\n", (int)ret, nbread);
	    }

	    memset(cbuff, 0, sizeof(cbuff));

     }

     /* close socket to clamd */
     if (wsockd > -1) {
        ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: End Clamd connection, attempting to read result.\n");
	close(wsockd);
     }

     memset (clbuf, 0, sizeof(clbuf));
     while ((nbread = read(sockd, clbuf, SMALL_BUFF)) > 0) {
	ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: received from Clamd: %s", clbuf);
	if (strstr (clbuf, "FOUND\n")) {
	   data->virus = 1;
	   if (!ci_req_sent_data(req)) {
		chomp(clbuf);
		char *urlredir = (char *) malloc( sizeof(char)*MAX_URL_SIZE );
		snprintf(urlredir, MAX_URL_SIZE, "%s?url=%s&source=%s&user=%s&virus=%s", redirect_url, data->url, data->clientip, data->user, clbuf);
		if (logredir == 0)
		   ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: Virus redirection: %s.\n", urlredir);
		if (logredir)
		    ci_debug_printf(0, "INFO squidclamav_end_of_data_handler: Virus redirection: %s.\n", urlredir);
		generate_redirect_page(urlredir, req, data);
		xfree(urlredir);
	   }
	   ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: Virus found, ending download.\n");
	   break;
	}
	memset(clbuf, 0, sizeof(clbuf));
     }
     /* close second socket to clamd */
     if (sockd > -1) {
        ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: Closing Clamd connection.\n");
	close(sockd);
     }

     if (data->virus) {
        ci_debug_printf(1, "DEBUG squidclamav_end_of_data_handler: Virus found, sending redirection header + error page.\n");
          return CI_MOD_DONE;
     }

     if (!ci_req_sent_data(req)) {
	ci_debug_printf(2, "DEBUG squidclamav_end_of_data_handler: Responding with allow 204\n");
	return CI_MOD_ALLOW204;
     }

     ci_debug_printf(3, "DEBUG squidclamav_end_of_data_handler: unlocking data to be sent.\n");
     ci_simple_file_unlock_all(body);

     return CI_MOD_DONE;
}
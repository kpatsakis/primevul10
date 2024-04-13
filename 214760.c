connection_state_machine_h1 (request_st * const r, connection * const con)
{
	const int log_state_handling = r->conf.log_state_handling;
	if (log_state_handling) {
		log_error(r->conf.errh, __FILE__, __LINE__,
		  "state at enter %d %s", con->fd, connection_get_state(r->state));
	}

	connection_state_machine_loop(r, con);

	if (log_state_handling) {
		log_error(r->conf.errh, __FILE__, __LINE__,
		  "state at exit: %d %s", con->fd, connection_get_state(r->state));
	}

	connection_set_fdevent_interest(r, con);
}
intel_engine_in_execlists_submission_mode(const struct intel_engine_cs *engine)
{
	return engine->set_default_submission ==
	       intel_execlists_set_default_submission;
}
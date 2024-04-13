
static bool io_register_op_must_quiesce(int op)
{
	switch (op) {
	case IORING_REGISTER_BUFFERS:
	case IORING_UNREGISTER_BUFFERS:
	case IORING_REGISTER_FILES:
	case IORING_UNREGISTER_FILES:
	case IORING_REGISTER_FILES_UPDATE:
	case IORING_REGISTER_PROBE:
	case IORING_REGISTER_PERSONALITY:
	case IORING_UNREGISTER_PERSONALITY:
	case IORING_REGISTER_FILES2:
	case IORING_REGISTER_FILES_UPDATE2:
	case IORING_REGISTER_BUFFERS2:
	case IORING_REGISTER_BUFFERS_UPDATE:
	case IORING_REGISTER_IOWQ_AFF:
	case IORING_UNREGISTER_IOWQ_AFF:
		return false;
	default:
		return true;
	}
static u32 intel_lr_indirect_ctx_offset(const struct intel_engine_cs *engine)
{
	u32 indirect_ctx_offset;

	switch (INTEL_GEN(engine->i915)) {
	default:
		MISSING_CASE(INTEL_GEN(engine->i915));
		/* fall through */
	case 12:
		indirect_ctx_offset =
			GEN12_CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT;
		break;
	case 11:
		indirect_ctx_offset =
			GEN11_CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT;
		break;
	case 10:
		indirect_ctx_offset =
			GEN10_CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT;
		break;
	case 9:
		indirect_ctx_offset =
			GEN9_CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT;
		break;
	case 8:
		indirect_ctx_offset =
			GEN8_CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT;
		break;
	}

	return indirect_ctx_offset;
}
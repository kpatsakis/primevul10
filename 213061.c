R_API int r_core_bind(RCore *core, RCoreBind *bnd) {
	bnd->core = core;
	bnd->bphit = (RCoreDebugBpHit)r_core_debug_breakpoint_hit;
	bnd->syshit = (RCoreDebugSyscallHit)r_core_debug_syscall_hit;
	bnd->cmd = (RCoreCmd)r_core_cmd0;
	bnd->cmdf = (RCoreCmdF)r_core_cmdf;
	bnd->cmdstr = (RCoreCmdStr)r_core_cmd_str;
	bnd->cmdstrf = (RCoreCmdStrF)r_core_cmd_strf;
	bnd->puts = (RCorePuts)r_cons_strcat;
	bnd->setab = (RCoreSetArchBits)setab;
	bnd->getName = (RCoreGetName)getName;
	bnd->getNameDelta = (RCoreGetNameDelta)getNameDelta;
	bnd->archbits = (RCoreSeekArchBits)archbits;
	bnd->cfggeti = (RCoreConfigGetI)cfggeti;
	bnd->cfgGet = (RCoreConfigGet)cfgget;
	bnd->numGet = (RCoreNumGet)numget;
	bnd->isMapped = (RCoreIsMapped)__isMapped;
	bnd->syncDebugMaps = (RCoreDebugMapsSync)__syncDebugMaps;
	return true;
}
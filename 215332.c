iasecc_get_algorithm(struct sc_context *ctx, const struct sc_security_env *env,
		unsigned operation, unsigned mechanism)
{
    const struct sc_supported_algo_info *info = NULL;
    int ii;

    if (!env)
        return 0;

    for (ii=0;ii<SC_MAX_SUPPORTED_ALGORITHMS && env->supported_algos[ii].reference; ii++)
        if ((env->supported_algos[ii].operations & operation)
			&& (env->supported_algos[ii].mechanism == mechanism))
            break;

    if (ii < SC_MAX_SUPPORTED_ALGORITHMS && env->supported_algos[ii].reference)   {
        info = &env->supported_algos[ii];
        sc_log(ctx, "found IAS/ECC algorithm %X:%X:%X:%X",
			info->reference, info->mechanism, info->operations, info->algo_ref);
    }
    else   {
        sc_log(ctx, "cannot find IAS/ECC algorithm (operation:%X,mechanism:%X)", operation, mechanism);
    }

    return info ? info->algo_ref : 0;
}
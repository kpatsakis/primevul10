void Parser::InitPids()
{
    AssertMemN(m_phtbl);
    wellKnownPropertyPids.arguments = m_phtbl->PidHashNameLen(g_ssym_arguments.sz, g_ssym_arguments.cch);
    wellKnownPropertyPids.async = m_phtbl->PidHashNameLen(g_ssym_async.sz, g_ssym_async.cch);
    wellKnownPropertyPids.eval = m_phtbl->PidHashNameLen(g_ssym_eval.sz, g_ssym_eval.cch);
    wellKnownPropertyPids.get = m_phtbl->PidHashNameLen(g_ssym_get.sz, g_ssym_get.cch);
    wellKnownPropertyPids.set = m_phtbl->PidHashNameLen(g_ssym_set.sz, g_ssym_set.cch);
    wellKnownPropertyPids.let = m_phtbl->PidHashNameLen(g_ssym_let.sz, g_ssym_let.cch);
    wellKnownPropertyPids.constructor = m_phtbl->PidHashNameLen(g_ssym_constructor.sz, g_ssym_constructor.cch);
    wellKnownPropertyPids.prototype = m_phtbl->PidHashNameLen(g_ssym_prototype.sz, g_ssym_prototype.cch);
    wellKnownPropertyPids.__proto__ = m_phtbl->PidHashNameLen(_u("__proto__"), sizeof("__proto__") - 1);
    wellKnownPropertyPids.of = m_phtbl->PidHashNameLen(_u("of"), sizeof("of") - 1);
    wellKnownPropertyPids.target = m_phtbl->PidHashNameLen(_u("target"), sizeof("target") - 1);
    wellKnownPropertyPids.as = m_phtbl->PidHashNameLen(_u("as"), sizeof("as") - 1);
    wellKnownPropertyPids.from = m_phtbl->PidHashNameLen(_u("from"), sizeof("from") - 1);
    wellKnownPropertyPids._default = m_phtbl->PidHashNameLen(_u("default"), sizeof("default") - 1);
    wellKnownPropertyPids._starDefaultStar = m_phtbl->PidHashNameLen(_u("*default*"), sizeof("*default*") - 1);
    wellKnownPropertyPids._star = m_phtbl->PidHashNameLen(_u("*"), sizeof("*") - 1);
}
ifconfig_push_constraint_satisfied(const struct context *c)
{
    const struct options *o = &c->options;
    if (o->push_ifconfig_constraint_defined && c->c2.push_ifconfig_defined)
    {
        return (o->push_ifconfig_constraint_netmask & c->c2.push_ifconfig_local) == o->push_ifconfig_constraint_network;
    }
    else
    {
        return true;
    }
}
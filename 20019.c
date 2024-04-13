FlatView *address_space_get_flatview(AddressSpace *as)
{
    FlatView *view;

    view = address_space_to_flatview(as);

    return view;
}
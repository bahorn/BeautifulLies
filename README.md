# BeautifulLies

An LKM to rewrite the printk ringbuffer, removing messages containing
"beautifullies: " in it on load.

This also:
* removes the `__mcount_loc` section to make everything in the module `notrace`
* restores the taint

TODO:
* removes `kallsyms_lookup_name()` from `touched_functions`
* a privesc payload

[b-complex - beautiful lies](https://youtube.com/watch?v=tzqw_Dqa0SU)

## License

GPL2.

The code to modify the ringbuffer contains direct copies of structs from the
kernel tree.

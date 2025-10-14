# BeautifulLies

An LKM to rewrite the printk ringbuffer, removing messages containing
"beautifullies: " in it on load.

This also:
* removes the `__mcount_loc` section to make everything in the module `notrace`
* restores the taint
* removes `kallsyms_lookup_name()` from `touched_functions`
* resets `printk_once` for the message that warns about an unsigned module
  being load. (HACKY APPROACH WARNING)

TODO:
* a privesc payload

[b-complex - beautiful lies](https://youtube.com/watch?v=tzqw_Dqa0SU)

## Usage

All you need is:
```
just extract-kernel # optional for reseting the printk_once()
just build
just load
just unload
```

## License

GPL2.

The code to modify the ringbuffer contains direct copies of structs from the
kernel tree.

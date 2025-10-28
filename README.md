# BeautifulLies

An LKM to rewrite the printk ringbuffer, removing messages containing
"beautifullies: " in it on load.

This also:
* removes the `__mcount_loc` section to make everything in the module `notrace`
* renames all symbols to "", so nothing shows up in kallsyms
* restores the taint to what it was before the module was loaded.
* removes `kallsyms_lookup_name()` from `touched_functions` (if `KPROBE_LOOKUP`
  is defined in `src/util.c`) else it uses the non-kprobe approach to find
  the symbol with less artifacts.
* resets `printk_once` for the message that warns about an unsigned module
  being load. (HACKY APPROACH WARNING)

TODO:
* a privesc payload

This does not:
* hide itself from the module list, or anywhere else.

[b-complex - beautiful lies](https://youtube.com/watch?v=tzqw_Dqa0SU)

## Usage

All you need is:
```
just extract-kernel # optional for reseting the printk_once()
just build
just load
just unload
```

This has only been tested on a Ubuntu 24.04 machine on a 6.8 kernel.
Your millage may vary, as this has its own copies of kernel structs which might
be different on your kernel.

## License

GPL2.

The code to modify the ringbuffer contains direct copies of structs from the
kernel tree.

#include <linux/init.h>
#include <linux/kprobes.h>
#include "util.h"

static int unresolved_syms = 0;
typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
kallsyms_lookup_name_t kallsyms_lookup_name_f = NULL;

static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
};

unsigned long resolve_sym(const char *sym_name)
{
    if (kallsyms_lookup_name_f == NULL) {
        register_kprobe(&kp);
        kallsyms_lookup_name_f = (kallsyms_lookup_name_t) kp.addr;
        unregister_kprobe(&kp);
    }
    unsigned long ret = kallsyms_lookup_name_f(sym_name);
    if (ret == (unsigned long) NULL) unresolved_syms++;
    return ret;
}

int is_unresolved_sym()
{
    return unresolved_syms > 0;
}

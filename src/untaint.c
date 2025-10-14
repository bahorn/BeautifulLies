#include "untaint.h"
#include "util.h"

/* Remove taint flags from the kernel. */
void untaint_kernel(unsigned long value)
{
    unsigned long* tainted_mask = (unsigned long *)resolve_sym("tainted_mask");
    *tainted_mask = value;
}

void reset_already_done(unsigned long offset)
{
    unsigned long target = (unsigned long)resolve_sym("module_augment_kernel_taints");
    target += offset;
    *(unsigned long*)target = 0;
}

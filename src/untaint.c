#include "untaint.h"
#include "util.h"

/* Remove taint flags from the kernel. */
void untaint_kernel(unsigned long value)
{
    unsigned long* tainted_mask = (unsigned long *)resolve_sym("tainted_mask");
    *tainted_mask = value;
}

#ifndef __UNTAINT__
#define __UNTAINT__

void untaint_kernel(unsigned long value);
void reset_already_done(unsigned long offset);

#endif

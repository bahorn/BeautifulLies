import subprocess
import sys
import os

fname = sys.argv[1]

if not os.path.exists(fname):
    print(0)
    exit()

# Run a command and get output as string
result = subprocess.run(
        ['objdump', '--disassemble=module_augment_kernel_taints', fname],
        capture_output=True,
        text=True
    )
output = result.stdout

fn_base = None
already_done = None

for line in output.split('\n'):
    if '<module_augment_kernel_taints>' in line and fn_base is None:
        fn_base = int(line.split(' ')[0], 16)
    
    if 'already_done' in line:
        already_done = int(line.split('# ')[1].split(' ')[0], 16)

print(already_done - fn_base)

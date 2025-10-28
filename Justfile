build:
    cd ./src && \
        make && \
        objcopy --remove-section=__mcount_loc -X --keep-global-symbol='' --strip-unneeded beautifullies.ko && \
        objcopy --strip-unneeded beautifullies.ko && \
        python3 ../tools/remove_syms.py beautifullies.ko

clean:
    cd ./src && make clean

load:
    sudo insmod ./src/beautifullies.ko \
        taint_value=`cat /proc/sys/kernel/tainted` \
        already_done_offset=`python3 ./tools/get_offset.py ./artifacts/kern.elf`

unload:
    sudo rmmod beautifullies

extract-kernel:
    mkdir -p artifacts
    sudo ./tools/extract-vmlinux.sh /boot/vmlinuz-`uname -r` > ./artifacts/kern.bin
    ./tools/vmlinux-to-elf/vmlinux-to-elf ./artifacts/kern.bin ./artifacts/kern.elf

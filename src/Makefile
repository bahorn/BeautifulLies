
TARGET = beautifullies
ccflags-y := -std=gnu99 \
	-Wno-declaration-after-statement \
	-fvisibility=hidden \
	 -fvisibility-inlines-hidden
ldflags-y := -r

obj-m += $(TARGET).o

$(TARGET)-m += ./util.o ./filter_dmesg.o ./untaint.o ./main.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

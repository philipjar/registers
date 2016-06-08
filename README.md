# registers
registers is a simple linux kernel module (LKM) which creates /proc/registers, a 
kernel proc file which holds the contents of eax, ebx, ecx, edx (x86 general 
purpose registers).
Pretty useless as they change during execution but a good way to start LKM coding and try out some inline assembler.

### Build

Build with:
```shell
make
```

### Usage

Insert into kernel:
```shell
sudo insmod registers.ko
```

Then type `dmesg` or `lsmod | grep registers` to see if the module got loaded.

To view the register contents, use 
```shell
cat /proc/registers
```

To unload the module, use:
```shell
sudo rmmod registers
```

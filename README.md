GPIO addresses are physical memory addresses, and a regular process runs in a virtual memory address.
You need to arrange for those physical addresses to show up as know virtual addresses in the address space of the process, OR you need to run in kernel space in some mode that uses physical addressing (not generally recommended or even always possible.)

Using GPIO through sysfs (the /sys/class/gpio "files") is clearly simpler for a user-level program, but also incurs the system call overhead for each operation on each GPIO pin, so for code with very low latency requirements, that may not work out so well.

The way to do this from user space, is to:

1) Open "/dev/mem" with read/write permissions (you need to be root to do this)
2) mmap() that opened file descriptor, offset (start-of-gpio) length (end-of-gpio minus start-of-gpio)
3) the address returned by mmap() should now be used instead of (start-of-gpio) when accessing the GPIO pins.


Assuming that the defined addresses for GPIO in your program are correct, here's how to use them.

This compiles and runs, and doesn't crash the kernel, but I didn't hook up a scope to see whether it did anything to any GPIOs.
Also, the pinmux/device-tree configuration caveats still apply; i e just because can can toggle a GPIO pin device, doesn't mean that that GPIO pin device is necessarily visible on the output pin you think it should be ...

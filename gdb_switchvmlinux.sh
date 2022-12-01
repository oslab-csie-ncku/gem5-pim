# !/bin/bash

cp /home/johnnylab/work/dl_pim_kernel/pim-kernel ${PWD}/master_work/pim-kernel/pim-kernel
echo "copy pim-kernel to gem5 repo"
cp /home/johnnylab/work/linux-nova-pim/vmlinux ${PWD}/master_work/gem5_images/x86-system/binaries/x86_64-vmlinux-4.18.0-nova-pohao
echo "copy vmlinux to gem5 repo"
rm /home/johnnylab/gem5_vmlinux
ln -s ${PWD}/master_work/gem5_images/x86-system/binaries/x86_64-vmlinux-4.18.0-nova-pohao /home/johnnylab/gem5_vmlinux
echo "update symbolic link to newest vmlinux"

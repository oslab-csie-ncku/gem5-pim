#!/bin/bash
# Program:
#	Build gem5, user can type
# History:
# 2021/11/26    johnnycck   First release
source ./master_work/scripts/run/compile_gem5.sh

ARCH=X86 #X86_MESI_Two_Level
GEM5_BIN_TYPE=opt # debug, opt, fast
BUILD_VARIABLES=''

if ! [ -z "$1" ]; then
    case "$1" in
        -c|--compile)
            compile_gem5 "$ARCH" "$GEM5_BIN_TYPE" "$BUILD_VARIABLES"
            exit
            ;;
        *)
            echo "Usage: "$0" [-c|--compile]"
            exit
            ;;
    esac
fi

export M5_PATH=./

GEM5_TARGET=./build/$ARCH/gem5.$GEM5_BIN_TYPE
FS_CONFIG=./configs/example/fs_pim.py


OUTDIR=pim_m5out
DEBUG_FLAGS=PseudoInst,AddrRanges # CacheFlushRange,SimpleCPU,ScratchpadMemory,Bridge,NoncoherentXBar,MemCtrl,MMU,Vma


NUM_CPUS=1
CPU_TYPE=AtomicSimpleCPU # AtomicSimpleCPU TimingSimpleCPU DerivO3CPU
RESTORE_CPU_TYPE="$CPU_TYPE"
CPU_CLOCK=2GHz

L1I_SIZE=32kB
L1D_SIZE=32kB
L2_SIZE=1MB

MEM_TYPE=DDR4_2400_8x8
MEM_CHANNELS=1
MEM_SIZE=16GB
NVM_TYPE=PCM_LPDDR2_400_8x8

# Must match the memmap in the kernel cmdline (?G!?G). Be careful with x86 3G hole
NVM_START=0x200000000 # 8G
NVM_SIZE=8GB

PIM_CPU_CLOCK=1.5GHz
PIM_L1I_CACHE_SIZE=4kB
PIM_L1D_CACHE_SIZE=4kB
PIM_BANDWIDTH_RATIO=8
PIM_SPM_START=0x450000000 # Must be after the SE memory range
PIM_SPM_SIZE=8kB # The minimum memory size is page size, but it won't actually be used so much
PIM_SPM_REG_FLUSH_ADDR=0x450000000
PIM_SPM_REG_FLUSH_SIZE=0x450000008
PIM_SE_MEM_START=0x440000000 # Must be after the host physical memory range
PIM_SE_MEM_SIZE=10MB # if cache, The minimum memory size of DDR4_2400_8x8 is 256kB, but it won't actually be used so much
#PIM_SE_MEM_SIZE=16kB # if no cache
PIM_KERNEL=./master_work/pim-kernel/pim-kernel
PIM_SE_INPUT=''
PIM_SE_OUTPUT=pim-stdout
PIM_SE_ERROUT=pim-errout
PIM_STACK_NUM=4
# /lib/modules/4.18.0+/kernel/fs/nova/nova.ko
# ./master_work/workloads/metadata/
# ./master_work/workloads/data/
# ./master_work/workloads/real/
# SCRIPT='./master_work/workloads/real/'


"$GEM5_TARGET" \
    --outdir="$OUTDIR" \
    --debug-flags="$DEBUG_FLAGS" \
    "$FS_CONFIG" \
    --cmd=./CCEH-PIM/YCSB-PIM-multistack-smt-myworkall/main \
    --num-cpus="$NUM_CPUS" \
    --cpu-type="$CPU_TYPE" \
    --cpu-clock="$CPU_CLOCK" \
    --caches \
    --l2cache \
    --l1i_size="$L1I_SIZE" \
    --l1d_size="$L1D_SIZE" \
    --l2_size="$L2_SIZE" \
    --mem-type="$MEM_TYPE" \
    --mem-channels="$MEM_CHANNELS" \
    --mem-size="$MEM_SIZE" \
    --dram-nvm \
    --dram-nvm-type="$NVM_TYPE" \
    --dram-nvm-start="$NVM_START" \
    --dram-nvm-size="$NVM_SIZE" \
    --pim-stack-num="$PIM_STACK_NUM" \
    --pim-se \
    --pim-cpu-clock="$PIM_CPU_CLOCK" \
    --pim-bandwidth-ratio="$PIM_BANDWIDTH_RATIO" \
    --pim-spm-start="$PIM_SPM_START" \
    --pim-spm-size="$PIM_SPM_SIZE" \
    --pim-spm-reg-flush-addr="$PIM_SPM_REG_FLUSH_ADDR" \
    --pim-spm-reg-flush-size="$PIM_SPM_REG_FLUSH_SIZE" \
    --pim-se-mem-start="$PIM_SE_MEM_START" \
    --pim-se-mem-size="$PIM_SE_MEM_SIZE" \
    --pim-kernel="$PIM_KERNEL" \
    --pim-se-input="$PIM_SE_INPUT" \
    --pim-se-output="$PIM_SE_OUTPUT" \
    --pim-se-errout="$PIM_SE_ERROUT" \
    `#--checkpoint-restore=1` \
    `#--restore-with-cpu="$RESTORE_CPU_TYPE"` \
    `#--script="$SCRIPT"` \
    `#--disk-image="$DISK_IMAGE"` \
    `#--disk-image="$DISK_IMAGE2"`
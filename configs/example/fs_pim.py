# Copyright (c) 2010-2013, 2016, 2019-2020 ARM Limited
# Copyright (c) 2020 Barkhausen Institut
# All rights reserved.
#
# The license below extends only to copyright in the software and shall
# not be construed as granting a license to any other intellectual
# property including but not limited to intellectual property relating
# to a hardware implementation of the functionality of the software
# licensed hereunder.  You may use the software subject to the license
# terms below provided that you ensure that this notice is replicated
# unmodified and in its entirety in all distributions of the software,
# modified or unmodified, in source code or in binary form.
#
# Copyright (c) 2012-2014 Mark D. Hill and David A. Wood
# Copyright (c) 2009-2011 Advanced Micro Devices, Inc.
# Copyright (c) 2006-2007 The Regents of The University of Michigan
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import argparse
import sys
import math

import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.util import addToPath, fatal, warn
from m5.util.fdthelper import *

addToPath('../')

from ruby import Ruby

from common.FSConfig import *
from common.SysPaths import *
from common.Benchmarks import *
from common import Simulation
from common import CacheConfig
from common import CpuConfig
from common import MemConfig
from common import ObjectList
from common.Caches import *
from common import Options

from pim import PIM
'''
def is_power_of_2(n):
    return (math.ceil(math.log(n, 2)) == math.floor(math.log(n, 2)))
'''
def cmd_line_template():
    if args.command_line and args.command_line_file:
        print("Error: --command-line and --command-line-file are "
              "mutually exclusive")
        sys.exit(1)
    if args.command_line:
        return args.command_line
    if args.command_line_file:
        return open(args.command_line_file).read().strip()
    return None

def build_test_system(np):
    cmdline = cmd_line_template()
    if buildEnv['TARGET_ISA'] == "x86":
        test_sys = makeLinuxX86System(test_mem_mode, np, bm[0], args.ruby,
                                      cmdline=cmdline)
    else:
        fatal("Incapable of building %s full system!", buildEnv['TARGET_ISA'])

    # Set the cache line size for the entire system
    test_sys.cache_line_size = args.cacheline_size

    # Create a top-level voltage domain
    test_sys.voltage_domain = VoltageDomain(voltage = args.sys_voltage)

    # Create a source clock for the system and set the clock period
    test_sys.clk_domain = SrcClockDomain(clock =  args.sys_clock,
            voltage_domain = test_sys.voltage_domain)

    # Create a CPU voltage domain
    test_sys.cpu_voltage_domain = VoltageDomain()

    # Create a source clock for the CPUs and set the clock period
    test_sys.cpu_clk_domain = SrcClockDomain(clock = args.cpu_clock,
                                             voltage_domain =
                                             test_sys.cpu_voltage_domain)

    if args.kernel is not None:
        test_sys.workload.object_file = binary(args.kernel)

    if args.script is not None:
        test_sys.readfile = args.script

    test_sys.init_param = args.init_param

    # For now, assign all the CPUs to the same clock domain
    test_sys.cpu = [TestCPUClass(clk_domain=test_sys.cpu_clk_domain, cpu_id=i)
                    for i in range(np)]


    if args.caches or args.l2cache:
        # By default the IOCache runs at the system clock
        test_sys.iocache = IOCache(addr_ranges = test_sys.mem_ranges)
        test_sys.iocache.cpu_side = test_sys.iobus.master
        test_sys.iocache.mem_side = test_sys.membus.slave
    elif not args.external_memory_system:
        test_sys.iobridge = Bridge(delay='50ns', ranges = test_sys.mem_ranges)
        test_sys.iobridge.slave = test_sys.iobus.master
        test_sys.iobridge.master = test_sys.membus.slave

    for i in range(np):
        test_sys.cpu[i].createThreads()

    CacheConfig.config_cache(args, test_sys)
    #MemConfig.config_fspim_mem(args, test_sys)
    #'''
    if args.hybrid_stack:
        MemConfig.config_fspim_hybridstack_mem(args, test_sys)
    else:
        MemConfig.config_fspim_mem(args, test_sys)
    #'''

    return test_sys

def build_drive_system(np):
    # driver system CPU is always simple, so is the memory
    # Note this is an assignment of a class, not an instance.
    DriveCPUClass = AtomicSimpleCPU
    drive_mem_mode = 'atomic'
    DriveMemClass = SimpleMemory

    cmdline = cmd_line_template()
    if buildEnv['TARGET_ISA'] == 'x86':
        drive_sys = makeLinuxX86System(drive_mem_mode, np, bm[1],
                                       cmdline=cmdline)

    # Create a top-level voltage domain
    drive_sys.voltage_domain = VoltageDomain(voltage = args.sys_voltage)

    # Create a source clock for the system and set the clock period
    drive_sys.clk_domain = SrcClockDomain(clock =  args.sys_clock,
            voltage_domain = drive_sys.voltage_domain)

    # Create a CPU voltage domain
    drive_sys.cpu_voltage_domain = VoltageDomain()

    # Create a source clock for the CPUs and set the clock period
    drive_sys.cpu_clk_domain = SrcClockDomain(clock = args.cpu_clock,
                                              voltage_domain =
                                              drive_sys.cpu_voltage_domain)

    drive_sys.cpu = DriveCPUClass(clk_domain=drive_sys.cpu_clk_domain,
                                  cpu_id=0)
    drive_sys.cpu.createThreads()
    drive_sys.cpu.createInterruptController()
    drive_sys.cpu.connectAllPorts(drive_sys.membus)
    if args.kernel is not None:
        drive_sys.workload.object_file = binary(args.kernel)

    drive_sys.iobridge = Bridge(delay='50ns',
                                ranges = drive_sys.mem_ranges)
    drive_sys.iobridge.slave = drive_sys.iobus.master
    drive_sys.iobridge.master = drive_sys.membus.slave

    # Create the appropriate memory controllers and connect them to the
    # memory bus
    drive_sys.mem_ctrls = [DriveMemClass(range = r)
                           for r in drive_sys.mem_ranges]
    for i in range(len(drive_sys.mem_ctrls)):
        drive_sys.mem_ctrls[i].port = drive_sys.membus.mem_side_ports

    drive_sys.init_param = args.init_param

    return drive_sys

# Add args
parser = argparse.ArgumentParser()
Options.addCommonOptions(parser)
Options.addFSOptions(parser)

# Add the PIM specific options
PIM.define_options(parser)

args = parser.parse_args()

# system under test can be any CPU
numThreads = 1
(TestCPUClass, test_mem_mode, FutureClass) = Simulation.setCPUClass(args)
# TestCPUClass.numThreads = numThreads
# Match the memories with the CPUs, based on the options for the test system
TestMemClass = Simulation.setMemClass(args)

if args.benchmark:
    try:
        bm = Benchmarks[args.benchmark]
    except KeyError:
        print("Error benchmark %s has not been defined." % args.benchmark)
        print("Valid benchmarks are: %s" % DefinedBenchmarks)
        sys.exit(1)
else:
    if args.dual:
        bm = [SysConfig(disks=args.disk_image, rootdev=args.root_device,
                        mem=args.mem_size, os_type=args.os_type),
              SysConfig(disks=args.disk_image, rootdev=args.root_device,
                        mem=args.mem_size, os_type=args.os_type)]
    else:
        bm = [SysConfig(disks=args.disk_image, rootdev=args.root_device,
                        mem=args.mem_size, os_type=args.os_type)]

np = args.num_cpus

# multistack PIM:
if args.pim_stack_num is None:
    fatal("number of pim memory stacks is not set")
elif args.pim_stack_num is 0:
    fatal("number of pim memory stacks cannot be 0")
'''
elif is_power_of_2(args.pim_stack_num) is False:
    fatal("number of pim memory stacks must be in powers of 2")
'''

pim_stack_num = args.pim_stack_num

test_sys = build_test_system(np)

if len(bm) == 2:
    drive_sys = build_drive_system(np)
    root = makeDualRoot(True, test_sys, drive_sys, args.etherdump)
elif len(bm) == 1 and args.dist:
    # This system is part of a dist-gem5 simulation
    root = makeDistRoot(test_sys,
                        args.dist_rank,
                        args.dist_size,
                        args.dist_server_name,
                        args.dist_server_port,
                        args.dist_sync_repeat,
                        args.dist_sync_start,
                        args.ethernet_linkspeed,
                        args.ethernet_linkdelay,
                        args.etherdump);
elif len(bm) == 1:
    root = Root(full_system=True, system=test_sys)
else:
    print("Error I don't know how to create more than 2 systems.")
    sys.exit(1)

if numThreads > 1:
    test_sys.multi_thread = True

# if args.pim_baremetal or args.pim_se:
#     root.pim_system = PIM.build_pim_system(args)
#     PIM.connect_to_host_system(args, test_sys, root.pim_system)

#     if args.pim_se:
#         root.se_mode_system_name = root.pim_system.get_name()
if args.pim_baremetal:
    if pim_stack_num > 1:
        warn("baremetal pim only support one stack pim system")
    root.pim_system = PIM.build_pim_system(args, 0)
    PIM.connect_to_host_system(args, test_sys, root.pim_system)

if buildEnv['TARGET_ISA'] == 'x86':
    for i in range(numThreads - 1):
        test_sys.cpu[0].interrupts[i+1].pio = test_sys.membus.mem_side_ports
        test_sys.cpu[0].interrupts[i+1].int_requestor = test_sys.membus.cpu_side_ports
        test_sys.cpu[0].interrupts[i+1].int_responder = test_sys.membus.mem_side_ports

if args.pim_se:
    root.pim_stack_num = pim_stack_num
    #root.multiple_se_system = True if int(root.pim_stack_num) > 1 else False

    if root.pim_stack_num == 1:
        root.pim_system = PIM.build_pim_system(args, 0)
        PIM.connect_to_host_system(args, test_sys, root.pim_system, 0)
        root.se_mode_system_name = root.pim_system.get_name()
    else:
        root.pim_system = [PIM.build_pim_system(args, i) for i in range(root.pim_stack_num)]
        for p, pim_sys in enumerate(root.pim_system):
            PIM.connect_to_host_system(args, test_sys, pim_sys, p)
        root.se_mode_systems_name = [pim_sys.get_name() for pim_sys in root.pim_system]

Simulation.setWorkCountOptions(test_sys, args)
Simulation.run(args, root, test_sys, FutureClass)

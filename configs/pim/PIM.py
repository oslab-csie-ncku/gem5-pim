from m5.objects import *
from m5.defines import buildEnv
from m5.util import convert

from common import Simulation
from common import SysPaths
from common import Caches
from common import MemConfig
from . import params

##
## PIM Options
##
def define_options(parser):
    parser.add_argument("--pim-baremetal", action="store_true",
                        help="Build PIM system in baremetal system")
    parser.add_argument("--pim-se", action="store_true",
                        help="Build PIM system in SE mode system")

    parser.add_argument("--pim-cpu-clock", action="store", type=str,
                        default=params.CPU_CLK,
                        help = "Clock for blocks running at PIM CPU speed")

    parser.add_argument("--pim-l1i-cache-size", action="store", type=str,
                        default=params.L1_ICACHE_SIZE,
                        help = "PIM L1 i-cache size")
    parser.add_argument("--pim-l1d-cache-size", action="store", type=str,
                        default=params.L1_DCACHE_SIZE,
                        help = "PIM L1 d-cache size")

    parser.add_argument("--pim-bandwidth-ratio", action="store",
                        type=int,
                        default=params.BANDWIDTH_RATIO,
                        help = "Bandwidth ratio")

    parser.add_argument("--pim-spm-start", action="store", default=None,
                        help="Specify the SPM start address for PIM")
    parser.add_argument("--pim-spm-size", action="store", type=str,
                        default=None,
                        help="Specify the SPM size for PIM")
    parser.add_argument("--pim-spm-reg-flush-addr", action="store",
                        default=None,
                        help="Specify the SPM flush addr reg address for PIM")
    parser.add_argument("--pim-spm-reg-flush-size", action="store",
                        default=None,
                        help="Specify the SPM flush size reg address for PIM")

    parser.add_argument("--pim-se-mem-start", action="store",
                        default=None,
                        help="Specify the SE memory start address for PIM")
    parser.add_argument("--pim-se-mem-size", action="store", type=str,
                        default=None,
                        help="Specify the SE memory size for PIM")

    parser.add_argument("--pim-kernel", action="store", type=str,
                        default=None,
                        help="PIM kernel program")

    parser.add_argument("--pim-se-input", action="store", type=str,
                        default=params.SE_INPUT,
                        help="Read stdin from a file")
    parser.add_argument("--pim-se-output", action="store", type=str,
                        default=params.SE_OUTPUT,
                        help="Redirect stdout to a file")
    parser.add_argument("--pim-se-errout", action="store", type=str,
                        default=params.SE_ERROUT,
                        help="Redirect stderr to a file")

##
## PIM Related Class
##
class PIMBus(NoncoherentXBar):
    ideal = True

    frontend_latency = params.BUS_FRONTEND_LATENCY_IDEAL
    forward_latency = params.BUS_FORWARD_LATENCY_IDEAL
    response_latency = params.BUS_RESPONSE_LATENCY_IDEAL
    width = params.BUS_WIDTH_IDEAL

    badaddr_responder = BadAddr(warn_access = "warn")
    default = Self.badaddr_responder.pio

class PIMBridge(Bridge):
    ideal = True

    req_size = params.BRIDGE_REQ_SIZE_IDEAL
    resp_size = params.BRIDGE_RESP_SIZE_IDEAL
    delay = params.BRIDGE_DELAY_IDEAL

if buildEnv['TARGET_ISA'] == 'arm':
    class PIMBaremetalSystem(ArmSystem):
        kernel_addr_check = False
        auto_reset_addr = True
        highest_el_is_64 = True

        realview = VExpress_GEM5_V1()
        bridge = Bridge(delay='50ns')
        iobus = IOXBar()
        pimbus = PIMBus()
        intrctrl = IntrControl()
        terminal = Terminal()
        vncserver = VncServer()

class PIMSESystem(System):
    pimbus = PIMBus()

##
## PIM Function
##
def build_pim_mem_subsystem(options, sys):
    if not hasattr(sys, 'membus'):
        fatal("Host system doesn't has attribute 'membus'")

    sys.memsubsystem = SubSystem()

    sys.memsubsystem.bridge = Bridge(req_size = 32, resp_size = 32,
                                     delay = params.BRIDGE_MEMSUBSYSTEM_DELAY)
    sys.memsubsystem.bridge.ranges = sys.mem_ranges
    #sys.memsubsystem.bridge.ranges.append(
    #    AddrRange(options.pim_spm_start, size = (options.pim_spm_size - 1)))
    
    sys.memsubsystem.bridge.ranges.append(
        AddrRange(options.pim_spm_start, size = options.pim_spm_size))

    sys.memsubsystem.xbar_clk_domain = SrcClockDomain(clock = '0.1GHz',
                                       voltage_domain = VoltageDomain())
    sys.memsubsystem.xbar = NoncoherentXBar(clk_domain = \
                                            sys.memsubsystem.xbar_clk_domain)
    sys.memsubsystem.xbar.frontend_latency = \
        params.BUS_INTERNAL_FRONTEND_LATENCY
    sys.memsubsystem.xbar.forward_latency = \
        params.BUS_INTERNAL_FORWARD_LATENCY
    sys.memsubsystem.xbar.response_latency = \
        params.BUS_INTERNAL_RESPONSE_LATENCY
    sys.memsubsystem.xbar.width = params.BUS_INTERNAL_WIDTH
    sys.memsubsystem.xbar.badaddr_responder = BadAddr(warn_access = "warn")
    sys.memsubsystem.xbar.default = sys.memsubsystem.xbar.badaddr_responder.pio

    sys.membus.mem_side_ports = sys.memsubsystem.bridge.cpu_side_port
    sys.memsubsystem.bridge.mem_side_port = sys.memsubsystem.xbar.cpu_side_ports

    return sys.memsubsystem

def build_pim_system(options):
    (CPUClass, MemMode, FutureClass) = Simulation.setCPUClass(options)

    # PIM mode check
    if not options.pim_baremetal and not options.pim_se:
        fatal("Must specify the mode of PIM")
    if options.pim_baremetal and options.pim_se:
        fatal("Cannot use pim-baremetal and pim-se options at the same time")
    if options.pim_baremetal and buildEnv['TARGET_ISA'] != 'arm':
        fatal("Option pim-baremetal can only be used under ARM arch")

    # PIM SPM check
    if options.pim_spm_size is None:
        fatal("SPM size is not set")
    if options.pim_spm_reg_flush_addr is None:
        fatal("SPM flush addr reg is not set")
    if options.pim_spm_reg_flush_size is None:
        fatal("SPM flush size reg is not set")

    # PIM SE memory check
    if options.pim_se and options.pim_se_mem_size is None:
        fatal("SE memory size is not set")

    # PIM kernel program check
    if options.pim_kernel is None:
        fatal("A PIM kernel must be provided to run in PIM")

    def build_baremetal_pim_system():
        self = PIMBaremetalSystem()

        self.realview.uart[0].end_on_eot = True

        self.bridge.slave = self.pimbus.master
        self.bridge.master = self.iobus.slave

        self.realview.attachOnChipIO(self.pimbus, self.bridge)
        self.realview.attachIO(self.iobus)

        spm_start = options.pim_spm_start
        if spm_start is None:
            spm_start = long(self.realview._mem_regions[0].start)

        # PIM memory check
        if spm_start < long(self.realview._mem_regions[0].start):
            fatal("The starting address of SPM of baremetal PIM cannot be "
                  "less than the memory start address %#x specified by ARM "
                  "VExpress" % long(self.realview._mem_regions[0].start))

        self.mem_ranges = [AddrRange(spm_start, size = options.pim_spm_size)]
        self.spm = ScratchpadMemory(range = self.mem_ranges[0])
        self.spm.port = self.pimbus.master
        
        return self

    def build_se_pim_system():
        self = PIMSESystem()

        se_mem_start = int(options.pim_se_mem_start, 16)
        #print("se_mem_start:" + str(se_mem_start))
        if se_mem_start is None:
            se_mem_start = 0x0

        spm_start = int(options.pim_spm_start, 16)
        if spm_start is None:
            spm_start = se_mem_start + \
                convert.toMemorySize(options.pim_se_mem_size)

        # PIM memory check
        if spm_start <= se_mem_start:
            fatal("The starting address of SPM needs to be larger than the "
                  "starting address of SE memory")
        elif spm_start < se_mem_start + \
            convert.toMemorySize(options.pim_se_mem_size):
            fatal("The starting address of SPM cannot overlap with the SE "
                  "memory range")

        #self.mem_ranges = [AddrRange(se_mem_start,
        #                             size = (options.pim_se_mem_size - 1))]
        #print(options.pim_se_mem_size)
        self.mem_ranges = [AddrRange(se_mem_start,
                                     size = options.pim_se_mem_size)]

        #self.spm = ScratchpadMemory(range = AddrRange(spm_start, size = \
        #                                              (options.pim_spm_size - 1)))

        self.spm = ScratchpadMemory(range = AddrRange(spm_start, size = \
                                                      options.pim_spm_size))
        #print('spm: ' + str(AddrRange(spm_start, size = options.pim_spm_size)))
        self.spm.in_addr_map = False
        self.spm.conf_table_reported = False
        self.spm.port = self.pimbus.mem_side_ports

        self.se_mem_ctrl = ScratchpadMemory(range = self.mem_ranges[0])
        #print('se_mem_ctrl: '+ str(self.mem_ranges[0]))
        self.se_mem_ctrl.port = self.pimbus.mem_side_ports

        return self

    if options.pim_baremetal:
        self = build_baremetal_pim_system()
    elif options.pim_se:
        self = build_se_pim_system()

    self.mem_mode = MemMode
    self.cache_line_size = options.cacheline_size

    self.system_port = self.pimbus.cpu_side_ports

    self.voltage_domain = VoltageDomain(voltage = options.sys_voltage)
    self.clk_domain = SrcClockDomain(clock = options.sys_clock,
                                     voltage_domain = self.voltage_domain)
    self.cpu_voltage_domain = VoltageDomain()
    self.cpu_clk_domain = SrcClockDomain(clock = options.pim_cpu_clock,
                                         voltage_domain =
                                         self.cpu_voltage_domain)

    self.cpu = CPUClass(clk_domain = self.cpu_clk_domain, cpu_id = 0,
                        numThreads = 1)

    self.cpu.createThreads()

    self.cpu.createInterruptController()

    self.cpu.connectAllPorts(self.pimbus)

    self.spm.support_flush = True
    self.spm.reg_flush_addr = options.pim_spm_reg_flush_addr
    self.spm.reg_flush_size = options.pim_spm_reg_flush_size

    if options.pim_baremetal:
        self.kernel = options.pim_kernel
    if options.pim_se:
        process = Process(cmd = [options.pim_kernel])

        if options.pim_se_input != None:
            process.input = options.pim_se_input
        if options.pim_se_output != None:
            process.output = options.pim_se_output
        if options.pim_se_errout != None:
            process.errout = options.pim_se_errout

        self.cpu.workload = [process]
        self.workload = SEWorkload.init_compatible(options.pim_kernel)

    return self

def connect_to_host_system(options, sys, pim_sys):
    if buildEnv['TARGET_ISA'] not in ['arm', 'x86']:
        fatal("PIM does not support %s ISA!", buildEnv['TARGET_ISA'])

    if not hasattr(sys, 'memsubsystem'):
        fatal("Host system doesn't has attribute 'memsubsystem'")

    if not hasattr(sys.memsubsystem, 'xbar'):
        fatal("Host mem subsystem doesn't has attribute 'xbar'")

    if not hasattr(pim_sys, 'pimbus'):
        fatal("PIM system doesn't has attribute 'pimbus'")

    sys.memsubsystem.topimbridge = PIMBridge(ranges = [pim_sys.spm.range])
    pim_sys.tohostbridge = PIMBridge(ranges = sys.mem_ranges)

    sys.memsubsystem.xbar.mem_side_ports = sys.memsubsystem.topimbridge.cpu_side_port
    sys.memsubsystem.topimbridge.mem_side_port = pim_sys.pimbus.cpu_side_ports

    pim_sys.pimbus.mem_side_ports = pim_sys.tohostbridge.cpu_side_port
    pim_sys.tohostbridge.mem_side_port = sys.memsubsystem.xbar.cpu_side_ports
    
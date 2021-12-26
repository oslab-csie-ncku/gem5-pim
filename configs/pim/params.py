##
## CPU
##
## PIM CPU clock based on:
## [1] Design and Evaluation of a Processing-in-Memory Architecture for the
##     Smart Memory Cube (http://dx.doi.org/10.1007/978-3-319-30695-7_2)
##
CPU_CLK = '1.5GHz'

##
## Cache
##
## L1 icache/dcache size is small than:
## [1] Google Workloads for Consumer Devices: Mitigating Data Movement
##     Bottlenecks (http://doi.acm.org/10.1145/3173162.3173177)
##
L1_ICACHE_SIZE='4kB'
L1_DCACHE_SIZE='4kB'

##
## Bandwidth
##
## Off-chip memory bandwidth and internal memory bandwidth ratio based on:
## [1] Google Workloads for Consumer Devices: Mitigating Data Movement
##     Bottlenecks (http://doi.acm.org/10.1145/3173162.3173177)
##
BANDWIDTH_RATIO = 8

##
## Bridge
##
BRIDGE_REQ_SIZE_IDEAL = 0xffffffff
BRIDGE_RESP_SIZE_IDEAL = 0xffffffff
BRIDGE_DELAY_IDEAL = '0ns'
BRIDGE_MEMSUBSYSTEM_DELAY = '50ns'

##
## Bus
##
BUS_FRONTEND_LATENCY_IDEAL = 0
BUS_FORWARD_LATENCY_IDEAL = 0
BUS_RESPONSE_LATENCY_IDEAL = 0
BUS_WIDTH_IDEAL = 0xffffffff
BUS_INTERNAL_FRONTEND_LATENCY = 3
BUS_INTERNAL_FORWARD_LATENCY = 4
BUS_INTERNAL_RESPONSE_LATENCY = 2
BUS_INTERNAL_WIDTH = 1

##
## I/O
##
SE_INPUT = None
SE_OUTPUT = None
SE_ERROUT = None

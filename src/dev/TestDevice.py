from m5.params import *
from m5.objects.Device import BasicPioDevice

class TestDevice(BasicPioDevice):
    type = 'TestDevice'
    cxx_header = "dev/testdev.hh"
    cxx_class = 'gem5::TestDevice'

    pio_size = Param.Addr(8, "Size of address range")
    ret_data = Param.UInt8(0x77, "Default data to return")

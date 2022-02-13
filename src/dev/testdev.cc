/** @file
 * Test Device implementation
 */

#include "dev/testdev.hh"
#include "params/TestDevice.hh"
#include "mem/packet_access.hh"

namespace gem5
{

TestDevice::TestDevice(const Params &p)
    : BasicPioDevice(p, p.pio_size)
{
    inform("Device %s pio address=%#llx size=%llu\n",
            name(), pioAddr, pioSize);

    retData = p.ret_data;
}

Tick
TestDevice::read(PacketPtr pkt)
{
    assert(pkt->getAddr() >= pioAddr &&
           pkt->getAddr() + pkt->getSize() - 1 < pioAddr + pioSize);

    pkt->makeAtomicResponse();

    std::memset(pkt->getPtr<uint8_t>(), retData, pkt->getSize());

    inform("Device %s accessed by read to address %#llx size=%u\n",
            name(), pkt->getAddr(), pkt->getSize());

    return pioDelay;
}

Tick
TestDevice::write(PacketPtr pkt)
{
    assert(pkt->getAddr() >= pioAddr &&
           pkt->getAddr() + pkt->getSize() - 1 < pioAddr + pioSize);

    pkt->makeAtomicResponse();

    uint64_t data;

    switch (pkt->getSize()) {
        case sizeof(uint64_t):
            data = pkt->getLE<uint64_t>();
            break;
        case sizeof(uint32_t):
            data = pkt->getLE<uint32_t>();
            break;
        case sizeof(uint16_t):
            data = pkt->getLE<uint16_t>();
            break;
        case sizeof(uint8_t):
            data = pkt->getLE<uint8_t>();
            break;
        default:
            panic("Device %s: invalid access size: %u\n",
                   name(), pkt->getSize());
    }

    inform("Device %s accessed by write to address %#llx size=%u data=%#llx\n",
            name(), pkt->getAddr(), pkt->getSize(), data);

    return pioDelay;
}

} // namespace gem5
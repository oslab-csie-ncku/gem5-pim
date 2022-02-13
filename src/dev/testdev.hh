/** @file
 * Declaration of a test device.
 */

#ifndef __DEV_TESTDEV_HH__
#define __DEV_TESTDEV_HH__

#include "dev/io_device.hh"
#include "mem/packet.hh"
#include "params/TestDevice.hh"

namespace gem5
{

/**
 * TestDevice
 * ...
 */
class TestDevice : public BasicPioDevice
{
  protected:
    uint8_t retData;

  public:
    using Params = TestDeviceParams;
    TestDevice(const Params &p);

    virtual Tick read(PacketPtr pkt);
    virtual Tick write(PacketPtr pkt);
};

#endif // __DEV_TESTDEV_HH__

} // namespace gem5
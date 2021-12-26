/**
 * @file
 * ScratchpadMemory declaration
 */

#ifndef __MEM_SCRATCHPAD_MEMORY_HH__
#define __MEM_SCRATCHPAD_MEMORY_HH__

#include "mem/simple_mem.hh"
#include "params/ScratchpadMemory.hh"

namespace gem5
{

class Cache;

namespace memory
{


/**
 * ScratchpadMemory
 * ...
 */
class ScratchpadMemory : public SimpleMemory
{
  private:
    const bool support_flush;
    const Addr reg_flush_addr;
    const Addr reg_flush_size;

    std::vector<Cache *> dcaches;

  public:
    ScratchpadMemory(const ScratchpadMemoryParams &p);

  private:
    void initSystemDcaches(void);

  public:
    void init() override;

  private:
    const uint32_t *readMem_l(const Addr addr) const;
    const uint64_t *readMem_q(const Addr addr) const;
    void flushSystemDcaches(const Addr addr, const uint32_t size) const;
    bool needFlush(const PacketPtr pkt) const;

  protected:
    Tick recvAtomic(PacketPtr pkt) override;
    Tick recvAtomicBackdoor(PacketPtr pkt, MemBackdoorPtr &_backdoor) override;
    void recvFunctional(PacketPtr pkt) override;
    bool recvTimingReq(PacketPtr pkt) override;
};

} // namespace memory
} // namespace gem5

#endif //__MEM_SCRATCHPAD_MEMORY_HH__

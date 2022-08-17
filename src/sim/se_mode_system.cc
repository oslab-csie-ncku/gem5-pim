#include "sim/se_mode_system.hh"

#include <cassert>

#include "cpu/base.hh"
#include "cpu/thread_context.hh"
#include "sim/system.hh"
namespace gem5
{

bool
semodesystem::belongSEsys(const System *const _system)
{
    assert(_system);

    if (MemStackNum == 1) {
        if (_system->name() == SEModeSystemName)
            return true;
    } else if (MemStackNum > 1){
        /* multistack PIM */
        for (int i=0; i<SEModeSystemsName.size(); i++){
            if (_system->name() == SEModeSystemsName[i])
                return true;
        }
    }

    return false;
}

bool
semodesystem::belongSEsys(const BaseCPU *const _cpu)
{
    assert(_cpu);

    return belongSEsys(_cpu->system);
}

bool
semodesystem::belongSEsys(ThreadContext *const _tc)
{
    assert(_tc);

    return belongSEsys(_tc->getSystemPtr());
}

} //namespace gem5
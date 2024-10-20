#include "PadClient.h"

using namespace std;
using namespace padbus;

namespace padbus {
    class InitializingState final : public PadClientStateBase {
        InitializingState() : PadClientStateBase(Initializing) {}
    };
}
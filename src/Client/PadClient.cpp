#include <PadBus/Client/PadClient.h>

using namespace std;
using namespace padbus;

PadClient::PadClient(const uint64_t globalAddress, shared_ptr<IBusIODriver> ioDriver):
        _messageQueue(queue<PadBusMessage>()),
        _ioDriver(move(ioDriver)),
        _globalAddress(globalAddress){
}

PadClient::~PadClient() = default;

void PadClient::initialize() {

}

PadClientStateBase::PadClientStateBase(const PadClientState id) : IState(id) {}


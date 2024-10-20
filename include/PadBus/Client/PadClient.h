#ifndef PADBUS_PADCLIENT_H
#define PADBUS_PADCLIENT_H

#include <queue>
#include <PadBus/IBusIODriver.h>
#include <PadBus/StateMachine.h>

using namespace std;

namespace padbus {
    enum PadClientState {
        Initializing,
        Idle,
        Listen,
        Send,
        Wait,
        Fault,
    };

    enum PadClientFaultCode {
        None,
        Unknown,
    };

    class PadClientStateContext : StateContext {
        public:
            uint8_t _localAddress;
            PadClientFaultCode _faultCode;
    };

    struct PadBusMessage {
        bool isExpirable;
        uint32_t queueTimeMs;
        uint32_t lifetimeMs;
        vector<uint8_t>* message;
    };

    class PadClientStateBase;

    class PadClient final : StateMachine<PadClientState, PadClientStateContext> {
        queue<PadBusMessage> _messageQueue;
        const shared_ptr<IBusIODriver> _ioDriver;
        const uint64_t _globalAddress;
        public:
            explicit PadClient(uint64_t globalAddress, shared_ptr<IBusIODriver> ioDriver);
            ~PadClient() override;
            void initialize();
            void onReset() override;
            void queueMessage(vector<uint8_t>* message);
            bool isFaulted();
            PadClientFaultCode getFaultCode();

        friend class PadClientStateBase;
    };

    class PadClientStateBase: IState<PadClientState, PadClientStateContext, PadClient> {
        protected:
            explicit PadClientStateBase(PadClientState id);
            ~PadClientStateBase() override = default;

            IBusIODriver* io();
            [[nodiscard]] const uint64_t globalAddress() const;
        public:

    };
}

#endif

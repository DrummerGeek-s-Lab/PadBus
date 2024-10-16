#ifndef PADBUS_IBUSIODRIVER_H
#define PADBUS_IBUSIODRIVER_H

#include "Event.h"
#include "IObservable.h"

namespace padbus {
    class IBusIODriver : IObservable {
        Event<> clockRisingEvent;
        Event<> clockFallingEvent;
        Event<> busActiveStartedEvent;
        Event<> busActiveEndedEvent;

        public:
            Event<>* clockRising() { return &clockRisingEvent; }
            Event<>* clockFalling() { return &clockFallingEvent; }
            Event<>* busActiveStarted() { return &busActiveStartedEvent; }
            Event<>* busActiveEnded() { return &busActiveEndedEvent; }

            [[nodiscard]] virtual bool getInputBitState() const = 0;
            [[nodiscard]] bool getOutputBitState() const { return outputBit; }
            virtual void setOutputBitState() const = 0;

            [[nodiscard]] virtual bool getBusActiveState() const = 0;
            virtual void setBusActive() const = 0;
            virtual void clearBusActive() const = 0;

        protected:
            bool outputBit = false;
            void onClockRising() { handleEvent(&clockRisingEvent, new EventArgs()); }
            void onClockFalling() { handleEvent(&clockFallingEvent, new EventArgs()); }
            void onBusActiveStarted() { handleEvent(&busActiveStartedEvent, new EventArgs()); }
            void onBusActiveEnded() { handleEvent(&busActiveEndedEvent, new EventArgs()); }

        virtual ~IBusIODriver() = default;
    };
}

#endif
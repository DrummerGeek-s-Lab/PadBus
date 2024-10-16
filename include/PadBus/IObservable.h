#ifndef PADBUS_IOBSERVABLE_H
#define PADBUS_IOBSERVABLE_H

namespace padbus {
    template <typename TArgs>
    class Event;
    class IObservable;

    struct EventArgs{
        IObservable* sender;

        EventArgs() = default;

        explicit EventArgs(const IObservable* sender);
    };

    class IObservable {
        protected:
            template <typename TArgs = EventArgs>
            void handleEvent(Event<TArgs>* event, TArgs* args) {
                args->sender = this;
                event->notifyObservers(args);
            }
    };

}

#endif
#ifndef PADBUS_IOBSERVABLE_H
#define PADBUS_IOBSERVABLE_H

namespace padbus {
    class IObservable;

    class EventArgs{
        IObservable* sender = nullptr;
        public:
            EventArgs() = default;
            [[nodiscard]] IObservable* getSender() const { return sender; }
            virtual ~EventArgs() = default;

        friend class IObservable;
    };

    template <typename TArgs>
    class Event;

    class IObservable {
        protected:
            template <typename TArgs = EventArgs>
            void handleEvent(Event<TArgs>* event) {
                handleEvent(event, TArgs());
            }
            template <typename TArgs = EventArgs>
            void handleEvent(Event<TArgs>* event, TArgs* args) {
                EventArgs* baseArgs = args;
                baseArgs->sender = this;
                event->notifyObservers(args);
            }
    };

}

#endif
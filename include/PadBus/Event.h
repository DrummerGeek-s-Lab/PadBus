#ifndef PADBUS_IEVENT_H
#define PADBUS_IEVENT_H

#include <vector>
#include <memory>
#include <functional>
#include "IObservable.h"

using namespace std;
using namespace padbus;

namespace padbus {
    class EventBase;
    class IObserverBase;

    class Subject {
        vector<IObserverBase*> _observers;

        public:
            void attachObserver(IObserverBase* observer);
            void detachObserver(IObserverBase* observer);
            void notifyObservers(EventArgs* args);
    };

    class IObserverBase {
        weak_ptr<Subject> _subject;

        virtual void updateInternal(EventArgs* args) = 0;
        friend class Subject;
        protected:
            explicit IObserverBase(const EventBase* event);
        public:
            virtual ~IObserverBase();
            bool enabled = true;
            function<bool()> getEnabledFunc;
    };

    class EventBase {
        friend class IObserverBase;

        protected:
            shared_ptr<Subject> _subject;

            explicit EventBase();
            void notifyObservers(EventArgs* args) const;
    };

    template<typename TArgs = EventArgs>
    class Event : public EventBase {
        void notifyObservers(TArgs* args) const {
            EventBase::notifyObservers(args);
        };

        friend class IObservable;
    };

    template<typename TArgs = EventArgs>
    class IObserver : public IObserverBase {
        virtual void update(TArgs* args) = 0;
        void updateInternal(EventArgs* args) final {
            update(static_cast<TArgs*>(args));
        }
        public:
            explicit IObserver(const Event<TArgs>* event) : IObserverBase(event) {}
    };

    template<typename TArgs = EventArgs>
    class FunctionObserver final : public IObserver<TArgs> {
        function<void(TArgs*)> _fn;

        void update(TArgs* args) override {
            _fn(args);
        }

        public:
            explicit FunctionObserver(Event<TArgs>* event, function<void(EventArgs*)> fn) :
                IObserver<TArgs>(event), _fn(fn) { }
    };
}

#endif
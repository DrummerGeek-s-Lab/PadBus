#ifndef PADBUS_IEVENT_H
#define PADBUS_IEVENT_H

#include <vector>
#include <memory>
#include <functional>
#include "IObservable.h"

using namespace std;

namespace padbus {
    template <typename TArgs = EventArgs>
    class Event {
        class Subject;

        public:
            Event() {
                _subject = make_shared<Subject>();
            }

            class IObserver {
                weak_ptr<Subject> _subject;

                virtual void update(TArgs* args) = 0;

                public:
                    explicit IObserver(Event* event) {
                        _subject = event->_subject;
                        event->_subject->attachObserver(this);
                    }

                    bool enabled = true;
                    function<bool()> getEnabledFunc;
                    [[nodiscard]] virtual bool getEnabled() const { return true; }

                    virtual ~IObserver() {
                        if (shared_ptr<Subject> ptr = _subject.lock()) {
                            ptr->detachObserver(this);
                        }
                    }

                    friend class Subject;
                };

        private:
            class Subject {
                vector<IObserver*> _observers;

                public:
                    void attachObserver(IObserver* observer);
                    void detachObserver(IObserver* observer);
                    void notifyObservers(TArgs* args);
            };

            shared_ptr<Subject> _subject;

            void notifyObservers(TArgs* args);
            friend class IObservable;
    };
}

#endif
#include <PadBus/Event.h>

using namespace std;
using namespace padbus;

IObserverBase::IObserverBase(const EventBase* event) {
    _subject = event->_subject;
    event->_subject->attachObserver(this);
}
IObserverBase::~IObserverBase() {
    if (const shared_ptr<Subject> ptr = _subject.lock()) {
        ptr->detachObserver(this);
    }
}

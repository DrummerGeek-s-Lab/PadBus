#include <PadBus/Event.h>

using namespace std;
using namespace padbus;

EventBase::EventBase() {
        _subject = make_shared<Subject>();
}
void EventBase::notifyObservers(EventArgs *args) const {
    _subject->notifyObservers(args);
}

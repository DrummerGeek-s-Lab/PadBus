#include <PadBus/Event.h>

using namespace std;
using namespace padbus;

void Subject::attachObserver(IObserverBase* observer) {
    _observers.push_back(observer);
}
void Subject::detachObserver(IObserverBase* observer) {
    _observers.erase(remove(_observers.begin(), _observers.end(), observer), _observers.end());
}
void Subject::notifyObservers(EventArgs* args) {
    for (IObserverBase* observer : _observers) {
        if ((!observer->getEnabledFunc && observer->enabled)
            || (observer->getEnabledFunc && observer->getEnabledFunc()))
            observer->updateInternal(args);
    }
}

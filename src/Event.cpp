#include "Event.h"

namespace padbus {
    template <typename TArgs>
    void Event<TArgs>::Subject::attachObserver(IObserver* observer) {
        if (observer == nullptr) {
            // Don't attach null observers
            return;
        }

        _observers.push_back(observer);
    }

    template <typename TArgs>
    void Event<TArgs>::Subject::detachObserver(IObserver* observer) {
        if (observer == nullptr) {
            // Ignore null observers
            return;
        }

        _observers.erase(remove(_observers.begin(), _observers.end(), observer), _observers.end());
    }

    template <typename TArgs>
    void Event<TArgs>::Subject::notifyObservers(TArgs* args) {
        for (IObserver* observer : _observers) {
            if ((!observer->getEnabledFunc && observer->enabled)
                || (observer->getEnabledFunc && observer->getEnabledFunc()))
            observer->update(args);
        }
    }

    template <typename TArgs>
    void Event<TArgs>::notifyObservers(TArgs* args) {
        _subject->notifyObservers(args);
    }
}
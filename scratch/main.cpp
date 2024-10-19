#include <iostream>
#include <string>
#include <Event.h>

using namespace std;
using namespace padbus;
struct ExtendedEventArgs : EventArgs {
    string extraData;
};

class MockObservable : public IObservable {
    Event<> normalEvent;
    Event<ExtendedEventArgs> extendedEvent;
public:
    Event<>* onNormalEvent() {return &normalEvent;}

    void FireEvent() {
        handleEvent(&normalEvent, new EventArgs());
    }

    void FireExtendedEvent(ExtendedEventArgs* e) {
        handleEvent(&extendedEvent, e);
    }
};

template <typename TArgs = EventArgs>
class TestObserver final : public Event<TArgs>::IObserver {
    int _triggerCount = 0;
    TArgs* _lastEventArgs;
    void update(TArgs* args) override {
        _triggerCount++;
        _lastEventArgs = args;
    }
public:
    ~TestObserver() = default;
    explicit TestObserver(Event<TArgs>* event) : Event<TArgs>::IObserver(event), _lastEventArgs(nullptr) { }

    void reset() { _triggerCount = 0; }
    [[nodiscard]] int getTriggerCount() const { return _triggerCount; }
    [[nodiscard]] TArgs* getLastEventArgs() { return _lastEventArgs; }
};

int main() {

    auto observable = MockObservable();
    //auto observer = TestObserver(observable.onNormalEvent());
    return 0;
}

#include <gtest/gtest.h>
#include <PadBus/Event.h>

using namespace std;
using namespace padbus;

class ExtendedEventArgs : public EventArgs {
    public:
        string extraData;
};

class MockObservable : public IObservable {
    Event<> normalEvent;
    Event<ExtendedEventArgs> extendedEvent;
    public:
        Event<>* onNormalEvent() {return &normalEvent;}
        Event<ExtendedEventArgs>* onExtendedEvent() {return &extendedEvent;}

        void FireEvent() {
            handleEvent(&normalEvent, new EventArgs());
        }

        void FireExtendedEvent(ExtendedEventArgs* e) {
            handleEvent(&extendedEvent, e);
        }
};

template <typename TArgs = EventArgs>
class TestObserver final : public IObserver<TArgs> {
    int _triggerCount = 0;
    TArgs* _lastEventArgs;
    void update(TArgs* args) override {
        _triggerCount++;
        _lastEventArgs = args;
    }
    public:
        explicit TestObserver(Event<TArgs>* event) : IObserver<TArgs>(event), _lastEventArgs(nullptr) { }
        ~TestObserver() override = default;
        void reset() { _triggerCount = 0; }
        [[nodiscard]] int getTriggerCount() const { return _triggerCount; }
        [[nodiscard]] TArgs* getLastEventArgs() { return _lastEventArgs; }
};

TEST (Event_tests, Simple_Event_Triggers_Observer) {
    auto observable = MockObservable();
    auto observer = TestObserver(observable.onNormalEvent());
    ASSERT_EQ(0, observer.getTriggerCount());
    observable.FireEvent();
    ASSERT_EQ(1, observer.getTriggerCount());
    observable.FireEvent();
    ASSERT_EQ(2, observer.getTriggerCount());
}

TEST (Event_tests, Disabled_Observer_Static_Is_Not_Called) {
    auto observable = MockObservable();
    auto observer = TestObserver(observable.onNormalEvent());
    ASSERT_EQ(0, observer.getTriggerCount());
    observable.FireEvent();
    ASSERT_EQ(1, observer.getTriggerCount());
    observer.enabled = false;
    observable.FireEvent();
    ASSERT_EQ(1, observer.getTriggerCount());
}

TEST (Event_tests, Disabled_Observer_Dynamic_Is_Not_Called) {
    auto observable = MockObservable();
    auto observer = TestObserver(observable.onNormalEvent());

    observer.getEnabledFunc = [&observer]{ return observer.getTriggerCount() < 1; };

    ASSERT_EQ(0, observer.getTriggerCount());
    observable.FireEvent();
    ASSERT_EQ(1, observer.getTriggerCount());
    ASSERT_EQ(&observable, observer.getLastEventArgs()->getSender());
    observable.FireEvent();
    ASSERT_EQ(1, observer.getTriggerCount());
    ASSERT_EQ(&observable, observer.getLastEventArgs()->getSender());
}

TEST (Event_tests, Multiple_Observers_Are_Called_As_Expected) {
    auto observable = MockObservable();
    auto observer1 = TestObserver(observable.onNormalEvent());
    auto observer2 = TestObserver(observable.onNormalEvent());
    auto observer3 = TestObserver(observable.onNormalEvent());
    observable.FireEvent();
    ASSERT_EQ(1, observer1.getTriggerCount());
    ASSERT_EQ(&observable, observer1.getLastEventArgs()->getSender());
    ASSERT_EQ(1, observer2.getTriggerCount());
    ASSERT_EQ(&observable, observer2.getLastEventArgs()->getSender());
    ASSERT_EQ(1, observer3.getTriggerCount());
    ASSERT_EQ(&observable, observer3.getLastEventArgs()->getSender());
    observer2.enabled = false;
    observable.FireEvent();
    ASSERT_EQ(2, observer1.getTriggerCount());
    ASSERT_EQ(1, observer2.getTriggerCount());
    ASSERT_EQ(2, observer3.getTriggerCount());
}

TEST (Event_tests, Extended_EventArgs_Are_Sent_To_Observer) {
    auto observable = MockObservable();
    auto observer = TestObserver(observable.onExtendedEvent());
    ExtendedEventArgs extendedEventArgs = ExtendedEventArgs();
    extendedEventArgs.extraData = "Test Data!";
    observable.FireExtendedEvent(&extendedEventArgs);
    ASSERT_EQ(1, observer.getTriggerCount());
    ASSERT_EQ(&observable, observer.getLastEventArgs()->getSender());
    ASSERT_EQ(&extendedEventArgs, observer.getLastEventArgs());
}

TEST (Event_tests, Deleted_Observers_Are_No_Longer_Called) {
    auto observable = MockObservable();
    auto* observer1 = new TestObserver(observable.onNormalEvent());
    auto* observer2 = new TestObserver(observable.onNormalEvent());
    observable.FireEvent();
    delete observer1;
    observable.FireEvent();
    ASSERT_EQ(2, observer2->getTriggerCount());
    delete observer2;
}
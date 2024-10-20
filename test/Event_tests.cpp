#include <gtest/gtest.h>
#include <PadBus/Event.h>

using namespace std;
using namespace padbus;

class ExtendedEventArgs final : public EventArgs {
    public:
        string extraData;
};

class MockObservable : public IObservable {
    Event<> normalEvent;
    Event<ExtendedEventArgs> extendedEvent;
    public:
        Event<>* onNormalEvent() {return &normalEvent;}
        Event<ExtendedEventArgs>* onExtendedEvent() {return &extendedEvent;}

        void fireNormalEvent() {
            handleEvent(&normalEvent, new EventArgs());
        }

        void fireExtendedEvent(ExtendedEventArgs* e) {
            handleEvent(&extendedEvent, e);
        }
};

template <typename TArgs = EventArgs>
class TestObserver final : public IObserver<TArgs> {
    int _triggerCount;
    TArgs* _lastEventArgs;
    void update(TArgs* args) override {
        _triggerCount++;
        _lastEventArgs = args;
    }
    public:
        explicit TestObserver(Event<TArgs>* event) :
            IObserver<TArgs>(event), _triggerCount(0), _lastEventArgs(nullptr) { }
        ~TestObserver() override = default;
        void reset() {
            this->getEnabledFunc = nullptr;
            this->enabled = true;
            _triggerCount = 0;
            _lastEventArgs = nullptr;
        }
        [[nodiscard]] int getTriggerCount() const { return _triggerCount; }
        [[nodiscard]] TArgs* getLastEventArgs() { return _lastEventArgs; }
};

class Event_tests : public ::testing::Test {
    public:
        MockObservable* observable;
        TestObserver<>* normalObserver1;
        TestObserver<>* normalObserver2;
        TestObserver<>* normalObserver3;
        TestObserver<ExtendedEventArgs>* extendedObserver1;
        TestObserver<ExtendedEventArgs>* extendedObserver2;
        TestObserver<ExtendedEventArgs>* extendedObserver3;

        Event_tests() :
            observable(new MockObservable()),
            normalObserver1(new TestObserver(observable->onNormalEvent())),
            normalObserver2(new TestObserver(observable->onNormalEvent())),
            normalObserver3(new TestObserver(observable->onNormalEvent())),
            extendedObserver1(new TestObserver(observable->onExtendedEvent())),
            extendedObserver2(new TestObserver(observable->onExtendedEvent())),
            extendedObserver3(new TestObserver(observable->onExtendedEvent())) {}

        ~Event_tests() override {
            delete observable;
            delete normalObserver1;
            delete normalObserver2;
            delete normalObserver3;
            delete extendedObserver1;
            delete extendedObserver2;
            delete extendedObserver3;
        }

        void SetUp() override {
            normalObserver1->reset();
            normalObserver2->reset();
            normalObserver3->reset();
            extendedObserver1->reset();
            extendedObserver2->reset();
            extendedObserver3->reset();

            assertNormalObservers(0);
            assertExtendedObservers(0, nullptr);
        }

        template <typename TArgs = EventArgs>
        void assertObserver(TestObserver<TArgs>* observer, const int expectedTriggerCount) const {
            ASSERT_EQ(expectedTriggerCount, observer->getTriggerCount());
            if (expectedTriggerCount > 0) {
                ASSERT_NE(observer->getLastEventArgs(), nullptr);
                ASSERT_EQ(observer->getLastEventArgs()->getSender(), observable);
            } else {
                ASSERT_EQ(observer->getLastEventArgs(), nullptr);
            }
        }

        void assertNormalObservers(const int expectedCount1, const int expectedCount2, const int expectedCount3) const {
            assertObserver(normalObserver1, expectedCount1);
            assertObserver(normalObserver2, expectedCount2);
            assertObserver(normalObserver3, expectedCount3);
        }

        void assertNormalObservers(const int expectedCount) const {
            assertNormalObservers(expectedCount, expectedCount, expectedCount);
        }

        void assertExtendedObservers(const int expectedCount1, const int expectedCount2, const int expectedCount3) const {
            assertObserver(extendedObserver1, expectedCount1);
            assertObserver(extendedObserver2, expectedCount2);
            assertObserver(extendedObserver3, expectedCount3);
        }

        void assertExtendedObservers(const int expectedCount, const string* extraData) const {
            assertExtendedObservers(expectedCount, expectedCount, expectedCount);
            if (expectedCount > 0) {
                ASSERT_EQ(extendedObserver1->getLastEventArgs()->extraData, *extraData);
                ASSERT_EQ(extendedObserver2->getLastEventArgs()->extraData, *extraData);
                ASSERT_EQ(extendedObserver3->getLastEventArgs()->extraData, *extraData);
            }
        }
};

TEST_F (Event_tests, Simple_Event_Triggers_Observer) {
    observable->fireNormalEvent();
    assertNormalObservers(1);
    observable->fireNormalEvent();
    assertNormalObservers(2);
    observable->fireNormalEvent();
    assertNormalObservers(3);
}

TEST_F (Event_tests, Disabled_Observer_Static_Is_Not_Called) {
    observable->fireNormalEvent();
    assertNormalObservers(1);
    normalObserver1->enabled = false;
    observable->fireNormalEvent();
    assertNormalObservers(1, 2, 2);
    normalObserver1->enabled = true;
    normalObserver2->enabled = false;
    observable->fireNormalEvent();
    assertNormalObservers(2, 2, 3);
}

TEST_F (Event_tests, Disabled_Observer_Dynamic_Is_Not_Called) {
    normalObserver1->getEnabledFunc = [this]{ return normalObserver2->getTriggerCount() != 1; };
    normalObserver3->getEnabledFunc = [this]{ return normalObserver2->getTriggerCount() < 3; };

    observable->fireNormalEvent();
    assertNormalObservers(1);

    observable->fireNormalEvent();
    assertNormalObservers(1, 2, 2);

    observable->fireNormalEvent();
    assertNormalObservers(2, 3, 2);
}

TEST_F (Event_tests, Extended_EventArgs_Are_Sent_To_Observer) {
    auto args = ExtendedEventArgs();
    args.extraData = "Test Data!";

    observable->fireExtendedEvent(&args);
    assertExtendedObservers(1, &args.extraData);
}

TEST_F (Event_tests, Deleted_Observers_Are_No_Longer_Called) {
    auto* observer = new TestObserver(observable->onNormalEvent());
    observable->fireNormalEvent();
    assertObserver<EventArgs>(observer, 1);
    delete observer;
    observable->fireNormalEvent();
}
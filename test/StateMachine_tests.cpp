#include <gtest/gtest.h>
#include <PadBus/IState.h>
#include <PadBus/StateMachine.h>
#include <PadBus/Event.h>

using namespace std;
using namespace padbus;

enum TestState {
    StateA,
    StateB,
    StateC
};
class TestStateBase;
class TestStateContext : StateContext {
    public:
        unordered_map<TestState, int> enterCounts = unordered_map<TestState, int>();
        unordered_map<TestState, int> exitCounts = unordered_map<TestState, int>();
        unordered_map<TestState, function<bool()>> enableFunctions = unordered_map<TestState, function<bool()>>();
        unordered_map<TestState, function<void(TestStateBase*)>> enterFunctions = unordered_map<TestState, function<void(TestStateBase*)>>();
        unordered_map<TestState, function<void(TestStateBase*)>> exitFunctions = unordered_map<TestState, function<void(TestStateBase*)>>();
        unordered_map<TestState, TestStateBase*> testStates = unordered_map<TestState, TestStateBase*>();
};

class TestStateBase : public IState<TestState, TestStateContext> {
    protected:
        explicit TestStateBase(const TestState id) : IState(id) {}
    public:
        ~TestStateBase() override = default;

        void onEnterState() final {
            ++context()->enterCounts[id()];
            cout << "Entering State:" << id() << endl;
            context()->enterFunctions[id()](this);
        }

        bool enabled() override {
            return context()->enableFunctions.at(id())();
        }

        void onExitState() final {
            ++context()->exitCounts[id()];
            cout << "Exited State:" << id() << endl;
            context()->exitFunctions[id()](this);
        }

        void onRegister() override {
            testReset();
            context()->enableFunctions[id()] = []{return true;};
            context()->enterFunctions[id()] = [](const TestStateBase*) {  };
            context()->exitFunctions[id()] = [](const TestStateBase*) {  };
            context()->testStates[id()] = this;
        }

        void testReset() {
            context()->enterCounts[id()] = 0;
            context()->exitCounts[id()] = 0;
        }

        // Exposing protected method calls for dynamic functions in tests
        void callWait() { wait(); }
        void callResume() { resume(); }
        void callJumpToState(const TestState state) { jumpToState(state); }
};

class StateA_class final : public TestStateBase {
    public:
        StateA_class() : TestStateBase(StateA) {}
};

class StateB_class final : public TestStateBase {
    public:
        StateB_class() : TestStateBase(StateB) {}
};

class StateC_class final : public TestStateBase {
    public:
        StateC_class() : TestStateBase(StateC) {}
};

class StateMachine_tests : public ::testing::Test, public IObservable {
    Event<> event;
    public:
        StateMachine<TestState, TestStateContext>* stateMachine;

        StateMachine_tests() : stateMachine(new StateMachine<TestState, TestStateContext>()) {
            stateMachine->registerState<StateA_class>();
            stateMachine->registerState<StateB_class>();
            stateMachine->registerState<StateC_class>();
        }
        ~StateMachine_tests() override {
            delete stateMachine;
        }

        Event<>* onEvent() {return &event;}
        void fireEvent() {handleEvent(&event, new EventArgs());}
        [[nodiscard]] TestStateContext* context() const { return stateMachine->context(); }
};

TEST_F (StateMachine_tests, States_transition_as_expected) {
    context()->enableFunctions[StateB] = [this]{return context()->enterCounts[StateB] == 0;};
    context()->enterFunctions[StateB] = [this](TestStateBase* state) { state->callJumpToState(StateA); };
    stateMachine->initialize();
    ASSERT_EQ(context()->enterCounts[StateA], 2);
    ASSERT_EQ(context()->exitCounts[StateA], 2);
    ASSERT_EQ(context()->enterCounts[StateB], 1);
    ASSERT_EQ(context()->exitCounts[StateB], 1);
    ASSERT_EQ(context()->enterCounts[StateC], 1);
    ASSERT_EQ(context()->exitCounts[StateC], 1);
}

TEST_F (StateMachine_tests, State_Waits_For_External_Event_Before_Continuing) {

    auto eventObserver = FunctionObserver<>(onEvent(), [this](EventArgs*) {
        cout << "Event Caught!" << endl;
        context()->testStates[StateB]->callResume();
    });
    context()->enterFunctions[StateB] = [this](TestStateBase* state) {
        state->callWait();
    };
    stateMachine->initialize();
    ASSERT_EQ(context()->enterCounts[StateA], 1);
    ASSERT_EQ(context()->exitCounts[StateA], 1);
    ASSERT_EQ(context()->enterCounts[StateB], 1);
    ASSERT_EQ(context()->exitCounts[StateB], 0);
    fireEvent();
    ASSERT_EQ(context()->exitCounts[StateB], 1);
    ASSERT_EQ(context()->enterCounts[StateC], 1);
    ASSERT_EQ(context()->exitCounts[StateC], 1);
}
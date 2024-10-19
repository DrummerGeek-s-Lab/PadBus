#ifndef PADBUS_ISTATE_H
#define PADBUS_ISTATE_H

#include <memory>
#include <string>

using namespace std;

namespace padbus {
    struct StateContext {};
    class StateMachineBase;

    template <typename TContext = StateContext>
    class StateMachine;

    class IStateBase {
        protected:
            virtual void setStateMachine(StateMachineBase* stateMachine) = 0;
            [[nodiscard]] virtual StateMachineBase* getStateMachine() const = 0;
            void transitionInternal(shared_ptr<IStateBase> nextState) const;
            virtual void onEnter() = 0;
            virtual void onExit() {}
        public:
            [[nodiscard]] virtual string getName() const = 0;
            virtual ~IStateBase() = default;

        friend class StateMachineBase;
    };

    template <class TContext = StateContext>
    class IState : public IStateBase {
        StateMachine<TContext>* _stateMachine = nullptr;;
        void setStateMachine(StateMachineBase* stateMachine) final { _stateMachine = stateMachine; }
        StateMachine<TContext>* getStateMachine() const final { return _stateMachine; }

        protected:
            TContext* getContext() { return _stateMachine->getContext(); }
            void transition(shared_ptr<IState> nextState) { transitionInternal(nextState); }

        public:
            ~IState() override = default;
    };
}

#endif

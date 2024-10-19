#ifndef PADBUS_STATEMACHINE_H
#define PADBUS_STATEMACHINE_H

#include <memory>
#include "IState.h"

using namespace std;

namespace padbus {
    class StateMachineBase {
        bool _isInitialized = false;
        shared_ptr<IStateBase> _initialState = nullptr;
        shared_ptr<IStateBase> _currentState = nullptr;

        protected:
            void initializeInternal(shared_ptr<IStateBase> initialState);
            void transitionInternal(shared_ptr<IStateBase> nextState);
            void resetInternal();
            [[nodiscard]] shared_ptr<IStateBase> getCurrentStateInternal();

        public:
            virtual ~StateMachineBase() = default;

        friend class IStateBase;
    };

    template<typename TContext>
    class StateMachine : public StateMachineBase {
        TContext _context = TContext();
        public:
            void initialize(shared_ptr<IState<TContext>> initialState) { initializeInternal(initialState); }
            void transition(shared_ptr<IState<TContext>> nextState) { transitionInternal(nextState); }
            void reset() {
                _context = TContext();
                resetInternal();
            }
            IState<TContext> getCurrentState() { return getCurrentStateInternal(); }
            TContext getContext() { return _context; }
            ~StateMachine() override = default;
    };
}

#endif
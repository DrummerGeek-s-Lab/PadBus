#ifndef PADBUS_STATEMACHINE_H
#define PADBUS_STATEMACHINE_H

#include <memory>
#include "IState.h"

using namespace std;

namespace padbus {
    template <typename TContext>
    class StateMachine {
        bool _isInitialized = false;
        shared_ptr<IState<TContext>> _initialState = nullptr;
        shared_ptr<IState<TContext>> _currentState = nullptr;
        TContext _context;

        public:
            void initialize(shared_ptr<IState<TContext>> initialState);
            void transition(shared_ptr<IState<TContext>> nextState);
            void reset();
            [[nodiscard]] TContext* getContext();
            [[nodiscard]] shared_ptr<IState<TContext>> getCurrentState();
            ~StateMachine() = default;
    };
}

#endif
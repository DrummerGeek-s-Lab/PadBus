#include "StateMachine.h"

using namespace std;

namespace padbus {
    template<typename TContext>
    void StateMachine<TContext>::initialize(shared_ptr<IState<TContext>> initialState) {
        _initialState = move(initialState);
        _isInitialized = true;
        transition(_initialState);
    }

    template <typename TContext>
    void StateMachine<TContext>::transition(shared_ptr<IState<TContext>> nextState) {
        if (_currentState) {
            _currentState->onExit();
        }

        _currentState = move(nextState);
        _currentState->_stateMachine = this;
        _currentState->onEnter();
    }

    template<typename TContext>
    void StateMachine<TContext>::reset() {
        if (_currentState) {
            _currentState->onExit();
        }

        _currentState = nullptr;
        _context = TContext();
        transition(_initialState);
    }

    template<typename TContext>
    TContext* StateMachine<TContext>::getContext() {
        return &_context;
    }

    template<typename TContext>
    shared_ptr<IState<TContext> > StateMachine<TContext>::getCurrentState() {
        return _currentState;
    }

    template<typename TContext>
    TContext* IState<TContext>::getContext() {
        return _stateMachine->getContext();
    }

    template<typename TContext>
    void IState<TContext>::transition(shared_ptr<IState> nextState) {
        _stateMachine->transition(nextState);
    }
}
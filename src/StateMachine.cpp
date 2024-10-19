#include "StateMachine.h"

using namespace std;

namespace padbus {
    void StateMachineBase::initializeInternal(shared_ptr<IStateBase> initialState) {
        _initialState = move(initialState);
        _isInitialized = true;
        transitionInternal(_initialState);
    }

    void StateMachineBase::transitionInternal(shared_ptr<IStateBase> nextState) {
        if (_currentState) {
            _currentState->onExit();
        }

        _currentState = move(nextState);
        _currentState->setStateMachine(this);
        _currentState->onEnter();
    }

    void StateMachineBase::resetInternal() {
        if (_currentState) {
            _currentState->onExit();
        }

        _currentState = nullptr;
        transitionInternal(_initialState);
    }

    shared_ptr<IStateBase > StateMachineBase::getCurrentStateInternal() {
        return _currentState;
    }

    void IStateBase::transitionInternal(shared_ptr<IStateBase> nextState) const {
        getStateMachine()->transitionInternal(move(nextState));
    }
}
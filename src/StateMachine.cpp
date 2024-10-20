#include <stdexcept>
#include <PadBus/StateMachine.h>

using namespace std;

namespace padbus {
    StateMachineBase::StateMachineBase() :
        _registeredStates(map<int, unique_ptr<IStateBase>>()),
        _isInitialized(false),
        _isEnabled(false),
        _currentState(_registeredStates.begin()) { }

    void StateMachineBase::registerStateInternal(unique_ptr<IStateBase> state) {
        if (_isInitialized) {
            throw logic_error("Unable to register states once the state machine has been initialized");
        }
        if (_registeredStates.find(state->_id) != _registeredStates.end()) {
            throw invalid_argument("State ID already in use!");
        }

        auto stateId = state->_id;
        state->_stateMachine = this;
        _registeredStates[state->idInternal()] = move(state);
        _registeredStates[stateId]->onRegister();
    }

    void StateMachineBase::enterCurrentState() {
        // If we aren't enabled, have reached the end of our registered states,
        // have already entered the current state, or the state is disabled, just exit, there is nothing to do.
        if (!_isEnabled
            || _currentState == _registeredStates.end()
            || _currentState->second->_entered
            || !_currentState->second->enabled()) {
            return;
        }
        onEnteringState();
        _currentState->second->enterState();
        onEnteredState();
    }

    void StateMachineBase::exitCurrentState() {
        // If we aren't enabled, have reached the end of our registered states,
        // or have not entered the current state, just exit, there is nothing to do.
        if (!_isEnabled || _currentState == _registeredStates.end() || !_currentState->second->_entered) {
            return;
        }

        onExitingState();
        _currentState->second->exitState();
        onExitedState();
    }

    void StateMachineBase::nextState() {
        if (_currentState == _registeredStates.end()) {
            // We aren't on a valid state, exit the method
            return;
        }

        exitCurrentState();

        while(_nextState != _registeredStates.end() && !_nextState->second->enabled()) {
            ++_nextState;
        }

        _currentState = _nextState;

        if (_currentState == _registeredStates.end()) {
            // We've reached the end of the state list, exit the method
            return;
        }

        ++_nextState;

        enterCurrentState();
    }

    void StateMachineBase::jumpToStateInternal(const int stateId) {
        _nextState = _registeredStates.find(stateId);
    }

    void StateMachineBase::reset() {
        exitCurrentState();
        onReset();
        _currentState = _registeredStates.begin();
        enterCurrentState();
    }

    int StateMachineBase::currentStateInternal() const {
        if (_currentState == _registeredStates.end()) {
            return int();
        }

        return _currentState->first;
    }

    bool StateMachineBase::hasReachedEnd() {
        return _currentState == _registeredStates.end();
    }

    void StateMachineBase::initialize() {
        if (_isInitialized) {
            return;
        }

        _nextState = _currentState = _registeredStates.begin();
        ++_nextState;
        enable();
    }


    void StateMachineBase::disable() {
        if (!_isEnabled) {
            return;
        }
        onDisabling();
        exitCurrentState();
        _isEnabled = false;
        onDisabled();
    }

    void StateMachineBase::enable() {
        if (_isEnabled) {
            return;
        }
        onEnabling();
        _isEnabled = true;
        enterCurrentState();
        onEnabled();
    }

    IStateBase::IStateBase(const int id)
        : _id(id), _stateMachine(nullptr), _entered(false), _waiting(false) {
    }

    StateMachineBase* IStateBase::stateMachineInternal() const {
        return _stateMachine;
    }

    int IStateBase::idInternal() const {
        return _id;
    }

    void IStateBase::wait() {
        _waiting = true;
    }

    void IStateBase::resume() {
        _waiting = false;
        onResume();
    }

    void IStateBase::jumpToStateInternal(const int stateId) const {
        stateMachineInternal()->jumpToStateInternal(stateId);
    }

    bool IStateBase::enabled() {
        return true;
    }

    void IStateBase::enterState() {
        _entered = true;
        onEnterState();
        onResume();
    }

    void IStateBase::exitState() {
        if (!_entered) {
            return;
        }

        onExitState();
        reset();
    }

    void IStateBase::reset() {
        onReset();
        _entered = false;
        _waiting = false;
    }

    void IStateBase::onResume() {
        if (!_waiting) {
            stateMachineInternal()->nextState();
        }
    }

}
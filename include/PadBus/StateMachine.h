#ifndef PADBUS_STATEMACHINE_H
#define PADBUS_STATEMACHINE_H

#include <memory>
#include <map>

#include "IState.h"

using namespace std;

namespace padbus {
    class StateMachineBase {
        map<int, unique_ptr<IStateBase>> _registeredStates;
        bool _isInitialized;
        bool _isEnabled;
        map<int, unique_ptr<IStateBase>>::iterator _currentState;
        map<int, unique_ptr<IStateBase>>::iterator _nextState;
        void enterCurrentState();
        void exitCurrentState();

        protected:
            StateMachineBase();
            void registerStateInternal(unique_ptr<IStateBase> state);
            virtual void onEnteringState() {}
            virtual void onEnteredState() {}
            virtual void onExitingState() {}
            virtual void onExitedState() {}
            virtual void onEnabling() {}
            virtual void onEnabled() {}
            virtual void onDisabling() {}
            virtual void onDisabled() {}
            void nextState();
            void jumpToStateInternal(int stateId);
            virtual void onReset() {}
            [[nodiscard]] int currentStateInternal() const;

        public:
            bool hasReachedEnd();
            void initialize();
            void disable();
            void enable();
            void reset();
            virtual ~StateMachineBase() = default;

        friend class IStateBase;
    };

    template<typename TEnum, typename TContext, enable_if_t<is_enum_v<TEnum>, bool>>
    class StateMachine : public StateMachineBase {
        TContext _context = TContext();
        public:
            template<typename TState = IState<TEnum, TContext>>
            void registerState() {
                registerStateInternal(make_unique<TState>());
            }
            virtual void jumpToState(TEnum stateId) {
                jumpToStateInternal(static_cast<int>(stateId));
            }
            void onReset() override {
                _context = TContext();
            }
            [[nodiscard]] TContext* context() {
                return &_context;
            }
            [[nodiscard]] TEnum currentState() const {
                return static_cast<TEnum>(currentStateInternal());
            }
            ~StateMachine() override = default;
    };
}

#endif
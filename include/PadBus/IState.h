#ifndef PADBUS_ISTATE_H
#define PADBUS_ISTATE_H

using namespace std;

namespace padbus {
    struct StateContext {};
    class StateMachineBase;

    template<
        typename TEnum,
        typename TContext = StateContext,
        enable_if_t<is_enum_v<TEnum>, bool> = true
    >
    class StateMachine;

    class IStateBase {
        int _id;
        StateMachineBase* _stateMachine;
        bool _entered;
        bool _waiting;
        void reset();
        void onResume();
        protected:
            explicit IStateBase(int id);
            [[nodiscard]] StateMachineBase *stateMachineInternal() const;
            [[nodiscard]] int idInternal() const;
            void wait();
            void resume();
            void jumpToStateInternal(int stateId) const;
            virtual void onEnterState() {}
            virtual void onExitState() {}
            virtual void onReset() {}
            virtual void onRegister() {}
        public:
            virtual ~IStateBase() = default;
            virtual bool enabled();
            void enterState();
            void exitState();

        friend class StateMachineBase;
    };

    template<
        typename TEnum,
        typename TContext = StateContext,
        typename TStateMachine = StateMachine<TEnum, TContext>,
        enable_if_t<is_enum_v<TEnum>, bool> = true
    >
    class IState : public IStateBase {
        protected:
            explicit IState(TEnum id) : IStateBase(id) {}
            TStateMachine* stateMachine() const { return static_cast<TStateMachine*>(stateMachineInternal()); }
            TContext* context() { return stateMachine()->context(); }
            void jumpToState(TEnum stateId) { jumpToStateInternal(static_cast<int>(stateId)); }
        public:
            [[nodiscard]] TEnum id() const { return static_cast<TEnum>(idInternal()); };
            ~IState() override = default;
    };
}

#endif

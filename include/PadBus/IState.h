#ifndef PADBUS_ISTATE_H
#define PADBUS_ISTATE_H

#include <memory>
#include <string>

using namespace std;

namespace padbus {
    struct StateContext {};

    template <class TContext = StateContext>
    class StateMachine;

    template <class TContext = StateContext>
    class IState {
        StateMachine<TContext>* _stateMachine;

        protected:
            TContext* getContext();
            void transition(shared_ptr<IState> nextState);
            virtual void onEnter() = 0;
            virtual void onExit() {}

        public:
            [[nodiscard]] virtual string getName() const = 0;
            virtual ~IState() = default;

        friend class StateMachine<TContext>;
    };
}

#endif

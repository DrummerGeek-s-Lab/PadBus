#include <functional>
#include "Event.h"

using namespace std;

namespace padbus {
    template <typename TArgs = EventArgs>
    class FunctionObserver final : public Event<TArgs>::IObserver {
        function<void(TArgs*)> _fn;

        void update(TArgs* args) override {
            _fn(args);
        }

        public:
            explicit FunctionObserver(Event<TArgs>* event, function<void(TArgs*)> fn) :
                Event<TArgs>::IObserver(event), _fn(fn) {}
    };
}

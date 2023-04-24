#include "function_callable.h"

#include "interpreter.h"

Literal::Ptr Function_Callable::call(Interpreter &interpreter, const std::vector<Literal::Ptr> &arguments) const {
    Environment::Ptr env = std::make_shared<Environment>(interpreter.globals);
    for (int i = 0; i < definition->params.size(); ++i) {
        env->define(definition->params[i].lexeme, arguments[i]);
    }
    try {
        interpreter.execute_block(*definition->body, env);
    } catch (const Interpreter::Return &ret) {
        return ret.value;
    }
    return {};
}

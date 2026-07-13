#pragma once

namespace qlang {
    template <typename Fallback, typename... Lambdas>
    struct match : Fallback, Lambdas... {
        explicit match(Fallback&& f, Lambdas&&... l)
            : Fallback(std::forward<Fallback>(f))
            , Lambdas(std::forward<Lambdas>(l))... {}
        using Lambdas::operator()...;
        using Fallback::operator();
    };
}
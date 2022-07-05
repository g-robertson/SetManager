/*
    union-set.hpp
    Union set is a derivative set that holds the union of two already existing subsets of its parent
*/ 
#include "union-set.hpp"

#include <algorithm>

UnionSet::UnionSet(UserSet* parent, const std::string& name, UserSet* set1, UserSet* set2) noexcept
    : DerivativeSet(parent, name, {set1, set2})
{}

UnionSet::UnionSet(UserSet* parent, const std::string& name) noexcept
    : DerivativeSet(parent, name)
{}

UserSet* UnionSet::createSet(UserSet& parent, const std::string& name) noexcept {
    auto* set1 = parent.queryForSubset();
    if (set1 == nullptr) {
        return nullptr;
    }
    auto* set2 = parent.queryForSubset();
    if (set2 == nullptr) {
        return nullptr;
    }

    return new UnionSet(&parent, name, set1, set2);
}

const std::set<std::string>* UnionSet::elements() noexcept {
    const auto* set1Elements = derivesFrom().at(0)->elements();
    const auto* set2Elements = derivesFrom().at(1)->elements();

    if (set1Elements == nullptr || set2Elements == nullptr) {
        // infinite union something will always be infinite
        return nullptr;
    }

    output.clear();
    // does exactly what it says
    std::set_union(
        set1Elements->begin(), set1Elements->end(),
        set2Elements->begin(), set2Elements->end(),
        std::inserter(output, output.begin())
    );
    return &output;
}

const std::set<std::string>* UnionSet::complementElements() noexcept {
    const auto* set1ComplementElements = derivesFrom().at(0)->complementElements();
    const auto* set2ComplementElements = derivesFrom().at(1)->complementElements();

    if (set1ComplementElements == nullptr && set2ComplementElements == nullptr) {
        // only if both sets are finite is the union not infinite
        return nullptr;
    }

    output.clear();

    if (set2ComplementElements == nullptr) {
        // https://proofwiki.org/wiki/Set_Difference_as_Intersection_with_Complement
        // A Difference B = A Intersect ~B
        // => ~A Difference B = ~A Intersect ~B
        // https://proofwiki.org/wiki/De_Morgan%27s_Laws_(Set_Theory)/Set_Complement/Complement_of_Union
        // ~(A Union B) = ~A Intersect ~B
        // => ~A Difference B = ~(A Union B)
        // => ~(A Union B) = ~A Difference B
        const auto* set2Elements = derivesFrom().at(1)->elements();
        std::set_difference(
            set1ComplementElements->begin(), set1ComplementElements->end(),
            set2Elements->begin(), set2Elements->end(),
            std::inserter(output, output.begin())
        );
    } else if (set1ComplementElements == nullptr) {
        // same logic as prior case, but in reverse
        const auto* set1Elements = derivesFrom().at(0)->elements();
        std::set_difference(
            set2ComplementElements->begin(), set2ComplementElements->end(),
            set1Elements->begin(), set1Elements->end(),
            std::inserter(output, output.begin())
        );
    } else {
        // https://proofwiki.org/wiki/De_Morgan%27s_Laws_(Set_Theory)/Set_Complement/Complement_of_Union
        // ~(A Union B) = ~A Intersect ~B
        std::set_union(
            set1ComplementElements->begin(), set1ComplementElements->end(),
            set2ComplementElements->begin(), set2ComplementElements->end(),
            std::inserter(output, output.begin())
        );
    }
    return &output;
}
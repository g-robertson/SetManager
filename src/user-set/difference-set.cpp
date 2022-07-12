/*
    difference-set.hpp
    Difference set is a derivative set that holds the difference of two already existing subsets of its parent
*/ 
#include "difference-set.hpp"

#include <algorithm>

DifferenceSet::DifferenceSet(UserSet* parent, const pstring& name, UserSet* set1, UserSet* set2) noexcept
    : DerivativeSet(parent, name, {set1, set2})
{}

DifferenceSet::DifferenceSet(UserSet* parent, const pstring& name) noexcept
    : DerivativeSet(parent, name)
{}

UserSet* DifferenceSet::createSet(UserSet& parent, const pstring& name) noexcept {
    auto* set1 = parent.queryForSubset();
    if (set1 == nullptr) {
        return nullptr;
    }
    auto* set2 = parent.queryForSubset();
    if (set2 == nullptr) {
        return nullptr;
    }

    return new DifferenceSet(&parent, name, set1, set2);
}

void DifferenceSet::updateElements() noexcept {
    elements_.release();
    complementElements_.release();

    const auto* set1Elements = derivesFrom().at(0)->elements();
    const auto* set2Elements = derivesFrom().at(1)->elements();

    if (set1Elements == nullptr && set2Elements != nullptr) {
        // infinite - finite will always be infinite, anything else (including infinite - infinite) will always be finite
        complementElements_ = std::make_unique<std::set<pstring>>();
        const auto* set1ComplementElements = derivesFrom().at(0)->complementElements();
        // https://proofwiki.org/wiki/Set_Difference_as_Intersection_with_Complement
        // A Difference B = A Intersect ~B
        // => ~(A Difference B) = ~(A Intersect ~B)
        // https://proofwiki.org/wiki/De_Morgan%27s_Laws_(Set_Theory)/Set_Complement/Complement_of_Union
        // ~(A Intersect B) = ~A Union ~B
        // => ~(A Difference B) = ~A Union B
        std::set_union(
            set1ComplementElements->begin(), set1ComplementElements->end(),
            set2Elements->begin(), set2Elements->end(),
            std::inserter(*complementElements_, complementElements_->begin())
        );
    } else {
        elements_ = std::make_unique<std::set<pstring>>();
        if (set1Elements == nullptr && set2Elements == nullptr) {
            // https://proofwiki.org/wiki/Set_Difference_of_Complements
            // ~B Difference ~A = A Difference B
            // => A Difference B = ~B Difference ~A
            const auto* set1ComplementElements = derivesFrom().at(0)->complementElements();
            const auto* set2ComplementElements = derivesFrom().at(1)->complementElements();
            std::set_difference(
                set2ComplementElements->begin(), set2ComplementElements->end(),
                set1ComplementElements->begin(), set1ComplementElements->end(),
                std::inserter(*elements_, elements_->begin())
            );
        } else if (set2Elements == nullptr) {
            // https://proofwiki.org/wiki/Set_Difference_as_Intersection_with_Complement
            // A Difference B = A Intersect ~B
            const auto* set2ComplementElements = derivesFrom().at(1)->complementElements();
            std::set_intersection(
                set1Elements->begin(), set1Elements->end(),
                set2ComplementElements->begin(), set2ComplementElements->end(),
                std::inserter(*elements_, elements_->begin())
            );
        } else {
            // does exactly what it says
            std::set_difference(
                set1Elements->begin(), set1Elements->end(),
                set2Elements->begin(), set2Elements->end(),
                std::inserter(*elements_, elements_->begin())
            );
        }
    }
}
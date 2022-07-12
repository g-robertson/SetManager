/*
    intersection-set.hpp
    Intersection set is a derivative set that holds the intersection of two already existing subsets of its parent
*/ 
#include "intersection-set.hpp"

#include <algorithm>

IntersectionSet::IntersectionSet(UserSet* parent, const pstring& name, UserSet* set1, UserSet* set2) noexcept
    : DerivativeSet(parent, name, {set1, set2})
{}

IntersectionSet::IntersectionSet(UserSet* parent, const pstring& name) noexcept
    : DerivativeSet(parent, name)
{}

UserSet* IntersectionSet::createSet(UserSet& parent, const pstring& name) noexcept {
    auto* set1 = parent.queryForSubset();
    if (set1 == nullptr) {
        return nullptr;
    }
    auto* set2 = parent.queryForSubset();
    if (set2 == nullptr) {
        return nullptr;
    }

    return new IntersectionSet(&parent, name, set1, set2);
}

void IntersectionSet::updateElements() noexcept {
    elements_.release();
    complementElements_.release();

    const auto* set1Elements = derivesFrom().at(0)->elements();
    const auto* set2Elements = derivesFrom().at(1)->elements();
    if (set1Elements == nullptr && set2Elements == nullptr) {
        // you cannot specify an infinite number of complement elements to negate the infinite sets that will be combined, so this set must be infinite
        complementElements_ = std::make_unique<std::set<pstring>>();
        const auto* set1ComplementElements = derivesFrom().at(0)->complementElements();
        const auto* set2ComplementElements = derivesFrom().at(1)->complementElements();
        // https://proofwiki.org/wiki/De_Morgan%27s_Laws_(Set_Theory)/Set_Complement/Complement_of_Intersection
        // ~(A Intersect B) = ~A Union ~B
        std::set_union(
            set1ComplementElements->begin(), set1ComplementElements->end(),
            set2ComplementElements->begin(), set2ComplementElements->end(),
            std::inserter(*complementElements_, complementElements_->begin())
        );
    } else {
        elements_ = std::make_unique<std::set<pstring>>();
        if (set2Elements == nullptr) {
            // https://proofwiki.org/wiki/Set_Difference_as_Intersection_with_Complement
            // A Difference B = A Intersect ~B
            // => A Difference ~B = A Intersect B
            const auto* set2ComplementElements = derivesFrom().at(1)->complementElements();
            std::set_difference(
                set1Elements->begin(), set1Elements->end(),
                set2ComplementElements->begin(), set2ComplementElements->end(),
                std::inserter(*elements_, elements_->begin())
            );
        } else if (set1Elements == nullptr) {
            // same logic as prior case, but in reverse
            const auto* set1ComplementElements = derivesFrom().at(0)->complementElements();
            std::set_difference(
                set2Elements->begin(), set2Elements->end(),
                set1ComplementElements->begin(), set1ComplementElements->end(),
                std::inserter(*elements_, elements_->begin())
            );
        } else {
            // does exactly what it says
            std::set_intersection(
                set1Elements->begin(), set1Elements->end(),
                set2Elements->begin(), set2Elements->end(),
                std::inserter(*elements_, elements_->begin())
            );
        }
    }
    
}
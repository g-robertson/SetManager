/*
    symmetric-difference-set.hpp
    SymmetricDifference set is a derivative set that holds the symmetric difference of two already existing subsets of its parent
*/ 
#include "symmetric-difference-set.hpp"

#include <algorithm>

SymmetricDifferenceSet::SymmetricDifferenceSet(UserSet* parent, const std::string& name, UserSet* set1, UserSet* set2) noexcept
    : DerivativeSet(parent, name, {set1, set2})
{}

SymmetricDifferenceSet::SymmetricDifferenceSet(UserSet* parent, const std::string& name) noexcept
    : DerivativeSet(parent, name)
{}

UserSet* SymmetricDifferenceSet::createSet(UserSet& parent, const std::string& name) noexcept {
    auto* set1 = parent.queryForSubset();
    if (set1 == nullptr) {
        return nullptr;
    }
    auto* set2 = parent.queryForSubset();
    if (set2 == nullptr) {
        return nullptr;
    }

    return new SymmetricDifferenceSet(&parent, name, set1, set2);
}

const std::set<std::string>* SymmetricDifferenceSet::elements() noexcept {
    const auto* set1Elements = derivesFrom().at(0)->elements();
    const auto* set2Elements = derivesFrom().at(1)->elements();

    // symmetric difference between a finite set and infinite set yields an infinite set,
    // so if set1 is different infinity type from set2, return nullptr
    if ((set1Elements == nullptr) != (set2Elements == nullptr)) {
        return nullptr;
    }

    if (set1Elements == nullptr && set2Elements == nullptr) {
        const auto* set1ComplementElements = derivesFrom().at(0)->complementElements();
        const auto* set2ComplementElements = derivesFrom().at(1)->complementElements();
        // https://proofwiki.org/wiki/Symmetric_Difference_of_Complements
        // ~A SymmetricDifference ~B = A SymmetricDifference B
        std::set_symmetric_difference(
            set1ComplementElements->begin(), set1ComplementElements->end(),
            set2ComplementElements->begin(), set2ComplementElements->end(),
            std::inserter(output, output.begin())
        );
    } else {
        // does exactly what it says
        std::set_symmetric_difference(
            set1Elements->begin(), set1Elements->end(),
            set2Elements->begin(), set2Elements->end(),
            std::inserter(output, output.begin())
        );
    }

    return &output;
}

const std::set<std::string>* SymmetricDifferenceSet::complementElements() noexcept {
    const auto* set1ComplementElements = derivesFrom().at(0)->complementElements();
    const auto* set2ComplementElements = derivesFrom().at(1)->complementElements();

    // symmetric difference between a finite set and infinite set yields an infinite set,
    // so if set1 is the same infinity type as set2, return nullptr because this is a finite set
    if ((set1ComplementElements == nullptr) == (set2ComplementElements == nullptr)) {
        return nullptr;
    }
    
    if (set2ComplementElements == nullptr) {
        const auto* set2Elements = derivesFrom().at(1)->elements();
        // https://proofwiki.org/wiki/Category:Symmetric_Difference Definition 3
        // A SymmetricDifference B = (A Intersect ~B) Union (~A Intersect B)
        // ~(A SymmetricDifference B) = ~((A Intersect ~B) Union (~A Intersect B))
        // https://proofwiki.org/wiki/De_Morgan%27s_Laws_(Set_Theory)/Set_Complement/Complement_of_Union
        // ~(A Union B) = ~A Intersect ~B
        // => ~(A SymmetricDifference B) = ~(A Intersect ~B) Intersect ~(~A Intersect B)
        // https://proofwiki.org/wiki/Set_Difference_as_Intersection_with_Complement
        // A Difference B = A Intersect ~B
        // => ~(A SymmetricDifference B) = ~(A Intersect ~B) Difference (~A Intersect B)
        // https://proofwiki.org/wiki/De_Morgan%27s_Laws_(Set_Theory)/Set_Complement/Complement_of_Intersection
        // ~(A Intersect B) = ~A Union ~B
        // => ~(A SymmetricDifference B) = (~A Union B) Difference (~A Intersect B)
        std::set<std::string> unionSet;
        std::set<std::string> intersectSet;
        std::set_union(
            set1ComplementElements->begin(), set1ComplementElements->end(),
            set2Elements->begin(), set2Elements->end(),
            std::inserter(unionSet, unionSet.begin())
        );
        std::set_intersection(
            set1ComplementElements->begin(), set1ComplementElements->end(),
            set2Elements->begin(), set2Elements->end(),
            std::inserter(intersectSet, intersectSet.begin())
        );
        std::set_difference(
            unionSet.begin(), unionSet.end(),
            intersectSet.begin(), intersectSet.end(),
            std::inserter(output, output.begin())
        );
    } else {
        const auto* set1Elements = derivesFrom().at(0)->elements();
        // same thing as above, but backwards
        std::set<std::string> unionSet;
        std::set<std::string> intersectSet;
        std::set_union(
            set2ComplementElements->begin(), set2ComplementElements->end(),
            set1Elements->begin(), set1Elements->end(),
            std::inserter(unionSet, unionSet.begin())
        );
        std::set_intersection(
            set2ComplementElements->begin(), set2ComplementElements->end(),
            set1Elements->begin(), set1Elements->end(),
            std::inserter(intersectSet, intersectSet.begin())
        );
        std::set_difference(
            unionSet.begin(), unionSet.end(),
            intersectSet.begin(), intersectSet.end(),
            std::inserter(output, output.begin())
        );
    }

    return &output;
}
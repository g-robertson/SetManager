/*
    relative-complement-set.hpp
    RelativeComplement set is a derivative set that holds the relative complement of an existing subset relative to the parent
*/ 
#include "relative-complement-set.hpp"

#include <algorithm>

RelativeComplementSet::RelativeComplementSet(UserSet* parent, const std::string& name, UserSet* set) noexcept
    : DerivativeSet(parent, name, {set})
{}

RelativeComplementSet::RelativeComplementSet(UserSet* parent, const std::string& name) noexcept
    : DerivativeSet(parent, name)
{}

UserSet* RelativeComplementSet::createSet(UserSet& parent, const std::string& name) noexcept {
    auto* set = parent.queryForSubset();
    if (set == nullptr) {
        return nullptr;
    }

    return new RelativeComplementSet(&parent, name, set);
}

void RelativeComplementSet::updateElements() noexcept {
    elements_.release();
    complementElements_.release();

    const auto* parentElements = parent_->elements();
    const auto* setElements = derivesFrom().at(0)->elements();

    if (parentElements == nullptr && setElements != nullptr) {
        // the only infinite relative complement set occurs when both parent set is infinite, and subset is finite
        complementElements_ = std::make_unique<std::set<std::string>>();
        const auto* parentComplementElements = parent_->complementElements();
        // https://proofwiki.org/wiki/Set_Difference_as_Intersection_with_Complement
        // A Difference B = A Intersect ~B
        // => ~(A Difference B) = ~(A Intersect ~B)
        // https://proofwiki.org/wiki/De_Morgan%27s_Laws_(Set_Theory)/Set_Complement/Complement_of_Union
        // ~(A Intersect B) = ~A Union ~B
        // => ~(A Difference B) = ~A Union B
        // https://proofwiki.org/wiki/Definition:Relative_Complement
        // RelativeComplement(A, B) = A Difference B
        // => ~(RelativeComplement(A, B)) = ~A Union B
        std::set_union(
            parentComplementElements->begin(), parentComplementElements->end(),
            setElements->begin(), setElements->end(),
            std::inserter(*complementElements_, complementElements_->begin())
        );
    } else {
        elements_ = std::make_unique<std::set<std::string>>();
        if (parentElements == nullptr && setElements == nullptr) {
            const auto* parentComplementElements = parent_->complementElements();
            const auto* setComplementElements = derivesFrom()[0]->complementElements();
            // https://proofwiki.org/wiki/Definition:Relative_Complement
            // RelativeComplement(A, B) = A Difference B
            // https://proofwiki.org/wiki/Set_Difference_of_Complements
            // ~B Difference ~A = A Difference B
            // => A Difference B = ~B Difference ~A
            // => RelativeComplement(A, B) = ~B Difference ~A
            std::set_difference(
                setComplementElements->begin(), setComplementElements->end(),
                parentComplementElements->begin(), parentComplementElements->end(),
                std::inserter(*elements_, elements_->begin())
            );
        // parentElements and setElements must be finite at this point, therefore, sanity checks
        } else if (parentElements == nullptr) {
            nowide::cout << "[FATAL ERROR]: A condition where a parent has infinite elements despite passing the only infinite element case should never happen.\n";
            exitProgram();
        } else if (setElements == nullptr) {
            nowide::cout << "[FATAL ERROR]: A condition where a subset of parent has infinite elements where the parent has finite should never happen.\n";
            exitProgram();
        // parentElements and setElements REALLY must be finite at this point
        } else {
            // https://proofwiki.org/wiki/Definition:Relative_Complement
            // RelativeComplement(A, B) = A Difference B
            std::set_difference(
                parentElements->begin(), parentElements->end(),
                setElements->begin(), setElements->end(),
                std::inserter(*elements_, elements_->begin())
            );
        }
    }
}
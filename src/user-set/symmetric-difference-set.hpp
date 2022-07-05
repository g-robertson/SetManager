/*
    symmetric-difference-set.hpp
    SymmetricDifference set is a derivative set that holds the symmetric difference of two already existing subsets of its parent
*/ 
#pragma once

#include "derivative-set.hpp"

class SymmetricDifferenceSet : public DerivativeSet {
    public:
        SymmetricDifferenceSet(UserSet* parent, const std::string& name, UserSet* set1, UserSet* set2) noexcept;
        SymmetricDifferenceSet(UserSet* parent, const std::string& name) noexcept;
    
        // #region SubSet public members override 
        static UserSet* createSet(UserSet& parent, const std::string& name) noexcept;
        // #endregion 
        // #region UserSet public members override 
        static constexpr char type_ = 'S';
        char type() const noexcept override { return type_; }
        void updateElements() noexcept override;
        // #endregion 
};
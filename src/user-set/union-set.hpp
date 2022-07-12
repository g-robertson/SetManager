/*
    union-set.hpp
    Union set is a derivative set that holds the union of two already existing subsets of its parent
*/ 
#pragma once

#include "derivative-set.hpp"

class UnionSet : public DerivativeSet {
    public:
        UnionSet(UserSet* parent, const pstring& name, UserSet* set1, UserSet* set2) noexcept;
        UnionSet(UserSet* parent, const pstring& name) noexcept;
    
        // #region SubSet public members override 
        static UserSet* createSet(UserSet& parent, const pstring& name) noexcept;
        // #endregion 
        // #region UserSet public members override 
        static constexpr char type_ = 'U';
        char type() const noexcept override { return type_; }
        void updateElements() noexcept override;
        // #endregion 
};
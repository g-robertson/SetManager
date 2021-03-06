/*
    intersection-set.hpp
    Intersection set is a derivative set that holds the intersection of two already existing subsets of its parent
*/ 
#pragma once

#include "derivative-set.hpp"

class IntersectionSet : public DerivativeSet {
    public:
        IntersectionSet(UserSet* parent, const std::string& name, UserSet* set1, UserSet* set2) noexcept;
        IntersectionSet(UserSet* parent, const std::string& name) noexcept;
    
        // #region SubSet public members override 
        static UserSet* createSet(UserSet& parent, const std::string& name) noexcept;
        // #endregion 
        // #region UserSet public members override 
        static constexpr char type_ = 'I';
        char type() const noexcept override { return type_; }
        void updateElements() noexcept override;
        // #endregion 
};
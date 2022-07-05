/*
    difference-set.hpp
    Difference set is a derivative set that holds the difference of two already existing subsets of its parent
*/ 
#pragma once

#include "derivative-set.hpp"

class DifferenceSet : public DerivativeSet {
    public:
        DifferenceSet(UserSet* parent, const std::string& name, UserSet* set1, UserSet* set2) noexcept;
        DifferenceSet(UserSet* parent, const std::string& name) noexcept;
    
        // #region SubSet public members override 
        static UserSet* createSet(UserSet& parent, const std::string& name) noexcept;
        // #endregion 
        // #region UserSet public members override 
        static constexpr char type_ = '-';
        char type() const noexcept override { return type_; }
    
        const std::set<std::string>* elements() const noexcept override;
        const std::set<std::string>* complementElements() const noexcept override;
        // #endregion 

    private:
        mutable std::set<std::string> output;
};
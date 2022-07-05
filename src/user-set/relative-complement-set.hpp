/*
    relative-complement-set.hpp
    RelativeComplement set is a derivative set that holds the relative complement of an existing subset relative to the parent
*/ 
#pragma once

#include "derivative-set.hpp"

class RelativeComplementSet : public DerivativeSet {
    public:
        RelativeComplementSet(UserSet* parent, const std::string& name, UserSet* set) noexcept;
        RelativeComplementSet(UserSet* parent, const std::string& name) noexcept;
    
        // #region SubSet public members override 
        static UserSet* createSet(UserSet& parent, const std::string& name) noexcept;
        // #endregion 
        // #region UserSet public members override 
        static constexpr char type_ = 'C';
        char type() const noexcept override { return type_; }
        void updateElements() noexcept override;
        // #endregion 
};
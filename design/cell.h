#pragma once

#include "common.h"
#include "formula.h"
#include <sstream>

class Cell : public CellInterface {
public:
    Cell();
    Cell(const Cell& ex) {
        Set(ex.GetText());
    }
    ~Cell();

    void Set(std::string text) override;
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    
    std::vector<Position> GetReferencedCells() const override;

    void ClearCache();
    bool HasCache() const;

private:

    class Impl{
    public:
        virtual void Set(std::string text) =0;
        virtual void Clear() = 0;

        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        
        virtual std::vector<Position> GetReferencedCells() const;
    };
    
    class TextImpl: public Impl{
    public:
        void Set(std::string text) override{
            text_ = text;
        }
        void Clear() override {
            text_.clear();
        }
        
        Value GetValue() const override{
            if(text_[0] == '\'')
               return text_.substr(1);
            return text_;
        }
        
        std::string GetText() const override{
            return text_;
        }
    private:
        std::string text_;
    };
    
    class FormulaImpl: public Impl{
    public:
        void Set(std::string text) override {
            formula_ = ParseFormula(text);
        }

        void Clear() override {
            formula_.reset();
        }
        std::string GetText() const override {
            return "=" + formula_->GetExpression();
        }

        Value GetValue() const override {
            auto value = formula_->Evaluate(sheet_);
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            } else {
                return std::get<FormulaError>(value);
            }
        }
        
        std::vector<Position> GetReferencedCells() const override;
        
    private:
        std::unique_ptr<FormulaInterface> formula_;
        SheetInterface& sheet_;
    };
    class EmptyImpl: public Impl{
    public:
        std::string GetText() const override{
            return "";
        }
        
        void Clear() override {}
        void Set(std::string text) override {}
        
        Value GetValue() const override{
            return "";
        }
        
    };
    
    std::unique_ptr<Impl> impl_;
    SheetInterface& sheet_;
};

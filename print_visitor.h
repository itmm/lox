#pragma once

#include <ostream>

#include "binary.h"
#include "expr.h"
#include "grouping.h"
#include "literal.h"
#include "token.h"
#include "unary.h"

class Print_Visitor: public Expr_Visitor {
        std::ostream &out_;

        void write_expr(const std::unique_ptr<Expr> &e) {
            if (e) {
                e->accept(*this);
            } else { out_ << "nullptr"; }
        }

    public:
        explicit Print_Visitor(std::ostream &out, const Expr &e): out_ { out } { e.accept(*this); }

        void visit(const Binary &binary) {
            out_ << "(" << to_string(binary.token.type) << " ";
            write_expr(binary.left);
            out_ << " ";
            write_expr(binary.right);
            out_ << ")";
        }

        void visit(const Grouping &grouping) {
            out_ << "(group "; write_expr(grouping.expression); out_ << ")";
        }

        void visit(const Literal &literal) {
            out_ << static_cast<std::string>(*literal.value);
        }

        void visit(const Unary &unary) {
            out_ << "(" << to_string(unary.token.type) << " ";
            write_expr(unary.right);
            out_ << ")";
        }
};
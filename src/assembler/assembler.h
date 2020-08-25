#pragma once

#include "isainfo.h"

#include <set>
#include <variant>

namespace Ripes {

class Assembler {
public:
    using Symbol = QString;
    using Symbols = std::set<Symbol>;
    using SymbolMap = std::map<Symbol, unsigned>;
    using LineTokens = QStringList;
    struct SourceLine {
        LineTokens tokens;
        unsigned source_line = 0;
        QString symbol;
    };
    using SymbolLinePair = std::pair<Symbol, LineTokens>;
    using Program = std::vector<SourceLine>;
    using NoPassResult = std::monostate;

    // An error is defined as a reference to a source line index + an error string
    using Error = std::pair<unsigned, QString>;
    using Errors = std::vector<Error>;
    /**
     * @brief The Result struct
     * An assembly result is determined to be valid iff errors is empty.
     */
    struct Result {
        Errors errors;
        QByteArray program;
    };

    Result assemble(const QString& program) const;

private:
    /**
     * @brief pass0
     * Line tokenization and source line recording
     */
    std::variant<Errors, Program> pass0(const QStringList& program) const;

    /**
     * @brief pass1
     * Pseudo-op expansion. If @return errors is empty, pass succeeded.
     */
    std::variant<Errors, Program> pass1(const Program& tokenizedLines) const;
    /**
     * @brief pass2
     * Symbol recording. If @return errors is empty, pass succeeded.
     */
    std::variant<Errors, NoPassResult> pass2(Program& tokenizedLines, Symbols& symbols) const;
    /**
     * @brief pass3
     * Machine code translation. If @return errors is empty, pass succeeded.
     */
    std::variant<Errors, QByteArray> pass3(const Program& tokenizedLines, const SymbolMap& symbolMap) const;

protected:
    virtual std::variant<Error, std::optional<std::vector<LineTokens>>>
    expandPseudoOp(const SourceLine& line) const = 0;
    virtual std::variant<Error, LineTokens> tokenize(const QString& line) const;
    virtual std::variant<Error, QByteArray> assembleInstruction(const SourceLine& instruction,
                                                                const SymbolMap& symbols) const;
    /**
     * @brief splitSymbol
     * @returns a pair consisting of a symbol and the the input @p line tokens where the symbol has been removed.
     */
    virtual std::variant<Error, SymbolLinePair> splitSymbolFromLine(const SourceLine& line) const = 0;
};

}  // namespace Ripes

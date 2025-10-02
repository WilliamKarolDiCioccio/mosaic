#include "codex/parser.hpp"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <future>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <tree_sitter/api.h>

#include "codex/nodes.hpp"

#include "tree_sitter_cpp.hpp"

namespace codex
{

class SingleParser
{
   private:
    TSParser* m_parser;
    std::shared_ptr<Source> m_source;
    std::shared_ptr<CommentNode> m_leadingComment;
    std::shared_ptr<TemplateNode> m_templateDeclaration;

   public:
    SingleParser(const std::shared_ptr<Source>& _source);
    ~SingleParser();

   public:
    std::shared_ptr<SourceNode> parse();

   private:
    std::shared_ptr<Node> dispatch(TSNode _node);

    // Dependent nodes (requiring to be linked to other nodes and leaving outisde the main tree)
    std::shared_ptr<CommentNode> parseComment(const TSNode& _node);
    std::shared_ptr<TemplateNode> parseTemplate(const TSNode& _node);

    // Dependent nodes consumption helpers
    std::shared_ptr<CommentNode> getLeadingComment();
    std::shared_ptr<TemplateNode> getTemplateDeclaration();
    void clearLeadingComment() { m_leadingComment = nullptr; }
    void clearTemplateDeclaration() { m_templateDeclaration = nullptr; }

    // Independent nodes
    std::shared_ptr<IncludeNode> parseInclude(const TSNode& _node);
    std::shared_ptr<ObjectLikeMacroNode> parseObjectLikeMacro(const TSNode& _node);
    std::shared_ptr<FunctionLikeMacroNode> parseFunctionLikeMacro(const TSNode& _node);
    std::shared_ptr<NamespaceNode> parseNamespace(const TSNode& _node);
    std::shared_ptr<NamespaceAliasNode> parseNamespaceAlias(const TSNode& _node);
    std::shared_ptr<UsingNamespaceNode> parseUsingNamespace(const TSNode& _node);
    std::shared_ptr<TypedefNode> parseTypedef(const TSNode& _node);
    std::shared_ptr<TypeAliasNode> parseTypeAlias(const TSNode& _node);
    std::shared_ptr<EnumNode> parseEnum(const TSNode& _node);
    std::shared_ptr<ConceptNode> parseConcept(const TSNode& _node);
    std::shared_ptr<VariableNode> parseVariable(const TSNode& _node);
    std::shared_ptr<FunctionNode> parseFunction(const TSNode& _node);
    std::shared_ptr<OperatorNode> parseOperator(const TSNode& _node);
    std::shared_ptr<UnionNode> parseUnion(const TSNode& _node);
    std::shared_ptr<Node> parseFriend(const TSNode& _node);
    std::shared_ptr<StructNode> parseStruct(const TSNode& _node);
    std::shared_ptr<ClassNode> parseClass(const TSNode& _node);

    // Custom AST helpers
    std::shared_ptr<Node> parseAmbiguousDeclaration(const TSNode& _node);
    std::shared_ptr<Node> parseAmbiguousDefinition(const TSNode& _node);

    void parseInitDeclarator(const TSNode& _node, std::shared_ptr<VariableNode>& _varNode);
    void parseFunctionDeclarator(const TSNode& _node, std::shared_ptr<FunctionNode>& _fn);
    void parseOperatorDeclarator(const TSNode& _node, std::shared_ptr<OperatorNode>& _op);
    void parseMemberList(const TSNode& _listNode, const std::string& _parentName,
                         std::vector<std::shared_ptr<Node>>& _memberVars,
                         std::vector<std::shared_ptr<Node>>& _staticMemberVars,
                         std::vector<std::shared_ptr<Node>>& _memberFuncs,
                         std::vector<std::shared_ptr<Node>>& _staticMemberFuncs,
                         std::vector<std::shared_ptr<Node>>& _ctors,
                         std::vector<std::shared_ptr<Node>>& _dtors,
                         std::vector<std::shared_ptr<Node>>& _ops,
                         std::vector<std::shared_ptr<Node>>& _friends,
                         std::vector<std::shared_ptr<Node>>& _nestedTypes);
    void parseClassMemberList(
        const TSNode& _listNode, const std::string& _parentName,
        std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _memberVars,
        std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _staticMemberVars,
        std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _memberFuncs,
        std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _staticMemberFuncs,
        std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _ctors,
        std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _dtors,
        std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _ops,
        std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _friends,
        std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _nestedTypes);

    TypeSignature parseTypeSignature(const TSNode& _node);
    std::vector<GenericParameter> parseGenericParametersList(const TSNode& _node);
    std::vector<TemplateArgument> parseTemplateArgumentsList(const TSNode& _node);
};

SingleParser::SingleParser(const std::shared_ptr<Source>& _source) : m_source(_source)
{
    m_parser = ts_parser_new();

    if (!m_parser)
    {
        throw std::runtime_error("Failed to create TSParser");
    }

    ts_parser_set_language(m_parser, tree_sitter_cpp());
}

SingleParser::~SingleParser() { ts_parser_delete(m_parser); }

std::shared_ptr<SourceNode> SingleParser::parse()
{
    TSTree* tree = ts_parser_parse_string(m_parser, nullptr, m_source->sourceCode.c_str(),
                                          static_cast<uint32_t>(m_source->sourceCode.size()));

    TSNode root = ts_tree_root_node(tree);
    TSPoint rootEnd = ts_node_end_point(root);
    auto srcNode = std::make_shared<SourceNode>(0, 0, static_cast<int>(rootEnd.row),
                                                static_cast<int>(rootEnd.column));

    srcNode->source = m_source;

    uint32_t childCount = ts_node_child_count(root);

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(root, i);
        auto childNode = dispatch(child);

        if (childNode) srcNode->children.emplace_back(childNode);
    }

    ts_tree_delete(tree);

    return srcNode;
}

std::shared_ptr<Node> SingleParser::dispatch(TSNode _node)
{
    std::string type = ts_node_type(_node);

    if (type == "comment")
    {
        m_leadingComment = parseComment(_node);
    }
    else if (type == "template_declaration")
    {
        m_templateDeclaration = parseTemplate(_node);

        uint32_t childCount = ts_node_child_count(_node);

        for (uint32_t i = 0; i < childCount; ++i)
        {
            auto child = dispatch(ts_node_child(_node, i));
            if (child) return child;
        }
    }
    else if (type == "namespace_definition")
    {
        auto ns = parseNamespace(_node);

        uint32_t childCount = ts_node_child_count(_node);

        for (uint32_t i = 0; i < childCount; ++i)
        {
            TSNode child = ts_node_child(_node, i);
            std::string childType = ts_node_type(child);

            if (childType == "declaration_list")
            {
                uint32_t subChildCount = ts_node_child_count(child);

                for (uint32_t j = 0; j < subChildCount; ++j)
                {
                    auto subChild = dispatch(ts_node_child(child, j));
                    if (subChild) ns->children.emplace_back(subChild);
                }
            }
        }

        return ns;
    }
    else if (type == "preproc_include")
    {
        return parseInclude(_node);
    }
    else if (type == "preproc_def")
    {
        return parseObjectLikeMacro(_node);
    }
    else if (type == "preproc_function_def")
    {
        return parseFunctionLikeMacro(_node);
    }
    else if (type == "namespace_alias_definition")
    {
        return parseNamespaceAlias(_node);
    }
    else if (type == "type_definition")
    {
        return parseTypedef(_node);
    }
    else if (type == "alias_declaration")
    {
        return parseTypeAlias(_node);
    }
    else if (type == "using_declaration")
    {
        return parseUsingNamespace(_node);
    }
    else if (type == "enum_specifier")
    {
        return parseEnum(_node);
    }
    // might be a function, an operator or a variable
    else if (type == "declaration" || type == "field_declaration")
    {
        return parseAmbiguousDeclaration(_node);
    }
    // might be a function or an operator
    else if (type == "function_definition")
    {
        return parseAmbiguousDefinition(_node);
    }
    else if (type == "concept_definition")
    {
        return parseConcept(_node);
    }
    else if (type == "union_specifier")
    {
        return parseUnion(_node);
    }
    else if (type == "struct_specifier")
    {
        return parseStruct(_node);
    }
    else if (type == "class_specifier")
    {
        return parseClass(_node);
    }

    return nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Dependent nodes (requiring to be linked to other nodes and leaving outisde the main tree)
/////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<CommentNode> SingleParser::parseComment(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);

    auto text = m_source->sourceCode.substr(ts_node_start_byte(_node),
                                            ts_node_end_byte(_node) - ts_node_start_byte(_node));

    return std::make_shared<CommentNode>(text, start.row, start.column, end.row, end.column);
}

std::shared_ptr<TemplateNode> SingleParser::parseTemplate(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto templateDeclNode =
        std::make_shared<TemplateNode>(start.row, start.column, end.row, end.column);

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "template_parameter_list")
        {
            const uint32_t paramCount = ts_node_child_count(child);

            for (uint32_t j = 0; j < paramCount; ++j)
            {
                TSNode param = ts_node_child(child, j);
                std::string paramType = ts_node_type(param);

                TemplateParameter tp;

                if (paramType == "type_parameter_declaration")
                {
                    // e.g. "typename T"
                    for (uint32_t k = 0; k < ts_node_child_count(param); ++k)
                    {
                        TSNode sub = ts_node_child(param, k);
                        std::string subType = ts_node_type(sub);

                        if (subType == "typename" || subType == "class")
                        {
                            tp.keyword = getNodeText(sub, m_source->sourceCode);
                        }
                        else if (subType == "type_identifier")
                        {
                            tp.name = getNodeText(sub, m_source->sourceCode);
                        }
                    }
                }
                else if (paramType == "variadic_type_parameter_declaration")
                {
                    tp.isVariadic = true;

                    for (uint32_t k = 0; k < ts_node_child_count(param); ++k)
                    {
                        TSNode sub = ts_node_child(param, k);
                        std::string subType = ts_node_type(sub);

                        if (subType == "class" || subType == "typename")
                        {
                            tp.keyword = getNodeText(sub, m_source->sourceCode);
                        }
                        else if (subType == "type_identifier")
                        {
                            tp.name = getNodeText(sub, m_source->sourceCode);
                        }
                    }
                }

                if (!tp.keyword.empty() || !tp.name.empty())
                {
                    templateDeclNode->parameters.emplace_back(std::move(tp));
                }
            }
        }
    }

    return templateDeclNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Dependent nodes consumption helpers
/////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<CommentNode> SingleParser::getLeadingComment()
{
    if (!m_leadingComment) return nullptr;

    auto copy = m_leadingComment;

    m_leadingComment = nullptr;

    return copy;
}

std::shared_ptr<TemplateNode> SingleParser::getTemplateDeclaration()
{
    if (!m_templateDeclaration) return nullptr;

    auto copy = m_templateDeclaration;

    m_templateDeclaration = nullptr;

    return copy;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Independent nodes
/////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<IncludeNode> SingleParser::parseInclude(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    uint32_t childCount = ts_node_child_count(_node);

    auto incNode = std::make_shared<IncludeNode>(start.row, start.column, end.row, end.column);

    clearTemplateDeclaration();
    incNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);
        std::string text = getNodeText(child, m_source->sourceCode);

        if (type == "system_lib_string")
        {
            incNode->path = text.substr(1, text.size() - 2); // remove < >
            incNode->isSystem = true;
        }
        else if (type == "string_literal")
        {
            incNode->path = text.substr(1, text.size() - 2); // remove " "
            incNode->isSystem = false;
        }
    }

    return incNode;
}

std::shared_ptr<ObjectLikeMacroNode> SingleParser::parseObjectLikeMacro(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto objMacroNode =
        std::make_shared<ObjectLikeMacroNode>(start.row, start.column, end.row, end.column);

    clearTemplateDeclaration();
    objMacroNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "identifier" && objMacroNode->name.empty())
        {
            objMacroNode->name = getNodeText(child, m_source->sourceCode);
        }
        else if (childType == "preproc_arg")
        {
            objMacroNode->body = getNodeText(child, m_source->sourceCode);
        }
    }

    return objMacroNode;
}

std::shared_ptr<FunctionLikeMacroNode> SingleParser::parseFunctionLikeMacro(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto fnMacroNode =
        std::make_shared<FunctionLikeMacroNode>(start.row, start.column, end.row, end.column);

    clearTemplateDeclaration();
    fnMacroNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "identifier" && fnMacroNode->name.empty())
        {
            fnMacroNode->name = getNodeText(child, m_source->sourceCode);
        }
        else if (childType == "preproc_params")
        {
            const uint32_t paramCount = ts_node_child_count(child);

            for (uint32_t j = 0; j < paramCount; ++j)
            {
                TSNode param = ts_node_child(child, j);
                std::string paramType = ts_node_type(param);

                MacroParameter mp;

                if (paramType == "identifier")
                {
                    mp.name = getNodeText(param, m_source->sourceCode);
                }
                else if (paramType == "...")
                {
                    mp.name = "...";
                    mp.isVariadic = true;
                }

                if (!mp.name.empty()) fnMacroNode->parameters.emplace_back(mp);
            }
        }
        else if (childType == "preproc_arg")
        {
            fnMacroNode->body = getNodeText(child, m_source->sourceCode);
        }
    }

    return fnMacroNode;
}

std::shared_ptr<NamespaceNode> SingleParser::parseNamespace(const TSNode& _node)
{
    auto collectNamespaceParts = [&](const TSNode& node, auto&& self,
                                     std::vector<std::string>& out) -> void
    {
        const uint32_t cnt = ts_node_child_count(node);

        for (uint32_t i = 0; i < cnt; ++i)
        {
            TSNode child = ts_node_child(node, i);
            std::string type = ts_node_type(child);

            if (type == "namespace_identifier")
            {
                out.emplace_back(getNodeText(child, m_source->sourceCode));
            }
            else if (type == "nested_namespace_specifier")
            {
                self(child, self, out);
            }
        }
    };

    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto nsNode = std::make_shared<NamespaceNode>(start.row, start.column, end.row, end.column);

    clearTemplateDeclaration();
    nsNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);

        if (type == "comment")
        {
            m_leadingComment = parseComment(_node);
        }
        else if (type == "namespace_identifier")
        {
            // e.g. "mynamespace"
            nsNode->name = getNodeText(child, m_source->sourceCode);
            nsNode->isAnonymous = false;
        }
        else if (type == "nested_namespace_specifier")
        {
            // e.g. "outer::inner"
            nsNode->isNested = true;
            nsNode->isAnonymous = false;

            std::vector<std::string> parts;
            collectNamespaceParts(child, collectNamespaceParts, parts);
            nsNode->name = fmt::format("{}", fmt::join(parts, "::"));
        }
    }

    return nsNode;
}

std::shared_ptr<NamespaceAliasNode> SingleParser::parseNamespaceAlias(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto nsAliasNode =
        std::make_shared<NamespaceAliasNode>(start.row, start.column, end.row, end.column);

    clearTemplateDeclaration();
    nsAliasNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "namespace_identifier")
        {
            if (nsAliasNode->aliasName.empty())
            {
                nsAliasNode->aliasName = getNodeText(child, m_source->sourceCode); // first is alias
            }
            else
            {
                nsAliasNode->targetNamespace =
                    getNodeText(child, m_source->sourceCode); // second is target
            }
        }
    }

    return nsAliasNode;
}

std::shared_ptr<UsingNamespaceNode> SingleParser::parseUsingNamespace(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto usingNsNode =
        std::make_shared<UsingNamespaceNode>(start.row, start.column, end.row, end.column);

    clearTemplateDeclaration();
    usingNsNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "namespace" || childType == "identifier")
        {
            usingNsNode->name = getNodeText(child, m_source->sourceCode);
        }
    }

    return usingNsNode;
}

std::shared_ptr<TypedefNode> SingleParser::parseTypedef(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    uint32_t childCount = ts_node_child_count(_node);

    auto typedefNode = std::make_shared<TypedefNode>(start.row, start.column, end.row, end.column);

    clearTemplateDeclaration();
    typedefNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);

        if (type == "primitive_type")
        {
            // e.g. "int"
            typedefNode->targetType = getNodeText(child, m_source->sourceCode);
        }
        else if (type == "struct_specifier" || type == "class_specifier" ||
                 type == "union_specifier")
        {
            // e.g. "struct MyStruct { ... }"
            typedefNode->targetType = getNodeText(child, m_source->sourceCode);
        }
        else if (type == "type_identifier")
        {
            // e.g. "MyType"
            typedefNode->aliasName = getNodeText(child, m_source->sourceCode);
        }
    }

    return typedefNode;
}

std::shared_ptr<TypeAliasNode> SingleParser::parseTypeAlias(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto typeAliasNode =
        std::make_shared<TypeAliasNode>(start.row, start.column, end.row, end.column);

    typeAliasNode->templateDecl = getTemplateDeclaration();
    typeAliasNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "type_identifier")
        {
            typeAliasNode->aliasName = getNodeText(child, m_source->sourceCode);
        }
        else if (childType == "type_descriptor" || childType == "primitive_type" ||
                 childType == "identifier")
        {
            typeAliasNode->targetType = getNodeText(child, m_source->sourceCode);
        }
    }

    return typeAliasNode;
}

std::shared_ptr<EnumNode> SingleParser::parseEnum(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto enumNode = std::make_shared<EnumNode>(start.row, start.column, end.row, end.column);

    clearTemplateDeclaration();
    enumNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);

        if (type == "type_identifier")
        {
            enumNode->name = getNodeText(child, m_source->sourceCode);
        }
        else if (type == "class")
        {
            enumNode->isScoped = true;
        }
        else if (type == "primitive_type")
        {
            enumNode->underlyingType = getNodeText(child, m_source->sourceCode);
        }
        else if (type == "enumerator_list")
        {
            const uint32_t enumCount = ts_node_child_count(child);

            for (uint32_t j = 0; j < enumCount; ++j)
            {
                TSNode eChild = ts_node_child(child, j);
                std::string eType = ts_node_type(eChild);

                if (eType == "comment")
                {
                    m_leadingComment = parseComment(_node);
                }
                if (eType == "enumerator")
                {
                    std::string enumName;
                    std::string enumValue;

                    auto [s, e] = getPositionData(eChild);
                    const uint32_t parts = ts_node_child_count(eChild);

                    for (uint32_t k = 0; k < parts; ++k)
                    {
                        TSNode part = ts_node_child(eChild, k);
                        std::string pType = ts_node_type(part);

                        if (pType == "identifier")
                        {
                            // e.g. "VALUE1"
                            enumName = getNodeText(part, m_source->sourceCode);
                        }
                        else if (pType == "number_literal" || pType == "string_literal")
                        {
                            // e.g. "42" or "\"value\""
                            enumValue = getNodeText(part, m_source->sourceCode);
                        }
                    }

                    auto enumSpecNode =
                        std::make_shared<EnumSpecifierNode>(s.row, s.column, e.row, e.column);

                    enumSpecNode->name = enumName;
                    enumSpecNode->value = enumValue;

                    enumNode->enumerators.emplace_back(enumSpecNode);
                }
            }
        }
    }

    return enumNode;
}

std::shared_ptr<ConceptNode> SingleParser::parseConcept(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto conceptNode = std::make_shared<ConceptNode>(start.row, start.column, end.row, end.column);

    conceptNode->comment = getLeadingComment();
    conceptNode->templateDecl = getTemplateDeclaration();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "identifier")
        {
            // e.g. "Integral"
            conceptNode->name = getNodeText(child, m_source->sourceCode);
        }
        else if (childType == "qualified_identifier" || childType == "identifier" ||
                 childType == "template_function")
        {
            // Entire constraint expression just grab text
            conceptNode->constraint = getNodeText(child, m_source->sourceCode);
        }
    }

    return conceptNode;
}

std::shared_ptr<VariableNode> SingleParser::parseVariable(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto varNode = std::make_shared<VariableNode>(start.row, start.column, end.row, end.column);

    clearTemplateDeclaration();
    varNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "identifier" || childType == "field_identifier")
        {
            varNode->name = getNodeText(child, m_source->sourceCode);
        }
        else if (childType == "type_qualifier")
        {
            std::string q = getNodeText(child, m_source->sourceCode);

            if (q == "const")
                varNode->isConst = true;
            else if (q == "constexpr")
                varNode->isConstexpr = true;
            else if (q == "constinit")
                varNode->isConstinit = true;
            else if (q == "mutable")
                varNode->isMutable = true;
            else if (q == "volatile")
                varNode->isVolatile = true;
            else if (q == "static")
                varNode->isStatic = true;
            else if (q == "thread_local")
                varNode->isThreadLocal = true;
            else if (q == "inline")
                varNode->isInline = true;
            else if (q == "extern")
                varNode->isExtern = true;
        }
        else if (childType == "primitive_type" || childType == "type_identifier")
        {
            varNode->type = getNodeText(child, m_source->sourceCode);
        }
        else if (childType == "init_declarator")
        {
            parseInitDeclarator(child, varNode);
        }
    }

    return varNode;
}

void SingleParser::parseInitDeclarator(const TSNode& _node, std::shared_ptr<VariableNode>& _varNode)
{
    const uint32_t childCount = ts_node_child_count(_node);

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);

        if (type == "identifier" || type == "field_identifier")
        {
            _varNode->name = getNodeText(child, m_source->sourceCode);
        }
        else if (type == "number_literal" || type == "string_literal")
        {
            _varNode->initialValue = getNodeText(child, m_source->sourceCode);
        }
        else if (type == "reference_declarator" || type == "pointer_declarator")
        {
            const uint32_t subChildCount = ts_node_child_count(child);

            for (uint32_t j = 0; j < subChildCount; ++j)
            {
                TSNode subChild = ts_node_child(child, j);
                std::string subType = ts_node_type(subChild);

                if (subType == "identifier")
                {
                    _varNode->name = getNodeText(subChild, m_source->sourceCode);
                }
                else if (subType == "number_literal" || subType == "string_literal")
                {
                    _varNode->initialValue = getNodeText(subChild, m_source->sourceCode);
                }
            }
        }
    }
}

std::shared_ptr<FunctionNode> SingleParser::parseFunction(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto fnNode = std::make_shared<FunctionNode>(start.row, start.column, end.row, end.column);

    fnNode->comment = getLeadingComment();
    fnNode->templateDecl = getTemplateDeclaration();
    fnNode->returnSignature = parseTypeSignature(_node);

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);

        if (type == "attribute_declaration")
        {
            fnNode->attributes.emplace_back(getNodeText(child, m_source->sourceCode));
        }
        else if (type == "storage_class_specifier")
        {
            std::string txt = getNodeText(child, m_source->sourceCode);
            if (txt == "static") fnNode->isStatic = true;
            if (txt == "inline") fnNode->isInline = true;
        }
        else if (type == "type_qualifier")
        {
            std::string txt = getNodeText(child, m_source->sourceCode);
            if (txt == "constexpr") fnNode->isConstexpr = true;
            if (txt == "consteval") fnNode->isConsteval = true;
            if (txt == "const") fnNode->isConst = true;
            if (txt == "volatile") fnNode->isVolatile = true;
        }
        else
        {
            if (type == "reference_declarator" || type == "pointer_declarator")
            {
                const uint32_t subChildCount = ts_node_child_count(child);

                for (uint32_t j = 0; j < subChildCount; ++j)
                {
                    TSNode subChild = ts_node_child(child, j);
                    std::string subType = ts_node_type(subChild);

                    if (subType != "function_declarator") continue;

                    parseFunctionDeclarator(subChild, fnNode);
                }
            }
            else if (type == "function_declarator")
            {
                parseFunctionDeclarator(child, fnNode);
            }
        }
    }

    return fnNode;
}

void SingleParser::parseFunctionDeclarator(const TSNode& _node, std::shared_ptr<FunctionNode>& _fn)
{
    const uint32_t childCount = ts_node_child_count(_node);

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);

        if (type == "identifier" || type == "field_identifier" || type == "destructor_name")
        {
            _fn->name = getNodeText(child, m_source->sourceCode);
        }
        else if (type == "template_function" || type == "template_method")
        {
            if (ts_node_child_count(child) > 1)
            {
                TSNode argList = ts_node_child(child, 1);

                if (std::string(ts_node_type(argList)) == "template_argument_list")
                {
                    _fn->templateArgs = parseTemplateArgumentsList(argList);
                }
            }
        }
        else if (type == "parameter_list")
        {
            _fn->parameters = parseGenericParametersList(child);
        }
        else if (type == "noexcept")
        {
            _fn->isNoexcept = true;
        }
        else if (type == "override")
        {
            _fn->isOverride = true;
        }
        else if (type == "final")
        {
            _fn->isFinal = true;
        }
        else if (type == "const")
        {
            _fn->isConst = true;
        }
        else if (type == "virtual")
        {
            _fn->isVirtual = true;
        }
    }
}

std::shared_ptr<OperatorNode> SingleParser::parseOperator(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto opNode = std::make_shared<OperatorNode>(start.row, start.column, end.row, end.column);

    opNode->comment = getLeadingComment();
    opNode->templateDecl = getTemplateDeclaration();
    opNode->returnSignature = parseTypeSignature(_node);

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);

        if (type == "function_declarator")
        {
            parseOperatorDeclarator(child, opNode);
        }
        else if (type == "storage_class_specifier")
        {
            std::string txt = getNodeText(child, m_source->sourceCode);
            if (txt == "static") opNode->isStatic = true;
            if (txt == "inline") opNode->isInline = true;
        }
        else if (type == "type_qualifier")
        {
            std::string text = getNodeText(child, m_source->sourceCode);
            if (text == "const") opNode->isConst = true;
            if (text == "constexpr") opNode->isConstexpr = true;
            if (text == "explicit") opNode->isExplicit = true;
        }
    }

    return opNode;
}

void SingleParser::parseOperatorDeclarator(const TSNode& _node, std::shared_ptr<OperatorNode>& _op)
{
    const uint32_t childCount = ts_node_child_count(_node);

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);

        if (type == "operator_name")
        {
            _op->operatorSymbol =
                getNodeText(child, m_source->sourceCode).substr(8); // remove "operator"
        }
        else if (type == "parameter_list")
        {
            _op->parameters = parseGenericParametersList(child);
        }
        else if (type == "template_function" || type == "template_method")
        {
            if (ts_node_child_count(child) > 1)
            {
                TSNode argList = ts_node_child(child, 1);

                if (std::string(ts_node_type(argList)) == "template_argument_list")
                {
                    _op->templateArgs = parseTemplateArgumentsList(argList);
                }
            }
        }
        else if (type == "virtual")
        {
            _op->isVirtual = true;
        }
        else if (type == "override")
        {
            _op->isOverride = true;
        }
        else if (type == "final")
        {
            _op->isFinal = true;
        }
        else if (type == "pure_virtual_specifier")
        {
            _op->isPureVirtual = true;
        }
        else if (type == "noexcept")
        {
            _op->isNoexcept = true;
        }
    }
}

auto toConstructorNode = [](const std::shared_ptr<FunctionNode>& func)
{
    auto ctor = std::make_shared<ConstructorNode>(func->startLine, func->startColumn, func->endLine,
                                                  func->endColumn);

    ctor->name = func->name;

    ctor->isExplicit = func->isExplicit;
    ctor->isNoexcept = func->isNoexcept;
    ctor->isConstexpr = func->isConstexpr;
    ctor->isInline = func->isInline;

    ctor->isDefaulted = false;
    ctor->isDeleted = false;

    ctor->parameters = func->parameters;
    ctor->templateDecl = func->templateDecl;
    ctor->templateArgs = func->templateArgs;

    ctor->comment = func->comment;
    return ctor;
};

auto toDestructorNode = [](const std::shared_ptr<FunctionNode>& func)
{
    auto dtor = std::make_shared<DestructorNode>(func->startLine, func->startColumn, func->endLine,
                                                 func->endColumn);

    dtor->name = func->name;

    dtor->isVirtual = func->isVirtual;
    dtor->isPureVirtual = func->isPureVirtual;
    dtor->isNoexcept = func->isNoexcept;
    dtor->isConstexpr = func->isConstexpr;
    dtor->isInline = func->isInline;

    dtor->isDefaulted = false;
    dtor->isDeleted = false;

    dtor->comment = func->comment;
    return dtor;
};

std::shared_ptr<UnionNode> SingleParser::parseUnion(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto unionNode = std::make_shared<UnionNode>(start.row, start.column, end.row, end.column);

    unionNode->comment = getLeadingComment();
    unionNode->templateDecl = getTemplateDeclaration();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "type_identifier")
        {
            unionNode->name = getNodeText(child, m_source->sourceCode);
        }
        else if (childType == "template_type")
        {
            uint16_t subChildCount = ts_node_child_count(child);

            if (subChildCount == 0) continue;

            auto text = getNodeText(ts_node_child(child, 0), m_source->sourceCode);

            unionNode->name = text;
        }
    }

    unionNode->isAnonymous = unionNode->name.empty();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "field_declaration_list")
        {
            std::vector<std::shared_ptr<Node>> dummyFriends;

            parseMemberList(child, unionNode->name, unionNode->memberVariables,
                            unionNode->staticMemberVariables, unionNode->memberFunctions,
                            unionNode->staticMemberFunctions, unionNode->constructors,
                            unionNode->destructors, unionNode->operators, dummyFriends,
                            unionNode->nestedTypes);
        }
    }

    return unionNode;
}

std::shared_ptr<Node> SingleParser::parseFriend(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto friendNode = std::make_shared<FriendNode>(start.row, start.column, end.row, end.column);

    clearTemplateDeclaration();
    friendNode->comment = getLeadingComment();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "struct" || childType == "class")
        {
            friendNode->kind = getNodeText(child, m_source->sourceCode);
        }
        else if (childType == "type_identifier" || childType == "qualified_identifier")
        {
            friendNode->name = getNodeText(child, m_source->sourceCode);
        }
    }

    return friendNode;
}

std::shared_ptr<StructNode> SingleParser::parseStruct(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    const uint32_t childCount = ts_node_child_count(_node);

    auto structNode = std::make_shared<StructNode>(start.row, start.column, end.row, end.column);

    structNode->comment = getLeadingComment();
    structNode->templateDecl = getTemplateDeclaration();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "type_identifier")
        {
            structNode->name = getNodeText(child, m_source->sourceCode);
        }
        else if (childType == "template_type")
        {
            uint16_t subChildCount = ts_node_child_count(child);

            if (subChildCount == 0) continue;

            auto text = getNodeText(ts_node_child(child, 0), m_source->sourceCode);

            structNode->name = text;
        }
    }

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "base_class_clause")
        {
            const uint32_t baseCount = ts_node_child_count(child);

            for (uint32_t j = 0; j < baseCount; ++j)
            {
                TSNode baseNode = ts_node_child(child, j);
                std::string baseType = ts_node_type(baseNode);

                if (baseType == "qualified_identifier" || baseType == "type_identifier")
                {
                    structNode->baseClasses.emplace_back(
                        getNodeText(baseNode, m_source->sourceCode));
                }
            }
        }
        else if (childType == "virtual_specifier")
        {
            structNode->isFinal = getNodeText(child, m_source->sourceCode) == "final";
        }
        else if (childType == "field_declaration_list")
        {
            parseMemberList(child, structNode->name, structNode->memberVariables,
                            structNode->staticMemberVariables, structNode->memberFunctions,
                            structNode->staticMemberFunctions, structNode->constructors,
                            structNode->destructors, structNode->operators, structNode->friends,
                            structNode->nestedTypes);
        }
    }

    return structNode;
}

std::shared_ptr<ClassNode> SingleParser::parseClass(const TSNode& _node)
{
    auto [start, end] = getPositionData(_node);
    auto classNode = std::make_shared<ClassNode>(start.row, start.column, end.row, end.column);

    const uint32_t childCount = ts_node_child_count(_node);

    classNode->comment = getLeadingComment();
    classNode->templateDecl = getTemplateDeclaration();

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "type_identifier")
        {
            classNode->name = getNodeText(child, m_source->sourceCode);
        }
        else if (childType == "template_type")
        {
            uint16_t subChildCount = ts_node_child_count(child);

            if (subChildCount == 0) continue;

            auto text = getNodeText(ts_node_child(child, 0), m_source->sourceCode);

            classNode->name = text;
        }
    }

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);

        if (type == "virtual_specifier")
        {
            std::string txt = getNodeText(child, m_source->sourceCode);
            if (txt == "final") classNode->isFinal = true;
        }
        else if (type == "base_class_clause")
        {
            // Collect base class names
            for (uint32_t j = 0; j < ts_node_child_count(child); ++j)
            {
                TSNode baseChild = ts_node_child(child, j);
                if (ts_node_type(baseChild) == std::string("qualified_identifier") ||
                    ts_node_type(baseChild) == std::string("type_identifier"))
                {
                    classNode->baseClasses.push_back(getNodeText(baseChild, m_source->sourceCode));
                }
            }
        }
        else if (type == "field_declaration_list")
        {
            parseClassMemberList(child, classNode->name, classNode->memberVariables,
                                 classNode->staticMemberVariables, classNode->memberFunctions,
                                 classNode->staticMemberFunctions, classNode->constructors,
                                 classNode->destructors, classNode->operators, classNode->friends,
                                 classNode->nestedTypes);
        }
    }

    return classNode;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Custom AST helpers
/////////////////////////////////////////////////////////////////////////////////////////////

auto determineKind = [](const TSNode& _node)
{
    uint32_t childCount = ts_node_child_count(_node);

    for (uint32_t j = 0; j < childCount; ++j)
    {
        TSNode child = ts_node_child(_node, j);
        std::string type = ts_node_type(child);

        if (type == "operator_name" || type == "operator_cast") return NodeKind::Operator;
    }

    return NodeKind::Function;
};

std::shared_ptr<Node> SingleParser::parseAmbiguousDeclaration(const TSNode& _node)
{
    NodeKind kind = NodeKind::Variable;

    uint32_t childCount = ts_node_child_count(_node);

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "function_declarator")
        {
            kind = determineKind(child);
        }
        else if (childType == "reference_declarator" || childType == "pointer_declarator" ||
                 childType == "array_declarator")
        {
            uint32_t subChildCount = ts_node_child_count(child);

            for (uint32_t j = 0; j < subChildCount; ++j)
            {
                TSNode subChild = ts_node_child(child, j);
                std::string subChildType = ts_node_type(subChild);

                if (subChildType == "function_declarator")
                {
                    kind = determineKind(subChild);
                    break;
                }
            }
        }
    }

    switch (kind)
    {
        case NodeKind::Function:
            return parseFunction(_node);
        case NodeKind::Operator:
            return parseOperator(_node);
        default:
            return parseVariable(_node);
    }
}

std::shared_ptr<Node> SingleParser::parseAmbiguousDefinition(const TSNode& _node)
{
    NodeKind kind = NodeKind::Function;

    uint32_t childCount = ts_node_child_count(_node);

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string childType = ts_node_type(child);

        if (childType == "function_declarator")
        {
            kind = determineKind(child);
        }
        else if (childType == "reference_declarator" || childType == "pointer_declarator" ||
                 childType == "array_declarator")
        {
            uint32_t subChildCount = ts_node_child_count(child);

            for (uint32_t j = 0; j < subChildCount; ++j)
            {
                TSNode subChild = ts_node_child(child, j);
                std::string subChildType = ts_node_type(subChild);

                if (subChildType == "function_declarator")
                {
                    kind = determineKind(subChild);
                    break;
                }
            }
        }
    }

    switch (kind)
    {
        case NodeKind::Operator:
            return parseOperator(_node);
        default:
            return parseFunction(_node);
    }
}

TypeSignature SingleParser::parseTypeSignature(const TSNode& _node)
{
    TypeSignature sig;

    const uint32_t childCount = ts_node_child_count(_node);
    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        std::string type = ts_node_type(child);

        if (type == "primitive_type" || type == "type_identifier")
        {
            sig.baseType += getNodeText(child, m_source->sourceCode);
        }
        else if (type == "qualified_identifier")
        {
            sig.baseType += getNodeText(child, m_source->sourceCode);
        }
        else if (type == "type_qualifier")
        {
            std::string txt = getNodeText(child, m_source->sourceCode);
            if (txt == "const") sig.isConst = true;
            if (txt == "volatile") sig.isVolatile = true;
            if (txt == "mutable") sig.isMutable = true;
        }
        else if (type == "pointer_declarator")
        {
            sig.isPointer = true;
        }
        else if (type == "reference_declarator")
        {
            std::string txt = getNodeText(child, m_source->sourceCode);

            if (txt.find("&&") != std::string::npos)
            {
                sig.isRValueRef = true;
            }
            else
            {
                sig.isLValueRef = true;
            }
        }
        else if (type == "template_type")
        {
            sig.baseType =
                getNodeText(ts_node_child(child, 0), m_source->sourceCode); // the identifier

            if (ts_node_child_count(child) > 1)
            {
                TSNode argList = ts_node_child(child, 1);

                if (std::string(ts_node_type(argList)) == "template_argument_list")
                {
                    sig.templateArgs = parseTemplateArgumentsList(argList);
                }
            }
        }
    }

    return sig;
}

std::vector<GenericParameter> SingleParser::parseGenericParametersList(const TSNode& _node)
{
    std::vector<GenericParameter> result;

    const uint32_t childCount = ts_node_child_count(_node);

    result.reserve(childCount);

    for (uint32_t i = 0; i < childCount; ++i)
    {
        TSNode child = ts_node_child(_node, i);
        if (ts_node_type(child) != std::string("parameter_declaration")) continue;

        GenericParameter gp;
        gp.typeSignature = parseTypeSignature(child);

        const uint32_t subCount = ts_node_child_count(child);

        for (uint32_t j = 0; j < subCount; ++j)
        {
            TSNode sub = ts_node_child(child, j);
            if (ts_node_type(sub) == std::string("default_value"))
            {
                gp.defaultValue = getNodeText(sub, m_source->sourceCode);
            }
        }

        result.emplace_back(std::move(gp));
    }

    return result;
}

std::vector<TemplateArgument> SingleParser::parseTemplateArgumentsList(const TSNode& node)
{
    std::vector<TemplateArgument> args;

    const uint32_t count = ts_node_child_count(node);

    args.reserve(count);

    for (uint32_t i = 0; i < count; ++i)
    {
        TSNode child = ts_node_child(node, i);
        std::string type = ts_node_type(child);

        TemplateArgument arg;

        if (type == "type_descriptor" || type == "primitive_type" || type == "type_identifier")
        {
            arg.typeSignature = parseTypeSignature(child);
        }
        else if (type == "number_literal" || type == "string_literal" || type == "true" ||
                 type == "false" || type == "identifier" || type == "qualified_identifier")
        {
            arg.value = getNodeText(child, m_source->sourceCode);
        }
        else
        {
            // fallback: raw text
            arg.value = getNodeText(child, m_source->sourceCode);
        }

        args.emplace_back(std::move(arg));
    }

    return args;
}

void SingleParser::parseMemberList(const TSNode& _listNode, const std::string& _parentName,
                                   std::vector<std::shared_ptr<Node>>& _memberVars,
                                   std::vector<std::shared_ptr<Node>>& _staticMemberVars,
                                   std::vector<std::shared_ptr<Node>>& _memberFuncs,
                                   std::vector<std::shared_ptr<Node>>& _staticMemberFuncs,
                                   std::vector<std::shared_ptr<Node>>& _ctors,
                                   std::vector<std::shared_ptr<Node>>& _dtors,
                                   std::vector<std::shared_ptr<Node>>& _ops,
                                   std::vector<std::shared_ptr<Node>>& _friends,
                                   std::vector<std::shared_ptr<Node>>& _nestedTypes)
{
    const uint32_t fieldCount = ts_node_child_count(_listNode);

    for (uint32_t j = 0; j < fieldCount; ++j)
    {
        TSNode subChild = ts_node_child(_listNode, j);
        std::string subType = ts_node_type(subChild);

        if (subType == "comment")
        {
            m_leadingComment = parseComment(subChild);
        }
        else if (subType == "template_declaration")
        {
            m_templateDeclaration = parseTemplate(subChild);
        }
        if (subType == "field_declaration")
        {
            auto declNode = parseAmbiguousDeclaration(subChild);

            if (declNode->kind == NodeKind::Variable)
            {
                auto varNode = std::dynamic_pointer_cast<VariableNode>(declNode);
                (varNode->isStatic ? _staticMemberVars : _memberVars).emplace_back(declNode);
            }
            else if (declNode->kind == NodeKind::Function)
            {
                auto funcNode = std::dynamic_pointer_cast<FunctionNode>(declNode);
                (funcNode->isStatic ? _staticMemberFuncs : _memberFuncs).emplace_back(declNode);
            }
        }
        else if (subType == "function_definition")
        {
            auto defNode = parseAmbiguousDefinition(subChild);

            if (defNode->kind == NodeKind::Function)
            {
                auto funcNode = std::dynamic_pointer_cast<FunctionNode>(defNode);

                if (funcNode->name == _parentName) // constructor
                {
                    _ctors.emplace_back(toConstructorNode(funcNode));
                }
                else if (!funcNode->name.empty() && funcNode->name[0] == '~') // destructor
                {
                    _dtors.emplace_back(toDestructorNode(funcNode));
                }
                else
                {
                    (funcNode->isStatic ? _staticMemberFuncs : _memberFuncs).emplace_back(funcNode);
                }
            }
            else
            {
                _ops.emplace_back(defNode);
            }
        }
        else if (subType == "friend_declaration")
        {
            _friends.emplace_back(parseFriend(subChild));
        }
        else if (subType == "struct_specifier" || subType == "class_specifier" ||
                 subType == "union_specifier" || subType == "enum_specifier")
        {
            auto nested = dispatch(subChild);
            _nestedTypes.emplace_back(nested);
        }
    }
}

void SingleParser::parseClassMemberList(
    const TSNode& _listNode, const std::string& _parentName,
    std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _memberVars,
    std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _staticMemberVars,
    std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _memberFuncs,
    std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _staticMemberFuncs,
    std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _ctors,
    std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _dtors,
    std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _ops,
    std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _friends,
    std::vector<std::pair<AccessSpecifier, std::shared_ptr<Node>>>& _nestedTypes)
{
    const uint32_t fieldCount = ts_node_child_count(_listNode);

    AccessSpecifier currentAccess = AccessSpecifier::Private;

    for (uint32_t j = 0; j < fieldCount; ++j)
    {
        TSNode subChild = ts_node_child(_listNode, j);
        std::string subType = ts_node_type(subChild);

        if (subType == "comment")
        {
            m_leadingComment = parseComment(subChild);
        }
        else if (subType == "template_declaration")
        {
            m_templateDeclaration = parseTemplate(subChild);
        }
        else if (subType == "access_specifier")
        {
            std::string txt = getNodeText(subChild, m_source->sourceCode);

            if (txt == "public")
                currentAccess = AccessSpecifier::Public;
            else if (txt == "protected")
                currentAccess = AccessSpecifier::Protected;
            else if (txt == "private")
                currentAccess = AccessSpecifier::Private;
        }
        else if (subType == "field_declaration")
        {
            auto declNode = parseAmbiguousDeclaration(subChild);

            if (declNode->kind == NodeKind::Variable)
            {
                auto varNode = std::dynamic_pointer_cast<VariableNode>(declNode);
                (varNode->isStatic ? _staticMemberVars : _memberVars)
                    .emplace_back(std::make_pair(currentAccess, declNode));
            }
            else if (declNode->kind == NodeKind::Function)
            {
                auto funcNode = std::dynamic_pointer_cast<FunctionNode>(declNode);
                (funcNode->isStatic ? _staticMemberFuncs : _memberFuncs)
                    .emplace_back(std::make_pair(currentAccess, declNode));
            }
        }
        else if (subType == "function_definition")
        {
            auto defNode = parseAmbiguousDefinition(subChild);

            if (defNode->kind == NodeKind::Function)
            {
                auto funcNode = std::dynamic_pointer_cast<FunctionNode>(defNode);

                if (funcNode->name == _parentName) // constructor
                {
                    _ctors.emplace_back(std::make_pair(currentAccess, toConstructorNode(funcNode)));
                }
                else if (!funcNode->name.empty() && funcNode->name[0] == '~') // destructor
                {
                    _dtors.emplace_back(std::make_pair(currentAccess, toDestructorNode(funcNode)));
                }
                else
                {
                    (funcNode->isStatic ? _staticMemberFuncs : _memberFuncs)
                        .emplace_back(std::make_pair(currentAccess, funcNode));
                }
            }
            else
            {
                _ops.emplace_back(std::make_pair(currentAccess, defNode));
            }
        }
        else if (subType == "friend_declaration")
        {
            _friends.emplace_back(std::make_pair(currentAccess, parseFriend(subChild)));
        }
        else if (subType == "struct_specifier" || subType == "class_specifier" ||
                 subType == "union_specifier" || subType == "enum_specifier")
        {
            auto nested = dispatch(subChild);
            _nestedTypes.emplace_back(std::make_pair(currentAccess, nested));
        }
    }
}

std::vector<std::shared_ptr<SourceNode>> Parser::parse(
    const std::vector<std::shared_ptr<Source>>& _sources)
{
    std::vector<std::future<std::shared_ptr<SourceNode>>> futures;
    std::vector<std::shared_ptr<SourceNode>> results;

    futures.reserve(_sources.size());
    results.reserve(_sources.size());

    for (const auto& source : _sources)
    {
        futures.emplace_back(std::async(std::launch::async,
                                        [this, source]() -> std::shared_ptr<SourceNode>
                                        { return SingleParser(source).parse(); }));
    }

    for (auto& future : futures)
    {
        try
        {
            auto result = future.get();
            if (result) results.emplace_back(result);
        }
        catch (const std::exception& e)
        {
            std::cout << "Error parsing file: " << e.what() << "\n";
        }
    }

    return results;
}

} // namespace codex

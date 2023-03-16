#pragma once

#include "InterpretedComponent.h"
#include "StringStream.h"
#include "Token.h"
#include "TypeDefs.h"

#ifdef __GNUC__
#include <cxxabi.h>
#endif
namespace VeNo::VUI {

struct VariableTable {
  VariableTable() = default;
  std::unordered_map<std::string, std::variant<std::string, double>> Data;
  auto &Get(const std::string &name) { return Data.at(name); }
  void Set(const std::string &name, const std::string &value) {
    Data[name] = value;
  }
  void Set(const std::string &name, double value) { Data[name] = value; }
};

#define CreateAstClass(T)                                                      \
  struct V_CAT(Ast, T) : public AstNodeInstance<V_CAT(Ast, T), AstNodeType::T>

#define CreateAstClassWithExtra(T, X)                                          \
  struct V_CAT(Ast, T)                                                         \
      : public AstNodeInstance<V_CAT(Ast, T), AstNodeType::T>,                 \
        public X

enum class AstNodeType : u32 {
  Unknown = 0,
  Property,
  String,
  Number,
  PercentNumber,
  Component,
  OperationNode,
  Loop,
  Include,
  Variable,
  MathExpression,
  Invalid = 0xdead,
};

struct AstNode {
  AstNodeType Type{AstNodeType::Unknown};
  template <class T> T *As() { return dynamic_cast<T *>(this); }
  virtual void Dump(int depth) = 0;
  virtual std::string GetTypeName() = 0;
  virtual void Run(VUI::InterpretedComponent &, VariableTable &){};

  template <class... Args> bool IsOneOf(Args... args) {
    for (const auto p : {args...}) {
      if (p == Type)
        return true;
    }
    return false;
  }
};

struct NumberExpression {
  virtual double Process(double parentSize) = 0;
};

static void WriteDepth(int depth = 0) {
  std::string indent(depth * 2, ' ');
  std::cout << indent;
}

template <class T, AstNodeType type> struct AstNodeInstance : public AstNode {
  template <typename... Args> static Ref<T> Create(Args &&...args) {
    auto ref = CreateRef<T>(std::forward<Args>(args)...);
    ref->Type = type;
    return ref;
  }

  [[nodiscard]] std::string GetTypeName() override {
    std::string name;
#ifdef __GNUC__
    int status;
    char *realname;
    realname = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    name = realname;
    free(realname);
#else
    name = typeid(T).name();
#endif
    return name.substr(name.find_last_of(':') + 1);
  }
  void Dump(int depth) override {
    WriteDepth(depth);
    std::cout << "Node<" << GetTypeName() << ">\n";
  }
};

CreateAstClass(Invalid){};
CreateAstClassWithExtra(MathExpression, NumberExpression) {
  std::vector<Ref<AstNode>> Children{};
  void Dump(int depth) override;
  double Process(double parentSize) override;
};
CreateAstClass(String) {
  std::string Value{};
  void Dump(int depth) override;
};
CreateAstClass(Variable) { std::string Key; };
CreateAstClassWithExtra(Number, NumberExpression) {
  explicit AstNumber(double _value) : Value(_value) {}
  double Value{0};
  void Dump(int depth) override;
  double Process(double parentSize) override;
};

// NOTE: PercentNumber will be converted to the right value based on parent
//       Height or Width (selected by Property (x,w,y,h)
CreateAstClassWithExtra(PercentNumber, NumberExpression) {
  explicit AstPercentNumber(double _value) : Value(_value) {}
  double Value{0};
  void Dump(int depth) override;
  double Process(double parentSize) override;
};
CreateAstClass(OperationNode) {
  enum class OperationType {
    Addition,
    Subtraction,
    Multiplication,
    Division,
    None,
  };
  explicit AstOperationNode(OperationType type) : Operation(type) {}
  OperationType Operation{OperationType::None};
  void Dump(int depth) override;
};

CreateAstClass(Loop) {
  Ref<AstNumber> Start, End, Increment;
  std::string Iterator;
  std::vector<Ref<AstNode>> Body{};
  void Dump(int depth) override;
  void Run(VUI::InterpretedComponent &, VariableTable &) override;
};

CreateAstClass(Include) {
  std::string IncludePath;
  void Dump(int depth) override;
};

CreateAstClass(Property) {
  std::string Name{};
  Ref<AstNode> Value{nullptr};
  void Dump(int depth) override;
  void Run(VUI::InterpretedComponent &, VariableTable &) override;
};

CreateAstClass(Component) {
  std::string Component{};
  std::vector<Ref<AstProperty>> Properties{};
  std::vector<Ref<AstNode>> Children{};
  void Dump(int depth) override;
  void Run(VUI::InterpretedComponent &, VariableTable &) override;
};

class AstUtils {
public:
  static Ref<AstNode> ParseToken(TokenStream &);

protected:
  static Ref<AstNode> CreateAstComponent(TokenStream &);
  static Ref<AstProperty> CreatePropertyNode(Token &token, TokenStream &);
  static Ref<AstMathExpression>
  CreateMathExpressionNode(Token &, TokenStream &,
                           const Ref<AstNode> &optionalFirst = nullptr);
  static Ref<AstNode> CreateKeywordNode(Token &, TokenStream &);
  static Ref<AstNode> CreateIncludeNode(Token &, TokenStream &);
  static Ref<AstNode> CreateLoopNode(Token &, TokenStream &);
  static Ref<AstNumber> CreateCleanNumberNode(Token &token);
  static Ref<AstNode> CreateNumberNode(Token &, TokenStream &);
  static Ref<AstNode> CreateMathOrNumberNode(Token &, TokenStream &);
};

} // namespace VeNo::VUI

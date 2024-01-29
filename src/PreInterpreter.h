#pragma once

#include "AstNode.h"
#include "InterpretedComponent.h"
#include "Token.h"

#include <vector>

namespace VUI {

class PreInterpreter {
public:
  void ConvertTokensToNodes(TokenStream &);
  Ref<VUI::InterpretedComponent> Run();

  void Dump();

protected:
  friend AstNode;
  std::vector<Ref<AstNode>> m_AstNodes;
  VariableTable m_VariableTable{};
};
} // namespace VUI
#pragma once

#include "AstNode.h"
#include "InterpretedComponent.h"
#include "Token.h"

#include <vector>

namespace VeNo::VUI {

class PreInterpreter {
public:
  void ConvertTokensToNodes(TokenStream &);
  Ref<VUI::InterpretedComponent> Run();

protected:
  friend AstNode;
  std::vector<Ref<AstNode>> m_AstNodes;
  VariableTable m_VariableTable{};
};
} // namespace VeNo::VUI
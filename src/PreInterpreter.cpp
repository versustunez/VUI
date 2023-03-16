#include "PreInterpreter.h"

namespace VeNo::VUI {

void PreInterpreter::ConvertTokensToNodes(TokenStream &tokenStream) {
  while (tokenStream.IsNotEnd())
    m_AstNodes.push_back(AstUtils::ParseToken(tokenStream));
}

Ref<VUI::InterpretedComponent> PreInterpreter::Run() {
  if (m_AstNodes.empty())
    return nullptr;

  auto base = CreateRef<VUI::InterpretedComponent>();
  base->ComponentName = "Container";
  // set if found properties in home! ;) but we need some default to not crash
  base->ComponentPosition = {0, 0, 1200, 700};
  for (auto &astNode : m_AstNodes) {
    astNode->Run(*base, m_VariableTable);
  }
  return base;
}

} // namespace VeNo::VUI
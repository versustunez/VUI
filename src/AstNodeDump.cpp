#include "AstNode.h"

namespace VeNo::VUI {

static void DumpComponentType(AstNode *node, int depth) {
  WriteDepth(depth);
  std::cout << "Node<" << node->GetTypeName() << "> ";
}

void AstMathExpression::Dump(int depth) {
  AstNodeInstance::Dump(depth);
  for (auto &child : Children) {
    child->Dump(depth + 1);
  }
}

void AstString::Dump(int depth) {
  DumpComponentType(this, depth);
  std::cout << "\"" << Value << "\"\n";
}

void AstNumber::Dump(int depth) {
  DumpComponentType(this, depth);
  std::cout << Value << "\n";
}

void AstPercentNumber::Dump(int depth) {
  DumpComponentType(this, depth);
  std::cout << Value << " (" << Value * 100 << "%)\n";
}

void AstOperationNode::Dump(int depth) {
  DumpComponentType(this, depth);
  switch (Operation) {
  case OperationType::Addition: std::cout << "+\n"; return;
  case OperationType::Subtraction: std::cout << "-\n"; return;
  case OperationType::Multiplication: std::cout << "*\n"; return;
  case OperationType::Division: std::cout << "/\n"; return;
  case OperationType::None: std::cout << "None\n"; return;
  }
}
void AstLoop::Dump(int depth) {
  AstNodeInstance::Dump(depth);
  WriteDepth(depth + 1);
  std::cout << "Iterator: " << Iterator << "\n";
  WriteDepth(depth + 1);
  std::cout << "Start: " << Start->Value << "\n";
  WriteDepth(depth + 1);
  std::cout << "End: " << End->Value << "\n";
  WriteDepth(depth + 1);
  std::cout << "Increment: " << Increment->Value << "\n";
  for (auto &child : Body) {
    child->Dump(depth + 1);
  }
}

void AstInclude::Dump(int depth) {
  DumpComponentType(this, depth);
  std::cout << IncludePath << "\n";
}

void AstProperty::Dump(int depth) {
  AstNodeInstance::Dump(depth);
  WriteDepth(depth + 1);
  std::cout << "Property: " << Name << "\n";
  if (Value) {
    Value->Dump(depth + 1);
  }
}

void AstComponent::Dump(int depth) {
  DumpComponentType(this, depth);
  std::cout << "@" << Component << "\n";
  if (!Properties.empty()) {
    for (auto &p : Properties) {
      p->Dump(depth + 1);
    }
  }
  if (!Children.empty()) {
    for (auto &c : Children) {
      c->Dump(depth + 1);
    }
  }
}
} // namespace VeNo::VUI
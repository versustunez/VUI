#include "AstNode.h"
namespace VUI {

double AstMathExpression::Process(double parentSize) {
  if (Children.empty())
    return 0.0;

  double result = Children[0]->As<NumberExpression>()->Process(parentSize);
  for (size_t i = 1; i < Children.size(); i += 2) {
    auto *op = Children[i]->As<AstOperationNode>();
    if (op) {
      auto *numberExpression = Children[i + 1]->As<NumberExpression>();
      if (numberExpression) {
        switch (op->Operation) {
        case AstOperationNode::OperationType::Addition:
          result += numberExpression->Process(parentSize);
          break;
        case AstOperationNode::OperationType::Subtraction:
          result -= numberExpression->Process(parentSize);
          break;
        case AstOperationNode::OperationType::Multiplication:
          result *= numberExpression->Process(parentSize);
          break;
        case AstOperationNode::OperationType::Division:
          result /= numberExpression->Process(parentSize);
          break;
        case AstOperationNode::OperationType::None: break;
        }
      }
    }
  }
  return result;
}

double AstNumber::Process(double parentSize) { return Value; }

// we need the base value for this ;)
double AstPercentNumber::Process(double parentSize) {
  return parentSize * Value;
}

void AstLoop::Run(InterpretedComponent &component, VariableTable &table) {
  int start = (int)Start->Value;
  int end = (int)End->Value;
  int inc = (int)Increment->Value;
  for (int i = start; i < end; i += inc) {
    // We have to set the variable that is specified to the value of the loop
    table.Set(Iterator, i);
    // No we have to run the Loop ;)
    for (auto &bodyItem : Body) {
      bodyItem->Run(component, table);
    }
  }
}

void AstProperty::Run(InterpretedComponent &component, VariableTable &table) {
  Value->Run(component, table);
  double percentCalculationValue = component.ComponentPosition.Width;
  if (Name == "height" || Name == "h" || Name == "y") {
    percentCalculationValue = component.ComponentPosition.Height;
  }
  if (Value->IsOneOf(AstNodeType::Number, AstNodeType::PercentNumber,
                     AstNodeType::MathExpression)) {
    double val =
        Value->As<NumberExpression>()->Process(percentCalculationValue);
    if (Name == "height" || Name == "h" || Name == "y") {
      if (Name == "height" || Name == "h") {
        component.ComponentPosition.Height = (int)val;
      } else {
        component.ComponentPosition.Y = (int)val;
      }
    } else if (Name == "width" || Name == "w" || Name == "x") {
      if (Name == "width" || Name == "w") {
        component.ComponentPosition.Width = (int)val;
      } else {
        component.ComponentPosition.X = (int)val;
      }
    } else {
      component.Properties.push_back({Name, val});
    }
  } else if (Value->Type == AstNodeType::String) {
    component.Properties.push_back({Name, Value->As<AstString>()->Value});
  } else {
    // Unhandled Property... StringLiterals are still missing so we have to
    // support them soon
  }
}

void AstComponent::Run(InterpretedComponent &component, VariableTable &table) {
  auto &newComponent = component.Children.emplace_back();
  newComponent.ComponentPosition = {0, 0, component.ComponentPosition.Width,
                                    component.ComponentPosition.Height};
  newComponent.ComponentName = Component;
  for (auto &property : Properties) {
    property->Run(newComponent, table);
  }
  for (auto &child : Children) {
    child->Run(newComponent, table);
  }
}
} // namespace VUI
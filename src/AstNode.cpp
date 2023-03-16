#include "AstNode.h"

#include <cassert>

namespace VeNo::VUI {

static double stringToDouble(const std::string &numberString) {
  char *end;
  auto result = strtod(numberString.c_str(), &end);
  if (end == numberString.c_str() || *end != '\0')
    result = 0;
  return result;
}

static Ref<AstNode> CreateInvalidNode(TokenStream &tokenStream) {
  tokenStream.SetIsValid(false);
  return AstInvalid::Create();
}

Ref<AstNode> AstUtils::CreateNumberNode(Token &token,
                                        TokenStream &tokenStream) {
  std::string numberString = std::string(token.Value);
  double number = stringToDouble(numberString);

  if (tokenStream.PeekType() == TokenType::Percent) {
    tokenStream.Consume();
    return CreateRef<AstPercentNumber>(number / 100.0f);
  }
  return AstNumber::Create(number);
}

Ref<AstNumber> AstUtils::CreateCleanNumberNode(Token &token) {
  std::string numberString = std::string(token.Value);
  double number = stringToDouble(numberString);
  return AstNumber::Create(number);
}

Ref<AstNode> AstUtils::CreateLoopNode(Token &token, TokenStream &tokenStream) {
  auto loopNode = AstLoop::Create();
  if (tokenStream.PeekType() != TokenType::LeftParenthesis)
    return CreateInvalidNode(tokenStream);

  tokenStream.Consume();
  if (tokenStream.PeekType() != TokenType::Identifier)
    return CreateInvalidNode(tokenStream);

  loopNode->Iterator = std::string(tokenStream.Consume().Value);

  if (tokenStream.PeekType() != TokenType::Identifier &&
      tokenStream.Peek().Value != "in")
    return CreateInvalidNode(tokenStream);

  tokenStream.Consume();

  if (tokenStream.PeekType() != TokenType::Number)
    return CreateInvalidNode(tokenStream);

  auto numberToken = tokenStream.Consume();
  loopNode->Start = CreateCleanNumberNode(numberToken);
  tokenStream.ConsumeIfMatch(TokenType::Colon);
  numberToken = tokenStream.Consume();
  loopNode->End = CreateCleanNumberNode(numberToken);
  if (tokenStream.PeekType() == TokenType::Colon) {
    tokenStream.Consume();
    numberToken = tokenStream.Consume();
    loopNode->Increment = CreateCleanNumberNode(numberToken);
  } else {
    loopNode->Increment = AstNumber::Create(1);
  }

  if (tokenStream.PeekType() != TokenType::RightParenthesis)
    return CreateInvalidNode(tokenStream);

  tokenStream.Consume();
  if (tokenStream.PeekType() != TokenType::LeftCurlyBrace)
    return CreateInvalidNode(tokenStream);

  tokenStream.Consume();
  while (tokenStream.IsNotEnd() &&
         tokenStream.PeekType() != TokenType::RightCurlyBrace) {
    loopNode->Body.push_back(ParseToken(tokenStream));
  }
  if (tokenStream.IsNotEnd()) {
    tokenStream.Consume();
  }

  return loopNode;
}

Ref<AstNode> AstUtils::CreateIncludeNode(Token &, TokenStream &tokenStream) {
  auto node = AstInclude::Create();
  if (tokenStream.PeekType() != TokenType::StringLiteral)
    return CreateInvalidNode(tokenStream);

  node->IncludePath = std::string(tokenStream.Consume().Value);
  tokenStream.ConsumeIfMatch(TokenType::Semicolon);
  return node;
}

Ref<AstNode> AstUtils::CreateKeywordNode(Token &token,
                                         TokenStream &tokenStream) {
  auto keywordString = token.Value;
  if (keywordString == "loop") {
    return CreateLoopNode(token, tokenStream);
  } else if (keywordString == "include") {
    return CreateIncludeNode(token, tokenStream);
  }
  return CreateInvalidNode(tokenStream);
}

static bool IsOperatorType(TokenType type) {
  return type == TokenType::Plus || type == TokenType::Minus ||
         type == TokenType::Slash || type == TokenType::Multiply;
}

Ref<AstNode> AstUtils::CreateMathOrNumberNode(Token &token,
                                              TokenStream &tokenStream) {
  auto firstNumberNode = CreateNumberNode(token, tokenStream);
  if (IsOperatorType(tokenStream.PeekType())) {
    return CreateMathExpressionNode(token, tokenStream, firstNumberNode);
  }

  return firstNumberNode;
}

Ref<AstMathExpression>
AstUtils::CreateMathExpressionNode(Token &, TokenStream &tokenStream,
                                   const Ref<AstNode> &optionalFirst) {
  auto node = CreateRef<AstMathExpression>();
  if (optionalFirst)
    node->Children.push_back(optionalFirst);
  while (tokenStream.IsNotEnd() &&
         tokenStream.PeekType() != TokenType::Semicolon) {
    auto token = tokenStream.Consume();
    if (token.Type == TokenType::Number) {
      node->Children.push_back(CreateNumberNode(token, tokenStream));
    } else if (token.Type == TokenType::Plus) {
      node->Children.push_back(
          AstOperationNode::Create(AstOperationNode::OperationType::Addition));
    } else if (token.Type == TokenType::Minus) {
      node->Children.push_back(AstOperationNode::Create(
          AstOperationNode::OperationType::Subtraction));
    } else if (token.Type == TokenType::Slash) {
      node->Children.push_back(
          AstOperationNode::Create(AstOperationNode::OperationType::Division));
    } else if (token.Type == TokenType::Multiply) {
      node->Children.push_back(AstOperationNode::Create(
          AstOperationNode::OperationType::Multiplication));
    } else if (token.Type == TokenType::LeftParenthesis) {
      node->Children.push_back(CreateMathExpressionNode(token, tokenStream));
    } else if (token.Type == TokenType::RightParenthesis) {
      return node;
    }
  }
  return node;
}

Ref<AstProperty> AstUtils::CreatePropertyNode(Token &token,
                                              TokenStream &tokenStream) {
  auto node = AstProperty::Create();
  node->Name = token.Value;
  if (tokenStream.PeekType() != TokenType::Colon) {
    node->Value = CreateInvalidNode(tokenStream);
    return node;
  }

  tokenStream.Consume();
  token = tokenStream.Consume();

  if (token.Type == TokenType::Number) {
    node->Value = CreateMathOrNumberNode(token, tokenStream);
  } else if (token.Type == TokenType::StringLiteral
             || token.Type == TokenType::DollarSign
             || token.Type == TokenType::Identifier) {
    if (token.Type == TokenType::DollarSign) {
      token = tokenStream.Consume();
    }
    node->Value = AstString::Create();
    node->Value->As<AstString>()->Value = token.Value;
  } else {
    node->Value = CreateInvalidNode(tokenStream);
  }
  tokenStream.ConsumeIfMatch(TokenType::Semicolon);
  return node;
}

Ref<AstNode> AstUtils::CreateAstComponent(TokenStream &tokenStream) {
  auto node = AstComponent::Create();
  if (tokenStream.PeekType() != TokenType::Identifier)
    return CreateInvalidNode(tokenStream);

  auto token = tokenStream.Consume();
  node->Component = std::string(token.Value);
  if (tokenStream.PeekType() != TokenType::LeftCurlyBrace)
    return CreateInvalidNode(tokenStream);

  tokenStream.Consume();
  while (tokenStream.PeekType() != TokenType::RightCurlyBrace) {
    token = tokenStream.Consume();
    if (token.Type == TokenType::Component) {
      node->Children.push_back(CreateAstComponent(tokenStream));
    } else if (token.Type == TokenType::Identifier) {
      node->Properties.push_back(CreatePropertyNode(token, tokenStream));
    } else if (token.Type == TokenType::Keyword) {
      node->Children.push_back(CreateKeywordNode(token, tokenStream));
    } else {
      return CreateInvalidNode(tokenStream);
    }
  }
  if (tokenStream.IsNotEnd()) {
    tokenStream.Consume();
  }
  return node;
}

Ref<AstNode> AstUtils::ParseToken(TokenStream &tokenStream) {
  auto currentToken = tokenStream.Consume();
  switch (currentToken.Type) {
  case TokenType::Component: return CreateAstComponent(tokenStream);
  case TokenType::Keyword: return CreateKeywordNode(currentToken, tokenStream);
  case TokenType::Identifier:
  default: {
    return CreateInvalidNode(tokenStream);
  }
  }
}
} // namespace VeNo::VUI
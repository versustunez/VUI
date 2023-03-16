#include "src/Compiler.h"
#include "src/PreInterpreter.h"

int main() {
  using namespace VeNo::VUI;
  Compiler compiler{"example.vui"};
  compiler.Parse();
  /*  compiler.DumpTokens();*/
  PreInterpreter interpreter;
  interpreter.ConvertTokensToNodes(compiler.GetTokenStream());
  if (compiler.GetTokenStream().IsValid()) {
    interpreter.Run();
  } else {
    std::cout << "Error: compilation Failed" << std::endl;
  }

  return 0;
}

#include "src/Compiler.h"
#include "src/PreInterpreter.h"

int main(int argc, char *argv[]) {
  using namespace VUI;

  if (argc == 1) {
    std::cout << argv[0] << " Usage: {fileToTranspile} {?namespace}";
    return 1;
  }
  const auto fileName = argv[1];
  Compiler compiler{fileName};
  compiler.Parse();
  /*  compiler.DumpTokens();*/
  PreInterpreter interpreter;
  interpreter.ConvertTokensToNodes(compiler.GetTokenStream());
  interpreter.Dump();
  if (!compiler.GetTokenStream().IsValid()) {
    std::cerr << "This is not a valid source file." << std::endl;
    std::cerr << "Error: compilation Failed" << std::endl;
    return 1;
  }
  auto data = interpreter.Run();
  return 0;
}

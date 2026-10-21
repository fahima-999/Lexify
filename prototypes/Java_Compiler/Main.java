import java.io.IOException;
import java.nio.file.*;import java.util.*;
public class Main {
 public static void main(String[] args){try{
  if(args.length==0){System.err.println("Usage: java Main <source-file> [--tokens|--ast|--tac|--asm]");System.exit(1);}
  String mode=args.length>1?args[1]:"";
  if(!mode.isEmpty()&&!List.of("--tokens","--ast","--tac","--asm").contains(mode))throw new IllegalArgumentException("Unknown option: "+mode);
  String src;
  try{src=Files.readString(Path.of(args[0]));}catch(IOException e){throw new IllegalArgumentException("Cannot open input file: "+args[0]);}
  List<Token> tokens=new Lexer(src).scan();
  if(mode.equals("--tokens")){tokens.forEach(System.out::println);return;}
  Ast.Program program=new Parser(tokens).parse();new SemanticAnalyzer().check(program);
  switch(mode){
   case "--ast"->System.out.println(program);
   case "--tac"->new CodeGenerator().generate(program).forEach(System.out::println);
   case "--asm"->System.out.print(new AssemblyGenerator().generate(new CodeGenerator().generate(program)));
   default->new Interpreter().run(program);
  }
 }catch(Exception e){System.err.println("Lexify error: "+e.getMessage());System.exit(1);}}
}

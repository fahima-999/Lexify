import java.nio.file.*;import java.util.*;
public class Main {
 public static void main(String[] args){try{
 if(args.length==0){System.err.println("Usage: java Main <source-file> [--tokens|--ast|--tac]");System.exit(1);}
 String src=Files.readString(Path.of(args[0]));List<Token> tokens=new Lexer(src).scan();Ast.Program program=new Parser(tokens).parse();new SemanticAnalyzer().check(program);
 if(args.length>1){switch(args[1]){case "--tokens"->tokens.forEach(System.out::println);case "--ast"->System.out.println(program);case "--tac"->new CodeGenerator().generate(program).forEach(System.out::println);default->throw new IllegalArgumentException("Unknown option: "+args[1]);}}
 else new Interpreter().run(program);
 }catch(Exception e){System.err.println("Lexify error: "+e.getMessage());System.exit(1);}}
}

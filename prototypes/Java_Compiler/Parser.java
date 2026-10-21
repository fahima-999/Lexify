import java.util.*;
public class Parser {
 private final List<Token> tokens;private int at=0;
 public Parser(List<Token> tokens){this.tokens=tokens;}
 private Token peek(){return tokens.get(at);} private boolean check(TokenType t){return peek().type()==t;}
 private Token take(TokenType t){
  if(!check(t)){Token p=peek();throw new IllegalArgumentException("Expected "+t+" at line "+p.line()+" but found "+(p.type()==TokenType.EOF?"end of file":"'"+p.text()+"'"));}
  return tokens.get(at++);
 }
 private boolean match(TokenType t){if(check(t)){at++;return true;}return false;}

 // program -> [public] class Name { public static void main(String[] args) { statements } }
 public Ast.Program parse(){
  List<Ast.Stmt> stmts=new ArrayList<>();
  match(TokenType.PUBLIC);take(TokenType.CLASS);take(TokenType.IDENT);take(TokenType.LBRACE);
  take(TokenType.PUBLIC);take(TokenType.STATIC);take(TokenType.VOID);take(TokenType.MAIN);
  take(TokenType.LPAREN);take(TokenType.STRING);take(TokenType.LBRACKET);take(TokenType.RBRACKET);take(TokenType.IDENT);take(TokenType.RPAREN);
  take(TokenType.LBRACE);
  while(!check(TokenType.RBRACE)&&!check(TokenType.EOF))stmts.add(stmt());
  take(TokenType.RBRACE);take(TokenType.RBRACE);
  if(!check(TokenType.EOF))throw new IllegalArgumentException("Unexpected input after class at line "+peek().line());
  return new Ast.Program(stmts);
 }
 private Ast.Stmt stmt(){
  if(check(TokenType.INT)||check(TokenType.DOUBLE)){
   Ast.Type type=tokens.get(at++).type()==TokenType.INT?Ast.Type.INT:Ast.Type.DOUBLE;
   String n=take(TokenType.IDENT).text();Ast.Expr e=null;if(match(TokenType.ASSIGN))e=expr();take(TokenType.SEMI);return new Ast.Decl(type,n,e);
  }
  if(match(TokenType.SYSTEM)){
   take(TokenType.DOT);take(TokenType.OUT);take(TokenType.DOT);take(TokenType.PRINTLN);take(TokenType.LPAREN);
   Ast.Expr e=expr();take(TokenType.RPAREN);take(TokenType.SEMI);return new Ast.Print(e);
  }
  if(match(TokenType.RETURN)){take(TokenType.SEMI);return new Ast.Return();}
  String n=take(TokenType.IDENT).text();take(TokenType.ASSIGN);Ast.Expr e=expr();take(TokenType.SEMI);return new Ast.Assign(n,e);
 }
 private Ast.Expr expr(){Ast.Expr e=term();while(check(TokenType.PLUS)||check(TokenType.MINUS)){String op=tokens.get(at++).text();e=new Ast.Binary(op,e,term());}return e;}
 private Ast.Expr term(){Ast.Expr e=factor();while(check(TokenType.STAR)||check(TokenType.SLASH)){String op=tokens.get(at++).text();e=new Ast.Binary(op,e,factor());}return e;}
 private Ast.Expr factor(){
  if(match(TokenType.MINUS))return new Ast.Unary("-",factor());
  if(check(TokenType.NUMBER)){
   Token t=take(TokenType.NUMBER);
   if(t.text().contains("."))return new Ast.Real(Double.parseDouble(t.text()));
   try{return new Ast.Num(Integer.parseInt(t.text()));}
   catch(NumberFormatException ex){throw new IllegalArgumentException("Integer literal too large at line "+t.line()+": "+t.text());}
  }
  if(check(TokenType.IDENT))return new Ast.Var(take(TokenType.IDENT).text());
  if(match(TokenType.LPAREN)){Ast.Expr e=expr();take(TokenType.RPAREN);return e;}
  throw new IllegalArgumentException("Expected expression at line "+peek().line());
 }
}

import java.util.*;
public class Parser {
 private final List<Token> tokens;private int at=0;
 public Parser(List<Token> tokens){this.tokens=tokens;}
 private Token peek(){return tokens.get(at);} private boolean check(TokenType t){return peek().type()==t;}
 private Token take(TokenType t){if(!check(t))throw new IllegalArgumentException("Expected "+t+" but got "+peek());return tokens.get(at++);}
 private boolean match(TokenType t){if(check(t)){at++;return true;}return false;}
 public Ast.Program parse(){List<Ast.Stmt> stmts=new ArrayList<>();while(!check(TokenType.EOF))stmts.add(stmt());return new Ast.Program(stmts);}
 private Ast.Stmt stmt(){if(match(TokenType.INT)){String n=take(TokenType.IDENT).text();Ast.Expr e=null;if(match(TokenType.ASSIGN))e=expr();take(TokenType.SEMI);return new Ast.Decl(n,e);}
 if(match(TokenType.PRINT)){take(TokenType.LPAREN);Ast.Expr e=expr();take(TokenType.RPAREN);take(TokenType.SEMI);return new Ast.Print(e);}
 String n=take(TokenType.IDENT).text();take(TokenType.ASSIGN);Ast.Expr e=expr();take(TokenType.SEMI);return new Ast.Assign(n,e);}
 private Ast.Expr expr(){Ast.Expr e=term();while(check(TokenType.PLUS)||check(TokenType.MINUS)){String op=tokens.get(at++).text();e=new Ast.Binary(op,e,term());}return e;}
 private Ast.Expr term(){Ast.Expr e=factor();while(check(TokenType.STAR)||check(TokenType.SLASH)){String op=tokens.get(at++).text();e=new Ast.Binary(op,e,factor());}return e;}
 private Ast.Expr factor(){if(match(TokenType.MINUS))return new Ast.Unary("-",factor());if(check(TokenType.NUMBER))return new Ast.Num(Integer.parseInt(take(TokenType.NUMBER).text()));if(check(TokenType.IDENT))return new Ast.Var(take(TokenType.IDENT).text());if(match(TokenType.LPAREN)){Ast.Expr e=expr();take(TokenType.RPAREN);return e;}throw new IllegalArgumentException("Expected expression at "+peek());}
}

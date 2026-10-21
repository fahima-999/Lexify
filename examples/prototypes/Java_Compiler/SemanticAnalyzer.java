import java.util.*;
public class SemanticAnalyzer {
 private final Set<String> declared=new HashSet<>();
 public void check(Ast.Program p){for(Ast.Stmt s:p.statements()){
 if(s instanceof Ast.Decl d){if(declared.contains(d.name()))throw new IllegalArgumentException("Duplicate declaration: "+d.name());if(d.init()!=null)checkExpr(d.init());declared.add(d.name());}
 else if(s instanceof Ast.Assign a){require(a.name());checkExpr(a.value());}
 else if(s instanceof Ast.Print pr)checkExpr(pr.value());
 }}
 private void require(String n){if(!declared.contains(n))throw new IllegalArgumentException("Undeclared variable: "+n);}
 private void checkExpr(Ast.Expr e){if(e instanceof Ast.Var v)require(v.name());else if(e instanceof Ast.Binary b){checkExpr(b.left());checkExpr(b.right());}else if(e instanceof Ast.Unary u)checkExpr(u.value());}
}

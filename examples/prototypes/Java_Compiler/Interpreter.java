public class Interpreter {
 private final SymbolTable vars=new SymbolTable();
 public void run(Ast.Program p){for(Ast.Stmt s:p.statements()){
 if(s instanceof Ast.Decl d)vars.declare(d.name(),d.init()==null?0:eval(d.init()));
 else if(s instanceof Ast.Assign a)vars.set(a.name(),eval(a.value()));
 else if(s instanceof Ast.Print pr)System.out.println(eval(pr.value()));
 }}
 private int eval(Ast.Expr e){if(e instanceof Ast.Num n)return n.value();if(e instanceof Ast.Var v)return vars.get(v.name());if(e instanceof Ast.Unary u)return -eval(u.value());if(e instanceof Ast.Binary b){int l=eval(b.left()),r=eval(b.right());return switch(b.op()){case "+"->l+r;case "-"->l-r;case "*"->l*r;case "/"->{if(r==0)throw new ArithmeticException("Division by zero");yield l/r;}default->throw new IllegalStateException();};}throw new IllegalStateException();}
}

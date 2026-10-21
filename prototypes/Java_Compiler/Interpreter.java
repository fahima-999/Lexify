public class Interpreter {
 private final SymbolTable vars=new SymbolTable();
 public void run(Ast.Program p){for(Ast.Stmt s:p.statements()){
  if(s instanceof Ast.Decl d)vars.declare(d.name(),d.type(),d.init()==null?defaultValue(d.type()):eval(d.init()));
  else if(s instanceof Ast.Assign a)vars.set(a.name(),eval(a.value()));
  else if(s instanceof Ast.Print pr)System.out.println(eval(pr.value()));
  else if(s instanceof Ast.Return)return;
 }}
 private static Object defaultValue(Ast.Type t){return t==Ast.Type.INT?(Object)0:(Object)0.0;}
 private Object eval(Ast.Expr e){
  if(e instanceof Ast.Num n)return n.value();
  if(e instanceof Ast.Real r)return r.value();
  if(e instanceof Ast.Var v)return vars.get(v.name());
  if(e instanceof Ast.Unary u){Object x=eval(u.value());if(x instanceof Integer i)return -i;return -((Double)x);}
  if(e instanceof Ast.Binary b){
   Object l=eval(b.left()),r=eval(b.right());
   if(l instanceof Integer x&&r instanceof Integer y){
    return switch(b.op()){case "+"->x+y;case "-"->x-y;case "*"->x*y;case "/"->{if(y==0)throw new ArithmeticException("Division by zero");yield x/y;}default->throw new IllegalStateException();};
   }
   double x=((Number)l).doubleValue(),y=((Number)r).doubleValue();
   return switch(b.op()){case "+"->x+y;case "-"->x-y;case "*"->x*y;case "/"->{if(y==0)throw new ArithmeticException("Division by zero");yield x/y;}default->throw new IllegalStateException();};
  }
  throw new IllegalStateException();
 }
}

import java.util.*;
public class TacGenerator {
 private int counter=0;private final List<String> code=new ArrayList<>();
 public List<String> generate(Ast.Program p){code.clear();counter=0;for(Ast.Stmt s:p.statements()){
  if(s instanceof Ast.Decl d){
   String v=d.init()==null?"0":expr(d.init());
   code.add("declare "+d.type()+" "+d.name());
   if(d.init()!=null)code.add(d.name()+" = "+v);
  }
  else if(s instanceof Ast.Assign a)code.add(a.name()+" = "+expr(a.value()));
  else if(s instanceof Ast.Print pr)code.add("print "+expr(pr.value()));
  else if(s instanceof Ast.Return)code.add("return");
 }return List.copyOf(code);}
 private String expr(Ast.Expr e){
  if(e instanceof Ast.Num n)return Integer.toString(n.value());
  if(e instanceof Ast.Real r)return Double.toString(r.value());
  if(e instanceof Ast.Var v)return v.name();
  if(e instanceof Ast.Unary u){String x=expr(u.value()),t=temp();code.add(t+" = -"+x);return t;}
  if(e instanceof Ast.Binary b){String l=expr(b.left()),r=expr(b.right()),t=temp();code.add(t+" = "+l+" "+b.op()+" "+r);return t;}
  throw new IllegalStateException();
 }
 private String temp(){return "t"+(++counter);}
}

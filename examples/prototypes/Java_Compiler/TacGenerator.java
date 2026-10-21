import java.util.*;
public class TacGenerator {
 private int counter=0;private final List<String> code=new ArrayList<>();
 public List<String> generate(Ast.Program p){code.clear();counter=0;for(Ast.Stmt s:p.statements()){
 if(s instanceof Ast.Decl d)code.add(d.name()+" = "+(d.init()==null?"0":expr(d.init())));
 else if(s instanceof Ast.Assign a)code.add(a.name()+" = "+expr(a.value()));
 else if(s instanceof Ast.Print pr)code.add("print "+expr(pr.value()));
 }return List.copyOf(code);}
 private String expr(Ast.Expr e){if(e instanceof Ast.Num n)return Integer.toString(n.value());if(e instanceof Ast.Var v)return v.name();if(e instanceof Ast.Unary u){String x=expr(u.value()),t=temp();code.add(t+" = -"+x);return t;}if(e instanceof Ast.Binary b){String l=expr(b.left()),r=expr(b.right()),t=temp();code.add(t+" = "+l+" "+b.op()+" "+r);return t;}throw new IllegalStateException();}
 private String temp(){return "t"+(++counter);}
}

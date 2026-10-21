import java.util.*;
public class SemanticAnalyzer {
 private final Map<String,Ast.Type> declared=new HashMap<>();
 public void check(Ast.Program p){for(Ast.Stmt s:p.statements()){
  if(s instanceof Ast.Decl d){
   if(declared.containsKey(d.name()))throw new IllegalArgumentException("Duplicate declaration: "+d.name());
   if(d.init()!=null)assignable(d.type(),typeOf(d.init()),d.name());
   declared.put(d.name(),d.type());
  }
  else if(s instanceof Ast.Assign a){Ast.Type target=require(a.name());assignable(target,typeOf(a.value()),a.name());}
  else if(s instanceof Ast.Print pr)typeOf(pr.value());
 }}
 private Ast.Type require(String n){Ast.Type t=declared.get(n);if(t==null)throw new IllegalArgumentException("Undeclared variable: "+n);return t;}
 // Java rule: int widens to double, but double -> int would lose data
 private void assignable(Ast.Type target,Ast.Type value,String name){
  if(target==Ast.Type.INT&&value==Ast.Type.DOUBLE)throw new IllegalArgumentException("Cannot assign double to int variable '"+name+"' (possible lossy conversion)");
 }
 private Ast.Type typeOf(Ast.Expr e){
  if(e instanceof Ast.Num)return Ast.Type.INT;
  if(e instanceof Ast.Real)return Ast.Type.DOUBLE;
  if(e instanceof Ast.Var v)return require(v.name());
  if(e instanceof Ast.Unary u)return typeOf(u.value());
  if(e instanceof Ast.Binary b){Ast.Type l=typeOf(b.left()),r=typeOf(b.right());return(l==Ast.Type.DOUBLE||r==Ast.Type.DOUBLE)?Ast.Type.DOUBLE:Ast.Type.INT;}
  throw new IllegalStateException();
 }
}

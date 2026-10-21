import java.util.*;
public class Ast {
 public enum Type {
  INT("int"), DOUBLE("double");
  private final String label;
  Type(String label){this.label=label;}
  public String label(){return label;}
  @Override public String toString(){return label;}
 }
 public sealed interface Expr permits Num,Real,Var,Binary,Unary {}
 public record Num(int value) implements Expr {}
 public record Real(double value) implements Expr {}
 public record Var(String name) implements Expr {}
 public record Binary(String op,Expr left,Expr right) implements Expr {}
 public record Unary(String op,Expr value) implements Expr {}
 public sealed interface Stmt permits Decl,Assign,Print,Return {}
 public record Decl(Type type,String name,Expr init) implements Stmt {}
 public record Assign(String name,Expr value) implements Stmt {}
 public record Print(Expr value) implements Stmt {}
 public record Return() implements Stmt {}
 public record Program(List<Stmt> statements) {}
}

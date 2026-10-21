import java.util.*;
public class Ast {
 public sealed interface Expr permits Num,Var,Binary,Unary {}
 public record Num(int value) implements Expr {}
 public record Var(String name) implements Expr {}
 public record Binary(String op,Expr left,Expr right) implements Expr {}
 public record Unary(String op,Expr value) implements Expr {}
 public sealed interface Stmt permits Decl,Assign,Print {}
 public record Decl(String name,Expr init) implements Stmt {}
 public record Assign(String name,Expr value) implements Stmt {}
 public record Print(Expr value) implements Stmt {}
 public record Program(List<Stmt> statements) {}
}

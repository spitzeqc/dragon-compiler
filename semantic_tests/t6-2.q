(* ERROR: procedure passed wrong number/type of arguments *)

def main( input, output );

  var b: integral;
  var y: rational;

  proc boo( a: integral; x: rational );
  begin
  end;

begin
  boo(y,10);
  boo(b,y,b)
end.


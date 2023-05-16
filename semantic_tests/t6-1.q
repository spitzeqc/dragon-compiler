(* ERROR: procedures don't return values *)

def main( input, output );
  var a: integral;

  proc boo( b: integral );
  begin
    a := 1
  end;

begin
  a := boo(a)
end.


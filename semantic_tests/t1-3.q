(* LEGAL: non-local names visible from inner scopes *)

def main( input, output );
  var a: integral;

  proc boo( x: integral );
  begin
    a := x
  end;

begin
  boo( a )
end.


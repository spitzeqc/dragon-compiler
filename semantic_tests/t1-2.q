(* LEGAL: local names should hide non-local names *)

def main( input, output );
  var a,b: integral;

  proc boo( a: integral );
    var b: rational;

  begin
    b := 1.23
  end;

begin
  boo( b )
end.


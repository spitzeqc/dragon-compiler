(* ERROR: local objects not defined outside their scopes *)

def main( input, output );
  var a: integral;

  proc boo( a: integral );
    var x: integral;
  begin
  end;

begin
  a := x
end.


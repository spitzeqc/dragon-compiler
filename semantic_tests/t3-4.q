(* ERROR: mismatched type for index variable in FOR loops *)

def main( input, output );
  var a: integral;
  var x: rational;
begin
  for x := 1..5 do
    a := a + 1
end.


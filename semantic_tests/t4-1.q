(* ERROR: non-integer type for array index *)

def main( input, output );
  var a: array[3 .. 7] of rational;
  var x: integral;

begin
  a[x] := 1.23
end.


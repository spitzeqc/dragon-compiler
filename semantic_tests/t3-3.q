(* dangling ELSE binds to closest IF *)

def main( input, output );
  var a: integral;

begin
  scan( "%d", a);
  if ( a < 10 ) then
    if ( a >= 10 ) then
      a := 1
  else
      a := 0;
  print( "%d", a)
end.


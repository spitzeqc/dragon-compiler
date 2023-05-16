(* ERROR: function passed wrong number/type of arguments *)

def main( input, output );

  var b: integral;
  var y: rational;
  var a: array[1 .. 13] of integral;
  var z: array[1 .. 13] of rational;

  func foo( a: integral; x: rational ): integral;
  begin
    foo := a
  end;

begin
  b := foo(y,b) + foo(b,y,10)
end.





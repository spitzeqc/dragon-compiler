(* ERROR: function updating non-local variable *)

def main( input, output );
  var a: integral;
  
  func foo( b: integral ): integral;
  begin
    foo := a;
    a := b
  end;

begin

end.



(* reading/writing/modifying a variable *)

def main( output );
	var x: integral;
begin
	scan( "%d", x );
	x := x*x - 2*x + 1;	
	print( "%d", x );
end.


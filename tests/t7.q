(* PROC testing *)

def main( output );
	var x,y: integral;

	proc boo( a: integral );
	begin
		(* legal update of nonlocal name *)
		(* y := a*a + 2*a + 1; *)
		y := a*a + a*a + a;
	end;
begin
	scan( "%d", x );
	boo( x );
	print( "%d", y );
end.


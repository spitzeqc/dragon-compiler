(* FUNC testing *)

def main( output );
	var x,y: integral;

	func foo( a: integral ) : integral;
	begin
		foo := a*a + 2*a + 1
	end;
begin
	read( x );
	y := foo( x );
	write( y )
end.


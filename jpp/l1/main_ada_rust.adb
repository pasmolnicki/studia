with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with Interfaces.C; use Interfaces.C;
with Nums_Rust_Bindings; use Nums_Rust_Bindings;

procedure Main_Ada_Rust is

    A: int;
    B: int;
    C: int;
    Temp: Integer;

begin
    Put_Line ("Enter a, b, c for the equation a*x - b*y = c: ");
    Get (Temp);
    A := int (Temp);
    Get (Temp);
    B := int (Temp);
    Get (Temp);
    C := int (Temp);

    declare
        Result: Diofant_Result;
    begin
        Result := Diofant (A, B, C);
        if Result.X = 0 and Result.Y = 0 then
            Put_Line ("No solutions.");
        else
            Put ("One solution is x = ");
            Put (Integer (Result.X));
            Put (", y = ");
            Put (Integer (Result.Y));
            New_Line;
        end if;
    end;

end Main_Ada_Rust;

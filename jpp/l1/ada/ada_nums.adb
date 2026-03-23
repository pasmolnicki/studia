with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with Interfaces.C; use Interfaces.C;

package body Ada_Nums is
   
   function Gcd (A: int; B: int) return int is 
      X: int := A;
      Y: int := B;
      Temp: int;
   begin
      while Y /= 0 loop
         Temp := Y;
         Y := X mod Y;
         X := Temp;
      end loop;
      return abs(X);
   end Gcd;

   function Min_Divider (N: int) return int is
      I: int := 2;
   begin
      if N < 2 then
         return 0; -- No prime factors for numbers less than 2
      end if;
      while I * I <= N loop
         if N mod I = 0 then
            return I;
         end if;
         I := I + 1;
      end loop;
      return N; -- N is prime
   end Min_Divider;

   function Totient (N: int) return int is
      Result: int := N;
      I: int := 2;
      M: int := N;
   begin
      while I * I <= M loop
         if M mod I = 0 then
            while M mod I = 0 loop
               M := M / I;
            end loop;
            Result := Result - (Result / I);
         end if;
         I := I + 1;
      end loop;

      if M > 1 then
         Result := Result - (Result / M);
      end if;
      return Result;
   end Totient;

   function Diofant (A0: int; B0: int; C0: int) return Diofant_Result is
      Result: Diofant_Result;
      G: int := Gcd(A0, B0);
      X0: int := 1;
      Y0: int := 0;
      X1: int := 0;
      Y1: int := 1;
      a: int := A0;
      b: int := B0;
      c: int := C0;
   begin
      if C mod G /= 0 then
         return Result; -- No solutions
      end if;

      a := A0 / G;
      b := B0 / G;
      c := C0 / G;

      while b /= 0 loop
         declare
            Q: int := a / b;
            Temp: int;
         begin
            Temp := X0 - Q * X1;
            X0 := X1;
            X1 := Temp;

            Temp := Y0 - Q * Y1;
            Y0 := Y1;
            Y1 := Temp;

            Temp := a mod b;
            a := b;
            b := Temp;
         end;
      end loop;

      Result.X := X0 * c;
      Result.Y := -Y0 * c;
      return Result;
   end Diofant;

end Ada_Nums;
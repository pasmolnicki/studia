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
      Result: int := 0;
      I: int := 1;
   begin
      while I <= N loop
         if Gcd(I, N) = 1 then
            Result := Result + 1;
         end if;
         I := I + 1;
      end loop;
      return Result;
   end Totient;

   -- Extended Euclidean Algorithm
   -- In Ada, we use 'out' parameters to return multiple values
   procedure Extended_GCD (A, B : int; G, X, Y : out int) is
      X1, Y1 : int;
      Temp_G : int;
   begin
      if A = 0 then
         G := B;
         X := 0;
         Y := 1;
         return;
      end if;

      Extended_GCD (B mod A, A, Temp_G, X1, Y1);
      
      G := Temp_G;
      X := Y1 - (B / A) * X1;
      Y := X1;
   end Extended_GCD;

   function Diofant (A, B, C : int) return Diofant_Result is
      G, X0, Y0 : int;
      X, Y      : int;
      Step_X    : int;
      Step_Y    : int;
   begin
      Extended_GCD (A, B, G, X0, Y0);

      -- Check if a solution exists (c must be divisible by gcd(a, b))
      if C mod G /= 0 then
         return (X => -1, Y => -1);
      end if;

      -- Initial solution for ax - by = c
      X := X0 * (C / G);
      Y := -Y0 * (C / G);

      Step_X := B / G;
      Step_Y := A / G;

      -- Adjust to find the smallest natural solution (x >= 0, y >= 0)
      -- In ax - by = c, x and y move in the same direction
      while X < 0 or Y < 0 loop
         X := X + Step_X;
         Y := Y + Step_Y;
      end loop;

      while X - Step_X >= 0 and Y - Step_Y >= 0 loop
         X := X - Step_X;
         Y := Y - Step_Y;
      end loop;

      return (X => X, Y => Y);
   end Diofant;

   -- Ada wrappers exported with ada_ prefix
   function Ada_Gcd (A: int; B: int) return int is
   begin
      return Gcd(A, B);
   end Ada_Gcd;

   function Ada_Min_Divider (N: int) return int is
   begin
      return Min_Divider(N);
   end Ada_Min_Divider;

   function Ada_Totient (N: int) return int is
   begin
      return Totient(N);
   end Ada_Totient;

   function Ada_Diofant (A0: int; B0: int; C0: int) return Diofant_Result is
   begin
      return Diofant(A0, B0, C0);
   end Ada_Diofant;

end Ada_Nums;
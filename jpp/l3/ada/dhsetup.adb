with Ring;
with Ada.Numerics.Discrete_Random;

package body DHSetup is
   package body Generic_DH is

      function Factorize return Natural_Vectors.Vector is
         Factors : Natural_Vectors.Vector;
         N : Natural := P - 1;
         Div : Natural := 2;
         First_Div : Boolean := True;
      begin
         while N /= 1 loop
            if N mod Div = 0 then
               if First_Div then
                  Factors.Append (Div);
                  First_Div := False;
               end if;
               N := N / Div;
            else
               Div := Div + 1;
               First_Div := True;
            end if;
         end loop;
         return Factors;
      end Factorize;

      function Is_Generator (Factors : Natural_Vectors.Vector; N : Natural) return Boolean is
         package Ring_P is new Ring.Generic_Ring (N => P);
         use Ring_P;
      begin
         for F of Factors loop
            declare
               Exp : Natural := (P - 1) / F;
               Base : Ring_Type := Create (N);
               Result : Ring_Type := Pow (Base, Exp);
            begin
               if Value (Result) = 1 then
                  return False;
               end if;
            end;
         end loop;
         return True;
      end Is_Generator;

      function Find_Generator return Natural is
         package Random_Natural is new Ada.Numerics.Discrete_Random (Natural);
         Gen : Random_Natural.Generator;
         Factors : Natural_Vectors.Vector := Factorize;
         G : Natural;
      begin
         Random_Natural.Reset (Gen);
         loop
            G := Random_Natural.Random (Gen) mod (P - 1) + 1;
            exit when Is_Generator (Factors, G);
         end loop;
         return G;
      end Find_Generator;

      function Create return DH_Type is
      begin
         return (Gen => Find_Generator);
      end Create;

      function Get_Generator (Dh : DH_Type) return Natural is
      begin
         return Dh.Gen;
      end Get_Generator;

      function Power (Dh : DH_Type; A : Natural; B : Natural) return Natural is
         package Ring_P is new Ring.Generic_Ring (N => P);
         use Ring_P;
         
         Base : Ring_Type := Create (A);
         Result : Ring_Type := Pow (Base, B);
      begin
         return Value (Result);
      end Power;

   end Generic_DH;
end DHSetup;

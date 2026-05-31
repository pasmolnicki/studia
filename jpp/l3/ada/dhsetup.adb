with Ada.Numerics.Discrete_Random;

package body DHSetup is
   package body Generic_DHSetup is
      use Natural_Vectors;

      function Factorize return Vector is
         N : Natural := P - 1;
         Div : Natural := 2;
         First_Div : Boolean := True;
         Factors : Vector;
      begin
         while N /= 1 loop
            if N mod Div = 0 then
               if First_Div then
                  Factors.Append(Div);
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

      function Is_Generator (Factors : Vector; N : Natural) return Boolean is
         package Ring_P is new Ring.Generic_Ring(N => P);
      begin
         for F of Factors loop
            declare
               Exp : Natural := (P - 1) / F;
               Result_Ring : Ring_P.Ring_Type := Ring_P.Pow(Ring_P.Create(N), Exp);
               Result : Natural := Ring_P.Value(Result_Ring);
            begin
               if Result = 1 then
                  return False;
               end if;
            end;
         end loop;
         return True;
      end Is_Generator;

      function Find_Generator return Natural is
         package Random_Gen is new Ada.Numerics.Discrete_Random(Natural);
         Generator : Random_Gen.Generator;
         Factors : Vector := Factorize;
         G : Natural;
      begin
         Random_Gen.Reset(Generator);

         loop
            G := Random_Gen.Random(Generator) mod (P - 1);
            if G >= 2 then
               if Is_Generator(Factors, G) then
                  return G;
               end if;
            end if;
         end loop;
      end Find_Generator;

      function Create return DHSetup_Type is
      begin
         return (Gen => Find_Generator);
      end Create;

      function Get_Generator (Item : DHSetup_Type) return Natural is
      begin
         return Item.Gen;
      end Get_Generator;

      function Power (Item : DHSetup_Type; A : Natural; B : Natural) return Natural is
         package Ring_P is new Ring.Generic_Ring(N => P);
         A_Ring : Ring_P.Ring_Type := Ring_P.Create(A);
         Result_Ring : Ring_P.Ring_Type := Ring_P.Pow(A_Ring, B);
      begin
         return Ring_P.Value(Result_Ring);
      end Power;

   end Generic_DHSetup;
end DHSetup;

with Ring;
with Ada.Numerics.Discrete_Random;

package body RSA is
   package body Generic_RSA is

      N : constant Natural := P * Q;

      function GCD (A, B : Natural) return Natural is
         Temp_A : Natural := A;
         Temp_B : Natural := B;
      begin
         while Temp_B /= 0 loop
            declare
               Temp : Natural := Temp_B;
            begin
               Temp_B := Temp_A mod Temp_B;
               Temp_A := Temp;
            end;
         end loop;
         return Temp_A;
      end GCD;

      function Get_LCM return Natural is
         Lambda : Natural := (P - 1) * (Q - 1) / GCD(P - 1, Q - 1);
      begin
         return Lambda;
      end Get_LCM;

      function Find_Exponent (Lambda : Natural) return Natural is
         package Random_Natural is new Ada.Numerics.Discrete_Random (Natural);
         Gen : Random_Natural.Generator;
         E : Natural;
      begin
         Random_Natural.Reset (Gen);
         loop
            E := Random_Natural.Random (Gen) mod (Lambda - 2) + 2;
            exit when GCD (E, Lambda) = 1;
         end loop;
         return E;
      end Find_Exponent;

      function Create return RSA_Type is
         Lambda : Natural := Get_LCM;
         E : Natural := Find_Exponent (Lambda);
         
         package Ring_Lambda is new Ring.Generic_Ring (N => Lambda);
         use Ring_Lambda;
         
         Ring_E : Ring_Type := Create (E);
         Ring_D : Ring_Type := Inverse (Ring_E);
         D : Natural := Value (Ring_D);
      begin
         return (Exp => E, Lcm => Lambda, D => D);
      end Create;

      function Get_Public_Key (Rsa : RSA_Type) return Natural is
      begin
         return Rsa.Exp;
      end Get_Public_Key;

      function Encrypt (Rsa : RSA_Type; Message : Natural; Public_Key : Natural) return Natural is
         package Ring_N is new Ring.Generic_Ring (N => N);
         use Ring_N;
         
         Message_Ring : Ring_Type := Create (Message);
         Result_Ring : Ring_Type := Pow (Message_Ring, Public_Key);
      begin
         return Value (Result_Ring);
      end Encrypt;

      function Decrypt (Rsa : RSA_Type; Cipher : Natural) return Natural is
         package Ring_N is new Ring.Generic_Ring (N => N);
         use Ring_N;
         
         Cipher_Ring : Ring_Type := Create (Cipher);
         Result_Ring : Ring_Type := Pow (Cipher_Ring, Rsa.D);
      begin
         return Value (Result_Ring);
      end Decrypt;

   end Generic_RSA;
end RSA;

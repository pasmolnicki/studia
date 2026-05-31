with Ada.Text_IO; use Ada.Text_IO;
with Ada.Numerics.Discrete_Random;

package body RSA is
   package body Generic_RSA is

      function GCD (A, B : Natural) return Natural is
         A_Copy : Natural := A;
         B_Copy : Natural := B;
         Temp : Natural;
      begin
         while B_Copy /= 0 loop
            Temp := B_Copy;
            B_Copy := A_Copy mod B_Copy;
            A_Copy := Temp;
         end loop;
         return A_Copy;
      end GCD;

      function Get_LCM return Natural is
         P_Minus_1 : Natural := P - 1;
         Q_Minus_1 : Natural := Q - 1;
         GCD_Val : Natural := GCD(P_Minus_1, Q_Minus_1);
      begin
         return (P_Minus_1 * Q_Minus_1) / GCD_Val;
      end Get_LCM;

      function Find_Exponent (Lambda : Natural) return Natural is
         package Random_Exponent is new Ada.Numerics.Discrete_Random(Natural);
         Generator : Random_Exponent.Generator;
         E : Natural;
      begin
         Random_Exponent.Reset(Generator);
         
         loop
            E := Random_Exponent.Random(Generator) mod (Lambda - 1);
            if E < 2 then
               E := 2;
            end if;
            exit when GCD(E, Lambda) = 1;
         end loop;
         
         return E;
      end Find_Exponent;

      function Create return RSA_Type is
         N : constant Natural := P * Q;
         Lambda : constant Natural := Get_LCM;
         E : constant Natural := Find_Exponent(Lambda);
         
         -- Use Ring to calculate inverse
         package Ring_Lambda is new Ring.Generic_Ring(N => Lambda);
         E_Ring : Ring_Lambda.Ring_Type := Ring_Lambda.Create(E);
         D_Ring : Ring_Lambda.Ring_Type := Ring_Lambda.Inverse(E_Ring);
         D : Natural := Ring_Lambda.Value(D_Ring);
      begin
         return (Exp => E, LCM => Lambda, D => D);
      end Create;

      function Get_Public_Key (Item : RSA_Type) return Natural is
      begin
         return Item.Exp;
      end Get_Public_Key;

      function Encrypt (Item : RSA_Type; Message : Natural; Public_Key : Natural) return Natural is
         N : constant Natural := P * Q;
         package Ring_N is new Ring.Generic_Ring(N => N);
         Msg_Ring : Ring_N.Ring_Type := Ring_N.Create(Message);
         Result_Ring : Ring_N.Ring_Type := Ring_N.Pow(Msg_Ring, Public_Key);
      begin
         return Ring_N.Value(Result_Ring);
      end Encrypt;

      function Decrypt (Item : RSA_Type; Cipher : Natural) return Natural is
         N : constant Natural := P * Q;
         package Ring_N is new Ring.Generic_Ring(N => N);
         Cipher_Ring : Ring_N.Ring_Type := Ring_N.Create(Cipher);
         Result_Ring : Ring_N.Ring_Type := Ring_N.Pow(Cipher_Ring, Item.D);
      begin
         return Ring_N.Value(Result_Ring);
      end Decrypt;

   end Generic_RSA;
end RSA;

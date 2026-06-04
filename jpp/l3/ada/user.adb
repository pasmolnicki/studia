with Ring;
with Ada.Numerics.Discrete_Random;

package body User is
   package body Generic_User is

      function Create (Gen : Natural) return User_Type is
         package Random_Natural is new Ada.Numerics.Discrete_Random (Natural);
         package Ring_P is new Ring.Generic_Ring (N => P);
         use Ring_P;
         
         Rng : Random_Natural.Generator;
         Secret : Natural;
         Gen_Ring : Ring_Type;
         PK_Ring : Ring_Type;
      begin
         Random_Natural.Reset (Rng);
         Secret := Random_Natural.Random (Rng) mod (P - 2) + 2;
         
         Gen_Ring := Create (Gen);
         PK_Ring := Pow (Gen_Ring, Secret);
         
         return (
            Gen => Gen,
            Secret => Secret,
            Public_Key => Value (PK_Ring),
            Secret_Key => 0
         );
      end Create;

      function Get_Public_Key (User : User_Type) return Natural is
      begin
         return User.Public_Key;
      end Get_Public_Key;

      procedure Set_Key (User : in out User_Type; Key : Natural) is
         package Ring_P is new Ring.Generic_Ring (N => P);
         use Ring_P;
         
         Key_Ring : Ring_Type := Create (Key);
         Result_Ring : Ring_Type := Pow (Key_Ring, User.Secret);
      begin
         User.Secret_Key := Value (Result_Ring);
      end Set_Key;

      function Encrypt (User : User_Type; Msg : Natural) return Natural is
         package Ring_P is new Ring.Generic_Ring (N => P);
         use Ring_P;
         
         Msg_Ring : Ring_Type := Create (Msg);
         SK_Ring : Ring_Type := Create (User.Secret_Key);
         Result_Ring : Ring_Type := Msg_Ring * SK_Ring;
      begin
         return Value (Result_Ring);
      end Encrypt;

      function Decrypt (User : User_Type; Cipher : Natural) return Natural is
         package Ring_P is new Ring.Generic_Ring (N => P);
         use Ring_P;
         
         Cipher_Ring : Ring_Type := Create (Cipher);
         SK_Ring : Ring_Type := Create (User.Secret_Key);
         Result_Ring : Ring_Type := Cipher_Ring / SK_Ring;
      begin
         return Value (Result_Ring);
      end Decrypt;

   end Generic_User;
end User;

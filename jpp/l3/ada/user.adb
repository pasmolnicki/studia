with Ada.Numerics.Discrete_Random;
with Ring;

package body User is
   package body Generic_User is

      function Create (Gen : Natural) return User_Type is
         package Random_Secret is new Ada.Numerics.Discrete_Random(Natural);
         Generator : Random_Secret.Generator;
         package Ring_P is new Ring.Generic_Ring(N => P);
         
         Secret_Val : Natural;
         Gen_Ring : Ring_P.Ring_Type;
         Pub_Key_Ring : Ring_P.Ring_Type;
      begin
         Random_Secret.Reset(Generator);
         Secret_Val := 2 + (Random_Secret.Random(Generator) mod (P - 2));
         
         Gen_Ring := Ring_P.Create(Gen);
         Pub_Key_Ring := Ring_P.Pow(Gen_Ring, Secret_Val);
         
         return (
            Gen => Gen,
            Secret => Secret_Val,
            Public_Key => Ring_P.Value(Pub_Key_Ring),
            Secret_Key => 0
         );
      end Create;

      function Get_Public_Key (Item : User_Type) return Natural is
      begin
         return Item.Public_Key;
      end Get_Public_Key;

      procedure Set_Key (Item : in out User_Type; Key : Natural) is
         package Ring_P is new Ring.Generic_Ring(N => P);
         Key_Ring : Ring_P.Ring_Type := Ring_P.Create(Key);
         Result_Ring : Ring_P.Ring_Type := Ring_P.Pow(Key_Ring, Item.Secret);
      begin
         Item.Secret_Key := Ring_P.Value(Result_Ring);
      end Set_Key;

      function Encrypt (Item : User_Type; Msg : Natural) return Natural is
         package Ring_P is new Ring.Generic_Ring(N => P);
         Msg_Ring : Ring_P.Ring_Type := Ring_P.Create(Msg);
         Key_Ring : Ring_P.Ring_Type := Ring_P.Create(Item.Secret_Key);
         Result_Ring : Ring_P.Ring_Type;
      begin
         Result_Ring := Ring_P."*"(Msg_Ring, Key_Ring);
         return Ring_P.Value(Result_Ring);
      end Encrypt;

      function Decrypt (Item : User_Type; Cipher : Natural) return Natural is
         package Ring_P is new Ring.Generic_Ring(N => P);
         Cipher_Ring : Ring_P.Ring_Type := Ring_P.Create(Cipher);
         Key_Ring : Ring_P.Ring_Type := Ring_P.Create(Item.Secret_Key);
         Result_Ring : Ring_P.Ring_Type;
      begin
         Result_Ring := Ring_P."/"(Cipher_Ring, Key_Ring);
         return Ring_P.Value(Result_Ring);
      end Decrypt;

   end Generic_User;
end User;

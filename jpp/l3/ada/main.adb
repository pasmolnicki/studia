with Ada.Text_IO;
with RSA;
with DHSetup;
with User;

procedure Main is
   use Ada.Text_IO;

   package RSA_10007_10009 is new RSA.Generic_RSA (P => 10007, Q => 10009);
   use RSA_10007_10009;

   package DH_Setup is new DHSetup.Generic_DH (P => 1234567891);
   use DH_Setup;

   package DH_User is new User.Generic_User (P => 1234567891);
   use DH_User;

   procedure Print_Results (
      Alice : RSA_Type;
      Bob : RSA_Type;
      Cipher : Natural;
      Decrypted : Natural;
      Message : Natural
   ) is
   begin
      Put_Line ("Bob's public key: " & Natural'Image (Get_Public_Key (Bob)));
      Put_Line ("Alice's public key: " & Natural'Image (Get_Public_Key (Alice)));
      Put_Line ("Message: " & Natural'Image (Message));
      Put_Line ("Cipher: " & Natural'Image (Cipher));
      Put_Line ("Decrypted: " & Natural'Image (Decrypted));
      if Decrypted = Message then
         Put_Line ("Decryption successful: Yes");
      else
         Put_Line ("Decryption successful: No");
      end if;
   end Print_Results;

   procedure Print_Results_User (
      Alice : DH_User.User_Type;
      Bob : DH_User.User_Type;
      Cipher : Natural;
      Decrypted : Natural;
      Message : Natural
   ) is
   begin
      Put_Line ("Bob's public key: " & Natural'Image (Get_Public_Key (Bob)));
      Put_Line ("Alice's public key: " & Natural'Image (Get_Public_Key (Alice)));
      Put_Line ("Message: " & Natural'Image (Message));
      Put_Line ("Cipher: " & Natural'Image (Cipher));
      Put_Line ("Decrypted: " & Natural'Image (Decrypted));
      if Decrypted = Message then
         Put_Line ("Decryption successful: Yes");
      else
         Put_Line ("Decryption successful: No");
      end if;
   end Print_Results_User;

   procedure RSA_Example is
      Bob : RSA_Type := Create;
      Alice : RSA_Type := Create;
      Message : Natural := 12345;
      Cipher : Natural;
      Decrypted : Natural;
   begin
      Put_Line ("RSA encryption example:");
      Cipher := Encrypt (Bob, Message, Get_Public_Key (Alice));
      Decrypted := Decrypt (Alice, Cipher);
      Print_Results (Alice, Bob, Cipher, Decrypted, Message);
   end RSA_Example;

   procedure DH_Example is
      Setup : DH_Type := Create;
      Alice : DH_User.User_Type;
      Bob : DH_User.User_Type;
      Message : Natural := 1234;
      Cipher : Natural;
      Decrypted : Natural;
   begin
      Put_Line ("Diffie-Hellman key exchange example:");
      Put_Line ("Generator: " & Natural'Image (Get_Generator (Setup)));
      
      Alice := Create (Get_Generator (Setup));
      Bob := Create (Get_Generator (Setup));

      declare
         Alice_Copy : DH_User.User_Type := Alice;
         Bob_Copy : DH_User.User_Type := Bob;
      begin
         Set_Key (Alice_Copy, Get_Public_Key (Bob));
         Set_Key (Bob_Copy, Get_Public_Key (Alice));

         Cipher := Encrypt (Alice_Copy, Message);
         Decrypted := Decrypt (Bob_Copy, Cipher);

         --  Put_Line ("Generator: " & Natural'Image (Get_Generator (Setup)));
         Print_Results_User (Alice_Copy, Bob_Copy, Cipher, Decrypted, Message);
      end;
   end DH_Example;

begin
   RSA_Example;
   Put_Line ("");
   Put_Line ("");
   DH_Example;
end Main;

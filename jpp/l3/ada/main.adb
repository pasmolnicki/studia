with Ada.Text_IO; use Ada.Text_IO;
with Ring;
with RSA;
with DHSetup;
with User;

procedure Main is
   
   procedure Print_Results (
      Alice_Pub_Key : Natural;
      Bob_Pub_Key : Natural;
      Message : Natural;
      Cipher : Natural;
      Decrypted : Natural
   ) is
   begin
      Put_Line("Bob's public key: " & Natural'Image(Bob_Pub_Key));
      Put_Line("Alice's public key: " & Natural'Image(Alice_Pub_Key));
      Put_Line("Message: " & Natural'Image(Message));
      Put_Line("Cipher: " & Natural'Image(Cipher));
      Put_Line("Decrypted: " & Natural'Image(Decrypted));
      if Decrypted = Message then
         Put_Line("Decryption successful: Yes");
      else
         Put_Line("Decryption successful: No");
      end if;
   end Print_Results;

   procedure RSA_Example is
      package RSA_Impl is new RSA.Generic_RSA(P => 10007, Q => 10009);
      
      Bob : RSA_Impl.RSA_Type;
      Alice : RSA_Impl.RSA_Type;
      
      Message : constant Natural := 12345;
      Cipher : Natural;
      Decrypted : Natural;
   begin
      Put_Line("RSA encryption example:");
      
      Bob := RSA_Impl.Create;
      Alice := RSA_Impl.Create;
      
      Cipher := RSA_Impl.Encrypt(Bob, Message, RSA_Impl.Get_Public_Key(Alice));
      Decrypted := RSA_Impl.Decrypt(Alice, Cipher);
      
      Print_Results(
         RSA_Impl.Get_Public_Key(Alice),
         RSA_Impl.Get_Public_Key(Bob),
         Message,
         Cipher,
         Decrypted
      );
   end RSA_Example;

   procedure DH_Example is
      package DH_Setup_Impl is new DHSetup.Generic_DHSetup(P => 1234567891);
      package User_Impl is new User.Generic_User(P => 1234567891);
      
      Setup : DH_Setup_Impl.DHSetup_Type;
      Alice : User_Impl.User_Type;
      Bob : User_Impl.User_Type;
      
      Message : constant Natural := 1234;
      Cipher : Natural;
      Decrypted : Natural;
   begin
      Put_Line("Diffie-Hellman key exchange example:");
      
      Setup := DH_Setup_Impl.Create;
      Put_Line("Generator: " & Natural'Image(DH_Setup_Impl.Get_Generator(Setup)));
      
      Alice := User_Impl.Create(DH_Setup_Impl.Get_Generator(Setup));
      Bob := User_Impl.Create(DH_Setup_Impl.Get_Generator(Setup));
      
      User_Impl.Set_Key(Alice, User_Impl.Get_Public_Key(Bob));
      User_Impl.Set_Key(Bob, User_Impl.Get_Public_Key(Alice));
      
      Cipher := User_Impl.Encrypt(Alice, Message);
      Decrypted := User_Impl.Decrypt(Bob, Cipher);
      
      Put_Line("Generator: " & Natural'Image(DH_Setup_Impl.Get_Generator(Setup)));
      Print_Results(
         User_Impl.Get_Public_Key(Alice),
         User_Impl.Get_Public_Key(Bob),
         Message,
         Cipher,
         Decrypted
      );
   end DH_Example;

begin
   RSA_Example;
   Put_Line("");
   Put_Line("");
   DH_Example;
end Main;

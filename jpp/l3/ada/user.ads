with Ring;

package User is
   generic
      P : Positive;
   package Generic_User is
      type User_Type is private;

      function Create (Gen : Natural) return User_Type;

      function Get_Public_Key (Item : User_Type) return Natural;

      procedure Set_Key (Item : in out User_Type; Key : Natural);

      function Encrypt (Item : User_Type; Msg : Natural) return Natural;

      function Decrypt (Item : User_Type; Cipher : Natural) return Natural;

   private
      type User_Type is record
         Gen : Natural;
         Secret : Natural;
         Public_Key : Natural;
         Secret_Key : Natural;
      end record;
   end Generic_User;
end User;

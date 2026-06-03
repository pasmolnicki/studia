with Ring;

package RSA is
   generic
      P : Positive;
      Q : Positive;
   package Generic_RSA is
      type RSA_Type is private;

      function Create return RSA_Type;
      function Get_Public_Key (Rsa : RSA_Type) return Natural;
      function Encrypt (Rsa : RSA_Type; Message : Natural; Public_Key : Natural) return Natural;
      function Decrypt (Rsa : RSA_Type; Cipher : Natural) return Natural;

   private
      type RSA_Type is record
         Exp : Natural;
         Lcm : Natural;
         D   : Natural;
      end record;
   end Generic_RSA;
end RSA;

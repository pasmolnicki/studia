package Ring is
   generic
      N : Positive;
   package Generic_Ring is
      type Ring_Type is private;

      function Create (Value : Natural) return Ring_Type;
      
      function "+" (Left, Right : Ring_Type) return Ring_Type;
      function "-" (Left, Right : Ring_Type) return Ring_Type;
      function "*" (Left, Right : Ring_Type) return Ring_Type;
      function "/" (Left, Right : Ring_Type) return Ring_Type;
      function "-" (Right : Ring_Type) return Ring_Type;
      
      function Inverse (Item : Ring_Type) return Ring_Type;
      
      function Pow (Item : Ring_Type; Exp : Natural) return Ring_Type;
      
      function Image (Item : Ring_Type) return String;
      function Value (Item : Ring_Type) return Natural;
      function Valid (Item : Ring_Type) return Boolean;
      
   private
      type Ring_Type is record
         Val   : Natural := 0;
         Valid : Boolean := True;
      end record;
   end Generic_Ring;
end Ring;

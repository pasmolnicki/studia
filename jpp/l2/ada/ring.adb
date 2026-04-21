package body Ring is
   package body Generic_Ring is

      function Create (Value : Natural) return Ring_Type is
      begin
         return (Val => Value mod N, Valid => True);
      end Create;

      function "+" (Left, Right : Ring_Type) return Ring_Type is
      begin
         if not Left.Valid or not Right.Valid then
            return (Val => 0, Valid => False);
         end if;
         return (Val => (Left.Val + Right.Val) mod N, Valid => True);
      end "+";

      function "-" (Left, Right : Ring_Type) return Ring_Type is
      begin
         if not Left.Valid or not Right.Valid then
            return (Val => 0, Valid => False);
         end if;
         return (Val => (Left.Val + N - (Right.Val mod N)) mod N, Valid => True);
      end "-";

      function "*" (Left, Right : Ring_Type) return Ring_Type is
      begin
         if not Left.Valid or not Right.Valid then
            return (Val => 0, Valid => False);
         end if;
         return (Val => (Left.Val * Right.Val) mod N, Valid => True);
      end "*";

      function Inverse (Item : Natural) return Natural is
      begin
         for I in 1 .. N - 1 loop
            if (Item * I) mod N = 1 then
               return I;
            end if;
         end loop;
         return 0;
      end Inverse;

      function "/" (Left, Right : Ring_Type) return Ring_Type is
         Inv : Natural;
      begin
         if not Left.Valid or not Right.Valid then
            return (Val => 0, Valid => False);
         end if;
         
         Inv := Inverse (Right.Val);
         if Inv = 0 then
            return (Val => 0, Valid => False);
         end if;
         
         return (Val => (Left.Val * Inv) mod N, Valid => True);
      end "/";
      
      function "-" (Right : Ring_Type) return Ring_Type is
      begin
         if not Right.Valid then
            return (Val => 0, Valid => False);
         end if;
         return (Val => (N - (Right.Val mod N)) mod N, Valid => True);
      end "-";

      function Image (Item : Ring_Type) return String is
      begin
         if not Item.Valid then
            return "None";
         else
            return Natural'Image(Item.Val);
         end if;
      end Image;
      
      function Value (Item : Ring_Type) return Natural is
      begin
         return Item.Val;
      end Value;
      
      function Valid (Item : Ring_Type) return Boolean is
      begin
         return Item.Valid;
      end Valid;

   end Generic_Ring;
end Ring;

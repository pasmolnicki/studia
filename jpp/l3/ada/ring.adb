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
         Temp_Result : Long_Long_Integer;
      begin
         if not Left.Valid or not Right.Valid then
            return (Val => 0, Valid => False);
         end if;
         Temp_Result := Long_Long_Integer(Left.Val) * Long_Long_Integer(Right.Val);
         return (Val => Natural(Temp_Result mod Long_Long_Integer(N)), Valid => True);
      end "*";

      --  Add here Pow(N: Natural) (fast exponentiation using square-and-multiply algorithm)


      function Inverse (Item : Natural) return Natural is
         T     : Integer := 0;
         New_T : Integer := 1;
         R     : Integer := Integer (N);
         New_R : Integer := Integer (Item mod N);
         Q     : Integer;
         Temp  : Integer;
      begin
         while New_R /= 0 loop
            Q := R / New_R;

            Temp := T - Q * New_T;
            T := New_T;
            New_T := Temp;

            Temp := R - Q * New_R;
            R := New_R;
            New_R := Temp;
         end loop;

         if R > 1 then
            return 0;
         end if;

         if T < 0 then
            T := T + Integer (N);
         end if;

         return Natural (T);
      end Inverse;

      function "/" (Left, Right : Ring_Type) return Ring_Type is
         Inv : Natural;
         Temp_Result : Long_Long_Integer;
      begin
         if not Left.Valid or not Right.Valid then
            return (Val => 0, Valid => False);
         end if;
         
         Inv := Inverse (Right.Val);
         if Inv = 0 then
            return (Val => 0, Valid => False);
         end if;
         
         Temp_Result := Long_Long_Integer(Left.Val) * Long_Long_Integer(Inv);
         return (Val => Natural(Temp_Result mod Long_Long_Integer(N)), Valid => True);
      end "/";
      
      function "-" (Right : Ring_Type) return Ring_Type is
      begin
         if not Right.Valid then
            return (Val => 0, Valid => False);
         end if;
         return (Val => (N - (Right.Val mod N)) mod N, Valid => True);
      end "-";

      function Inverse (Item : Ring_Type) return Ring_Type is
         Inv : Natural;
      begin
         if not Item.Valid then
            return (Val => 0, Valid => False);
         end if;
         Inv := Inverse(Item.Val);
         if Inv = 0 then
            return (Val => 0, Valid => False);
         end if;
         return (Val => Inv, Valid => True);
      end Inverse;

      function Pow (Item : Ring_Type; Exp : Natural) return Ring_Type is
         Result : Natural := 1;
         Base : Natural := Item.Val;
         Exp_Copy : Natural := Exp;
         Temp_Result : Long_Long_Integer;
         Temp_Base : Long_Long_Integer;
      begin
         if not Item.Valid then
            return (Val => 0, Valid => False);
         end if;

         while Exp_Copy > 0 loop
            if (Exp_Copy mod 2) = 1 then
               Temp_Result := Long_Long_Integer(Result) * Long_Long_Integer(Base);
               Result := Natural(Temp_Result mod Long_Long_Integer(N));
            end if;
            Exp_Copy := Exp_Copy / 2;
            Temp_Base := Long_Long_Integer(Base) * Long_Long_Integer(Base);
            Base := Natural(Temp_Base mod Long_Long_Integer(N));
         end loop;

         return (Val => Result, Valid => True);
      end Pow;

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

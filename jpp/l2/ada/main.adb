with Ada.Text_IO; use Ada.Text_IO;
with Ring;

procedure Main is
   package Ring_5 is new Ring.Generic_Ring (N => 5);
   use Ring_5;
   
   package Ring_7 is new Ring.Generic_Ring (N => 7);
   use Ring_7;

   package Ring_6 is new Ring.Generic_Ring (N => 6);
   use Ring_6;
   
   procedure Assert (Condition : Boolean; Message : String) is
   begin
      if not Condition then
         Put_Line ("Assertion failed: " & Message);
      end if;
   end Assert;
begin
   Put_Line ("Running tests...");
   
   --  test_constructors
   declare
      R0 : Ring_5.Ring_Type := Create (0);
      R1 : Ring_5.Ring_Type := Create (3);
      R2 : Ring_5.Ring_Type := Create (8);
   begin
      Assert (Value (R0) = 0 and Valid (R0), "R0 = 0");
      Assert (Value (R1) = 3 and Valid (R1), "R1 = 3");
      Assert (Value (R2) = 3 and Valid (R2), "R2 = 3");
   end;
   
   --  test_addition
   declare
      R1 : Ring_5.Ring_Type := Create (3);
      R2 : Ring_5.Ring_Type := Create (4);
      R3 : Ring_5.Ring_Type := R1 + R2;
   begin
      Assert (Value (R3) = 2 and Valid (R3), "3 + 4 mod 5 = 2");
   end;
   
   --  test_subtraction
   declare
      R1 : Ring_7.Ring_Type := Create (2);
      R2 : Ring_7.Ring_Type := Create (5);
      R3 : Ring_7.Ring_Type := R1 - R2;
      R4 : Ring_7.Ring_Type := R2 - R1;
   begin
      Assert (Value (R3) = 4 and Valid (R3), "2 - 5 mod 7 = 4");
      Assert (Value (R4) = 3 and Valid (R4), "5 - 2 mod 7 = 3");
   end;
   
   --  test_multiplication
   declare
      R1 : Ring_6.Ring_Type := Create (2);
      R2 : Ring_6.Ring_Type := Create (3);
      R3 : Ring_6.Ring_Type := R1 * R2;
   begin
      Assert (Value (R3) = 0 and Valid (R3), "2 * 3 mod 6 = 0");
   end;
   
   --  test_division
   declare
      R1 : Ring_6.Ring_Type := Create (2);
      R2 : Ring_6.Ring_Type := Create (3);
      R3 : Ring_6.Ring_Type := R1 / R2;
      
      R4 : Ring_7.Ring_Type := Create (2);
      R5 : Ring_7.Ring_Type := Create (3);
      R6 : Ring_7.Ring_Type := R4 / R5;
   begin
      Assert (not Valid (R3), "2 / 3 mod 6 is None");
      Assert (Value (R6) = 3 and Valid (R6), "2 / 3 mod 7 = 3");
   end;
   
   --  test_negation
   declare
      R1 : Ring_5.Ring_Type := Create (2);
      R2 : Ring_5.Ring_Type := -R1;
   begin
      Assert (Value (R2) = 3 and Valid (R2), "-2 mod 5 = 3");
   end;
   
   Put_Line ("Tests finished.");
end Main;

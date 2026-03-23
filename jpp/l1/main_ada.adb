with Ada.Text_IO; use Ada.Text_IO;
with Ada.Integer_Text_IO; use Ada.Integer_Text_IO;
with Interfaces.C; use Interfaces.C;
with Nums_C_Bindings; use Nums_C_Bindings;
with Nums_Rust_Bindings; use Nums_Rust_Bindings;
with Ada_Nums; use Ada_Nums;

procedure Main_Ada is

    type Diofant_Tuple is record
        A: int;
        B: int;
        C: int;
    end record;

    Static_D_Tests: constant array (1 .. 3) of Diofant_Tuple := (
        (A => 48, B => 18, C => 6),
        (A => 5, B => 3, C => 2),
        (A => 3, B => 2, C => 1)
    );

    procedure Run_Static is
        Result_C: Nums_C_Bindings.Diofant_Result;
        Result_Rust: Nums_Rust_Bindings.Diofant_Result;
        Result_Ada: Ada_Nums.Diofant_Result;
    begin
        Put_Line ("Running static tests for C, Rust and Ada libraries:");
        for I in Static_D_Tests'Range loop
            declare
                A : int := Static_D_Tests(I).A;
                B : int := Static_D_Tests(I).B;
                C : int := Static_D_Tests(I).C;
            begin
                Result_C := Nums_C_Bindings.Diofant (A, B, C);
                Result_Rust := Nums_Rust_Bindings.Rust_Diofant (A, B, C);
                Result_Ada := Ada_Nums.Diofant (A, B, C);

                Put ("Test: a="); Put (Integer (A)); Put (", b="); Put (Integer (B)); Put (", c="); Put (Integer (C)); New_Line;
                     Put ("  C: ");
                     if Result_C.X = 0 and Result_C.Y = 0 then
                         Put_Line ("no positive solution");
                     else
                         Put ("x="); Put (Integer (Result_C.X)); Put (", y="); Put (Integer (Result_C.Y)); New_Line;
                     end if;

                     Put ("  Rust: ");
                     if Result_Rust.X = 0 and Result_Rust.Y = 0 then
                         Put_Line ("no positive solution");
                     else
                         Put ("x="); Put (Integer (Result_Rust.X)); Put (", y="); Put (Integer (Result_Rust.Y)); New_Line;
                     end if;

                     Put ("  Ada: ");
                     if Result_Ada.X = 0 and Result_Ada.Y = 0 then
                         Put_Line ("no positive solution");
                     else
                         Put ("x="); Put (Integer (Result_Ada.X)); Put (", y="); Put (Integer (Result_Ada.Y)); New_Line;
                     end if;
                New_Line;
            end;
        end loop;
    end Run_Static;

    procedure Interactive is
        A: int; B: int; C: int; Temp: Integer;
        Result_C: Nums_C_Bindings.Diofant_Result;
        Result_Rust: Nums_Rust_Bindings.Diofant_Result;
        Result_Ada: Ada_Nums.Diofant_Result;
    begin
        Put_Line ("Interactive mode. Enter a b c (Ctrl+D to finish):");
        loop
           begin
              Get (Temp); A := int (Temp);
              Get (Temp); B := int (Temp);
              Get (Temp); C := int (Temp);
           exception
              when End_Error =>
                 exit;
           end;
              Result_C := Nums_C_Bindings.Diofant (A, B, C);
              Result_Rust := Nums_Rust_Bindings.Rust_Diofant (A, B, C);
              Result_Ada := Ada_Nums.Diofant (A, B, C);

              Put ("C: ");
              if Result_C.X = 0 and Result_C.Y = 0 then
                  Put_Line ("no positive solution");
              else
                  Put ("x="); Put (Integer (Result_C.X)); Put (", y="); Put (Integer (Result_C.Y)); New_Line;
              end if;

              Put ("Rust: ");
              if Result_Rust.X = 0 and Result_Rust.Y = 0 then
                  Put_Line ("no positive solution");
              else
                  Put ("x="); Put (Integer (Result_Rust.X)); Put (", y="); Put (Integer (Result_Rust.Y)); New_Line;
              end if;

              Put ("Ada: ");
              if Result_Ada.X = 0 and Result_Ada.Y = 0 then
                  Put_Line ("no positive solution");
              else
                  Put ("x="); Put (Integer (Result_Ada.X)); Put (", y="); Put (Integer (Result_Ada.Y)); New_Line;
              end if;
        end loop;
    end Interactive;

begin
    Run_Static;
    Put_Line ("To run interactive tests, re-run and enter values or press Ctrl+D to skip.");
    Interactive;

end Main_Ada;

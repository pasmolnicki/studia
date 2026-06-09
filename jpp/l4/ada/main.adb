with Ada.Text_IO;
with Ada.Command_Line;
with Ada.Integer_Text_IO;
with Philosopher;

procedure Main is
   use Ada.Text_IO;

   function Parse_Argument (Index : Positive) return Integer is
      Arg : String := Ada.Command_Line.Argument (Index);
   begin
      return Integer'Value (Arg);
   exception
      when others =>
         Put_Line ("Error: Invalid argument");
         raise;
   end Parse_Argument;

begin
   if Ada.Command_Line.Argument_Count < 2 then
      Put_Line ("Usage: ./main <n_philosophers> <n_meals_per_philosopher>");
      return;
   end if;

   declare
      N_Philosophers           : Integer;
      N_Meals_Per_Philosopher : Integer;
   begin
      N_Philosophers           := Parse_Argument (1);
      N_Meals_Per_Philosopher := Parse_Argument (2);

      Philosopher.Run_Simulation (N_Philosophers, N_Meals_Per_Philosopher);
   end;
end Main;

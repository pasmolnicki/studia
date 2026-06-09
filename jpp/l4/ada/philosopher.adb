with Ada.Text_IO;
with Ada.Numerics.Discrete_Random;
with Ada.Float_Text_IO;
with Ada.Integer_Text_IO;

package body Philosopher is

   use Ada.Text_IO;

   Buf_Size : constant Natural := 1;

   type Internal_Buf_Type is array (Natural range 0 .. Buf_Size - 1) of Boolean;

   protected type Fork_Channel is
      entry Read (Item : out Boolean);
      entry Write (Item : in Boolean);
   private
      Count       : Natural := 0;
      Buf         : Internal_Buf_Type;
      Write_Index : Natural := 0;
      Read_Index  : Natural := 0;
   end Fork_Channel;

   protected body Fork_Channel is
      entry Read (Item : out Boolean)
         when Count > 0 is
      begin
         Item       := Buf (Read_Index);
         Count      := Count - 1;
         Read_Index := (Read_Index + 1) mod Buf_Size;
      end Read;

      entry Write (Item : in Boolean)
         when Count < Buf_Size is
      begin
         Buf (Write_Index) := Item;
         Count             := Count + 1;
         Write_Index       := (Write_Index + 1) mod Buf_Size;
      end Write;
   end Fork_Channel;

   type Finished_Array is array (Positive range <>) of Boolean;

   protected type Finished_Tracker (N : Positive) is
      procedure Mark_Finished (Id : Positive);
      function All_Finished return Boolean;
   private
      Finished : Finished_Array (1 .. N) := (others => False);
   end Finished_Tracker;

   protected body Finished_Tracker is
      procedure Mark_Finished (Id : Positive) is
      begin
         Finished (Id) := True;
      end Mark_Finished;

      function All_Finished return Boolean is
      begin
         for I in Finished'Range loop
            if not Finished (I) then
               return False;
            end if;
         end loop;
         return True;
      end All_Finished;
   end Finished_Tracker;

   type Meals_Array is array (Positive range <>) of Integer;

   protected type Stats_Tracker (N : Positive) is
      procedure Set_Meals (Id : Positive; Meals : Integer);
      function Get_Meals (Id : Positive) return Integer;
   private
      Meals_Count : Meals_Array (1 .. N) := (others => 0);
   end Stats_Tracker;

   protected body Stats_Tracker is
      procedure Set_Meals (Id : Positive; Meals : Integer) is
      begin
         Meals_Count (Id) := Meals;
      end Set_Meals;

      function Get_Meals (Id : Positive) return Integer is
      begin
         return Meals_Count (Id);
      end Get_Meals;
   end Stats_Tracker;

   protected Sync_Print is
      procedure Println (S : String);
   end Sync_Print;

   protected body Sync_Print is
      procedure Println (S : String) is
      begin
         Put_Line ("[debug] " & S);
      end Println;
   end Sync_Print;

   package Random_Int is new Ada.Numerics.Discrete_Random (Integer);

   protected Random_Gen is
      procedure Reset;
      function Random_Value return Integer;
   private
      Gen : Random_Int.Generator;
   end Random_Gen;

   protected body Random_Gen is
      procedure Reset is
      begin
         Random_Int.Reset (Gen);
      end Reset;

      function Random_Value return Integer is
      begin
         return Random_Int.Random (Gen);
      end Random_Value;
   end Random_Gen;

   type Fork_Array is array (Positive range <>) of Fork_Channel;
   type Fork_Array_Ptr is access all Fork_Array;

   type Config_Type is record
      Id            : Positive := 1;
      N             : Positive := 1;
      N_Meals       : Positive := 1;
      Fork_Ref      : Fork_Array_Ptr := null;
      Finished_Ref  : access Finished_Tracker := null;
      Stats_Ref     : access Stats_Tracker := null;
   end record;

   task type Philosopher_Task is
      entry Set_Config (C : Config_Type);
   end Philosopher_Task;

   task body Philosopher_Task is
      Id            : Positive;
      N             : Positive;
      N_Meals       : Positive;
      Forks         : Fork_Array_Ptr;
      Finished_Ref  : access Finished_Tracker;
      Stats_Ref     : access Stats_Tracker;
      Eaten_Meals   : Integer := 0;
      Untagged      : Boolean := False;
      Dummy         : Boolean;
      Config        : Config_Type;

      function Left_Fork (Phil_Id : Positive; N_Phils : Positive) return Positive is
      begin
         if Phil_Id = N_Phils then
            return Phil_Id - 1;
         else
            return Phil_Id;
         end if;
      end Left_Fork;

      function Right_Fork (Phil_Id : Positive; N_Phils : Positive) return Positive is
      begin
         if Phil_Id = N_Phils then
            return 1;
         else
            return Phil_Id + 1;
         end if;
      end Right_Fork;

      procedure Eat_Meal is
         Rand_Duration : Integer;
      begin
         Rand_Duration := Random_Gen.Random_Value mod 401 + 100;
         Sync_Print.Println
            (Positive'Image (Id) & " is eating for" & Integer'Image (Rand_Duration) & "us");
         delay Duration (Rand_Duration) / 1_000_000.0;
      end Eat_Meal;

      procedure Think is
         Rand_Duration : Integer;
      begin
         Rand_Duration := Random_Gen.Random_Value mod 401 + 100;
         Sync_Print.Println
            (Positive'Image (Id) & " is thinking for" & Integer'Image (Rand_Duration) & "us");
         delay Duration (Rand_Duration) / 1_000_000.0;
      end Think;

   begin
      accept Set_Config (C : Config_Type) do
         Config := C;
      end Set_Config;

      Id            := Config.Id;
      N             := Config.N;
      N_Meals       := Config.N_Meals;
      Forks         := Config.Fork_Ref;
      Finished_Ref  := Config.Finished_Ref;
      Stats_Ref     := Config.Stats_Ref;

      loop
         Think;
         Forks (Left_Fork (Id, N)).Read (Dummy);
         Forks (Right_Fork (Id, N)).Read (Dummy);
         Eat_Meal;
         Forks (Left_Fork (Id, N)).Write (True);
         Forks (Right_Fork (Id, N)).Write (True);

         Eaten_Meals := Eaten_Meals + 1;
         if not Untagged and then Eaten_Meals >= N_Meals then
            Finished_Ref.Mark_Finished (Id);
            Untagged := True;
         end if;

         if Finished_Ref.All_Finished then
            exit;
         end if;
      end loop;

      Stats_Ref.Set_Meals (Id, Eaten_Meals);
   end Philosopher_Task;

   type Philosopher_Array is array (Positive range <>) of Philosopher_Task;
   type Philosopher_Array_Ptr is access all Philosopher_Array;

   procedure Run_Simulation (N_Philosophers : Positive; N_Meals_Per_Philosopher : Positive) is
      Philosophers : constant Philosopher_Array_Ptr :=
         new Philosopher_Array (1 .. N_Philosophers);
      Finished : aliased Finished_Tracker (N_Philosophers);
      Stats    : aliased Stats_Tracker (N_Philosophers);
      Forks : constant Fork_Array_Ptr :=
            new Fork_Array (1 .. N_Philosophers);

      Total  : Integer := 0;
      Min    : Integer := Integer'Last;
      Max    : Integer := 0;
      Avg    : Float;
      Meals  : Integer;
      Config : Config_Type;

   begin
      Random_Gen.Reset;

      for I in 1 .. N_Philosophers loop
         Forks (I).Write (True);
      end loop;

      for I in 1 .. N_Philosophers loop
         Config :=
            (Id           => I,
             N            => N_Philosophers,
             N_Meals      => N_Meals_Per_Philosopher,
             Fork_Ref     => Forks,
             Finished_Ref => Finished'Unchecked_Access,
             Stats_Ref    => Stats'Unchecked_Access);
         Philosophers (I).Set_Config (Config);
      end loop;

      while not Finished.All_Finished loop
         delay 0.1;
      end loop;

      for I in 1 .. N_Philosophers loop
         Meals := Stats.Get_Meals (I);
         Total := Total + Meals;
         if Meals < Min then
            Min := Meals;
         end if;
         if Meals > Max then
            Max := Meals;
         end if;
      end loop;

      Avg := Float (Total) / Float (N_Philosophers);

      Put_Line ("");
      Put_Line ("---------- Results ----------");
      Put ("Total eaten meals: ");
      Ada.Integer_Text_IO.Put (Total, 0);
      New_Line;
      Put ("Average meals per philo: ");
      Ada.Float_Text_IO.Put (Avg, Fore => 1, Aft => 1, Exp => 0);
      New_Line;
      Put ("Min-max eaten meals: min=");
      Ada.Integer_Text_IO.Put (Min, 0);
      Put (" max=");
      Ada.Integer_Text_IO.Put (Max, 0);
      New_Line;

      for I in 1 .. N_Philosophers loop
         Put (" [");
         Ada.Integer_Text_IO.Put (I, 0);
         Put ("] meals eaten: ");
         Ada.Integer_Text_IO.Put (Stats.Get_Meals (I), 0);
         New_Line;
      end loop;
   end Run_Simulation;

end Philosopher;

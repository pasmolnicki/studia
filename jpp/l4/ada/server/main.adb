with Ada.Text_IO;
with Ada.Command_Line;
with Ada.Strings.Unbounded;
with Ada.Numerics.Discrete_Random;

procedure Main is
begin
   -- Walidacja argumentów linii poleceń
   if Ada.Command_Line.Argument_Count /= 2 then
      Ada.Text_IO.Put_Line("Usage: " & Ada.Command_Line.Command_Name & " <n_users> <n_messages_per_user>");
      Ada.Text_IO.Put_Line("Example: " & Ada.Command_Line.Command_Name & " 5 10");
      return;
   end if;

   declare
      N_Users             : constant Integer := Integer'Value(Ada.Command_Line.Argument(1));
      N_Messages_Per_User : constant Integer := Integer'Value(Ada.Command_Line.Argument(2));

      -- Pomocnicza funkcja do formatowania liczb bez wiodącej spacji (cecha Integer'Image)
      function Img(I : Integer) return String is
         S : constant String := Integer'Image(I);
      begin
         if S(S'First) = ' ' then
            return S(S'First + 1 .. S'Last);
         else
            return S;
         end if;
      end Img;

      -- Struktura wiadomości
      type Message is record
         Msg      : Ada.Strings.Unbounded.Unbounded_String;
         Sender   : Integer;
         Receiver : Integer;
      end record;

      -- Odpowiednik SyncPrint z użyciem obiektu chronionego (Mutex)
      protected Sync_Print_Obj is
         procedure Println(S : String);
      end Sync_Print_Obj;

      protected body Sync_Print_Obj is
         procedure Println(S : String) is
         begin
            Ada.Text_IO.Put_Line(S);
         end Println;
      end Sync_Print_Obj;

      -- Bariera synchronizacyjna (odpowiednik sync.WaitGroup dla zakończenia serwera)
      protected Barrier is
         procedure Signal;
         entry Wait;
      private
         Done : Boolean := False;
      end Barrier;

      protected body Barrier is
         procedure Signal is
         begin
            Done := True;
         end Signal;
         entry Wait when Done is
         begin
            null;
         end Wait;
      end Barrier;

      -- Maszyna losująca użytkowników
      subtype User_Id_Range is Integer range 0 .. N_Users - 1;
      package Random_User_Pkg is new Ada.Numerics.Discrete_Random(User_Id_Range);
      Gen : Random_User_Pkg.Generator;

      protected Random_Provider is
         procedure Reset;
         function Get_Random_User return Integer;
      end Random_Provider;

      protected body Random_Provider is
         procedure Reset is
         begin
            Random_User_Pkg.Reset(Gen);
         end Reset;
         function Get_Random_User return Integer is
         begin
            return Random_User_Pkg.Random(Gen);
         end Get_Random_User;
      end Random_Provider;

      -- Stan użytkownika (odpowiednik is_interested, finished oraz liczników atomowych)
      protected type User_State is
         procedure Set_Interested;
         procedure Try_Consume_Interest(Success: out Boolean);
         procedure Set_Finished;
         function Is_Finished return Boolean;
         procedure Inc_Sent;
         procedure Inc_Received;
         function Get_Sent return Integer;
         function Get_Received return Integer;
      private
         Interested : Boolean := False;
         Finished   : Boolean := False;
         N_Sent     : Integer := 0;
         N_Received : Integer := 0;
      end User_State;

      protected body User_State is
         procedure Set_Interested is
         begin
            Interested := True;
         end Set_Interested;

        procedure Try_Consume_Interest (Success : out Boolean) is
        begin
          if Interested then
              Interested := False;
              Success    := True;
          else
              Success    := False;
          end if;
        end Try_Consume_Interest;

         procedure Set_Finished is
         begin
            Finished := True;
         end Set_Finished;

         function Is_Finished return Boolean is
         begin
            return Finished;
         end Is_Finished;

         procedure Inc_Sent is
         begin
            N_Sent := N_Sent + 1;
         end Inc_Sent;

         procedure Inc_Received is
         begin
            N_Received := N_Received + 1;
         end Inc_Received;

         function Get_Sent return Integer is
         begin
            return N_Sent;
         end Get_Sent;

         function Get_Received return Integer is
         begin
            return N_Received;
         end Get_Received;
      end User_State;

      type User_State_Access is access all User_State;

      -- Deklaracje wyprzedzające dla zadań i wskaźników
      type User_Task;
      type User_Task_Access is access all User_Task;

      type User_Type is record
         Id      : Integer;
         Task_Id : User_Task_Access;
         State   : User_State_Access;
      end record;
      type User_Ptr is access all User_Type;
      type User_Array is array (0 .. N_Users - 1) of User_Ptr;
      type User_Array_Access is access all User_Array;

      -- Odpowiednik struktury kanałów dla User
      task type User_Task is
         entry Init(Id_Val : Integer; N_Users_Val : Integer; N_Messages_Val : Integer; State_Val : User_State_Access; Array_Val : User_Array_Access);
         entry Deliver(M : Message);
         entry Terminate_User;
         entry Allowed_To_Send;
         entry Get_Message(M : out Message);
         entry Message_Delivered;
      end User_Task;

      -- Zadanie serwera
      task type Server_Task is
         entry Init(Array_Val : User_Array_Access);
      end Server_Task;

      Users  : User_Array_Access;
      Server : access Server_Task;

      ---------- Implementacja Pętli Użytkownika ----------
      task body User_Task is
         Id               : Integer;
         N_Users_Count    : Integer;
         N_Messages_Limit : Integer;
         State            : User_State_Access;
         Users_Ref        : User_Array_Access;
         Untagged         : Boolean := False;
         Msg_Buffer       : Message;
      begin
         accept Init(Id_Val : Integer; N_Users_Val : Integer; N_Messages_Val : Integer; State_Val : User_State_Access; Array_Val : User_Array_Access) do
            Id               := Id_Val;
            N_Users_Count    := N_Users_Val;
            N_Messages_Limit := N_Messages_Val;
            State            := State_Val;
            Users_Ref        := Array_Val;
         end Init;

         State.Set_Interested;
         Sync_Print_Obj.Println(ASCII.HT & "User[" & Img(Id) & "] is interested");

         loop
            select
               -- case received := <-user.incoming
               accept Deliver(M : Message) do
                  State.Inc_Received;
                  Sync_Print_Obj.Println(ASCII.HT & "User[" & Img(Id) & "] received: """ & 
                     Ada.Strings.Unbounded.To_String(M.Msg) & """ from " & Img(M.Sender));
               end Deliver;
            or
               -- case <-user.terminate
               accept Terminate_User;
               exit;
            or
               -- Dwufazowy odpowiednik case <-user.can_send i user.outgoing <- msg
               accept Allowed_To_Send;
               declare
                  Rand_Target : constant Integer := Random_Provider.Get_Random_User;
               begin
                  Msg_Buffer := (Msg      => Ada.Strings.Unbounded.To_Unbounded_String("Hello"),
                                 Sender   => Id,
                                 Receiver => Rand_Target);
               end;
               Sync_Print_Obj.Println(ASCII.HT & "User[" & Img(Id) & "] sending message: """ & 
                  Ada.Strings.Unbounded.To_String(Msg_Buffer.Msg) & """ to: " & Img(Msg_Buffer.Receiver));
               
               accept Get_Message(M : out Message) do
                  M := Msg_Buffer;
               end Get_Message;
            or
               -- case <-user.is_delivered
               accept Message_Delivered;
               State.Inc_Sent;
               if State.Get_Sent >= N_Messages_Limit and then not Untagged then
                  Untagged := True;
                  State.Set_Finished;
               else
                  State.Set_Interested;
               end if;
            end select;
         end loop;
      end User_Task;

      ---------- Implementacja Pętli Serwera ----------
      task body Server_Task is
         Users_Ref : User_Array_Access;
         Last_Iter : Integer;
         Sender    : User_Ptr;
         Msg       : Message;
         Receiver  : User_Ptr;

         function Check_If_All_Finished return Boolean is
         begin
            for I in Users_Ref'Range loop
               if not Users_Ref(I).State.Is_Finished then
                  return False;
               end if;
            end loop;
            return True;
         end Check_If_All_Finished;

         -- Odpowiednik choose_user z nieblokującym selectem (Try_Consume_Interest)
         procedure Choose_User(Found_User : out User_Ptr; Updated_Iter : in out Integer) is
          Consumed: Boolean;
         begin
            Found_User := null;
            for K in Users_Ref'Range loop
               Updated_Iter := Updated_Iter + 1;
               if Updated_Iter > Users_Ref'Last then
                  Updated_Iter := Users_Ref'First;
               end if;

               Users_ref(Updated_Iter).State.Try_Consume_Interest(Consumed);
               if Consumed then
                  Found_User := Users_Ref(Updated_Iter);
                  return;
               end if;
            end loop;
         end Choose_User;

      begin
         accept Init(Array_Val : User_Array_Access) do
            Users_Ref := Array_Val;
         end Init;

         Last_Iter := Users_Ref'Last;

         loop
            Choose_User(Sender, Last_Iter);

            if Sender = null then
               if Check_If_All_Finished then
                 delay 0.002;
                 exit;
               end if;
               delay 0.0001; -- Odpowiednik runtime.Gosched()
            else
               Sync_Print_Obj.Println("[server] Chose user: " & Img(Sender.Id));
               
               -- sender.can_send <- true
               Sender.Task_Id.Allowed_To_Send;
               Sync_Print_Obj.Println("[server] Allowed user[" & Img(Sender.Id) & "] to send");
               
               -- msg := <-sender.outgoing
               Sender.Task_Id.Get_Message(Msg);
               Sync_Print_Obj.Println("[server] Forwarding message: """ & Ada.Strings.Unbounded.To_String(Msg.Msg) & """ to: " & Img(Msg.Receiver));

               -- receiver.incoming <- msg
               Receiver := Users_Ref(Msg.Receiver);
               Receiver.Task_Id.Deliver(Msg);

               -- sender.is_delivered <- true
               Sender.Task_Id.Message_Delivered;
               Sync_Print_Obj.Println("[server] Message delivered");
            end if;
         end loop;

         -- terminate_all(users)
         for I in Users_Ref'Range loop
            Sync_Print_Obj.Println("[server] Terminating: " & Img(Users_Ref(I).Id) & "...");
            Users_Ref(I).Task_Id.Terminate_User;
         end loop;

         Sync_Print_Obj.Println("[server] Finished");
         Barrier.Signal;
      end Server_Task;

   begin
      -- Inicjalizacja generatora losowego
      Random_Provider.Reset;

      -- Tworzenie użytkowników i ich zadań (odpowiednik go user_loop)
      Users := new User_Array;
      for I in Users'Range loop
         Users(I)         := new User_Type;
         Users(I).Id      := I;
         Users(I).State   := new User_State;
         Users(I).Task_Id := new User_Task;
      end loop;

      for I in Users'Range loop
         Users(I).Task_Id.Init(I, N_Users, N_Messages_Per_User, Users(I).State, Users);
      end loop;

      -- Uruchomienie serwera (odpowiednik go server)
      Server := new Server_Task;
      Server.Init(Users);

      -- Oczekiwanie na sygnał zakończenia od serwera (odpowiednik wg.Wait())
      Barrier.Wait;

      -- Wyświetlanie wyników końcowych
      Ada.Text_IO.Put_Line("--------- Results ---------");

      declare
         Min_Received   : Integer := Integer'Last;
         Max_Received   : Integer := 0;
         Total_Sent     : Integer := 0;
         Total_Received : Integer := 0;
         Curr_Recv      : Integer;
      begin
         for I in Users'Range loop
            Curr_Recv      := Users(I).State.Get_Received;
            Total_Received := Total_Received + Curr_Recv;
            Total_Sent     := Total_Sent + Users(I).State.Get_Sent;

            if Min_Received > Curr_Recv then
               Min_Received := Curr_Recv;
            end if;
            if Max_Received < Curr_Recv then
               Max_Received := Curr_Recv;
            end if;
         end loop;

         Ada.Text_IO.Put_Line("Total sent: " & Img(Total_Sent));
         Ada.Text_IO.Put_Line("Total received: " & Img(Total_Received));
         Ada.Text_IO.Put_Line("Min-max received messages: min=" & Img(Min_Received) & " max=" & Img(Max_Received));

         for I in Users'Range loop
           Ada.Text_IO.Put_Line(ASCII.HT & "User[" & Img(I)& "] received: " &
                Img(Users(I).State.Get_Received) & ", sent: " & Img(Users(I).State.Get_Sent));
         end loop;
      end;
   end;
end Main;

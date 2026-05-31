with Ada.Containers.Vectors;
with Ring;

package DHSetup is
   generic
      P : Positive;
   package Generic_DHSetup is
      type DHSetup_Type is private;

      function Create return DHSetup_Type;

      function Get_Generator (Item : DHSetup_Type) return Natural;

      function Power (Item : DHSetup_Type; A : Natural; B : Natural) return Natural;

   private
      type DHSetup_Type is record
         Gen : Natural;
      end record;

      package Natural_Vectors is new Ada.Containers.Vectors(Natural, Natural);
      use Natural_Vectors;

   end Generic_DHSetup;
end DHSetup;

with Ring;
with Ada.Containers.Vectors;

package DHSetup is
   generic
      P : Positive;
   package Generic_DH is
      type DH_Type is private;

      function Create return DH_Type;
      function Get_Generator (Dh : DH_Type) return Natural;
      function Power (Dh : DH_Type; A : Natural; B : Natural) return Natural;

   private
      type DH_Type is record
         Gen : Natural;
      end record;

      package Natural_Vectors is new Ada.Containers.Vectors (Natural, Natural);
      use Natural_Vectors;
   end Generic_DH;
end DHSetup;

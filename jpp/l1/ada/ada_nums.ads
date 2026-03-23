with Interfaces.C; use Interfaces.C;

package Ada_Nums is 
    function Gcd (A: int; B: int) return int;
    function Min_Divider (N: int) return int;
    function Totient (N: int) return int;

    type Diofant_Result is record
        X: int := 0;
        Y: int := 0;
    end record;
    pragma Convention (C, Diofant_Result);

    function Diofant (A0: int; B0: int; C0: int) return Diofant_Result;

    -- wrappers exported under ada_ prefix to avoid symbol collisions
    function Ada_Gcd (A: int; B: int) return int;
    function Ada_Min_Divider (N: int) return int;
    function Ada_Totient (N: int) return int;
    function Ada_Diofant (A0: int; B0: int; C0: int) return Diofant_Result;

    pragma Export (C, Ada_Gcd, "ada_gcd");
    pragma Export (C, Ada_Min_Divider, "ada_min_divider");
    pragma Export (C, Ada_Totient, "ada_totient");
    pragma Export (C, Ada_Diofant, "ada_diofant");
end Ada_Nums;
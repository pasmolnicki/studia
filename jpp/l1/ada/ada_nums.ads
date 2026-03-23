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

    pragma Export (C, Gcd, "gcd");
    pragma Export (C, Min_Divider, "min_divider");
    pragma Export (C, Totient, "totient");
    pragma Export (C, Diofant, "diofant");
end Ada_Nums;
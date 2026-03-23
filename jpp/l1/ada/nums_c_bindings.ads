with Interfaces.C; use Interfaces.C;

package Nums_C_Bindings is 
    pragma Pure;

    function Gcd (A: int; B: int) return int
        with Import => True, Convention => C, External_Name => "gcd";

    function Min_Divider (N: int) return int
        with Import => True, Convention => C, External_Name => "min_divider";

    function Totient (N: int) return int
        with Import => True, Convention => C, External_Name => "totient";

    type Diofant_Result is record
        X: int := 0;
        Y: int := 0;
    end record
        with Convention => C;

    function Diofant (A0: int; B0: int; C0: int) return Diofant_Result
        with Import => True, Convention => C, External_Name => "diofant";

end Nums_C_Bindings;

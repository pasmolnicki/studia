with Interfaces.C; use Interfaces.C;

package Nums_Rust_Bindings is 
    pragma Pure;

    -- Bindings to Rust wrappers (rust_ prefix)
    function Rust_Gcd (A: int; B: int) return int
        with Import => True, Convention => C, External_Name => "rust_gcd";

    function Rust_Min_Divider (N: int) return int
        with Import => True, Convention => C, External_Name => "rust_min_divider";

    function Rust_Totient (N: int) return int
        with Import => True, Convention => C, External_Name => "rust_totient";

    type Diofant_Result is record
        X: int := 0;
        Y: int := 0;
    end record
        with Convention => C;

    function Rust_Diofant (A0: int; B0: int; C0: int) return Diofant_Result
        with Import => True, Convention => C, External_Name => "rust_diofant";

end Nums_Rust_Bindings;

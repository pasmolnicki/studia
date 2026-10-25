
func = (x::Float64) -> sin(x) + cos(3.0 * x)
deriv = (x::Float64) -> cos(x) - 3.0 * sin(3.0 * x)
approx_d = (x::Float64, h::Float64) -> (func(x + h) - func(x)) / h

function run_exp()
  h_values::Array{Float64} = [2.0^(-i) for i in 0:54]
  x0::Float64 = 1.0
  for h in h_values
    println("h=$h deriv=$(deriv(x0)), approx=$(approx_d(x0, h)), err=$(abs(deriv(x0) - approx_d(x0, h)))")
  end
end

run_exp()

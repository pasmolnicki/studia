function f(x::Float64)
  return sqrt(x^2 + 1) - Float64(1.0)
end

function g(x::Float64)
  return x^2 / (sqrt(x^2 + 1) + 1)
end

function run_exp()
  neg_pow = 1
  x = 1.0 / 8.0
  while true
    println("(8^-$neg_pow) $x: f(x)=$(f(x)) g(x)=$(g(x))")
    if f(x) == 0.0 && g(x) == 0.0
      break
    end
    x /= 8.0
    neg_pow += 1
  end
end

run_exp()

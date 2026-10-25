function step(x::Float64)
  s = nextfloat(x) - x
  println("exponent=2^$(Int(log2(s)))")
  return s
end

println("[0.5, 1): ", step(0.5))
println("[1, 2): ", step(1.0))
println("[2, 4): ", step(2.0))

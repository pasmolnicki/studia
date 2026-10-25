function calc_macheps(::Type{T}) where {T <: AbstractFloat}
  prev = T(1.0)
  machesp = T(1.0)
  while T(1.0) + machesp > T(1.0)
    prev = machesp
    machesp = machesp / T(2.0)
  end
  return prev
end

println("macheps: ", calc_macheps(Float64), " ", eps(Float64))
println("macheps: ", calc_macheps(Float32), " ", eps(Float32))
println("macheps: ", calc_macheps(Float16), " ", eps(Float16))

function calc_eta(::Type{T}) where {T <: AbstractFloat}
  prev = T(1.0)
  eta = T(1.0)
  while eta > T(0.0)
    prev = eta
    eta = eta / T(2.0)
  end
  return prev
end

println("eta: ", calc_eta(Float64), " ", nextfloat(Float64(0.0)))
println("eta: ", calc_eta(Float32), " ", nextfloat(Float32(0.0)))
println("eta: ", calc_eta(Float16), " ", nextfloat(Float16(0.0)))

println("floatmin: ", floatmin(Float64))
println("floatmin: ", floatmin(Float32))

"""
Wyznacza najwięszką wartość reprezentowalną w podanym typie zmiennoprzecinkowym
Oblicza iteracyjnie największą taką liczbę n, że !isinf(n) && isinf(n * 2)
(liczba ma najwięszką możliwą cechę)
następnie dodaje kolejno n/2,n/4,n/8,... dopóki prawdą jest, że !isinf(n)
"""
function calc_max(::Type{T}) where {T <: AbstractFloat}
  prev = T(1.0)
  m = T(1.0)
  while !isinf(m)
    prev = m
    m = m * T(2.0)
  end

  non_inf = prev
  eta = prev / T(2.0)
  while eta > T(0.0) && !isinf(prev)
    non_inf = prev
    prev += eta
    eta = eta / T(2.0)
  end
  return non_inf
end

println("max: ", calc_max(Float64), " ", floatmax(Float64))
println("max: ", calc_max(Float32), " ", floatmax(Float32))
println("max: ", calc_max(Float16), " ", floatmax(Float16))

# println(bitstring(calc_max(Float16)), " ", bitstring(floatmax(Float16)))

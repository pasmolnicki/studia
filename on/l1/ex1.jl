function calc_macheps(::Type{T}) where {T <: AbstractFloat}
  prev = T(1.0)
  machesp = T(1.0)
  while T(1.0) + machesp > T(1.0)
    prev = machesp
    machesp = machesp / T(2.0)
  end
  return prev
end

function calc_eta(::Type{T}) where {T <: AbstractFloat}
  prev = T(1.0)
  eta = T(1.0)
  while eta > T(0.0)
    prev = eta
    eta = eta / T(2.0)
  end
  return prev
end

"""
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

function run_experiment(::Type{T}) where {T <: AbstractFloat }
  println()
  println("-"^40)
  println(nameof(T), " macheps: ", calc_macheps(T), " ", eps(T))
  println(nameof(T), " eta: ", calc_eta(T), " ", nextfloat(T(0.0)))
  println(nameof(T), " floatmin: ", floatmin(T))
  println(nameof(T), " max: ", calc_max(T), " ", floatmax(T))
end

run_experiment(Float64)
run_experiment(Float32)
run_experiment(Float16)
# println(bitstring(calc_max(Float16)), " ", bitstring(floatmax(Float16)))

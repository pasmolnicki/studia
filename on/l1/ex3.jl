function find()
  while true
    n = rand() + 1.0
    if n * Float64(1.0 / n) != Float64(1.0)
      return n
      break
    end
  end
end

function find_min()
  n = nextfloat(Float64(1.0))
  while n * Float64(1.0 / n) == Float64(1.0)
    n = nextfloat(n)
  end
  return n
end

exp = find()
println("experimental: ", exp, " ", bitstring(exp))
min = find_min()
println("min: ", min, " ", bitstring(min))

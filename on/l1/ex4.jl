function sum_forward(x::Array{T}, y::Array{T}) where {T <: AbstractFloat }
  sum = T(0.0)
  for i in eachindex(x)
    sum = sum + x[i] * y[i]
  end
  return sum
end

function sum_backward(x::Array{T}, y::Array{T}) where {T <: AbstractFloat }
  sum = T(0.0)
  for i in reverse(eachindex(x))
    sum = sum + x[i] * y[i]
  end
  return sum
end

function get_neg_pos_products(x::Array{T}, y::Array{T}) where { T <: AbstractFloat }
  results = [x[i] * y[i] for i in eachindex(x)]
  positive = filter(x -> x > 0, results)
  negative = filter(x -> x < 0, results)
  return (positive, negative)
end

function sum_array(x::Array{T}) where {T <: AbstractFloat}
  s = T(0.0)
  for v in x
    s = s + v
  end
  return s
end

function sum_decreasing(x::Array{T}, y::Array{T}) where {T <: AbstractFloat }
  positive, negative = get_neg_pos_products(x, y)
  sort!(positive, rev=true)
  sort!(negative)
  return sum_array(positive) + sum_array(negative)
end

function sum_increasing(x::Array{T}, y::Array{T}) where {T <: AbstractFloat }
  positive, negative = get_neg_pos_products(x, y)
  sort!(positive)
  sort!(negative, rev=true)
  return sum_array(negative) + sum_array(positive)
end

function do_experiment(x::Array{T}, y::Array{T}) where {T <: AbstractFloat}
  expected = T(-1.00657107000000e-11)
  println(nameof(T), ": sum forward=", sum_forward(x, y), " err=", abs(sum_forward(x, y) - expected)/abs(expected)*T(100.0))
  println(nameof(T), ": sum backward=", sum_backward(x, y), " err=", abs(sum_backward(x, y) - expected)/abs(expected)*T(100.0))
  println(nameof(T), ": sum max -> min=", sum_decreasing(x, y), " err=", abs(sum_decreasing(x, y) - expected)/abs(expected)*T(100.0))
  println(nameof(T), ": sum min -> max=", sum_increasing(x, y), " err=", abs(sum_increasing(x, y) - expected)/abs(expected)*T(100.0))
end

x1::Array{Float64} = [2.718281828, −3.141592654, 1.414213562, 0.5772156649, 0.3010299957]
y1::Array{Float64} = [1486.2497, 878366.9879, −22.37492, 4773714.647, 0.000185049]

x2::Array{Float32} = [2.718281828, −3.141592654, 1.414213562, 0.5772156649, 0.3010299957]
y2::Array{Float32} = [1486.2497, 878366.9879, −22.37492, 4773714.647, 0.000185049]

do_experiment(x1, y1)
do_experiment(x2, y2)

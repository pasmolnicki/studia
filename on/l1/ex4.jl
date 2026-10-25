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

function sum_decreasing(x::Array{T}, y::Array{T}) where {T <: AbstractFloat }
  sort!(x, rev=true)
  sort!(y, rev=true)

  sum = T(0.0)
  for i in eachindex(x)
    sum = sum + x[i] * y[i]
  end
  return sum
end

function sum_increasing(x::Array{T}, y::Array{T}) where {T <: AbstractFloat }
  sort!(x)
  sort!(y)

  sum = T(0.0)
  for i in eachindex(x)
    sum = sum + x[i] * y[i]
  end
  return sum
end

function do_experiment(x::Array{T}, y::Array{T}) where {T <: AbstractFloat}
  println(nameof(T), ": sum forward=", sum_forward(x, y))
  println(nameof(T), ": sum backward=", sum_backward(x, y))
  println(nameof(T), ": sum max -> min=", sum_decreasing(x, y))
  println(nameof(T), ": sum min -> max=", sum_increasing(x, y))
end

x1::Array{Float64} = [2.718281828, −3.141592654, 1.414213562, 0.5772156649, 0.3010299957]
y1::Array{Float64} = [1486.2497, 878366.9879, −22.37492, 4773714.647, 0.000185049]

x2::Array{Float32} = [2.718281828, −3.141592654, 1.414213562, 0.5772156649, 0.3010299957]
y2::Array{Float32} = [1486.2497, 878366.9879, −22.37492, 4773714.647, 0.000185049]

do_experiment(x1, y1)
do_experiment(x2, y2)

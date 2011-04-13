#!/usr/bin/env ruby
# encoding: utf-8

def gen_matrix(m, n)
  out = []
  m.times do |i|
    n.times do |j|
      out[i] ||= []
      out[i][j] = rand
    end
  end
  out
end

def gen_vector(n)
  out = []
  n.times {|j| out << rand}
  out
end

def gen_data(m, n)
  @a = gen_matrix(m, n)
  @x = gen_vector(n)
  @y = []
  @a.each_with_index do |row, i|
    row.each_with_index do |v, j|
      @y[i] ||= 0
      @y[i] += v*@x[j]
    end
  end
end

def error(d, y)
  mse = 0.0
  d.each_with_index do |d1, index|
    mse += (d1 - y[index])**2
  end
  mse/2.0
end

def learn(alpha = 0.0005, e_min = 0.001)
  w = []

  # Делаем все нулевые к-ты
  #@a.first.size.times {|i| w[i] = rand/100.0}
  w = gen_vector(@a.first.size)
  #puts "Начальный веса нейросети"
  #p w



  d = []
  iteration = 0
  mse = 1000000
  begin
    @a.each_with_index do |row, i|
      d[i] = 0
      row.each_with_index do |v, j|
        d[i] += v*w[j]
      end
      w.each_with_index do |wi, n|
        w[n] = wi - alpha*(d[i] - @y[i])*row[n]
      end
      #puts "Коррекция весов"
      #p w
    end
  iteration += 1
  mse = error(@y, d)
  puts "Итерация #{iteration}, ошибка: #{mse}"  if iteration % 10 == 0
  #p d
  end while mse > e_min #&& iteration < 10000
  puts "Всего итераций: #{iteration}"
  d
end

gen_data(1500, 1500)
#puts "A: "
#p @a
#puts "X: "
#p @x
#puts "Y: "
#p @y


answer = learn
#puts "Y: "
#p @y
#puts "Answer: "
#p answer

puts "MSE: "
p error(@y, answer)


#!/bin/ruby
stats={}

stats['0']=0
stats['1']=0
stats['2']=0
stats['3']=0
stats['4']=0
stats['5']=0
stats['6']=0
stats['7']=0
stats['8']=0
stats['9']=0

1000000.times do
  (rand*10**14).to_i.to_s.split('').each do |number|
    stats[number] = 1+stats[number]
  end
end

puts "Statistics: \n"
stats.sort.each do |num,val|
  puts "#{num}: #{val}"
end

puts "MAX:   #{stats.values.max}"
puts "MIN:   #{stats.values.min}"
puts "DELTA: #{stats.values.max - stats.values.min}"
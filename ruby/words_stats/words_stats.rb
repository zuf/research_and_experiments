# -*- coding: utf-8 -*-
###!/bin/ruby

$KCODE = 'u'
require 'jcode'

require 'rubygems'
require 'unicode'


chars={}
chars_count = 0
n=0
ARGF.each do |line|
    n=n+1
    chars_count += line.length
    #puts "#{n}: #{line}" #if line =~ /login/
    clean_line = Unicode.upcase(line.gsub(/[^A-Za-zА-Яа-я0-9 ]/, ''))
    #puts clean_line
    clean_line.each(' ') do |w|
      w=w.chop
      #if w.length > 10
        chars[w] = 0 if chars[w].nil?
        chars[w]+=1
      #end
    end
end

i=0
chars.sort{|a,b| b[1]<=>a[1]}.each do |k,v|
  percent = ((((v.to_f)/(chars_count.to_f))*100.0)*100.0).round/100.0 
  s=''
  k='\\' if k=="\n"
  (percent * 20).to_i.times {s += '*'}  
  puts "[#{k}]: #{percent}% #{s} (#{v})" if i<100
  i+=1
end

puts "\nСтрок статистики: #{chars.size}"
puts "Всего символов: #{chars_count}"

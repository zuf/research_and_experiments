#!/usr/bin/env ruby

require 'rubygems'
require 'gpx'
require 'exifr'

class Coord

  def initialize(s)
    a = s.split /\s/
    @deg = eval "#{a[0]}.0"
    @min = eval "#{a[1]}.0"
    @sec = eval "#{a[2]}.0"
  end
  
  attr_accessor :deg, :min, :sec

  def to_s
    "#{deg}deg #{min}' #{sec}\""
  end
  
  def to_f
    @deg
  end
end

include GPX
  
gpx_file = GPXFile.new
track_point_data = []
waypoint_data = []
route_point_data = []

ELEVATION = 18.5

ARGV.each do |param|
  Dir.glob(param).each do |f|
    #image = Exiv2::ImageFactory.open(f)
    #image.read_metadata
    #exif_data = image.exif_datexif.to_hash
    #lat = Coord.new exif_data['Exif.GPSInfo.GPSLatitude']
    #lon = Coord.new exif_data['Exif.GPSInfo.GPSLongitude']
    
    exif = EXIFR::JPEG.new f
    
    lat = exif.exif[0].gps_latitude[0].to_f + (exif.exif[0].gps_latitude[1].to_f / 60) + (exif.exif[0].gps_latitude[2].to_f / 3600)
    long = exif.exif[0].gps_longitude[0].to_f + (exif.exif[0].gps_longitude[1].to_f / 60) + (exif.exif[0].gps_longitude[2].to_f / 3600)
    long = long * -1 if exif.exif[0].gps_longitude_ref == "W"   # (W is -, E is +)
    lat = lat * -1 if exif.exif[0].gps_latitude_ref == "S"      # (N is +, S is -)
    
    time = exif.date_time_original
    #puts "#{f}: #{lat} | #{long} at #{time}"
    if lat!=0.0 && long != 0.0
      point = {:lat => lat, 
        :lon => long, 
        :time => time, 
        :elevation => ELEVATION}
      track_point_data << point
      waypoint_data << point
      route_point_data << point
    end
  end
end

track = Track.new(:name => "Unnamed track")
segment = Segment.new

track.segments << segment
gpx_file.tracks << track
#gpx_file.tracks << track

track_point_data.each do |trk_pt_hash|
  segment.points << TrackPoint.new(trk_pt_hash)
end

#waypoint_data.each do |wpt_hash|
#  gpx_file.waypoints << Waypoint.new(wpt_hash)
#end

#route = Route.new()
#  route_point_data.each do |rte_pt_hash|
#  route.points << Point.new(rte_pt_hash)
#end
#gpx_file.routes << route

#gpx_file.write "/tmp/gps1.gpx"
puts gpx_file.to_s

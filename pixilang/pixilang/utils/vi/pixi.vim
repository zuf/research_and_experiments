" Vim syntax file
" Language: pixilang file
" Maintainer: goglus <goglus@gmail.com>
" Last change: 2008 Dec 13 

if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

"COMMENTS
syn region pixiComment start="/\*" end="\*/" 
syn region pixiComment start="//" skip="\\$" end="$" 

"COMMANDS 
syn keyword pixiCommands clear frame dot line box fbox print fps set_screen save_screen pixi 
syn keyword pixiCommands make_pixi make_ascii_pixi new_pixi load_pixi remove_pixi resize_pixi clone_pixi pixi_alpha pixi_transp
syn keyword pixiCommands get_dot get_pixi_xsize get_pixi_ysize  get_pixi_frames get_pixi_alpha 
syn keyword pixiCommands get_window_xsize get_window_ysize get_color get_red get_green get_blue get_blend
syn keyword pixiCommands pixi_font get_pixi_font if goto stop halt exit noesc file_dialog 
syn keyword pixiCommands triangle pixi_triangle triangles3d  sort_triangles transp effector pixel_shader
syn keyword pixiCommands get_pen_x gpx get_pen_y gpy get_pen_region gpr get_pen_key gpk handle_hard_keys get_hard_key ghk
syn keyword pixiCommands video_export_gif video_export_avi pause_video_export resume_video_export 
syn keyword pixiCommands stop_video_export video_export_fps video_export_realtime
syn keyword pixiCommands sin cos csin ccos rand rand_seed
syn keyword pixiCommands sound_volume get_sound_status send_sound_to send_sound load_music play_music stop_music 
syn keyword pixiCommands music_volume get_music_pattern get_music_position

"OPERATIONS 
syn keyword pixiOperations   if  else while  ret  goto  go

"SOURCE 
syn keyword pixiSource INCLUDE
syn region pixiSource start="\"" end="\""

"MATCH 
syntax match   pixiNumber "\<\d[0-9_]*\(\.\d[0-9_]*\)\=\([Ee][+-]\=\d[0-9_]*\)\=\>"

if version >= 508 || !exists("did_pixi_syntax_inits")
  if version < 508
    let did_pixi_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif
  HiLink pixiCommands   	Statement
  HiLink pixiComment    	Comment
  HiLink pixiOperations         Special 
  HiLink pixiNumber             Type
  HiLink pixiSource             Todo
  delcommand HiLink
endif

let b:current_syntax = 'pixi'



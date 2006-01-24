" Vim syntax file
" Language:     SVN log file
" Maintainer:   Adam Lazur <adam@lazur.org>
" URL:          XXX
" Last Change:  $Date: 2005-04-16 23:27:24 -0400 (Sat, 16 Apr 2005) $

" 1) Place this file in ~/.vim/syntax
"
" 2a) Then add the following lines to ~/.vimrc
"
"       au BufNewFile,BufRead  monotone-log.* setf monotone
"
" 2b) Or for autodetection, make a ~/.vim/scripts.vim with the following:
"
"       if did_filetype()       " filetype already set...
"           finish              " ...then don't do these checks
"       endif
"       if getline(1) =~ /^-\{72}$/
"           setfiletype monotonelog
"       endif



" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
    syntax clear
elseif exists("b:current_syntax")
    finish
endif


syn match monotoneLogSeparator /^-\{72}$/ skipwhite skipnl skipempty nextgroup=monotoneRevision

syn cluster monotoneLogInfoLine contains=monotoneRevision,monotoneLogUser,@monotoneLogDate,monotoneLogLines,monotoneLogInfoSeparator

syn match monotoneRevision /[r:]\d\+/ contained

syn match monotoneLogUser /\S\+/ contained
syn match monotoneLogInfoSeparator /|/ skipwhite nextgroup=@monotoneLogInfoLine
syn cluster monotoneLogDate contains=monotoneLogDateStamp,monotoneLogDateHuman
syn match monotoneLogDateStamp /\d\{4}-\d\d-\d\d \d\d:\d\d:\d\d -\d\{4}/ skipwhite nextgroup=monotoneLogDateHuman
syn match monotoneLogDateHuman /(.\D\{3} \d\d \D\{3} \d\{4})/he=e-1,hs=s+1 skipwhite nextgroup=monotoneLogInfoSeparator
syn match monotoneLogLines /\d\+ lines\?/ skipnl nextgroup=monotoneChangedPaths,monotoneLogComment

" Changed paths highlighting
syn region monotoneChangedPaths start=/^Changed paths:$/ end=/^$/ transparent contains=monotoneChangeLine,monotoneChange nextgroup=monotoneLogComment

syn match monotoneChangeLine /^Changed paths:$/ skipwhite skipnl skipempty contained nextgroup=@monotoneChange
syn cluster monotoneChange contains=monotoneChangeAdd,monotoneChangeDel,monotoneChangeMod,monotoneChangeCopy
syn match monotoneChangeDel /^   D .\+$/ contained skipnl nextgroup=@monotoneChange
syn match monotoneChangeMod /^   M .\+$/ contained skipnl nextgroup=@monotoneChange
syn match monotoneChangeAdd /^   A [^ ]\+/ contained skipnl nextgroup=@monotoneChange
syn match monotoneChangeCopy /^   A [^ ]* (from [^)]\+)$/ contained skipnl nextgroup=@monotoneChange contains=monotoneRevision

" The comment is a region, we're cheating and swallowing the next
" monotoneLogSeparator, but we add a contains to it'll highlight okay (breaks
" comments containing monotoneLogSeparator... but that shouldn't be super common)
syn region monotoneLogComment start=/\n/ end=/^-\{72}$/ skipnl keepend nextgroup=monotoneRevision contains=monotoneLogSeparator


" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_monotonelog_syn_inits")
    if version < 508
        let did_monotonelog_syn_inits = 1
        command -nargs=+ HiLink hi link <args>
    else
        command -nargs=+ HiLink hi def link <args>
    endif

    HiLink monotoneLogSeparator      Comment
    HiLink monotoneLogInfoSeparator  Comment
    HiLink monotoneRevision          Label
    HiLink monotoneLogUser           Normal
    HiLink monotoneLogDateStamp      Comment
    HiLink monotoneLogDateHuman      Comment
    HiLink monotoneLogLines          Comment

    HiLink monotoneLogComment        Normal

    HiLink monotoneChangeLine        Comment
    HiLink monotoneChangeAdd         Identifier
    HiLink monotoneChangeDel         WarningMsg
    HiLink monotoneChangeMod         PreProc
    HiLink monotoneChangeCopy        SpecialKey


    delcommand HiLink
endif

let b:current_syntax = "monotonelog"

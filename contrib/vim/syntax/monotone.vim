" Vim syntax file
" Language:	SVN commit file
" Maintainer:	Ben Collins <bcollins@debian.org>
" URL:		XXX
" Last Change:	Tue Oct 22 00:22:19 EDT 2002

" Based on the similar CVS commit file syntax

" Place this file as ~/.vim/syntax/monotone.vim
"
" Then add the following lines to ~/.vimrc
"
" au BufNewFile,BufRead  monotone-commit.* setf monotone

" For version 5.x: Clear all syntax items
" For version 6.x: Quit when a syntax file was already loaded
if version < 600
	syntax clear
elseif exists("b:current_syntax")
	finish
endif

syn cluster monotoneChange contains=monotoneAdd,monotoneDel,monotoneMod,monotoneProp
syn match monotoneLine /^--This line, and those below, will be ignored--$/ skipwhite skipnl skipempty nextgroup=@monotoneChange
syn match monotoneAdd /^A[M ]   .*$/ contained skipwhite skipnl skipempty nextgroup=@monotoneChange 
syn match monotoneDel /^D[M ]   .*$/ contained skipwhite skipnl skipempty nextgroup=@monotoneChange 
syn match monotoneMod /^M[M ]   .*$/ contained skipwhite skipnl skipempty nextgroup=@monotoneChange 
syn match monotoneProp /^_[M ]   .*$/ contained skipwhite skipnl skipempty nextgroup=@monotoneChange 

"The following is the old SVN template format markings
"
"syn region monotoneLine start="^SVN:" end="$" contains=monotoneAdd,monotoneDel,monotoneMod
"syn match monotoneAdd   contained "   [A_][ A]   .*"
"syn match monotoneDel   contained "   [D_][ D]   .*"
"syn match monotoneMod   contained "   [M_][ M]   .*"

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_monotone_syn_inits")
	if version < 508
		let did_monotone_syn_inits = 1
		command -nargs=+ HiLink hi link <args>
	else
		command -nargs=+ HiLink hi def link <args>
	endif

	HiLink monotoneAdd		Structure
	HiLink monotoneDel		SpecialChar
	HiLink monotoneMod		PreProc
	HiLink monotoneProp		Keyword
	HiLink monotoneLine		Comment

	delcommand HiLink
endif

let b:current_syntax = "monotone"

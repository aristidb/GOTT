" vim600: set foldmethod=marker:
"
" $Id: monotonecommand.vim 29 2005-04-19 00:22:27Z laz $
"
" Vim plugin to assist in working with MT-controlled files.
"
" Last Change:   $Date: 2005-04-18 20:22:27 -0400 (Mon, 18 Apr 2005) $
" Version:       1.67.3
" Maintainer:    Adam Lazur <adam@lazur.org>
" License:       This file is placed in the public domain.
" Credits:       Bob Hiestand and all the cvscommand.vim credits.
"
"                Various people who've emailed with valuable feedback about
"                monotonecommand.vim.
"
" Config Note:   I chose not to deviate from the cvscommand.vim defaults, but
"                I do recommend tweaking the user variables. I personally use:
"
"                let MTCommandEnableBufferSetup=1
"                let MTCommandCommitOnWrite=1
"                let MTCommandEdit='split'
"                let MTCommandNameResultBuffers=1
"                let MTCommandAutoSVK='svk'
"
" Section: Documentation {{{1
"
" This is mostly a s/CVS/MT/g on cvscommand.vim by Bob Hiestand
" <bob@hiestandfamily.org> with some monotone translations for various things.
" Most notable removals are support for cvs edit and watching, both of which
" subversion does not support.
"
" Provides functions to invoke various MT commands on the current file.
" The output of the commands is captured in a new scratch window.  For
" convenience, if the functions are invoked on a MT output window, the
" original file is used for the monotone operation instead after the window is
" split.  This is primarily useful when running MTCommit and you need to see
" the changes made, so that MTDiff is usable and shows up in another window.
"
" Command documentation {{{2
"
" MTAdd           Performs "monotone add" on the current file.
"
" MTAnnotate      Performs "monotone annotate" on the current file.  If an
"                  argument is given, the argument is used as a revision
"                  number to display.  If not given an argument, it uses the
"                  most recent version of the file on the current branch.
"                  Additionally, if the current buffer is a MTAnnotate buffer
"                  already, the version number just prior to the one on the
"                  current line is used.  This allows one to navigate back to
"                  examine the previous version of a line.
"
" MTCommit        This is a two-stage command.  The first step opens a buffer to
"                  accept a log message.  When that buffer is written, it is
"                  automatically closed and the file is committed using the
"                  information from that log message.  If the file should not be
"                  committed, just destroy the log message buffer without writing
"                  it.
"
" MTDiff          With no arguments, this performs "monotone diff" on the current
"                  file.  With one argument, "monotone diff" is performed on the
"                  current file against the specified revision.  With two
"                  arguments, monotone diff is performed between the specified
"                  revisions of the current file.  This command uses the
"                  'MTCommandDiffOpt' variable to specify diff options.  If
"                  that variable does not exist, then '' is assumed.  If
"                  you wish to have no options, then set it to the empty
"                  string.
"
" MTGotoOriginal  Returns the current window to the source buffer if the
"                  current buffer is a MT output buffer.
"
" MTLog           Performs "monotone log" on the current file.
"
" MTRevert        Replaces the modified version of the current file with the
"                  most recent version from the repository.
"
" MTReview        Retrieves a particular version of the current file.  If no
"                  argument is given, the most recent version of the file on
"                  the current branch is retrieved.  The specified revision is
"                  retrieved into a new buffer.
"
" MTStatus        Performs "monotone status -v" on the current file.
"
" MTStatus        Performs "monotone info" on the current file.
"
" MTUpdate        Performs "monotone update" on the current file.
"
" MTVimDiff       With no arguments, this prompts the user for a revision and
"                  then uses vimdiff to display the differences between the
"                  current file and the specified revision.  If no revision is
"                  specified, the most recent version of the file on the
"                  current branch is used.  With one argument, that argument
"                  is used as the revision as above.  With two arguments, the
"                  differences between the two revisions is displayed using
"                  vimdiff.
"
"                  With either zero or one argument, the original buffer is used
"                  to perform the vimdiff.  When the other buffer is closed, the
"                  original buffer will be returned to normal mode.
" MTCommitDiff
"               Will parse the commit buffer (that should be autogenerated by
"               monotone), and split the window with a corresponding diff. It is
"               highly convenient to review a diff when writing the log
"               message.
"
"               You may want to setup an autocommand in your vimrc like so:
"
"                   au BufNewFile,BufRead  monotone-commit.* setf monotone
"                   au FileType monotone map <Leader>sd :MTCommitDiff<CR>
"
"
" Mapping documentation: {{{2
"
" By default, a mapping is defined for each command.  User-provided mappings
" can be used instead by mapping to <Plug>CommandName, for instance:
"
" nnoremap ,ca <Plug>MTAdd
"
" The default mappings are as follow:
"
"   <Leader>sa MTAdd
"   <Leader>sn MTAnnotate
"   <Leader>sc MTCommit
"   <Leader>sd MTDiff
"   <Leader>sg MTGotoOriginal
"   <Leader>sG MTGotoOriginal!
"   <Leader>sl MTLog
"   <Leader>sr MTReview
"   <Leader>ss MTStatus
"   <Leader>si MTInfo
"   <Leader>su MTUpdate
"   <Leader>sv MTVimDiff
"
" Options documentation: {{{2
"
" Several variables are checked by the script to determine behavior as follow:
"
" MTCommandCommitOnWrite
"   This variable, if set to a non-zero value, causes the pending monotone commit
"   to take place immediately as soon as the log message buffer is written.
"   If set to zero, only the MTCommit mapping will cause the pending commit
"   to occur.  If not set, it defaults to 1.
"
" MTCommandDeleteOnHide
"   This variable, if set to a non-zero value, causes the temporary MT result
"   buffers to automatically delete themselves when hidden.
"
" MTCommandDiffOpt
"   This variable, if set, determines the options passed to the diff command
"   of MT.  If not set, it defaults to 'wbBc'.
"
" MTCommandDiffSplit
"   This variable overrides the MTCommandSplit variable, but only for buffers
"   created with MTVimDiff.
"
" MTCommandEdit
"   This variable controls whether the original buffer is replaced ('edit') or
"   split ('split').  If not set, it defaults to 'edit'.
"
" MTCommandEnableBufferSetup
"   This variable, if set to a non-zero value, activates MT buffer management
"   mode.  This mode means that two buffer variables, 'MTRevision' and
"   'MTBranch', are set if the file is MT-controlled.  This is useful for
"   displaying version information in the status bar.
"
" MTCommandInteractive
"   This variable, if set to a non-zero value, causes appropriate functions (for
"   the moment, only MTReview) to query the user for a revision to use
"   instead of the current revision if none is specified.
"
" MTCommandNameMarker
"   This variable, if set, configures the special attention-getting characters
"   that appear on either side of the monotone buffer type in the buffer name.
"   This has no effect unless 'MTCommandNameResultBuffers' is set to a true
"   value.  If not set, it defaults to '_'.  
"
" MTCommandNameResultBuffers
"   This variable, if set to a true value, causes the monotone result buffers to be
"   named in the old way ('<source file name> _<monotone command>_').  If not set
"   or set to a false value, the result buffer is nameless.
"
" MTCommandSplit
"   This variable controls the orientation of the various window splits that
"   may occur (such as with MTVimDiff, when using a MT command on a MT
"   command buffer, or when the 'MTCommandEdit' variable is set to 'split'.
"   If set to 'horizontal', the resulting windows will be on stacked on top of
"   one another.  If set to 'vertical', the resulting windows will be
"   side-by-side.  If not set, it defaults to 'horizontal' for all but
"   MTVimDiff windows.
"
" MTCommandAutoSVK
"   This variable is a hack to allow monotonecommand.vim to work for basic svk
"   operations. It MUST be used in combination with
"   MTCommandEnableBufferSetup. During buffer setup, the script will stat
"   .monotone/entries, and if it doesn't exist, it sets the monotone executable name
"   (b:MTCommandMTExec) to the value of MTCommandAutoSVK. This hack works
"   only because of the similarity in commandlines between monotone and svk.
"
" Event documentation {{{2
"   For additional customization, monotonecommand.vim uses User event autocommand
"   hooks.  Each event is in the MTCommand group, and different patterns
"   match the various hooks.
"
"   For instance, the following could be added to the vimrc to provide a 'q'
"   mapping to quit a MT buffer:
"
"   augroup MTCommand
"     au MTCommand User MTBufferCreated silent! nmap <unique> <buffer> q:bwipeout<cr> 
"   augroup END
"
"   The following hooks are available:
"
"   MTBufferCreated           This event is fired just after a monotone command
"                              result buffer is created and filled with the
"                              result of a monotone command.  It is executed within
"                              the context of the new buffer.
"
"   MTBufferSetup             This event is fired just after MT buffer setup
"                              occurs, if enabled.
"
"   MTPluginInit              This event is fired when the MTCommand plugin
"                              first loads.
"
"   MTPluginFinish            This event is fired just after the MTCommand
"                              plugin loads.
"
"   MTVimDiffFinish           This event is fired just after the MTVimDiff
"                              command executes to allow customization of,
"                              for instance, window placement and focus.
"
" Section: Plugin header {{{1
" loaded_monotonecommand is set to 1 when the initialization begins, and 2 when it
" completes.  This allows various actions to only be taken by functions after
" system initialization.

if exists("loaded_monotonecommand")
   finish
endif
let loaded_monotonecommand = 1

" Section: Event group setup {{{1

augroup MTCommand
augroup END

" Section: Plugin initialization {{{1
silent do MTCommand User MTPluginInit


" Section: Utility functions {{{1

" Function: s:MTResolveLink() {{{2
" Fully resolve the given file name to remove shortcuts or symbolic links.

function! s:MTResolveLink(fileName)
  let resolved = resolve(a:fileName)
  if resolved != a:fileName
    let resolved = s:MTResolveLink(resolved)
  endif
  return resolved
endfunction

" Function: s:MTChangeToCurrentFileDir() {{{2
" Go to the directory in which the current MT-controlled file is located.
" If this is a MT command buffer, first switch to the original file.

function! s:MTChangeToCurrentFileDir(fileName)
  let oldCwd=getcwd()
  let fileName=s:MTResolveLink(a:fileName)
  let newCwd=fnamemodify(fileName, ':h')
  if strlen(newCwd) > 0
    execute 'cd' escape(newCwd, ' ')
  endif
  return oldCwd
endfunction

" Function: s:MTGetOption(name, default) {{{2
" Grab a user-specified option to override the default provided.  Options are
" searched in the window, buffer, then global spaces.

function! s:MTGetOption(name, default)
  if exists("w:" . a:name)
    execute "return w:".a:name
  elseif exists("b:" . a:name)
    execute "return b:".a:name
  elseif exists("g:" . a:name)
    execute "return g:".a:name
  else
    return a:default
  endif
endfunction

" Function: s:MTEditFile(name, origBuffNR) {{{2
" Wrapper around the 'edit' command to provide some helpful error text if the
" current buffer can't be abandoned.  If name is provided, it is used;
" otherwise, a nameless scratch buffer is used.
" Returns: 0 if successful, -1 if an error occurs.

function! s:MTEditFile(name, origBuffNR)
  "Name parameter will be pasted into expression.
  let name = escape(a:name, ' *?\')

  let v:errmsg = ""
  let editCommand = s:MTGetOption('MTCommandEdit', 'edit')
  if editCommand != 'edit'
    if s:MTGetOption('MTCommandSplit', 'horizontal') == 'horizontal'
      if name == ""
        let editCommand = 'rightbelow new'
      else
        let editCommand = 'rightbelow split ' . name
      endif
    else
      if name == ""
        let editCommand = 'vert rightbelow new'
      else
        let editCommand = 'vert rightbelow split ' . name
      endif
    endif
  else
    if name == ""
      let editCommand = 'enew'
    else
      let editCommand = 'edit ' . name
    endif
  endif

  " Protect against useless buffer set-up
  let g:MTCommandEditFileRunning = 1
  execute editCommand
  unlet g:MTCommandEditFileRunning

  if v:errmsg != ""
    if &modified && !&hidden
      echoerr "Unable to open command buffer because 'nohidden' is set and the current buffer is modified (see :help 'hidden')."
    else
      echoerr "Unable to open command buffer" v:errmsg
    endif
    return -1
  endif
  let b:MTOrigBuffNR=a:origBuffNR
  let b:MTCommandEdit='split'
endfunction

" Function: s:MTCreateCommandBuffer(cmd, cmdName, statusText, filename) {{{2
" Creates a new scratch buffer and captures the output from execution of the
" given command.  The name of the scratch buffer is returned.

function! s:MTCreateCommandBuffer(cmd, cmdName, statusText, origBuffNR)
  let fileName=bufname(a:origBuffNR)

  let resultBufferName=''

  if s:MTGetOption("MTCommandNameResultBuffers", 0)
    let nameMarker = s:MTGetOption("MTCommandNameMarker", '_')
    if strlen(a:statusText) > 0
      let bufName=a:cmdName . ' -- ' . a:statusText
    else
      let bufName=a:cmdName
    endif
    let bufName=fileName . ' ' . nameMarker . bufName . nameMarker
    let counter=0
    let resultBufferName = bufName
    while buflisted(resultBufferName)
      let counter=counter + 1
      let resultBufferName=bufName . ' (' . counter . ')'
    endwhile
  endif

  let monotoneCommand = s:MTGetOption("MTCommandMTExec", "monotone") . " " . a:cmd
  let monotoneOut = system(monotoneCommand)
  if strlen(monotoneOut) == 0
    " Handle case of no output.  In this case, it is important to check the
    " file status, especially since monotone edit/unedit may change the attributes
    " of the file with no visible output.

    echomsg "No output from MT command"
    checktime
    return -1
  endif

  if s:MTEditFile(resultBufferName, a:origBuffNR) == -1
    return -1
  endif

  set buftype=nofile
  set noswapfile
  set filetype=

  if s:MTGetOption("MTCommandDeleteOnHide", 0)
    set bufhidden=delete
  endif

  silent 0put=monotoneOut

  " The last command left a blank line at the end of the buffer.  If the
  " last line is folded (a side effect of the 'put') then the attempt to
  " remove the blank line will kill the last fold.
  "
  " This could be fixed by explicitly detecting whether the last line is
  " within a fold, but I prefer to simply unfold the result buffer altogether.

  if has('folding')
    normal zR
  endif

  $d
  1

  " Define the environment and execute user-defined hooks.

  let b:MTSourceFile=fileName
  let b:MTCommand=a:cmdName
  if a:statusText != ""
    let b:MTStatusText=a:statusText
  endif

  silent do MTCommand User MTBufferCreated
  return bufnr("%")
endfunction

" Function: s:MTBufferCheck(monotoneBuffer) {{{2
" Attempts to locate the original file to which MT operations were applied
" for a given buffer.

function! s:MTBufferCheck(monotoneBuffer)
  let origBuffer = getbufvar(a:monotoneBuffer, "MTOrigBuffNR")
  if origBuffer
    if bufexists(origBuffer)
      return origBuffer
    else
      " Original buffer no longer exists.
      return -1 
    endif
  else
    " No original buffer
    return a:monotoneBuffer
  endif
endfunction

" Function: s:MTCurrentBufferCheck() {{{2
" Attempts to locate the original file to which MT operations were applied
" for the current buffer.

function! s:MTCurrentBufferCheck()
  return s:MTBufferCheck(bufnr("%"))
endfunction

" Function: s:MTToggleDeleteOnHide() {{{2
" Toggles on and off the delete-on-hide behavior of MT buffers

function! s:MTToggleDeleteOnHide()
  if exists("g:MTCommandDeleteOnHide")
    unlet g:MTCommandDeleteOnHide
  else
    let g:MTCommandDeleteOnHide=1
  endif
endfunction

" Function: s:MTDoCommand(monotonecmd, cmdName, statusText) {{{2
" General skeleton for MT function execution.
" Returns: name of the new command buffer containing the command results

function! s:MTDoCommand(cmd, cmdName, statusText)
  let monotoneBufferCheck=s:MTCurrentBufferCheck()
  if monotoneBufferCheck == -1 
    echo "Original buffer no longer exists, aborting."
    return -1
  endif

  let fileName=bufname(monotoneBufferCheck)
  let realFileName = fnamemodify(s:MTResolveLink(fileName), ':t')
  let oldCwd=s:MTChangeToCurrentFileDir(fileName)
  let fullCmd = a:cmd . ' "' . realFileName . '"'
  let resultBuffer=s:MTCreateCommandBuffer(fullCmd, a:cmdName, a:statusText, monotoneBufferCheck)
  execute 'cd' escape(oldCwd, ' ')
  return resultBuffer
endfunction


" Function: s:MTGetStatusVars(revision, branch, repository) {{{2
"
" Obtains a MT revision number and branch name.  The 'revisionVar',
" 'branchVar'and 'repositoryVar' arguments, if non-empty, contain the names of variables to hold
" the corresponding results.
"
" Returns: string to be exec'd that sets the multiple return values.

function! s:MTGetStatusVars(revisionVar, branchVar, repositoryVar)
  let monotoneBufferCheck=s:MTCurrentBufferCheck()
  if monotoneBufferCheck == -1 
    return ""
  endif
  let fileName=bufname(monotoneBufferCheck)
  let realFileName = fnamemodify(s:MTResolveLink(fileName), ':t')
  let oldCwd=s:MTChangeToCurrentFileDir(fileName)

  let monotoneCommand = s:MTGetOption("MTCommandMTExec", "monotone") . " info " . escape(realFileName, ' *?\')
  let statustext=system(monotoneCommand)
  if(v:shell_error)
    execute 'cd' escape(oldCwd, ' ')
    return ""
  endif
  let revision=substitute(statustext, '^\_.*Revision:\s*\(\d\+\)\_.*', '\1', "")

  " We can still be in a MT-controlled directory without this being a MT
  " file
  if match(revision, '^New file!$') >= 0
    let revision="NEW"
  elseif match(revision, '^\d\+$') >=0
  else
    execute 'cd' escape(oldCwd, ' ')
    return ""
  endif

  let returnExpression = "let " . a:revisionVar . "='" . revision . "'"

  if a:repositoryVar != ""
     let repository=substitute(statustext, '^\_.*URL:\s*\([^\s]\+\)\_.*$', '\1', "")
     let returnExpression=returnExpression . " | let " . a:repositoryVar . "='" . repository . "'"
  endif

  execute 'cd' escape(oldCwd, ' ')
  return returnExpression
endfunction

" Function: s:MTSetupBuffer() {{{2
" Attempts to set the b:MTBranch, b:MTRevision and b:MTRepository variables.

function! s:MTSetupBuffer()
  if (exists("b:MTBufferSetup") && b:MTBufferSetup)
    return
  endif

  if !s:MTGetOption("MTCommandEnableBufferSetup", 0)
        \ || @% == ""
        \ || exists("g:MTCommandEditFileRunning")
        \ || exists("b:MTOrigBuffNR")
    unlet! b:MTRevision
    unlet! b:MTBranch
    unlet! b:MTRepository
    return
  endif

  if !filereadable(expand("%"))
    return -1
  endif

  " check for MTCommandAutoSVK to do evil overriding
  if s:MTGetOption("MTCommandAutoSVK", 'unset') != 'unset'
    " The first thing we do is check for a .monotone file, if it's not there, then
    " we go into svk mode MUAHAHAHAHA
    if !filereadable('.monotone/entries')
      let b:MTCommandMTExec = s:MTGetOption("MTCommandAutoSVK", 'svk')
    endif
  endif

  let revision=""
  let branch=""
  let repository=""

  exec s:MTGetStatusVars('revision', 'branch', 'repository')
  if revision != ""
    let b:MTRevision=revision
  else
    unlet! b:MTRevision
  endif
  if branch != ""
    let b:MTBranch=branch
  else
    unlet! b:MTBranch
  endif
  if repository != ""
     let b:MTRepository=repository
  else
     unlet! b:MTRepository
  endif
  silent do MTCommand User MTBufferSetup
  let b:MTBufferSetup=1
endfunction

" Function: s:MTMarkOrigBufferForSetup(monotonebuffer) {{{2
" Resets the buffer setup state of the original buffer for a given MT buffer.
" Returns:  The MT buffer number in a passthrough mode.

function! s:MTMarkOrigBufferForSetup(monotoneBuffer)
  if a:monotoneBuffer != -1
    let origBuffer = s:MTBufferCheck(a:monotoneBuffer)
    "This should never not work, but I'm paranoid
    if origBuffer != a:monotoneBuffer
      call setbufvar(origBuffer, "MTBufferSetup", 0)
    endif
  endif
  return a:monotoneBuffer
endfunction

" Section: Public functions {{{1

" Function: MTGetRevision() {{{2
" Global function for retrieving the current buffer's MT revision number.
" Returns: Revision number or an empty string if an error occurs.

function! MTGetRevision()
  let revision=""
  exec s:MTGetStatusVars('revision', '', '')
  return revision
endfunction

" Function: MTDisableBufferSetup() {{{2
" Global function for deactivating the buffer autovariables.

function! MTDisableBufferSetup()
  let g:MTCommandEnableBufferSetup=0
  silent! augroup! MTCommandPlugin
endfunction

" Function: MTEnableBufferSetup() {{{2
" Global function for activating the buffer autovariables.

function! MTEnableBufferSetup()
  let g:MTCommandEnableBufferSetup=1
  augroup MTCommandPlugin
    au!
    au BufEnter * call s:MTSetupBuffer()
  augroup END

  " Only auto-load if the plugin is fully loaded.  This gives other plugins a
  " chance to run.
  if g:loaded_monotonecommand == 2
    call s:MTSetupBuffer()
  endif
endfunction

" Function: MTGetStatusLine() {{{2
" Default (sample) status line entry for MT files.  This is only useful if
" MT-managed buffer mode is on (see the MTCommandEnableBufferSetup variable
" for how to do this).

function! MTGetStatusLine()
  if exists('b:MTSourceFile')
    " This is a result buffer
    let value='[' . b:MTCommand . ' ' . b:MTSourceFile
    if exists('b:MTStatusText')
      let value=value . ' ' . b:MTStatusText
    endif
    let value = value . ']'
    return value
  endif

  if exists('b:MTRevision')
        \ && b:MTRevision != ''
        \ && exists('b:MTRepository')
        \ && b:MTRepository != ''
        \ && exists('g:MTCommandEnableBufferSetup')
        \ && g:MTCommandEnableBufferSetup
    return '[MT ' . b:MTRevision . '|' . b:MTRepository . ']'
  else
    return ''
  endif
endfunction

" Section: MT command functions {{{1

" Function: s:MTAdd() {{{2
function! s:MTAdd()
  return s:MTMarkOrigBufferForSetup(s:MTDoCommand('add', 'monotoneadd', ''))
endfunction

" Function: s:MTAnnotate(...) {{{2
function! s:MTAnnotate(...)
  let monotoneBufferCheck=s:MTCurrentBufferCheck()
  if monotoneBufferCheck == -1 
    echo "Original buffer no longer exists, aborting."
    return -1
  endif

  let fileName=bufname(monotoneBufferCheck)
  let realFileName = fnamemodify(s:MTResolveLink(fileName), ':t')
  let oldCwd=s:MTChangeToCurrentFileDir(fileName)

  let currentLine=line(".")

  if a:0 == 0
    " we already are in a MT Annotate buffer
    if &filetype == "MTAnnotate"
      let revision = substitute(getline("."),'\(^[0-9.]*\).*','\1','')
      let revmin = substitute(revision,'^[0-9.]*\.\([0-9]\+\)','\1','') + 0 -1 
      let revmaj = substitute(revision,'^\([0-9.]*\)\.[0-9]\+','\1','')
      if revmin == 0
        " Jump to ancestor branch
        let revision = substitute(revmaj,'^\([0-9.]*\)\.[0-9]\+','\1','')
      else
        let revision=revmaj . "." .  revmin
      endif
    else
      let revision=MTGetRevision()
      if revision == ""
        echo "Unable to obtain status for " . fileName
        execute 'cd' escape(oldCwd, ' ')
        return -1
      endif
    endif
  else
    let revision=a:1
  endif

  if revision == "NEW"
    echo "No annotatation available for new file " . fileName
    execute 'cd' escape(oldCwd, ' ')
    return -1
  endif

  let resultBuffer=s:MTCreateCommandBuffer('annotate -r ' . revision . ' "' . realFileName . '"', 'monotoneannotate', revision, monotoneBufferCheck) 
  if resultBuffer !=  -1
    exec currentLine
    set filetype=MTAnnotate
  endif

  execute 'cd' escape(oldCwd, ' ')
  return resultBuffer
endfunction

" Function: s:MTCommit() {{{2
function! s:MTCommit()
  let monotoneBufferCheck=s:MTCurrentBufferCheck()
  if monotoneBufferCheck ==  -1
    echo "Original buffer no longer exists, aborting."
    return -1
  endif

  " Protect against windows' backslashes in paths.  They confuse exec'd
  " commands.

  let shellSlashBak = &shellslash
  set shellslash

  let messageFileName = tempname()

  let fileName=bufname(monotoneBufferCheck)
  let realFilePath=s:MTResolveLink(fileName)
  let newCwd=fnamemodify(realFilePath, ':h')
  let realFileName=fnamemodify(realFilePath, ':t')

  if s:MTEditFile(messageFileName, monotoneBufferCheck) == -1
    let &shellslash = shellSlashBak
    return
  endif

  " Protect against case and backslash issues in Windows.
  let autoPattern = '\c' . messageFileName

  " Ensure existance of group
  augroup MTCommit
  augroup END

  execute 'au MTCommit BufDelete' autoPattern 'call delete("' . messageFileName . '")'
  execute 'au MTCommit BufDelete' autoPattern 'au! MTCommit * ' autoPattern

  " Create a commit mapping.  The mapping must clear all autocommands in case
  " it is invoked when MTCommandCommitOnWrite is active, as well as to not
  " invoke the buffer deletion autocommand.

  execute 'nnoremap <silent> <buffer> <Plug>MTCommit '.
        \ ':au! MTCommit * ' . autoPattern . '<CR>'.
        \ ':update<CR>'.
        \ ':call <SID>MTFinishCommit("' . messageFileName . '",' .
        \                             '"' . newCwd . '",' .
        \                             '"' . realFileName . '",' .
        \                             monotoneBufferCheck . ')<CR>'

  silent put='--This line, and those below, will be ignored--'

  let fullname = newCwd 
  if strlen(newCwd) > 0
    fullname = fullname . "/"
  fullname = fullname . realFileName
  let monotoneCommand = s:MTGetOption("MTCommandMTExec", "monotone") . " status " . escape(fullname, ' *?\')
  let statustext=system(monotoneCommand)

  silent put=statustext

  if s:MTGetOption('MTCommandCommitOnWrite', 1) == 1
    execute 'au MTCommit BufWritePost' autoPattern 'call s:MTFinishCommit("' . messageFileName . '", "' . newCwd . '", "' . realFileName . '", ' . monotoneBufferCheck . ') | au! * ' autoPattern
  endif

  0
  let b:MTSourceFile=fileName
  let b:MTCommand='MTCommit'
  set filetype=monotone
  let &shellslash = shellSlashBak
endfunction

" Function: s:MTDiff(...) {{{2
function! s:MTDiff(...)
  if a:0 == 1
    let revOptions = '-r' . a:1
    let caption = a:1 . ' -> current'
  elseif a:0 == 2
    let revOptions = '-r' . a:1 . ':' . a:2
    let caption = a:1 . ' -> ' . a:2
  else
    let revOptions = ''
    let caption = ''
  endif

  let monotonediffopt=s:MTGetOption('MTCommandDiffOpt', '')

  if monotonediffopt == ""
    let diffoptionstring=""
  else
    let diffoptionstring=" -" . monotonediffopt . " "
  endif

  let resultBuffer = s:MTDoCommand('diff ' . diffoptionstring . revOptions , 'monotonediff', caption)
  if resultBuffer != -1 
    set filetype=diff
  endif
  return resultBuffer
endfunction

" Function: s:MTGotoOriginal(["!]) {{{2
function! s:MTGotoOriginal(...)
  let origBuffNR = s:MTCurrentBufferCheck()
  if origBuffNR > 0
    execute 'buffer' origBuffNR
    if a:0 == 1
      if a:1 == "!"
        let buffnr = 1
        let buffmaxnr = bufnr("$")
        while buffnr <= buffmaxnr
          if getbufvar(buffnr, "MTOrigBuffNR") == origBuffNR
            execute "bw" buffnr
          endif
          let buffnr = buffnr + 1
        endwhile
      endif
    endif
  endif
endfunction

" Function: s:MTFinishCommit(messageFile, targetDir, targetFile) {{{2
function! s:MTFinishCommit(messageFile, targetDir, targetFile, origBuffNR)
  if filereadable(a:messageFile)
    let oldCwd=getcwd()
    if strlen(a:targetDir) > 0
      execute 'cd' escape(a:targetDir, ' ')
    endif
    let resultBuffer=s:MTCreateCommandBuffer('commit -F "' . a:messageFile . '" "'. a:targetFile . '"', 'monotonecommit', '', a:origBuffNR)
    execute 'cd' escape(oldCwd, ' ')
    execute 'bw' escape(a:messageFile, ' *?\')
    silent execute 'call delete("' . a:messageFile . '")'
    return s:MTMarkOrigBufferForSetup(resultBuffer)
  else
    echoerr "Can't read message file; no commit is possible."
    return -1
  endif
endfunction

" Function: s:MTLog() {{{2
function! s:MTLog(...)
  if a:0 == 0
    let versionOption = ""
    let caption = ''
  else
    let versionOption=" -r" . a:1
    let caption = a:1
  endif

  let resultBuffer=s:MTDoCommand('log -v' . versionOption, 'monotonelog', caption)
  if resultBuffer != ""
    set filetype=monotonelog
  endif
  return resultBuffer
endfunction

" Function: s:MTRevert() {{{2
function! s:MTRevert()
  return s:MTMarkOrigBufferForSetup(s:MTDoCommand('revert', 'monotonerevert', ''))
endfunction

" Function: s:MTReview(...) {{{2
function! s:MTReview(...)
  if a:0 == 0
    let versiontag=""
    if s:MTGetOption('MTCommandInteractive', 0)
      let versiontag=input('Revision:  ')
    endif
    if versiontag == ""
      let versiontag="(current)"
      let versionOption=""
    else
      let versionOption=" -r " . versiontag . " "
    endif
  else
    let versiontag=a:1
    let versionOption=" -r " . versiontag . " "
  endif

  let resultBuffer = s:MTDoCommand('cat ' . versionOption, 'monotonereview', versiontag)
  if resultBuffer > 0
    let &filetype=getbufvar(b:MTOrigBuffNR, '&filetype')
  endif

  return resultBuffer
endfunction

" Function: s:MTStatus() {{{2
function! s:MTStatus()
  return s:MTDoCommand('status -v', 'monotonestatus', '')
endfunction

" Function: s:MTInfo() {{{2
function! s:MTInfo()
  return s:MTDoCommand('info', 'monotoneinfo', '')
endfunction

" Function: s:MTUpdate() {{{2
function! s:MTUpdate()
  return s:MTMarkOrigBufferForSetup(s:MTDoCommand('update', 'update', ''))
endfunction

" Function: s:MTVimDiff(...) {{{2
function! s:MTVimDiff(...)
  if(a:0 == 2)
    let resultBuffer = s:MTReview(a:1)
    if resultBuffer < 0
      echomsg "Can't open MT revision " . a:1
      return resultBuffer
    endif
    diffthis
    " If no split method is defined, cheat, and set it to vertical.
    if s:MTGetOption('MTCommandDiffSplit', s:MTGetOption('MTCommandSplit', 'dummy')) == 'dummy'
      let b:MTCommandSplit='vertical'
    endif
    let resultBuffer=s:MTReview(a:2)
    if resultBuffer < 0
      echomsg "Can't open MT revision " . a:1
      return resultBuffer
    endif
    diffthis
  else
    if(a:0 == 0)
      let resultBuffer=s:MTReview()
      if resultBuffer < 0
        echomsg "Can't open current MT revision"
        return resultBuffer
      endif
      diffthis
    else
      let resultBuffer=s:MTReview(a:1)
      if resultBuffer < 0
        echomsg "Can't open MT revision " . a:1
        return resultBuffer
      endif
      diffthis
    endif
    let originalBuffer=b:MTOrigBuffNR
    let originalWindow=bufwinnr(originalBuffer)

    " Don't remove the just-created buffer
    let savedHideOption = getbufvar(resultBuffer, '&bufhidden')
    call setbufvar(resultBuffer, '&bufhidden', 'hide')

    execute "hide b" originalBuffer

    " If there's already a VimDiff'ed window, restore it.
    " There may only be one MTVimDiff original window at a time.

    if exists("g:MTCommandRestoreVimDiffStateCmd")
      execute g:MTCommandRestoreVimDiffStateCmd
    endif

    " Store the state of original buffer so that it can reset when the MT
    " buffer departs.

    let g:MTCommandRestoreVimDiffSourceBuffer = originalBuffer
    let g:MTCommandRestoreVimDiffScratchBuffer = resultBuffer
    let g:MTCommandRestoreVimDiffStateCmd = 
          \    "call setbufvar(".originalBuffer.", \"&diff\", ".getwinvar(originalWindow, '&diff').")"
          \ . "|call setbufvar(".originalBuffer.", \"&foldcolumn\", ".getwinvar(originalWindow, '&foldcolumn').")"
          \ . "|call setbufvar(".originalBuffer.", \"&foldenable\", ".getwinvar(originalWindow, '&foldenable').")"
          \ . "|call setbufvar(".originalBuffer.", \"&foldmethod\", '". getwinvar(originalWindow, '&foldmethod')."')"
          \ . "|call setbufvar(".originalBuffer.", \"&scrollbind\", ".getwinvar(originalWindow, '&scrollbind').")"
          \ . "|call setbufvar(".originalBuffer.", \"&wrap\", ".getwinvar(originalWindow, '&wrap').")"

    if getwinvar(originalWindow, "&foldmethod") == 'manual'
        let g:MTCommandRestoreVimDiffStateCmd = g:MTCommandRestoreVimDiffStateCmd
            \ . "|normal zE"
    endif

    diffthis

    let g:MTCommandEditFileRunning = 1
    if s:MTGetOption('MTCommandDiffSplit', s:MTGetOption('MTCommandSplit', 'vertical')) == 'horizontal'
      execute "silent rightbelow sbuffer" . resultBuffer
    else
      execute "silent vert rightbelow sbuffer" . resultBuffer
    endif
    unlet g:MTCommandEditFileRunning

    " Protect against VIM's splitting rules as they impact scrollbind
    call setbufvar(originalBuffer, '&scrollbind', 1)
    call setbufvar(resultBuffer, '&scrollbind', 1)

    call setbufvar(resultBuffer, '&bufhidden', savedHideOption)
  endif

  silent do MTCommand User MTVimDiffFinish
  return resultBuffer
endfunction

" Function: s:MTCommitDiff() {{{2
function! s:MTCommitDiff()
  let i = line('$')       " last line #
  let cmdline = ""

  let myline = getline(i)
  while i >= 0 && myline =~ "[DMA_][M ]   .*"
    " rip off the filename and prepend to cmdline
    let cmdline = strpart(myline, 5) . " " . cmdline

    " prepare for the next iteration
    let i = i - 1
    let myline = getline(i)
  endwhile

  if cmdline == ''
    echoerr "No well formed lines found"
    return
  endif

  let resultBuffer = s:MTDoCommand('diff ' . cmdline, 'monotonediff', '')
  if resultBuffer != -1
    set filetype=diff
  endif
  return resultBuffer

endfunction

" Section: Command definitions {{{1
" Section: Primary commands {{{2
com! MTAdd call s:MTAdd()
com! -nargs=? MTAnnotate call s:MTAnnotate(<f-args>)
com! MTCommit call s:MTCommit()
com! -nargs=* MTDiff call s:MTDiff(<f-args>)
com! -bang MTGotoOriginal call s:MTGotoOriginal(<q-bang>)
com! -nargs=? MTLog call s:MTLog(<f-args>)
com! MTRevert call s:MTRevert()
com! -nargs=? MTReview call s:MTReview(<f-args>)
com! MTStatus call s:MTStatus()
com! MTInfo call s:MTInfo()
com! MTUpdate call s:MTUpdate()
com! -nargs=* MTVimDiff call s:MTVimDiff(<f-args>)
com! MTCommitDiff call s:MTCommitDiff()

" Section: MT buffer management commands {{{2
com! MTDisableBufferSetup call MTDisableBufferSetup()
com! MTEnableBufferSetup call MTEnableBufferSetup()

" Allow reloading monotonecommand.vim
com! MTReload unlet! loaded_monotonecommand | runtime plugin/monotonecommand.vim

" Section: Plugin command mappings {{{1
nnoremap <silent> <Plug>MTAdd :MTAdd<CR>
nnoremap <silent> <Plug>MTAnnotate :MTAnnotate<CR>
nnoremap <silent> <Plug>MTCommit :MTCommit<CR>
nnoremap <silent> <Plug>MTDiff :MTDiff<CR>
nnoremap <silent> <Plug>MTGotoOriginal :MTGotoOriginal<CR>
nnoremap <silent> <Plug>MTClearAndGotoOriginal :MTGotoOriginal!<CR>
nnoremap <silent> <Plug>MTLog :MTLog<CR>
nnoremap <silent> <Plug>MTRevert :MTRevert<CR>
nnoremap <silent> <Plug>MTReview :MTReview<CR>
nnoremap <silent> <Plug>MTStatus :MTStatus<CR>
nnoremap <silent> <Plug>MTInfo :MTInfo<CR>
nnoremap <silent> <Plug>MTUnedit :MTUnedit<CR>
nnoremap <silent> <Plug>MTUpdate :MTUpdate<CR>
nnoremap <silent> <Plug>MTVimDiff :MTVimDiff<CR>
nnoremap <silent> <Plug>MTCommitDiff :MTCommitDiff<CR>

" Section: Default mappings {{{1
if !hasmapto('<Plug>MTAdd')
  nmap <unique> <Leader>sa <Plug>MTAdd
endif
if !hasmapto('<Plug>MTAnnotate')
  nmap <unique> <Leader>sn <Plug>MTAnnotate
endif
if !hasmapto('<Plug>MTClearAndGotoOriginal')
  nmap <unique> <Leader>sG <Plug>MTClearAndGotoOriginal
endif
if !hasmapto('<Plug>MTCommit')
  nmap <unique> <Leader>sc <Plug>MTCommit
endif
if !hasmapto('<Plug>MTDiff')
  nmap <unique> <Leader>sd <Plug>MTDiff
endif
if !hasmapto('<Plug>MTGotoOriginal')
  nmap <unique> <Leader>sg <Plug>MTGotoOriginal
endif
if !hasmapto('<Plug>MTLog')
  nmap <unique> <Leader>sl <Plug>MTLog
endif
if !hasmapto('<Plug>MTRevert')
  nmap <unique> <Leader>sq <Plug>MTRevert
endif
if !hasmapto('<Plug>MTReview')
  nmap <unique> <Leader>sr <Plug>MTReview
endif
if !hasmapto('<Plug>MTStatus')
  nmap <unique> <Leader>ss <Plug>MTStatus
endif
if !hasmapto('<Plug>MTInfo')
  nmap <unique> <Leader>si <Plug>MTInfo
endif
if !hasmapto('<Plug>MTUpdate')
  nmap <unique> <Leader>su <Plug>MTUpdate
endif
if !hasmapto('<Plug>MTVimDiff')
  nmap <unique> <Leader>sv <Plug>MTVimDiff
endif

" Section: Menu items {{{1
silent! aunmenu Plugin.MT
amenu <silent> &Plugin.MT.&Add        <Plug>MTAdd
amenu <silent> &Plugin.MT.A&nnotate   <Plug>MTAnnotate
amenu <silent> &Plugin.MT.&Commit     <Plug>MTCommit
amenu <silent> &Plugin.MT.&Diff       <Plug>MTDiff
amenu <silent> &Plugin.MT.&Log        <Plug>MTLog
amenu <silent> &Plugin.MT.Revert      <Plug>MTRevert
amenu <silent> &Plugin.MT.&Review     <Plug>MTReview
amenu <silent> &Plugin.MT.&Status     <Plug>MTStatus
amenu <silent> &Plugin.MT.&Info       <Plug>MTInfo
amenu <silent> &Plugin.MT.&Update     <Plug>MTUpdate
amenu <silent> &Plugin.MT.&VimDiff    <Plug>MTVimDiff

" Section: Autocommands to restore vimdiff state {{{1
function! s:MTVimDiffRestore(vimDiffBuff)
  if exists("g:MTCommandRestoreVimDiffScratchBuffer")
        \ && a:vimDiffBuff == g:MTCommandRestoreVimDiffScratchBuffer
    if exists("g:MTCommandRestoreVimDiffSourceBuffer")
      " Only restore if the source buffer is still in Diff mode
      if getbufvar(g:MTCommandRestoreVimDiffSourceBuffer, "&diff")
        if exists("g:MTCommandRestoreVimDiffStateCmd")
          execute g:MTCommandRestoreVimDiffStateCmd
          unlet g:MTCommandRestoreVimDiffStateCmd
        endif
      endif
      unlet g:MTCommandRestoreVimDiffSourceBuffer
    endif
    unlet g:MTCommandRestoreVimDiffScratchBuffer
  endif
endfunction

augroup MTVimDiffRestore
  au!
  au BufUnload * call s:MTVimDiffRestore(expand("<abuf>"))
augroup END

" Section: Optional activation of buffer management {{{1

if s:MTGetOption('MTCommandEnableBufferSetup', 0)
  call MTEnableBufferSetup()
endif

" Section: Plugin completion {{{1

let loaded_monotonecommand=2
silent do MTCommand User MTPluginFinish

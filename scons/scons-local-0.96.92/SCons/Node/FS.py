"""scons.Node.FS

File system nodes.

These Nodes represent the canonical external objects that people think
of when they think of building software: files and directories.

This holds a "default_fs" variable that should be initialized with an FS
that can be used by scripts or modules looking for the canonical default.

"""

#
# Copyright (c) 2001, 2002, 2003, 2004 The SCons Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
# KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

__revision__ = "/home/scons/scons/branch.0/branch.96/baseline/src/engine/SCons/Node/FS.py 0.96.92.D001 2006/04/10 23:13:27 knight"

import os
import os.path
import shutil
import stat
import string
import sys
import time
import cStringIO

import SCons.Action
from SCons.Debug import logInstanceCreation
import SCons.Errors
import SCons.Node
import SCons.Sig.MD5
import SCons.Subst
import SCons.Util
import SCons.Warnings

from SCons.Debug import Trace

# The max_drift value:  by default, use a cached signature value for
# any file that's been untouched for more than two days.
default_max_drift = 2*24*60*60

#
# We stringify these file system Nodes a lot.  Turning a file system Node
# into a string is non-trivial, because the final string representation
# can depend on a lot of factors:  whether it's a derived target or not,
# whether it's linked to a repository or source directory, and whether
# there's duplication going on.  The normal technique for optimizing
# calculations like this is to memoize (cache) the string value, so you
# only have to do the calculation once.
#
# A number of the above factors, however, can be set after we've already
# been asked to return a string for a Node, because a Repository() or
# BuildDir() call or the like may not occur until later in SConscript
# files.  So this variable controls whether we bother trying to save
# string values for Nodes.  The wrapper interface can set this whenever
# they're done mucking with Repository and BuildDir and the other stuff,
# to let this module know it can start returning saved string values
# for Nodes.
#
Save_Strings = None

def save_strings(val):
    global Save_Strings
    Save_Strings = val

#
# SCons.Action objects for interacting with the outside world.
#
# The Node.FS methods in this module should use these actions to
# create and/or remove files and directories; they should *not* use
# os.{link,symlink,unlink,mkdir}(), etc., directly.
#
# Using these SCons.Action objects ensures that descriptions of these
# external activities are properly displayed, that the displays are
# suppressed when the -s (silent) option is used, and (most importantly)
# the actions are disabled when the the -n option is used, in which case
# there should be *no* changes to the external file system(s)...
#

def _copy_func(src, dest):
    shutil.copy2(src, dest)
    st=os.stat(src)
    os.chmod(dest, stat.S_IMODE(st[stat.ST_MODE]) | stat.S_IWRITE)

Valid_Duplicates = ['hard-soft-copy', 'soft-hard-copy',
                    'hard-copy', 'soft-copy', 'copy']

Link_Funcs = [] # contains the callables of the specified duplication style

def set_duplicate(duplicate):
    # Fill in the Link_Funcs list according to the argument
    # (discarding those not available on the platform).

    # Set up the dictionary that maps the argument names to the
    # underlying implementations.  We do this inside this function,
    # not in the top-level module code, so that we can remap os.link
    # and os.symlink for testing purposes.
    try:
        _hardlink_func = os.link
    except AttributeError:
        _hardlink_func = None

    try:
        _softlink_func = os.symlink
    except AttributeError:
        _softlink_func = None

    link_dict = {
        'hard' : _hardlink_func,
        'soft' : _softlink_func,
        'copy' : _copy_func
    }

    if not duplicate in Valid_Duplicates:
        raise SCons.Errors.InternalError, ("The argument of set_duplicate "
                                           "should be in Valid_Duplicates")
    global Link_Funcs
    Link_Funcs = []
    for func in string.split(duplicate,'-'):
        if link_dict[func]:
            Link_Funcs.append(link_dict[func])

def LinkFunc(target, source, env):
    # Relative paths cause problems with symbolic links, so
    # we use absolute paths, which may be a problem for people
    # who want to move their soft-linked src-trees around. Those
    # people should use the 'hard-copy' mode, softlinks cannot be
    # used for that; at least I have no idea how ...
    src = source[0].abspath
    dest = target[0].abspath
    dir, file = os.path.split(dest)
    if dir and not target[0].fs.isdir(dir):
        os.makedirs(dir)
    if not Link_Funcs:
        # Set a default order of link functions.
        set_duplicate('hard-soft-copy')
    # Now link the files with the previously specified order.
    for func in Link_Funcs:
        try:
            func(src,dest)
            break
        except (IOError, OSError):
            # An OSError indicates something happened like a permissions
            # problem or an attempt to symlink across file-system
            # boundaries.  An IOError indicates something like the file
            # not existing.  In either case, keeping trying additional
            # functions in the list and only raise an error if the last
            # one failed.
            if func == Link_Funcs[-1]:
                # exception of the last link method (copy) are fatal
                raise
            else:
                pass
    return 0

Link = SCons.Action.Action(LinkFunc, None)
def LocalString(target, source, env):
    return 'Local copy of %s from %s' % (target[0], source[0])

LocalCopy = SCons.Action.Action(LinkFunc, LocalString)

def UnlinkFunc(target, source, env):
    t = target[0]
    t.fs.unlink(t.abspath)
    return 0

Unlink = SCons.Action.Action(UnlinkFunc, None)

def MkdirFunc(target, source, env):
    t = target[0]
    if not t.exists():
        t.fs.mkdir(t.abspath)
    return 0

Mkdir = SCons.Action.Action(MkdirFunc, None, presub=None)

MkdirBuilder = None

def get_MkdirBuilder():
    global MkdirBuilder
    if MkdirBuilder is None:
        import SCons.Builder
        import SCons.Defaults
        # "env" will get filled in by Executor.get_build_env()
        # calling SCons.Defaults.DefaultEnvironment() when necessary.
        MkdirBuilder = SCons.Builder.Builder(action = Mkdir,
                                             env = None,
                                             explain = None,
                                             is_explicit = None,
                                             target_scanner = SCons.Defaults.DirEntryScanner,
                                             name = "MkdirBuilder")
    return MkdirBuilder

def CacheRetrieveFunc(target, source, env):
    t = target[0]
    fs = t.fs
    cachedir, cachefile = t.cachepath()
    if fs.exists(cachefile):
        if SCons.Action.execute_actions:
            fs.copy2(cachefile, t.path)
            st = fs.stat(cachefile)
            fs.chmod(t.path, stat.S_IMODE(st[stat.ST_MODE]) | stat.S_IWRITE)
        return 0
    return 1

def CacheRetrieveString(target, source, env):
    t = target[0]
    cachedir, cachefile = t.cachepath()
    if t.fs.exists(cachefile):
        return "Retrieved `%s' from cache" % t.path
    return None

CacheRetrieve = SCons.Action.Action(CacheRetrieveFunc, CacheRetrieveString)

CacheRetrieveSilent = SCons.Action.Action(CacheRetrieveFunc, None)

def CachePushFunc(target, source, env):
    t = target[0]
    fs = t.fs
    cachedir, cachefile = t.cachepath()
    if fs.exists(cachefile):
        # Don't bother copying it if it's already there.
        return

    if not fs.isdir(cachedir):
        fs.makedirs(cachedir)

    tempfile = cachefile+'.tmp'
    try:
        fs.copy2(t.path, tempfile)
        fs.rename(tempfile, cachefile)
        st = fs.stat(t.path)
        fs.chmod(cachefile, stat.S_IMODE(st[stat.ST_MODE]) | stat.S_IWRITE)
    except (IOError, OSError):
        # It's possible someone else tried writing the file at the
        # same time we did, or else that there was some problem like
        # the CacheDir being on a separate file system that's full.
        # In any case, inability to push a file to cache doesn't affect
        # the correctness of the build, so just print a warning.
        SCons.Warnings.warn(SCons.Warnings.CacheWriteErrorWarning,
                            "Unable to copy %s to cache. Cache file is %s"
                                % (str(target), cachefile))
        return

CachePush = SCons.Action.Action(CachePushFunc, None)

class _Null:
    pass

_null = _Null()

DefaultSCCSBuilder = None
DefaultRCSBuilder = None

def get_DefaultSCCSBuilder():
    global DefaultSCCSBuilder
    if DefaultSCCSBuilder is None:
        import SCons.Builder
        # "env" will get filled in by Executor.get_build_env()
        # calling SCons.Defaults.DefaultEnvironment() when necessary.
        act = SCons.Action.Action('$SCCSCOM', '$SCCSCOMSTR')
        DefaultSCCSBuilder = SCons.Builder.Builder(action = act,
                                                   env = None,
                                                   name = "DefaultSCCSBuilder")
    return DefaultSCCSBuilder

def get_DefaultRCSBuilder():
    global DefaultRCSBuilder
    if DefaultRCSBuilder is None:
        import SCons.Builder
        # "env" will get filled in by Executor.get_build_env()
        # calling SCons.Defaults.DefaultEnvironment() when necessary.
        act = SCons.Action.Action('$RCS_COCOM', '$RCS_COCOMSTR')
        DefaultRCSBuilder = SCons.Builder.Builder(action = act,
                                                  env = None,
                                                  name = "DefaultRCSBuilder")
    return DefaultRCSBuilder

# Cygwin's os.path.normcase pretends it's on a case-sensitive filesystem.
_is_cygwin = sys.platform == "cygwin"
if os.path.normcase("TeSt") == os.path.normpath("TeSt") and not _is_cygwin:
    def _my_normcase(x):
        return x
else:
    def _my_normcase(x):
        return string.upper(x)



class DiskChecker:
    def __init__(self, type, do, ignore):
        self.type = type
        self.do = do
        self.ignore = ignore
        self.set_do()
    def set_do(self):
        self.__call__ = self.do
    def set_ignore(self):
        self.__call__ = self.ignore
    def set(self, list):
        if self.type in list:
            self.set_do()
        else:
            self.set_ignore()

def do_diskcheck_match(node, predicate, errorfmt):
    path = node.abspath
    if predicate(path):
        raise TypeError, errorfmt % path

def ignore_diskcheck_match(node, predicate, errorfmt):
    pass

def do_diskcheck_rcs(node, name):
    try:
        rcs_dir = node.rcs_dir
    except AttributeError:
        if node.entry_exists_on_disk('RCS'):
            rcs_dir = node.Dir('RCS')
        else:
            rcs_dir = None
        node.rcs_dir = rcs_dir
    if rcs_dir:
        return rcs_dir.entry_exists_on_disk(name+',v')
    return None

def ignore_diskcheck_rcs(node, name):
    return None

def do_diskcheck_sccs(node, name):
    try:
        sccs_dir = node.sccs_dir
    except AttributeError:
        if node.entry_exists_on_disk('SCCS'):
            sccs_dir = node.Dir('SCCS')
        else:
            sccs_dir = None
        node.sccs_dir = sccs_dir
    if sccs_dir:
        return sccs_dir.entry_exists_on_disk('s.'+name)
    return None

def ignore_diskcheck_sccs(node, name):
    return None

diskcheck_match = DiskChecker('match', do_diskcheck_match, ignore_diskcheck_match)
diskcheck_rcs = DiskChecker('rcs', do_diskcheck_rcs, ignore_diskcheck_rcs)
diskcheck_sccs = DiskChecker('sccs', do_diskcheck_sccs, ignore_diskcheck_sccs)

diskcheckers = [
    diskcheck_match,
    diskcheck_rcs,
    diskcheck_sccs,
]

def set_diskcheck(list):
    for dc in diskcheckers:
        dc.set(list)

def diskcheck_types():
    return map(lambda dc: dc.type, diskcheckers)



class EntryProxy(SCons.Util.Proxy):
    def __get_abspath(self):
        entry = self.get()
        return SCons.Subst.SpecialAttrWrapper(entry.get_abspath(),
                                             entry.name + "_abspath")

    def __get_filebase(self):
        name = self.get().name
        return SCons.Subst.SpecialAttrWrapper(SCons.Util.splitext(name)[0],
                                             name + "_filebase")

    def __get_suffix(self):
        name = self.get().name
        return SCons.Subst.SpecialAttrWrapper(SCons.Util.splitext(name)[1],
                                             name + "_suffix")

    def __get_file(self):
        name = self.get().name
        return SCons.Subst.SpecialAttrWrapper(name, name + "_file")

    def __get_base_path(self):
        """Return the file's directory and file name, with the
        suffix stripped."""
        entry = self.get()
        return SCons.Subst.SpecialAttrWrapper(SCons.Util.splitext(entry.get_path())[0],
                                             entry.name + "_base")

    def __get_posix_path(self):
        """Return the path with / as the path separator,
        regardless of platform."""
        if os.sep == '/':
            return self
        else:
            entry = self.get()
            r = string.replace(entry.get_path(), os.sep, '/')
            return SCons.Subst.SpecialAttrWrapper(r, entry.name + "_posix")

    def __get_windows_path(self):
        """Return the path with \ as the path separator,
        regardless of platform."""
        if os.sep == '\\':
            return self
        else:
            entry = self.get()
            r = string.replace(entry.get_path(), os.sep, '\\')
            return SCons.Subst.SpecialAttrWrapper(r, entry.name + "_windows")

    def __get_srcnode(self):
        return EntryProxy(self.get().srcnode())

    def __get_srcdir(self):
        """Returns the directory containing the source node linked to this
        node via BuildDir(), or the directory of this node if not linked."""
        return EntryProxy(self.get().srcnode().dir)

    def __get_rsrcnode(self):
        return EntryProxy(self.get().srcnode().rfile())

    def __get_rsrcdir(self):
        """Returns the directory containing the source node linked to this
        node via BuildDir(), or the directory of this node if not linked."""
        return EntryProxy(self.get().srcnode().rfile().dir)

    def __get_dir(self):
        return EntryProxy(self.get().dir)
    
    dictSpecialAttrs = { "base"     : __get_base_path,
                         "posix"    : __get_posix_path,
                         "windows"  : __get_windows_path,
                         "win32"    : __get_windows_path,
                         "srcpath"  : __get_srcnode,
                         "srcdir"   : __get_srcdir,
                         "dir"      : __get_dir,
                         "abspath"  : __get_abspath,
                         "filebase" : __get_filebase,
                         "suffix"   : __get_suffix,
                         "file"     : __get_file,
                         "rsrcpath" : __get_rsrcnode,
                         "rsrcdir"  : __get_rsrcdir,
                       }

    def __getattr__(self, name):
        # This is how we implement the "special" attributes
        # such as base, posix, srcdir, etc.
        try:
            attr_function = self.dictSpecialAttrs[name]
        except KeyError:
            try:
                attr = SCons.Util.Proxy.__getattr__(self, name)
            except AttributeError:
                entry = self.get()
                classname = string.split(str(entry.__class__), '.')[-1]
                if classname[-2:] == "'>":
                    # new-style classes report their name as:
                    #   "<class 'something'>"
                    # instead of the classic classes:
                    #   "something"
                    classname = classname[:-2]
                raise AttributeError, "%s instance '%s' has no attribute '%s'" % (classname, entry.name, name)
            return attr
        else:
            return attr_function(self)

class Base(SCons.Node.Node):
    """A generic class for file system entries.  This class is for
    when we don't know yet whether the entry being looked up is a file
    or a directory.  Instances of this class can morph into either
    Dir or File objects by a later, more precise lookup.

    Note: this class does not define __cmp__ and __hash__ for
    efficiency reasons.  SCons does a lot of comparing of
    Node.FS.{Base,Entry,File,Dir} objects, so those operations must be
    as fast as possible, which means we want to use Python's built-in
    object identity comparisons.
    """

    def __init__(self, name, directory, fs):
        """Initialize a generic Node.FS.Base object.
        
        Call the superclass initialization, take care of setting up
        our relative and absolute paths, identify our parent
        directory, and indicate that this node should use
        signatures."""
        if __debug__: logInstanceCreation(self, 'Node.FS.Base')
        SCons.Node.Node.__init__(self)

        self.name = name
        self.fs = fs

        assert directory, "A directory must be provided"

        self.abspath = directory.entry_abspath(name)
        if directory.path == '.':
            self.path = name
        else:
            self.path = directory.entry_path(name)
        if directory.tpath == '.':
            self.tpath = name
        else:
            self.tpath = directory.entry_tpath(name)
        self.path_elements = directory.path_elements + [self]

        self.dir = directory
        self.cwd = None # will hold the SConscript directory for target nodes
        self.duplicate = directory.duplicate

    def clear(self):
        """Completely clear a Node.FS.Base object of all its cached
        state (so that it can be re-evaluated by interfaces that do
        continuous integration builds).
        __cache_reset__
        """
        SCons.Node.Node.clear(self)

    def get_dir(self):
        return self.dir

    def get_suffix(self):
        "__cacheable__"
        return SCons.Util.splitext(self.name)[1]

    def rfile(self):
        return self

    def __str__(self):
        """A Node.FS.Base object's string representation is its path
        name."""
        global Save_Strings
        if Save_Strings:
            return self._save_str()
        return self._get_str()

    def _save_str(self):
        "__cacheable__"
        return self._get_str()

    def _get_str(self):
        if self.duplicate or self.is_derived():
            return self.get_path()
        return self.srcnode().get_path()

    rstr = __str__

    def stat(self):
        "__cacheable__"
        try: return self.fs.stat(self.abspath)
        except os.error: return None

    def exists(self):
        "__cacheable__"
        return not self.stat() is None

    def rexists(self):
        "__cacheable__"
        return self.rfile().exists()

    def getmtime(self):
        st = self.stat()
        if st: return st[stat.ST_MTIME]
        else: return None

    def getsize(self):
        st = self.stat()
        if st: return st[stat.ST_SIZE]
        else: return None

    def isdir(self):
        st = self.stat()
        return not st is None and stat.S_ISDIR(st[stat.ST_MODE])

    def isfile(self):
        st = self.stat()
        return not st is None and stat.S_ISREG(st[stat.ST_MODE])

    if hasattr(os, 'symlink'):
        def islink(self):
            try: st = self.fs.lstat(self.abspath)
            except os.error: return 0
            return stat.S_ISLNK(st[stat.ST_MODE])
    else:
        def islink(self):
            return 0                    # no symlinks

    def is_under(self, dir):
        if self is dir:
            return 1
        else:
            return self.dir.is_under(dir)

    def set_local(self):
        self._local = 1

    def srcnode(self):
        """If this node is in a build path, return the node
        corresponding to its source file.  Otherwise, return
        ourself.
        __cacheable__"""
        dir=self.dir
        name=self.name
        while dir:
            if dir.srcdir:
                srcnode = self.fs.Entry(name, dir.srcdir,
                                        klass=self.__class__)
                return srcnode
            name = dir.name + os.sep + name
            dir = dir.up()
        return self

    def get_path(self, dir=None):
        """Return path relative to the current working directory of the
        Node.FS.Base object that owns us."""
        if not dir:
            dir = self.fs.getcwd()
        if self == dir:
            return '.'
        path_elems = self.path_elements
        try: i = path_elems.index(dir)
        except ValueError: pass
        else: path_elems = path_elems[i+1:]
        path_elems = map(lambda n: n.name, path_elems)
        return string.join(path_elems, os.sep)

    def set_src_builder(self, builder):
        """Set the source code builder for this node."""
        self.sbuilder = builder
        if not self.has_builder():
            self.builder_set(builder)

    def src_builder(self):
        """Fetch the source code builder for this node.

        If there isn't one, we cache the source code builder specified
        for the directory (which in turn will cache the value from its
        parent directory, and so on up to the file system root).
        """
        try:
            scb = self.sbuilder
        except AttributeError:
            scb = self.dir.src_builder()
            self.sbuilder = scb
        return scb

    def get_abspath(self):
        """Get the absolute path of the file."""
        return self.abspath

    def for_signature(self):
        # Return just our name.  Even an absolute path would not work,
        # because that can change thanks to symlinks or remapped network
        # paths.
        return self.name

    def get_subst_proxy(self):
        try:
            return self._proxy
        except AttributeError:
            ret = EntryProxy(self)
            self._proxy = ret
            return ret

    def target_from_source(self, prefix, suffix, splitext=SCons.Util.splitext):
        return self.dir.Entry(prefix + splitext(self.name)[0] + suffix)

class Entry(Base):
    """This is the class for generic Node.FS entries--that is, things
    that could be a File or a Dir, but we're just not sure yet.
    Consequently, the methods in this class really exist just to
    transform their associated object into the right class when the
    time comes, and then call the same-named method in the transformed
    class."""

    def diskcheck_match(self):
        pass

    def disambiguate(self):
        if self.isdir():
            self.__class__ = Dir
            self._morph()
        else:
            self.__class__ = File
            self._morph()
            self.clear()
        return self

    def rfile(self):
        """We're a generic Entry, but the caller is actually looking for
        a File at this point, so morph into one."""
        self.__class__ = File
        self._morph()
        self.clear()
        return File.rfile(self)

    def scanner_key(self):
        return self.get_suffix()

    def get_contents(self):
        """Fetch the contents of the entry.
        
        Since this should return the real contents from the file
        system, we check to see into what sort of subclass we should
        morph this Entry."""
        if self.isfile():
            self.__class__ = File
            self._morph()
            return self.get_contents()
        if self.isdir():
            self.__class__ = Dir
            self._morph()
            return self.get_contents()
        if self.islink():
            return ''             # avoid errors for dangling symlinks
        raise AttributeError

    def must_be_a_Dir(self):
        """Called to make sure a Node is a Dir.  Since we're an
        Entry, we can morph into one."""
        self.__class__ = Dir
        self._morph()
        return self

    # The following methods can get called before the Taskmaster has
    # had a chance to call disambiguate() directly to see if this Entry
    # should really be a Dir or a File.  We therefore use these to call
    # disambiguate() transparently (from our caller's point of view).
    #
    # Right now, this minimal set of methods has been derived by just
    # looking at some of the methods that will obviously be called early
    # in any of the various Taskmasters' calling sequences, and then
    # empirically figuring out which additional methods are necessary
    # to make various tests pass.

    def exists(self):
        """Return if the Entry exists.  Check the file system to see
        what we should turn into first.  Assume a file if there's no
        directory."""
        return self.disambiguate().exists()

    def rel_path(self, other):
        d = self.disambiguate()
        if d.__class__ == Entry:
            raise "rel_path() could not disambiguate File/Dir"
        return d.rel_path(other)

# This is for later so we can differentiate between Entry the class and Entry
# the method of the FS class.
_classEntry = Entry


class LocalFS:

    if SCons.Memoize.use_memoizer:
        __metaclass__ = SCons.Memoize.Memoized_Metaclass
    
    # This class implements an abstraction layer for operations involving
    # a local file system.  Essentially, this wraps any function in
    # the os, os.path or shutil modules that we use to actually go do
    # anything with or to the local file system.
    #
    # Note that there's a very good chance we'll refactor this part of
    # the architecture in some way as we really implement the interface(s)
    # for remote file system Nodes.  For example, the right architecture
    # might be to have this be a subclass instead of a base class.
    # Nevertheless, we're using this as a first step in that direction.
    #
    # We're not using chdir() yet because the calling subclass method
    # needs to use os.chdir() directly to avoid recursion.  Will we
    # really need this one?
    #def chdir(self, path):
    #    return os.chdir(path)
    def chmod(self, path, mode):
        return os.chmod(path, mode)
    def copy2(self, src, dst):
        return shutil.copy2(src, dst)
    def exists(self, path):
        return os.path.exists(path)
    def getmtime(self, path):
        return os.path.getmtime(path)
    def getsize(self, path):
        return os.path.getsize(path)
    def isdir(self, path):
        return os.path.isdir(path)
    def isfile(self, path):
        return os.path.isfile(path)
    def link(self, src, dst):
        return os.link(src, dst)
    def lstat(self, path):
        return os.lstat(path)
    def listdir(self, path):
        return os.listdir(path)
    def makedirs(self, path):
        return os.makedirs(path)
    def mkdir(self, path):
        return os.mkdir(path)
    def rename(self, old, new):
        return os.rename(old, new)
    def stat(self, path):
        return os.stat(path)
    def symlink(self, src, dst):
        return os.symlink(src, dst)
    def open(self, path):
        return open(path)
    def unlink(self, path):
        return os.unlink(path)

    if hasattr(os, 'symlink'):
        def islink(self, path):
            return os.path.islink(path)
    else:
        def islink(self, path):
            return 0                    # no symlinks

if SCons.Memoize.use_old_memoization():
    _FSBase = LocalFS
    class LocalFS(SCons.Memoize.Memoizer, _FSBase):
        def __init__(self, *args, **kw):
            apply(_FSBase.__init__, (self,)+args, kw)
            SCons.Memoize.Memoizer.__init__(self)


#class RemoteFS:
#    # Skeleton for the obvious methods we might need from the
#    # abstraction layer for a remote filesystem.
#    def upload(self, local_src, remote_dst):
#        pass
#    def download(self, remote_src, local_dst):
#        pass


class FS(LocalFS):

    def __init__(self, path = None):
        """Initialize the Node.FS subsystem.

        The supplied path is the top of the source tree, where we
        expect to find the top-level build file.  If no path is
        supplied, the current directory is the default.

        The path argument must be a valid absolute path.
        """
        if __debug__: logInstanceCreation(self, 'Node.FS')

        self.Root = {}
        self.SConstruct_dir = None
        self.CachePath = None
        self.cache_force = None
        self.cache_show = None
        self.max_drift = default_max_drift

        self.Top = None
        if path is None:
            self.pathTop = os.getcwd()
        else:
            self.pathTop = path
        self.defaultDrive = _my_normcase(os.path.splitdrive(self.pathTop)[0])

        self.Top = self._doLookup(Dir, os.path.normpath(self.pathTop))
        self.Top.path = '.'
        self.Top.tpath = '.'
        self._cwd = self.Top

    def clear_cache(self):
        "__cache_reset__"
        pass
    
    def set_SConstruct_dir(self, dir):
        self.SConstruct_dir = dir

    def get_max_drift(self):
        return self.max_drift

    def set_max_drift(self, max_drift):
        self.max_drift = max_drift

    def getcwd(self):
        return self._cwd

    def __checkClass(self, node, klass):
        if isinstance(node, klass) or klass == Entry:
            return node
        if node.__class__ == Entry:
            node.__class__ = klass
            node._morph()
            return node
        raise TypeError, "Tried to lookup %s '%s' as a %s." % \
              (node.__class__.__name__, node.path, klass.__name__)
        
    def _doLookup(self, fsclass, name, directory = None, create = 1):
        """This method differs from the File and Dir factory methods in
        one important way: the meaning of the directory parameter.
        In this method, if directory is None or not supplied, the supplied
        name is expected to be an absolute path.  If you try to look up a
        relative path with directory=None, then an AssertionError will be
        raised.
        __cacheable__"""

        if not name:
            # This is a stupid hack to compensate for the fact that the
            # POSIX and Windows versions of os.path.normpath() behave
            # differently in older versions of Python.  In particular,
            # in POSIX:
            #   os.path.normpath('./') == '.'
            # in Windows:
            #   os.path.normpath('./') == ''
            #   os.path.normpath('.\\') == ''
            #
            # This is a definite bug in the Python library, but we have
            # to live with it.
            name = '.'
        path_orig = string.split(name, os.sep)
        path_norm = string.split(_my_normcase(name), os.sep)

        first_orig = path_orig.pop(0)   # strip first element
        first_norm = path_norm.pop(0)   # strip first element

        drive, path_first = os.path.splitdrive(first_orig)
        if path_first:
            path_orig = [ path_first, ] + path_orig
            path_norm = [ _my_normcase(path_first), ] + path_norm
        else:
            # Absolute path
            drive = _my_normcase(drive)
            try:
                directory = self.Root[drive]
            except KeyError:
                if not create:
                    raise SCons.Errors.UserError
                directory = RootDir(drive, self)
                self.Root[drive] = directory
                if not drive:
                    self.Root[self.defaultDrive] = directory
                elif drive == self.defaultDrive:
                    self.Root[''] = directory

        if not path_orig:
            return directory

        last_orig = path_orig.pop()     # strip last element
        last_norm = path_norm.pop()     # strip last element
            
        # Lookup the directory
        for orig, norm in map(None, path_orig, path_norm):
            try:
                entries = directory.entries
            except AttributeError:
                # We tried to look up the entry in either an Entry or
                # a File.  Give whatever it is a chance to do what's
                # appropriate: morph into a Dir or raise an exception.
                directory.must_be_a_Dir()
                entries = directory.entries
            try:
                directory = entries[norm]
            except KeyError:
                if not create:
                    raise SCons.Errors.UserError

                d = Dir(orig, directory, self)

                # Check the file system (or not, as configured) to make
                # sure there isn't already a file there.
                d.diskcheck_match()

                directory.entries[norm] = d
                directory.add_wkid(d)
                directory = d

        directory.must_be_a_Dir()

        try:
            e = directory.entries[last_norm]
        except KeyError:
            if not create:
                raise SCons.Errors.UserError

            result = fsclass(last_orig, directory, self)

            # Check the file system (or not, as configured) to make
            # sure there isn't already a directory at the path on
            # disk where we just created a File node, and vice versa.
            result.diskcheck_match()

            directory.entries[last_norm] = result 
            directory.add_wkid(result)
        else:
            result = self.__checkClass(e, fsclass)
        return result 

    def _transformPath(self, name, directory):
        """Take care of setting up the correct top-level directory,
        usually in preparation for a call to doLookup().

        If the path name is prepended with a '#', then it is unconditionally
        interpreted as relative to the top-level directory of this FS.

        If directory is None, and name is a relative path,
        then the same applies.
        """
        if not SCons.Util.is_String(name):
            # This handles cases where the object is a Proxy wrapping
            # a Node.FS.File object (e.g.).  It would be good to handle
            # this more directly some day by having the callers of this
            # function recognize that a Proxy can be treated like the
            # underlying object (that is, get rid of the isinstance()
            # calls that explicitly look for a Node.FS.Base object).
            name = str(name)
        if name and name[0] == '#':
            directory = self.Top
            name = name[1:]
            if name and (name[0] == os.sep or name[0] == '/'):
                # Correct such that '#/foo' is equivalent
                # to '#foo'.
                name = name[1:]
            name = os.path.join('.', os.path.normpath(name))
        elif not directory:
            directory = self._cwd
        return (os.path.normpath(name), directory)

    def chdir(self, dir, change_os_dir=0):
        """Change the current working directory for lookups.
        If change_os_dir is true, we will also change the "real" cwd
        to match.
        """
        curr=self._cwd
        try:
            if not dir is None:
                self._cwd = dir
                if change_os_dir:
                    os.chdir(dir.abspath)
        except OSError:
            self._cwd = curr
            raise

    def Entry(self, name, directory = None, create = 1, klass=None):
        """Lookup or create a generic Entry node with the specified name.
        If the name is a relative path (begins with ./, ../, or a file
        name), then it is looked up relative to the supplied directory
        node, or to the top level directory of the FS (supplied at
        construction time) if no directory is supplied.
        """

        if not klass:
            klass = Entry

        if isinstance(name, Base):
            return self.__checkClass(name, klass)
        else:
            if directory and not isinstance(directory, Dir):
                directory = self.Dir(directory)
            name, directory = self._transformPath(name, directory)
            return self._doLookup(klass, name, directory, create)
    
    def File(self, name, directory = None, create = 1):
        """Lookup or create a File node with the specified name.  If
        the name is a relative path (begins with ./, ../, or a file name),
        then it is looked up relative to the supplied directory node,
        or to the top level directory of the FS (supplied at construction
        time) if no directory is supplied.

        This method will raise TypeError if a directory is found at the
        specified path.
        """

        return self.Entry(name, directory, create, File)
    
    def Dir(self, name, directory = None, create = 1):
        """Lookup or create a Dir node with the specified name.  If
        the name is a relative path (begins with ./, ../, or a file name),
        then it is looked up relative to the supplied directory node,
        or to the top level directory of the FS (supplied at construction
        time) if no directory is supplied.

        This method will raise TypeError if a normal file is found at the
        specified path.
        """

        return self.Entry(name, directory, create, Dir)
    
    def BuildDir(self, build_dir, src_dir, duplicate=1):
        """Link the supplied build directory to the source directory
        for purposes of building files."""
        
        if not isinstance(src_dir, SCons.Node.Node):
            src_dir = self.Dir(src_dir)
        if not isinstance(build_dir, SCons.Node.Node):
            build_dir = self.Dir(build_dir)
        if src_dir.is_under(build_dir):
            raise SCons.Errors.UserError, "Source directory cannot be under build directory."
        if build_dir.srcdir:
            if build_dir.srcdir == src_dir:
                return # We already did this.
            raise SCons.Errors.UserError, "'%s' already has a source directory: '%s'."%(build_dir, build_dir.srcdir)
        build_dir.link(src_dir, duplicate)

    def Repository(self, *dirs):
        """Specify Repository directories to search."""
        for d in dirs:
            if not isinstance(d, SCons.Node.Node):
                d = self.Dir(d)
            self.Top.addRepository(d)

    def Rfindalldirs(self, pathlist, cwd):
        """__cacheable__"""
        if SCons.Util.is_String(pathlist):
            pathlist = string.split(pathlist, os.pathsep)
        if not SCons.Util.is_List(pathlist):
            pathlist = [pathlist]
        result = []
        for path in filter(None, pathlist):
            if isinstance(path, SCons.Node.Node):
                result.append(path)
                continue
            path, dir = self._transformPath(path, cwd)
            dir = dir.Dir(path)
            result.extend(dir.get_all_rdirs())
        return result

    def CacheDir(self, path):
        self.CachePath = path

    def build_dir_target_climb(self, orig, dir, tail):
        """Create targets in corresponding build directories

        Climb the directory tree, and look up path names
        relative to any linked build directories we find.
        __cacheable__
        """
        targets = []
        message = None
        fmt = "building associated BuildDir targets: %s"
        start_dir = dir
        while dir:
            for bd in dir.build_dirs:
                if start_dir.is_under(bd):
                    # If already in the build-dir location, don't reflect
                    return [orig], fmt % str(orig)
                p = apply(os.path.join, [bd.path] + tail)
                targets.append(self.Entry(p))
            tail = [dir.name] + tail
            dir = dir.up()
        if targets:
            message = fmt % string.join(map(str, targets))
        return targets, message

class DirNodeInfo(SCons.Node.NodeInfoBase):
    pass

class DirBuildInfo(SCons.Node.BuildInfoBase):
    pass

class Dir(Base):
    """A class for directories in a file system.
    """

    NodeInfo = DirNodeInfo
    BuildInfo = DirBuildInfo

    def __init__(self, name, directory, fs):
        if __debug__: logInstanceCreation(self, 'Node.FS.Dir')
        Base.__init__(self, name, directory, fs)
        self._morph()

    def _morph(self):
        """Turn a file system Node (either a freshly initialized directory
        object or a separate Entry object) into a proper directory object.

        Set up this directory's entries and hook it into the file
        system tree.  Specify that directories (this Node) don't use
        signatures for calculating whether they're current.
        __cache_reset__"""

        self.repositories = []
        self.srcdir = None

        self.entries = {}
        self.entries['.'] = self
        self.entries['..'] = self.dir
        self.cwd = self
        self.searched = 0
        self._sconsign = None
        self.build_dirs = []

        # Don't just reset the executor, replace its action list,
        # because it might have some pre-or post-actions that need to
        # be preserved.
        self.builder = get_MkdirBuilder()
        self.get_executor().set_action_list(self.builder.action)

    def diskcheck_match(self):
        diskcheck_match(self, self.fs.isfile,
                           "File %s found where directory expected.")

    def __clearRepositoryCache(self, duplicate=None):
        """Called when we change the repository(ies) for a directory.
        This clears any cached information that is invalidated by changing
        the repository."""

        for node in self.entries.values():
            if node != self.dir:
                if node != self and isinstance(node, Dir):
                    node.__clearRepositoryCache(duplicate)
                else:
                    node.clear()
                    try:
                        del node._srcreps
                    except AttributeError:
                        pass
                    if duplicate != None:
                        node.duplicate=duplicate
    
    def __resetDuplicate(self, node):
        if node != self:
            node.duplicate = node.get_dir().duplicate

    def Entry(self, name):
        """Create an entry node named 'name' relative to this directory."""
        return self.fs.Entry(name, self)

    def Dir(self, name):
        """Create a directory node named 'name' relative to this directory."""
        return self.fs.Dir(name, self)

    def File(self, name):
        """Create a file node named 'name' relative to this directory."""
        return self.fs.File(name, self)

    def link(self, srcdir, duplicate):
        """Set this directory as the build directory for the
        supplied source directory."""
        self.srcdir = srcdir
        self.duplicate = duplicate
        self.__clearRepositoryCache(duplicate)
        srcdir.build_dirs.append(self)

    def getRepositories(self):
        """Returns a list of repositories for this directory.
        __cacheable__"""
        if self.srcdir and not self.duplicate:
            return self.srcdir.get_all_rdirs() + self.repositories
        return self.repositories

    def get_all_rdirs(self):
        """__cacheable__"""
        result = [self]
        fname = '.'
        dir = self
        while dir:
            for rep in dir.getRepositories():
                result.append(rep.Dir(fname))
            fname = dir.name + os.sep + fname
            dir = dir.up()
        return result

    def addRepository(self, dir):
        if dir != self and not dir in self.repositories:
            self.repositories.append(dir)
            dir.tpath = '.'
            self.__clearRepositoryCache()

    def up(self):
        return self.entries['..']

    def rel_path(self, other):
        """Return a path to "other" relative to this directory.
        __cacheable__"""
        if isinstance(other, Dir):
            name = []
        else:
            try:
                name = [other.name]
                other = other.dir
            except AttributeError:
                return str(other)
        if self is other:
            return name and name[0] or '.'
        i = 0
        for x, y in map(None, self.path_elements, other.path_elements):
            if not x is y:
                break
            i = i + 1
        path_elems = ['..']*(len(self.path_elements)-i) \
                   + map(lambda n: n.name, other.path_elements[i:]) \
                   + name
             
        return string.join(path_elems, os.sep)

    def get_env_scanner(self, env, kw={}):
        return SCons.Defaults.DirEntryScanner

    def get_target_scanner(self):
        return SCons.Defaults.DirEntryScanner

    def get_found_includes(self, env, scanner, path):
        """Return the included implicit dependencies in this file.
        Cache results so we only scan the file once per path
        regardless of how many times this information is requested.
        __cacheable__"""
        if not scanner:
            return []
        # Clear cached info for this Dir.  If we already visited this
        # directory on our walk down the tree (because we didn't know at
        # that point it was being used as the source for another Node)
        # then we may have calculated build signature before realizing
        # we had to scan the disk.  Now that we have to, though, we need
        # to invalidate the old calculated signature so that any node
        # dependent on our directory structure gets one that includes
        # info about everything on disk.
        self.clear()
        return scanner(self, env, path)

    def build(self, **kw):
        """A null "builder" for directories."""
        global MkdirBuilder
        if not self.builder is MkdirBuilder:
            apply(SCons.Node.Node.build, [self,], kw)

    def _create(self):
        """Create this directory, silently and without worrying about
        whether the builder is the default or not."""
        listDirs = []
        parent = self
        while parent:
            if parent.exists():
                break
            listDirs.append(parent)
            p = parent.up()
            if p is None:
                raise SCons.Errors.StopError, parent.path
            parent = p
        listDirs.reverse()
        for dirnode in listDirs:
            try:
                # Don't call dirnode.build(), call the base Node method
                # directly because we definitely *must* create this
                # directory.  The dirnode.build() method will suppress
                # the build if it's the default builder.
                SCons.Node.Node.build(dirnode)
                dirnode.get_executor().nullify()
                # The build() action may or may not have actually
                # created the directory, depending on whether the -n
                # option was used or not.  Delete the _exists and
                # _rexists attributes so they can be reevaluated.
                dirnode.clear()
            except OSError:
                pass

    def multiple_side_effect_has_builder(self):
        global MkdirBuilder
        return not self.builder is MkdirBuilder and self.has_builder()

    def alter_targets(self):
        """Return any corresponding targets in a build directory.
        """
        return self.fs.build_dir_target_climb(self, self, [])

    def scanner_key(self):
        """A directory does not get scanned."""
        return None

    def get_contents(self):
        """Return aggregate contents of all our children."""
        contents = map(lambda n: n.get_contents(), self.children())
        return  string.join(contents, '')

    def prepare(self):
        pass

    def do_duplicate(self, src):
        pass

    def current(self, calc=None):
        """If any child is not up-to-date, then this directory isn't,
        either."""
        if not self.builder is MkdirBuilder and not self.exists():
            return 0
        up_to_date = SCons.Node.up_to_date
        for kid in self.children():
            if kid.get_state() > up_to_date:
                return 0
        return 1

    def rdir(self):
        "__cacheable__"
        if not self.exists():
            norm_name = _my_normcase(self.name)
            for dir in self.dir.get_all_rdirs():
                try: node = dir.entries[norm_name]
                except KeyError: node = dir.dir_on_disk(self.name)
                if node and node.exists() and \
                    (isinstance(dir, Dir) or isinstance(dir, Entry)):
                        return node
        return self

    def sconsign(self):
        """Return the .sconsign file info for this directory,
        creating it first if necessary."""
        if not self._sconsign:
            import SCons.SConsign
            self._sconsign = SCons.SConsign.ForDirectory(self)
        return self._sconsign

    def srcnode(self):
        """Dir has a special need for srcnode()...if we
        have a srcdir attribute set, then that *is* our srcnode."""
        if self.srcdir:
            return self.srcdir
        return Base.srcnode(self)

    def get_timestamp(self):
        """Return the latest timestamp from among our children"""
        stamp = 0
        for kid in self.children():
            if kid.get_timestamp() > stamp:
                stamp = kid.get_timestamp()
        return stamp

    def entry_abspath(self, name):
        return self.abspath + os.sep + name

    def entry_path(self, name):
        return self.path + os.sep + name

    def entry_tpath(self, name):
        return self.tpath + os.sep + name

    def must_be_a_Dir(self):
        """Called to make sure a Node is a Dir.  Since we're already
        one, this is a no-op for us."""
        return self

    def entry_exists_on_disk(self, name):
        """__cacheable__"""
        try:
            d = self.on_disk_entries
        except AttributeError:
            d = {}
            try:
                entries = os.listdir(self.abspath)
            except OSError:
                pass
            else:
                for entry in map(_my_normcase, entries):
                    d[entry] = 1
            self.on_disk_entries = d
        return d.has_key(_my_normcase(name))

    def srcdir_list(self):
        """__cacheable__"""
        result = []

        dirname = '.'
        dir = self
        while dir:
            if dir.srcdir:
                d = dir.srcdir.Dir(dirname)
                if d.is_under(dir):
                    # Shouldn't source from something in the build path:
                    # build_dir is probably under src_dir, in which case
                    # we are reflecting.
                    break
                result.append(d)
            dirname = dir.name + os.sep + dirname
            dir = dir.up()

        return result

    def srcdir_duplicate(self, name):
        for dir in self.srcdir_list():
            if dir.entry_exists_on_disk(name):
                srcnode = dir.File(name)
                if self.duplicate:
                    node = self.File(name)
                    node.do_duplicate(srcnode)
                    return node
                else:
                    return srcnode
        return None

    def srcdir_find_file(self, filename):
        """__cacheable__"""
        def func(node):
            if (isinstance(node, File) or isinstance(node, Entry)) and \
               (node.is_derived() or node.is_pseudo_derived() or node.exists()):
                    return node
            return None

        norm_name = _my_normcase(filename)

        for rdir in self.get_all_rdirs():
            try: node = rdir.entries[norm_name]
            except KeyError: node = rdir.file_on_disk(filename)
            else: node = func(node)
            if node:
                return node, self

        for srcdir in self.srcdir_list():
            for rdir in srcdir.get_all_rdirs():
                try: node = rdir.entries[norm_name]
                except KeyError: node = rdir.file_on_disk(filename)
                else: node = func(node)
                if node:
                    return File(filename, self, self.fs), srcdir

        return None, None

    def dir_on_disk(self, name):
        if self.entry_exists_on_disk(name):
            try: return self.Dir(name)
            except TypeError: pass
        return None

    def file_on_disk(self, name):
        if self.entry_exists_on_disk(name) or \
           diskcheck_rcs(self, name) or \
           diskcheck_sccs(self, name):
            try: return self.File(name)
            except TypeError: pass
        return self.srcdir_duplicate(name)

class RootDir(Dir):
    """A class for the root directory of a file system.

    This is the same as a Dir class, except that the path separator
    ('/' or '\\') is actually part of the name, so we don't need to
    add a separator when creating the path names of entries within
    this directory.
    """
    def __init__(self, name, fs):
        if __debug__: logInstanceCreation(self, 'Node.FS.RootDir')
        # We're going to be our own parent directory (".." entry and .dir
        # attribute) so we have to set up some values so Base.__init__()
        # won't gag won't it calls some of our methods.
        self.abspath = ''
        self.path = ''
        self.tpath = ''
        self.path_elements = []
        self.duplicate = 0
        Base.__init__(self, name, self, fs)

        # Now set our paths to what we really want them to be: the
        # initial drive letter (the name) plus the directory separator.
        self.abspath = name + os.sep
        self.path = name + os.sep
        self.tpath = name + os.sep
        self._morph()

    def __str__(self):
        return self.abspath

    def entry_abspath(self, name):
        return self.abspath + name

    def entry_path(self, name):
        return self.path + name

    def entry_tpath(self, name):
        return self.tpath + name

    def is_under(self, dir):
        if self is dir:
            return 1
        else:
            return 0

    def up(self):
        return None

    def get_dir(self):
        return None

    def src_builder(self):
        return _null

class FileNodeInfo(SCons.Node.NodeInfoBase):
    def __init__(self, node):
        SCons.Node.NodeInfoBase.__init__(self, node)
        self.update(node)
    def __cmp__(self, other):
        try: return cmp(self.bsig, other.bsig)
        except AttributeError: return 1
    def update(self, node):
        self.timestamp = node.get_timestamp()
        self.size = node.getsize()

class FileBuildInfo(SCons.Node.BuildInfoBase):
    def __init__(self, node):
        SCons.Node.BuildInfoBase.__init__(self, node)
        self.node = node
    def convert_to_sconsign(self):
        """Convert this FileBuildInfo object for writing to a .sconsign file

        We hung onto the node that we refer to so that we can translate
        the lists of bsources, bdepends and bimplicit Nodes into strings
        relative to the node, but we don't want to write out that Node
        itself to the .sconsign file, so we delete the attribute in
        preparation.
        """
        rel_path = self.node.rel_path
        delattr(self, 'node')
        for attr in ['bsources', 'bdepends', 'bimplicit']:
            try:
                val = getattr(self, attr)
            except AttributeError:
                pass
            else:
                setattr(self, attr, map(rel_path, val))
    def convert_from_sconsign(self, dir, name):
        """Convert a newly-read FileBuildInfo object for in-SCons use

        An on-disk BuildInfo comes without a reference to the node for
        which it's intended, so we have to convert the arguments and add
        back a self.node attribute.  We don't worry here about converting
        the bsources, bdepends and bimplicit lists from strings to Nodes
        because they're not used in the normal case of just deciding
        whether or not to rebuild things.
        """
        self.node = dir.Entry(name)
    def prepare_dependencies(self):
        """Prepare a FileBuildInfo object for explaining what changed

        The bsources, bdepends and bimplicit lists have all been stored
        on disk as paths relative to the Node for which they're stored
        as dependency info.  Convert the strings to actual Nodes (for
        use by the --debug=explain code and --implicit-cache).
        """
        Entry_func = self.node.dir.Entry
        for attr in ['bsources', 'bdepends', 'bimplicit']:
            try:
                val = getattr(self, attr)
            except AttributeError:
                pass
            else:
                setattr(self, attr, map(Entry_func, val))
    def format(self):
        result = [ self.ninfo.format() ]
        bkids = self.bsources + self.bdepends + self.bimplicit
        bkidsigs = self.bsourcesigs + self.bdependsigs + self.bimplicitsigs
        for i in xrange(len(bkids)):
            result.append(str(bkids[i]) + ': ' + bkidsigs[i].format())
        return string.join(result, '\n')

class NodeInfo(FileNodeInfo):
    pass

class BuildInfo(FileBuildInfo):
    pass

class File(Base):
    """A class for files in a file system.
    """

    NodeInfo = FileNodeInfo
    BuildInfo = FileBuildInfo

    def diskcheck_match(self):
        diskcheck_match(self, self.fs.isdir,
                           "Directory %s found where file expected.")

    def __init__(self, name, directory, fs):
        if __debug__: logInstanceCreation(self, 'Node.FS.File')
        Base.__init__(self, name, directory, fs)
        self._morph()

    def Entry(self, name):
        """Create an entry node named 'name' relative to
        the SConscript directory of this file."""
        return self.fs.Entry(name, self.cwd)

    def Dir(self, name):
        """Create a directory node named 'name' relative to
        the SConscript directory of this file."""
        return self.fs.Dir(name, self.cwd)

    def Dirs(self, pathlist):
        """Create a list of directories relative to the SConscript
        directory of this file."""
        return map(lambda p, s=self: s.Dir(p), pathlist)

    def File(self, name):
        """Create a file node named 'name' relative to
        the SConscript directory of this file."""
        return self.fs.File(name, self.cwd)

    def RDirs(self, pathlist):
        """Search for a list of directories in the Repository list."""
        return self.fs.Rfindalldirs(pathlist, self.cwd)

    #def generate_build_dict(self):
    #    """Return an appropriate dictionary of values for building
    #    this File."""
    #    return {'Dir' : self.Dir,
    #            'File' : self.File,
    #            'RDirs' : self.RDirs}

    def _morph(self):
        """Turn a file system node into a File object.  __cache_reset__"""
        self.scanner_paths = {}
        if not hasattr(self, '_local'):
            self._local = 0

    def scanner_key(self):
        return self.get_suffix()

    def get_contents(self):
        if not self.rexists():
            return ''
        return open(self.rfile().abspath, "rb").read()

    def get_timestamp(self):
        if self.rexists():
            return self.rfile().getmtime()
        else:
            return 0

    def store_info(self, obj):
        # Merge our build information into the already-stored entry.
        # This accomodates "chained builds" where a file that's a target
        # in one build (SConstruct file) is a source in a different build.
        # See test/chained-build.py for the use case.
        entry = self.get_stored_info()
        entry.merge(obj)
        self.dir.sconsign().set_entry(self.name, entry)

    def get_stored_info(self):
        "__cacheable__"
        try:
            stored = self.dir.sconsign().get_entry(self.name)
        except (KeyError, OSError):
            return self.new_binfo()
        else:
            if not hasattr(stored, 'ninfo'):
                # Transition:  The .sconsign file entry has no NodeInfo
                # object, which means it's a slightly older BuildInfo.
                # Copy over the relevant attributes.
                ninfo = stored.ninfo = self.new_ninfo()
                for attr in ninfo.__dict__.keys():
                    try:
                        setattr(ninfo, attr, getattr(stored, attr))
                    except AttributeError:
                        pass
            return stored

    def get_stored_implicit(self):
        binfo = self.get_stored_info()
        binfo.prepare_dependencies()
        try: return binfo.bimplicit
        except AttributeError: return None

    def rel_path(self, other):
        return self.dir.rel_path(other)

    def get_found_includes(self, env, scanner, path):
        """Return the included implicit dependencies in this file.
        Cache results so we only scan the file once per path
        regardless of how many times this information is requested.
        __cacheable__"""
        if not scanner:
            return []
        return scanner(self, env, path)

    def _createDir(self):
        # ensure that the directories for this node are
        # created.
        self.dir._create()

    def retrieve_from_cache(self):
        """Try to retrieve the node's content from a cache

        This method is called from multiple threads in a parallel build,
        so only do thread safe stuff here. Do thread unsafe stuff in
        built().

        Note that there's a special trick here with the execute flag
        (one that's not normally done for other actions).  Basically
        if the user requested a noexec (-n) build, then
        SCons.Action.execute_actions is set to 0 and when any action
        is called, it does its showing but then just returns zero
        instead of actually calling the action execution operation.
        The problem for caching is that if the file does NOT exist in
        cache then the CacheRetrieveString won't return anything to
        show for the task, but the Action.__call__ won't call
        CacheRetrieveFunc; instead it just returns zero, which makes
        the code below think that the file *was* successfully
        retrieved from the cache, therefore it doesn't do any
        subsequent building.  However, the CacheRetrieveString didn't
        print anything because it didn't actually exist in the cache,
        and no more build actions will be performed, so the user just
        sees nothing.  The fix is to tell Action.__call__ to always
        execute the CacheRetrieveFunc and then have the latter
        explicitly check SCons.Action.execute_actions itself.

        Returns true iff the node was successfully retrieved.
        """
        b = self.is_derived()
        if not b and not self.has_src_builder():
            return None
        if b and self.fs.CachePath:
            if self.fs.cache_show:
                if CacheRetrieveSilent(self, [], None, execute=1) == 0:
                    self.build(presub=0, execute=0)
                    return 1
            elif CacheRetrieve(self, [], None, execute=1) == 0:
                return 1
        return None

    def built(self):
        """Called just after this node is successfully built.
        __cache_reset__"""
        # Push this file out to cache before the superclass Node.built()
        # method has a chance to clear the build signature, which it
        # will do if this file has a source scanner.
        if self.fs.CachePath and self.exists():
            CachePush(self, [], None)
        self.fs.clear_cache()
        SCons.Node.Node.built(self)

    def visited(self):
        if self.fs.CachePath and self.fs.cache_force and self.exists():
            CachePush(self, None, None)

    def has_src_builder(self):
        """Return whether this Node has a source builder or not.

        If this Node doesn't have an explicit source code builder, this
        is where we figure out, on the fly, if there's a transparent
        source code builder for it.

        Note that if we found a source builder, we also set the
        self.builder attribute, so that all of the methods that actually
        *build* this file don't have to do anything different.
        """
        try:
            scb = self.sbuilder
        except AttributeError:
            if self.rexists():
                scb = None
            else:
                scb = self.dir.src_builder()
                if scb is _null:
                    if diskcheck_sccs(self.dir, self.name):
                        scb = get_DefaultSCCSBuilder()
                    elif diskcheck_rcs(self.dir, self.name):
                        scb = get_DefaultRCSBuilder()
                    else:
                        scb = None
                if scb is not None:
                    self.builder_set(scb)
            self.sbuilder = scb
        return not scb is None

    def alter_targets(self):
        """Return any corresponding targets in a build directory.
        """
        if self.is_derived():
            return [], None
        return self.fs.build_dir_target_climb(self, self.dir, [self.name])

    def is_pseudo_derived(self):
        "__cacheable__"
        return self.has_src_builder()

    def _rmv_existing(self):
        '__cache_reset__'
        Unlink(self, [], None)
        
    def prepare(self):
        """Prepare for this file to be created."""
        SCons.Node.Node.prepare(self)

        if self.get_state() != SCons.Node.up_to_date:
            if self.exists():
                if self.is_derived() and not self.precious:
                    self._rmv_existing()
            else:
                try:
                    self._createDir()
                except SCons.Errors.StopError, drive:
                    desc = "No drive `%s' for target `%s'." % (drive, self)
                    raise SCons.Errors.StopError, desc

    def remove(self):
        """Remove this file."""
        if self.exists() or self.islink():
            self.fs.unlink(self.path)
            return 1
        return None

    def do_duplicate(self, src):
        self._createDir()
        try:
            Unlink(self, None, None)
        except SCons.Errors.BuildError:
            pass
        try:
            Link(self, src, None)
        except SCons.Errors.BuildError, e:
            desc = "Cannot duplicate `%s' in `%s': %s." % (src.path, self.dir.path, e.errstr)
            raise SCons.Errors.StopError, desc
        self.linked = 1
        # The Link() action may or may not have actually
        # created the file, depending on whether the -n
        # option was used or not.  Delete the _exists and
        # _rexists attributes so they can be reevaluated.
        self.clear()

    def exists(self):
        "__cacheable__"
        # Duplicate from source path if we are set up to do this.
        if self.duplicate and not self.is_derived() and not self.linked:
            src = self.srcnode()
            if src is self:
                return Base.exists(self)
            # At this point, src is meant to be copied in a build directory.
            src = src.rfile()
            if src.abspath != self.abspath:
                if src.exists():
                    self.do_duplicate(src)
                    # Can't return 1 here because the duplication might
                    # not actually occur if the -n option is being used.
                else:
                    # The source file does not exist.  Make sure no old
                    # copy remains in the build directory.
                    if Base.exists(self) or self.islink():
                        self.fs.unlink(self.path)
                    # Return None explicitly because the Base.exists() call
                    # above will have cached its value if the file existed.
                    return None
        return Base.exists(self)

    #
    # SIGNATURE SUBSYSTEM
    #

    def get_csig(self, calc=None):
        """
        Generate a node's content signature, the digested signature
        of its content.

        node - the node
        cache - alternate node to use for the signature cache
        returns - the content signature
        """
        try:
            return self.binfo.ninfo.csig
        except AttributeError:
            pass

        if calc is None:
            calc = self.calculator()

        max_drift = self.fs.max_drift
        mtime = self.get_timestamp()
        use_stored = max_drift >= 0 and (time.time() - mtime) > max_drift

        csig = None
        if use_stored:
            old = self.get_stored_info().ninfo
            try:
                if old.timestamp and old.csig and old.timestamp == mtime:
                    csig = old.csig
            except AttributeError:
                pass
        if csig is None:
            csig = calc.module.signature(self)

        binfo = self.get_binfo()
        ninfo = binfo.ninfo
        ninfo.csig = csig
        ninfo.update(self)

        if use_stored:
            self.store_info(binfo)

        return csig

    #
    #
    #

    def is_up_to_date(self, node=None, bi=None):
        """Returns if the node is up-to-date with respect to stored
        BuildInfo.  The default is to compare it against our own
        previously stored BuildInfo, but the stored BuildInfo from another
        Node (typically one in a Repository) can be used instead."""
        if bi is None:
            if node is None:
                node = self
            bi = node.get_stored_info()
        new = self.get_binfo()
        return new == bi

    def current(self, calc=None):
        self.binfo = self.gen_binfo(calc)
        return self._cur2()
    def _cur2(self):
        "__cacheable__"
        if self.always_build:
            return None
        if not self.exists():
            # The file doesn't exist locally...
            r = self.rfile()
            if r != self:
                # ...but there is one in a Repository...
                if self.is_up_to_date(r):
                    # ...and it's even up-to-date...
                    if self._local:
                        # ...and they'd like a local copy.
                        LocalCopy(self, r, None)
                        self.store_info(self.get_binfo())
                    return 1
            return None
        else:
            return self.is_up_to_date()

    def rfile(self):
        "__cacheable__"
        if not self.exists():
            norm_name = _my_normcase(self.name)
            for dir in self.dir.get_all_rdirs():
                try: node = dir.entries[norm_name]
                except KeyError: node = dir.file_on_disk(self.name)
                if node and node.exists() and \
                   (isinstance(node, File) or isinstance(node, Entry) \
                    or not node.is_derived()):
                        return node
        return self

    def rstr(self):
        return str(self.rfile())

    def cachepath(self):
        if not self.fs.CachePath:
            return None, None
        ninfo = self.get_binfo().ninfo
        if not hasattr(ninfo, 'bsig'):
            raise SCons.Errors.InternalError, "cachepath(%s) found no bsig" % self.path
        elif ninfo.bsig is None:
            raise SCons.Errors.InternalError, "cachepath(%s) found a bsig of None" % self.path
        # Add the path to the cache signature, because multiple
        # targets built by the same action will all have the same
        # build signature, and we have to differentiate them somehow.
        cache_sig = SCons.Sig.MD5.collect([ninfo.bsig, self.path])
        subdir = string.upper(cache_sig[0])
        dir = os.path.join(self.fs.CachePath, subdir)
        return dir, os.path.join(dir, cache_sig)

    def must_be_a_Dir(self):
        """Called to make sure a Node is a Dir.  Since we're already a
        File, this is a TypeError..."""
        raise TypeError, "Tried to lookup File '%s' as a Dir." % self.path

default_fs = None

def find_file(filename, paths, verbose=None):
    """
    find_file(str, [Dir()]) -> [nodes]

    filename - a filename to find
    paths - a list of directory path *nodes* to search in.  Can be
            represented as a list, a tuple, or a callable that is
            called with no arguments and returns the list or tuple.

    returns - the node created from the found file.

    Find a node corresponding to either a derived file or a file
    that exists already.

    Only the first file found is returned, and none is returned
    if no file is found.
    __cacheable__
    """
    if verbose:
        if not SCons.Util.is_String(verbose):
            verbose = "find_file"
        if not callable(verbose):
            verbose = '  %s: ' % verbose
            verbose = lambda s, v=verbose: sys.stdout.write(v + s)
    else:
        verbose = lambda x: x

    if callable(paths):
        paths = paths()

    # Give Entries a chance to morph into Dirs.
    paths = map(lambda p: p.must_be_a_Dir(), paths)

    filedir, filename = os.path.split(filename)
    if filedir:
        def filedir_lookup(p, fd=filedir):
            try:
                return p.Dir(fd)
            except TypeError:
                # We tried to look up a Dir, but it seems there's already
                # a File (or something else) there.  No big.
                return None
        paths = filter(None, map(filedir_lookup, paths))

    for dir in paths:
        verbose("looking for '%s' in '%s' ...\n" % (filename, dir))
        node, d = dir.srcdir_find_file(filename)
        if node:
            verbose("... FOUND '%s' in '%s'\n" % (filename, d))
            return node
    return None

def find_files(filenames, paths):
    """
    find_files([str], [Dir()]) -> [nodes]

    filenames - a list of filenames to find
    paths - a list of directory path *nodes* to search in

    returns - the nodes created from the found files.

    Finds nodes corresponding to either derived files or files
    that exist already.

    Only the first file found is returned for each filename,
    and any files that aren't found are ignored.
    """
    nodes = map(lambda x, paths=paths: find_file(x, paths), filenames)
    return filter(None, nodes)

#!/usr/bin/env python
# :noTabs=true: :collapseFolds=1:

# (c) Copyright Rosetta Commons Member Institutions.
# (c) This file is part of the Rosetta software suite and is made available under license.
# (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
# (c) For more information, see http://www.rosettacommons.org. Questions about this can be
# (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

## @file   BuildBuindings.py
## @brief  Build Python buidings for mini
## @author Sergey Lyskov

import os, re, sys, time, commands, shutil, platform, os.path, itertools, gc

#from BuildModule import buildModule

from pyplusplus import module_builder
import pyplusplus, tools.DoxygenExtractorPyPP

import exclude

import tools.CppParser



from optparse import OptionParser, IndentedHelpFormatter

# Create global 'Platform' that will hold info of current system
if sys.platform.startswith("linux"): Platform = "linux" # can be linux1, linux2, etc
elif sys.platform == "darwin" : Platform = "macos"
elif sys.platform == "cygwin" : Platform = "cygwin"
else: Platform = "_unknown_"
PlatformBits = platform.architecture()[0][:2]


# global include dict 'file' --> bool;  True mean include file, False - exclude
# if key is not present that mean that this file/names space is new, it will be excluded and
#    added to IncludeDict.new
IncludeDict = None  # we set it on purpose to None instead of {}, this should cath uninitialize errors

#  Here we will store new files and dirs. This will be saved to 'IncludeDict.new' in the end.
IncludeDictNew = {}

Jobs = []  # Global list of spawned process pid's


def main(args):
    ''' Script to build mini Python buidings.
    '''
    parser = OptionParser(usage="usage: %prog [OPTIONS] [TESTS]")
    parser.set_description(main.__doc__)

    parser.add_option('-I',
      default=[],
      action="append",
      help="Additiona path to include (boost, python etc). Specify it only if this libs installed in non standard locations. May specify multiple times.",
    )

    parser.add_option('-L',
      default=[],
      action="append",
      help="Additiona path to libraries (boost, python etc). Specify it only if this libs installed in non standard locations. May specify multiple times.",
    )

    parser.add_option("-1", "--one",
      default=[], action="append",
      help="Build just one namespace instead of whole project, can be specified multiple times.",
    )

    parser.add_option("--BuildMiniLibs",
      default=True,
      )

    parser.add_option("--update",
      default=False,
      help="Debug only. Try to check time stamp of files before building them.",
      )

    parser.add_option("-u",
      action="store_true", dest="update",
      help="Debug only. Try to check time stamp of files before building them.",
      )


    parser.add_option("-d",
      action="store_false", dest="BuildMiniLibs",
      help="Disable building of mini libs.",
    )

    parser.add_option("--continue",
      default=True,
      action="store_false", dest="continue_",
      help="Debug only. Continue building after encounter the error.",
      )

    parser.add_option("--all", default=True,
      action="store_true", dest="build_all",
      help="Experimental. Build bindings for all source avaliable.",
      )

    parser.add_option("--partial",
      action="store_false", dest="build_all",
      help="Build only limited subsett of py-bindings.",
      )

    parser.add_option("--gccxml",
      default='gccxml',
      action="store",
      help="Path to gccxml executable. Default is just 'gccxml'.",
      )

    parser.add_option("--compiler",
      default='gcc',
      action="store",
      help="Default compiler that will be used to build PyRosetta. Default is 'gcc'.",
      )


    parser.add_option("--py-plus-plus", action="store_true", dest="py_plus_plus", default=False,
      help="Use Py++/PyGccXML parser and Python buildes to build PyRosetta [depricated]."
    )

    parser.add_option("--boost_lib",
      default='boost_python-xgcc40-mt-1_38',
      action="store",
      help="Name of boost dynamic library.",
      )

    parser.add_option("--python_lib",
      default='python2.5',
      action="store",
      help="Name of python dynamic library.",
      )

    parser.add_option("--update-IncludeDict",
                      action="store_true", dest='sort_IncludeDict', default=False,
      help="Developers only. Save sorter IncludeDict back to file.",
      )

    parser.add_option("--one-lib-file", action="store_true", dest="one_lib_file", default=True,
      help="Generate only one lib file for name spaces [experimental]."
    )

    parser.add_option("--many-lib-files", action="store_false", dest="one_lib_file",
      help="Generate only one lib file for name spaces [experimental]."
    )


    parser.add_option("--one-lib-max-files", default=0, type='int',
      help="Maximum number of files that goes in to one lib file. Default is unlimeted [0]."
    )

    parser.add_option("-j", "--jobs",
      default=1,
      type="int",
      help="Number of processors to use on when building(default: 1)",
    )

    (options, args) = parser.parse_args(args=args[1:])
    global Options;  Options = options

    print "-I", options.I
    print "-L", options.L
    print "-1", options.one
    print "--BuildMiniLibs", options.BuildMiniLibs
    print "--gccxml", options.gccxml
    print "--all", options.build_all
    print "--one-lib-file", options.one_lib_file
    print "--boost_lib", options.boost_lib
    print '--update', options.update

    if (Options.jobs > 1) and Options.sort_IncludeDict:
        print 'Option --update-IncludeDict could be used only with -j1... exiting...'
        return 1

    # assuming that we in mini/src/python/bindings directory at that point
    mini_path = os.path.abspath('./../../../')

    bindings_path = os.path.abspath('./rosetta')
    if not os.path.isdir(bindings_path): os.makedirs(bindings_path)
    shutil.copyfile('src/__init__.py', 'rosetta/__init__.py')

    if options.BuildMiniLibs:
        prepareMiniLibs(mini_path, bindings_path)

    os.chdir( './../../' )


    # loadind include file/namespaces dict
    global IncludeDict
    IncludeDict = eval( file('python/bindings/IncludeDict').read() )
    if type( IncludeDict['core'] ) == str:
        for k in IncludeDict: IncludeDict[k] = ({'+':True, '-':False}[IncludeDict[k]], 999, [])


    if options.one:
        print 'Building just following namespaces:', options.one
        for n in options.one:
            print 'n=', n
            buildModule(n, bindings_path, include_paths=options.I, libpaths=options.L, runtime_libpaths=options.L, gccxml_path=options.gccxml)

    else:
        buildModules('utility',   bindings_path, include_paths=options.I, libpaths=options.L, runtime_libpaths=options.L, gccxml_path=options.gccxml)
        buildModules('numeric',   bindings_path, include_paths=options.I, libpaths=options.L, runtime_libpaths=options.L, gccxml_path=options.gccxml)
        buildModules('basic',     bindings_path, include_paths=options.I, libpaths=options.L, runtime_libpaths=options.L, gccxml_path=options.gccxml)
        buildModules('core',      bindings_path, include_paths=options.I, libpaths=options.L, runtime_libpaths=options.L, gccxml_path=options.gccxml)
        buildModules('protocols', bindings_path, include_paths=options.I, libpaths=options.L, runtime_libpaths=options.L, gccxml_path=options.gccxml)

    error = False
    for p in Jobs:
        try:
            r = os.waitpid(p, 0)  # waiting for all child process to termintate...
            if r[1] :  # process ended but with error, special case we will have to wait for all process to terminate and call system exit.
                error = True

        except OSError:
            error = True


    if error:
        print '~Some of the build scripts return an error, PyRosetta build failed!'
        sys.exit(1)




    #buildModule(, bindings_path, include_paths=options.I, libpaths=options.L, runtime_libpaths=options.L)
    #buildModule('core/io/pdb', bindings_path, include_paths=options.I, libpaths=options.L, runtime_libpaths=options.L)

    #buildModule('core/kinematics', bindings_path, include_paths=options.I, libpaths=options.L, runtime_libpaths=options.L)


    #buildModule('core/conformation', bindings_path, include_paths=options.I,
    #            libpaths=options.L, runtime_libpaths=options.L)


    updateList = [ (IncludeDictNew,'IncludeDict.new') ]

    if options.sort_IncludeDict:
        updateList.append( (IncludeDict,'IncludeDict') )

    for dc, fl in updateList:
        print 'Updating include dictionary file %s...' % fl

        def f_cmp(a, b):
            a_, b_ = a.split('/'), b.split('/')
            if cmp( a_[:-1], b_[:-1] ) : return cmp( a_[:-1], b_[:-1] )
            elif dc[a][1] == dc[b][1]: return cmp( a_[-1], b_[-1] )
            else: return cmp(dc[a][1], dc[b][1])


        K = dc.keys();  K.sort(cmp=f_cmp)
        f = file('python/bindings/' + fl + '.tmp', 'w');  f.write('#(True/False - include/exclude file/dir, priority, dependency)\n{\n')
        for k in K:
            #f.write( '%s : %s,\n' % (repr(k), repr({True:'+', False:'-'}[ dc[k] ]) ) )
            #if dc[k][2] == []: dc[k] = (dc[k][0], dc[k][1], None)
            f.write( '%s : %s,\n' % (repr(k), repr(dc[k]) ) )

        f.write('}\n');  f.close()
        os.rename('python/bindings/' + fl + '.tmp', 'python/bindings/' + fl)


    print "Done!"



def execute(message, commandline, return_=False):
    print message
    print commandline
    (res, output) = commands.getstatusoutput(commandline)
    print output
    if res:
        print "\nEncounter error while executing: ", commandline
        if return_: return True
        else: sys.exit(1)

    return False


def mfork():
    ''' Check if number of child process is below Options.jobs. And if it is - fork the new pocees and return its pid.
    '''
    while len(Jobs) >= Options.jobs :
        for p in Jobs[:] :
            r = os.waitpid(p, os.WNOHANG)
            if r == (p, 0):  # process have ended without error
                Jobs.remove(p)
            elif r[0] == p :  # process ended but with error, special case we will have to wait for all process to terminate and call system exit.
                for p in Jobs:
                    try:
                        os.waitpid(p, 0)
                    except OSError: pass

                print 'Some of the build scripts return an error, PyRosetta build failed!'
                sys.exit(1)

        if len(Jobs) >= Options.jobs: time.sleep(.2)
    pid = os.fork()
    if pid: Jobs.append(pid) # We are parent!
    return pid

def getCompilerOptions():
    #if Platform == 'linux':
    if Platform != 'macos':
        add_option = '-ffloat-store -ffor-scope'
        if  PlatformBits == '32': add_option += ' -malign-double'
        else: add_option += ' -fPIC'
    else:
        if Options.compiler == 'clang': add_option = '-pipe -O3 -ffast-math -funroll-loops -finline-functions -fPIC'
        else: add_option = '-pipe -ffor-scope -O3 -ffast-math -funroll-loops -finline-functions -finline-limit=20000 -s -fPIC'
    #if Platform == 'cygwin' : add_option =''
    add_option += ' -DBOOST_PYTHON_MAX_ARITY=20'
    return add_option


def getLinkerOptions():
    ''' Return appropriate linking options based on platform info
    '''
    add_loption = ''
    #if Platform == 'linux':
    if Platform != 'macos':  # Linux and cygwin...
        add_loption += '-shared'
        #if PlatformBits == '32' and Platform != 'cygwin': add_loption += ' -malign-double'
        if PlatformBits == '32' : add_loption += ' -malign-double'
    else: add_loption = '-dynamiclib -Xlinker -headerpad_max_install_names'

    return add_loption




def buildModules(path, dest, include_paths, libpaths, runtime_libpaths, gccxml_path):
    ''' recursive build buinding for given dir name, and store them in dest.
    '''
    def visit(arg, dir_name, names):
        if dir_name.find('.svn') >= 0: return  # exclude all svn related namespaces

        # temp  for generating original exclude list
        #IncludeDict[dir_name] = not exclude.namespace(dir_name)

        #if exclude.namespace(dir_name): return

        if Options.build_all:
            if exclude.isBanned(dir_name):
                print 'Dir %s is banned! Skipping...' % dir_name
                return
        else:
            if dir_name in IncludeDict:
                if not IncludeDict[dir_name][0]:
                    print 'Skipping dir %s...' % dir_name
                    return
            else:
                print "Skipping new dir", dir_name
                IncludeDictNew[dir_name] = (False, 999, [])
                return



        print "buildModules(...): '%s', " % dir_name
        print "Directory: ", dir_name
        #dname = dest+'/' + os.path.dirname(dir_name)
        dname = dest+'/' + dir_name
        if not os.path.isdir(dname): os.makedirs(dname)
        if Options.jobs > 1:
            pid = mfork()
            if not pid:  # we are child process
                buildModule(dir_name, dest, include_paths, libpaths, runtime_libpaths, gccxml_path)
                sys.exit(0)

        else:
            IncludeDictNew.update( buildModule(dir_name, dest, include_paths, libpaths, runtime_libpaths, gccxml_path) )

    os.path.walk(path, visit, None)



def prepareMiniLibs(mini_path, bindings_path):
    #execute("Building mini libraries...", "cd %s && ./scons.py -j1 bin" % mini_path)
    # static added becuse it link faster, we dont really use libs, we just want scons to build all files...
    if Platform == "macos" and PlatformBits=='32': execute("Building mini libraries...", "cd %s && ./scons.py mode=pyrosetta arch=x86 arch_size=32 -j%s" % (mini_path, Options.jobs) )
    elif Platform == "macos" and PlatformBits=='64': execute("Building mini libraries...", "cd %s && ./scons.py mode=pyrosetta -j%s" % (mini_path, Options.jobs) )
    elif Platform == "cygwin": execute("Building mini libraries...", "cd %s && ./scons.py mode=pyrosetta bin -j%s" % (mini_path, Options.jobs) )
    else: execute("Building mini libraries...", "cd %s && ./scons.py mode=pyrosetta -j%s" % (mini_path, Options.jobs) )

    # fix this for diferent platform
    if Platform == "linux": lib_path = 'build/src/pyrosetta/linux/' + platform.release()[:3] + '/' + PlatformBits +'/x86/gcc/'
    elif Platform == "cygwin": lib_path = 'build/src/pyrosetta/cygwin/1.7/32/x86/gcc/'
    else:
        if Platform == "macos" and PlatformBits=='32': lib_path = 'build/src/pyrosetta/macos/10.5/32/x86/gcc/'
        if Platform == "macos" and PlatformBits=='64':
            if platform.release()[:2] == '10': lib_path = 'build/src/pyrosetta/macos/10.6/64/x86/gcc/'
            else: lib_path = 'build/src/pyrosetta/macos/10.7/64/x86/gcc/'

        #if Platform == "macos" and PlatformBits=='64'  and  platform.release().startswith('11.'): lib_path = 'build/src/pyrosetta/macos/11/64/x86/gcc/'
        #else: lib_path = 'build/src/pyrosetta/macos/10.6/64/x86/gcc/'

    #lib_path += 'static/'
    obj_suffix = '.os'
    #if Platform == "linux"  and  PlatformBits == '64': obj_suffix = '.os'

    # Now the funny part - we rebuild all libs to produce just one lib file...
    all_sources = []
    for scons_file in ['ObjexxFCL', 'utility', 'numeric', 'basic', 'core.1', 'core.2', 'core.3', 'core.4', 'core.5', 'protocols']:
    #for scons_file in ['utility']:
    #for scons_file in ['ObjexxFCL', 'numeric', 'utility',]:
        f = file('./../../%s.src.settings' % scons_file).read();  exec(f)
        for k in sources:
            for f in sources[k]:
                #all_sources.append( scons_file + '/' + k + '/' + f + obj_suffix)
                all_sources.append( k + '/' + f + obj_suffix)

    #all_sources.remove('protocols/forge/remodel/RemodelDesignMover' + obj_suffix) # <-- I have no idea what gcc does not like this file...

    extra_objs = [  # additioanal source for external libs
        'dbio/cppdb/atomic_counter', "dbio/cppdb/conn_manager", "dbio/cppdb/driver_manager", "dbio/cppdb/frontend",
        "dbio/cppdb/backend", "dbio/cppdb/mutex", "dbio/cppdb/pool", "dbio/cppdb/shared_object", "dbio/cppdb/sqlite3_backend",
        "dbio/cppdb/utils", 'dbio/sqlite3/sqlite3', ]

    all_sources += [ mini_path + '/' + lib_path.replace('/src/', '/external/') + x + obj_suffix for x in extra_objs ]

    objs = ' '.join(all_sources)

    suffix = 'so'
    if Platform == 'cygwin' : suffix = 'dll'
    if Platform == 'macos' : suffix = 'dylib'

    mini = mini_path + '/src/python/bindings/rosetta/libmini.' + suffix

    add_loption = getLinkerOptions()

    execute("Linking mini lib...",
            "cd %(mini_path)s && cd %(lib_path)s && gcc %(add_loption)s \
            %(objs)s -lz -lstdc++ -o %(mini)s" % dict(mini_path=mini_path, lib_path=lib_path, add_loption=add_loption, mini=mini, objs=objs, compiler=Options.compiler)
             )

    #if Platform == 'cygwin':
    #        execute("cp libs...", "cd %s && cp %s/*.dll %s" % (mini_path, lib_path, bindings_path) )
    #else:
    #        execute("cp libs...", "cd %s && cp %s/lib* %s" % (mini_path, lib_path, bindings_path) )

    if Platform == 'macos':
        #libs = ['libObjexxFCL.dylib', 'libnumeric.dylib', 'libprotocols.dylib', 'libdevel.dylib', 'libutility.dylib', 'libcore.dylib']
        libs = ['libmini.dylib']
        for l in libs:
            execute('Adjustin lib self path in %s' % l, 'install_name_tool -id rosetta/%s %s' % (l, bindings_path+'/'+l) )
            for k in libs:
                execute('Adjustin lib path in %s' % l, 'install_name_tool -change %s rosetta/%s %s' % (os.path.abspath(mini_path+'/'+lib_path+k), k, bindings_path+'/'+l) )

    shutil.copyfile(bindings_path+'/../src/__init__.py' , bindings_path+'/__init__.py')


_TestInludes_ = ''

def buildModule(path, dest, include_paths, libpaths, runtime_libpaths, gccxml_path):
    ''' Build one namespace and return dict of newly found heades.
    '''
    testName = 'python/bindings/TestIncludes.py'
    global _TestInludes_
    _TestInludes_ += 'import rosetta.%s\n' % path.replace('/', '.')
    f = file(testName, 'w');  f.write(_TestInludes_);  f.close()

    gc.collect()

    if Options.py_plus_plus:
        #if path == 'core':
        #    buildModule_All(path, dest, include_paths, libpaths, runtime_libpaths, gccxml_path)
        #else: buildModule_One(path, dest, include_paths, libpaths, runtime_libpaths, gccxml_path)
        return buildModule_One(path, dest, include_paths, libpaths, runtime_libpaths, gccxml_path)

    else:
        #print 'Using Clang...'
        return buildModule_UsingCppParser(path, dest, include_paths, libpaths, runtime_libpaths, gccxml_path)


def buildModule_UsingCppParser(path, dest, include_paths, libpaths, runtime_libpaths, gccxml_path):
    ''' Non recursive build buinding for given dir name, and store them in dest.
        path - relative path to namespace
        dest - path to root file destination, actual dest will be dest + path

        return dict of a newly found headers.
    '''
    print 'CppXML route: Processing', path, dest

    global Options

    # Creating list of headers
    if os.path.isfile(path): headers = [path]
    else:  headers = [os.path.join(path, d)
                        for d in os.listdir(path)
                            if os.path.isfile( os.path.join(path, d) )
                                and d.endswith('.hh')
                                and not d.endswith('.fwd.hh')
                                ]

    #headers.sort()
    # now, because of abstract class issue we have to sort header list first...
    headers.sort(key = lambda x: IncludeDict.get(x, (None, 999, None) )[1])

    # tmp  for generating original exclude list
    #for i in headers: IncludeDict[i] = True

    new_headers = {}
    for h in headers[:]:

        if Options.build_all:
            if exclude.isBanned(h):
                print "Banning header:", h
                headers.remove(h)

            continue  # do not exclude anything...

        if h in IncludeDict:
            #print 'IncludeDict[%s] --> %s' % (h, IncludeDict[h])
            if not IncludeDict[h][0]:
                print "Excluding header:", h
                headers.remove(h)

        else:
            print "Excluding new header:", h
            headers.remove(h)
            new_headers[h] = (False, 999, [])

    # Temporary injecting Mover in to protocols level
    #if path == 'protocols': headers.insert(0, 'protocols/moves/Mover.hh')

    print headers

    fname_base = dest + '/' + path

    if not os.path.isdir(fname_base): os.makedirs(fname_base)

    print 'Creating __init__.py file...'
    f = file( dest + '/' + path + '/__init__.py', 'w');  f.close()
    if not headers:  return new_headers  # if source files is empty then __init__.py should be empty too
    def finalize_init(current_fname):
            #print 'Finalizing Creating __init__.py file...'
            f = file( dest + '/' + path + '/__init__.py', 'a');
            f.write('from %s import *\n' % os.path.basename(current_fname)[:-3]);
            f.close()


    include_paths = ' -I'.join( [''] + include_paths + ['../src/platform/linux' , '../src'] )

    nsplit = 1  # 1 files per iteration

    libpaths = ' -L'.join( ['', dest] + libpaths )
    runtime_libpaths = ' -Xlinker -rpath '.join( [''] + runtime_libpaths + ['rosetta'] )
    #if Platform == 'cygwin': runtime_libpaths = ' '

    cpp_defines = '-DPYROSETTA -DPYROSETTA_DISABLE_LCAST_COMPILE_TIME_CHECK -DBOOST_NO_INITIALIZER_LISTS'

    cc_files = []
    add_option  = getCompilerOptions()
    #if Options.compiler != 'gcc': add_option += ' -Wno-local-type-template-args'
    add_loption = getLinkerOptions()

    by_hand_code = dest+ '/../src/' + path + '/_%s__by_hand.cc' % path.split('/')[-1]

    if Options.one_lib_file:
        all_at_once_base = '__' + path.split('/')[-1] + '_all_at_once_'
        all_at_once_source_cpp = fname_base + '/' + all_at_once_base + '.source.cc'
        all_at_once_cpp = fname_base + '/' + all_at_once_base + '.'
        all_at_once_obj = fname_base + '/' + all_at_once_base + '.'
        all_at_once_xml = fname_base + '/' + all_at_once_base + '.xml'
        all_at_once_lib = fname_base + '/' + all_at_once_base + '.so'
        if Platform == 'cygwin': all_at_once_lib = all_at_once_lib = fname_base + '/' + all_at_once_base + '.dll'
        all_at_once_relative_files = []
        xml_recompile = False

    for fl in headers:
        print '\033[32m\033[1m%s\033[0m' % fl

        #print 'Binding:', files
        hbase = fl.split('/')[-1][:-3]
        hbase = hbase.replace('.', '_')
        #print 'hbase = ', hbase
        #if hbase == 'init': hbase='tint'  # for some reason Boost don't like 'init' name ? by hand?

        fname =     fname_base + '/' + '_' + hbase + '.cc'
        inc_name =  fname_base + '/' + '_' + hbase + '.hh'
        obj_name =  fname_base + '/' + '_' + hbase + '.o'
        xml_name =  fname_base + '/' + '_' + hbase + '.xml'
        cc_for_xml_name =  fname_base + '/' + '_' + hbase + '.xml.cpp'
        dst_name =  fname_base + '/' + '_' + hbase + '.so'
        if Platform == 'cygwin' : dst_name =  fname_base + '/' + '_' + hbase + '.dll'
        decl_name = fname_base + '/' + '_' + hbase + '.exposed_decl.pypp.txt'

        cc_files.append(fname)

        if Options.update:
            try:
                if fl == headers[0]:  # for first header we additionaly check if 'by_hand' code is up to date
                    #print '__checking for:', by_hand_code
                    if os.path.isfile(by_hand_code) and os.path.getmtime(by_hand_code) > os.path.getmtime(all_at_once_lib):
                        raise os.error

                if Options.one_lib_file:
                    if os.path.getmtime(fl) > os.path.getmtime(all_at_once_lib):
                        #print 'xml_recompile = True'

                        xml_recompile = True
                    else:
                        print 'File: %s is up to date - skipping' % fl
                        #finalize_init(fname)
                        #continue
                else:
                    if os.path.getmtime(fl) < os.path.getmtime(dst_name):
                        print 'File: %s is up to date - skipping' % fl
                        finalize_init(fname)
                        continue
            except os.error: xml_recompile = True

        source_fwd_hh = fl.replace('.hh', '.fwd.hh')
        source_hh = fl
        source_cc = fl.replace('.hh', '.cc')

        if Options.one_lib_file:  # just collecting file names...
            all_at_once_relative_files.extend( [source_fwd_hh, source_hh, source_cc] )
            continue


        source_to_parse = fl.replace('.hh', '.cc')
        if not os.path.isfile( source_cc ):  # there is no cc file... ^_^ - generating it on the fly... (clang dont like .hh etc)
            _h = file(cc_for_xml_name, 'w');  _h.write('#include <%s>\n' % fl);  _h.close()
            source_cc = cc_for_xml_name
        # -cc1 -ast-print-xml
        #execute('pwd...', 'pwd' ) # -I../external/include  -I.
        #execute('Generating XML representation...', 'clang++ -cc1 -ast-print-xml %s  ' % (cc_original,) )

        # Clang++ version
        #execute('Generating XML representation...', 'clang++ -cc1 -ast-print-xml %s -o %s -I. -I../src/platform/linux  -I../external/include -I../external/boost_1_46_1 ' % (source_cc, xml_name) )

        # we need  -DBOOST_NO_INITIALIZER_LISTS or gccxml choke on protocols/genetic_algorithm/GeneticAlgorithm.hh
        # GCCXML version
        # -DPYROSETTA
        if execute('Generating XML representation...', 'gccxml  %s -fxml=%s %s -I. -I../src/platform/linux  -I../external/include -I../external/boost_1_46_1 -I../external/dbio ' % (source_hh, xml_name, cpp_defines), not Options.continue_): continue

        namespaces_to_wrap = ['::'+path.replace('/', '::')+'::']
        # Temporary injecting Mover in to protocols level
        #if path == 'protocols': namespaces_to_wrap.append('::protocols::moves::')

        code = tools.CppParser.parseAndWrapModule('_'+hbase, namespaces_to_wrap,  xml_name, [source_fwd_hh, source_hh, source_cc])
        if len(code) != 1:
            print 'Whats going on???'
            sys.exit(1)
        else: code = code[0]

        #code = '#include <%s>\n' % fl + code

        f = file(fname, 'w');  f.write(code);  f.close()

        #mb.build_code_creator( module_name = '_'+hbase, doc_extractor=doxygen.doxygen_doc_extractor() )
        #mb.code_creator.user_defined_directories.append( os.path.abspath('.') )  # make include relative
        #mb.write_module( os.path.join( os.path.abspath('.'), fname ) )

        exclude.finalize(fname, dest, path, None, module_name='_'+hbase, add_by_hand = (fl==headers[0]), files=[fl], add_includes=True)
        finalize_init(fname)

        if not Options.one_lib_file:
            if execute("Compiling...", # -fPIC
                "%(compiler)s %(fname)s -o %(obj_name)s -c \
                 %(add_option)s -I../external/include -I../external/dbio \
                 %(include_paths)s " % dict(add_option=add_option, fname=fname, obj_name=obj_name, include_paths=include_paths, compiler=Options.compiler),
                 not Options.continue_):
                pass
                '''
                print 'Compiliation failed... Creating empty C++ file to test includes...'

                fname =    fname_base + '/' + '_0_.cc'
                obj_name = fname_base + '/' + '_0_.o'
                f = file(fname, 'w');  f.write('#include <%s>\n' % fl);  f.close()
                execute("Compiling empty C++...",
                    "gcc %(fname)s -o %(obj_name)s -c \
                     %(add_option)s -I../external/include \
                     %(include_paths)s " % dict(add_option=add_option, fname=fname, obj_name=obj_name, include_paths=include_paths))
                '''
                #sys.exit(1)

            execute("Linking...", # -fPIC -ffloat-store -ffor-scope
                "cd %(dest)s/../ && %(compiler)s %(obj)s %(add_option)s  \
                -lmini -lstdc++ -lz \
                 -l%(python_lib)s \
                 -l%(boost_lib)s \
                 %(libpaths)s %(runtime_libpaths)s -o %(dst)s" % dict(add_option=add_loption, obj=obj_name,
                    dst=dst_name, libpaths=libpaths, runtime_libpaths=runtime_libpaths, dest=dest, boost_lib=Options.boost_lib,
                    python_lib=Options.python_lib, compiler=Options.compiler
                    ),
                 not Options.continue_)

    if Options.one_lib_file:
        f = file(all_at_once_source_cpp, 'w');
        for fl in headers: f.write('#include <%s>\n' % fl);
        f.close()

        #hbase = all_at_once_cpp.split('/')[-1][:-3]
        #hbase = hbase.replace('.', '_')

        #print 'Finalizing Creating __init__.py file...'
        namespace = os.path.basename(path)
        py_init_file = dest + '/../src/' + path + '/__init__.py'
        if os.path.isfile(py_init_file): t = file(py_init_file).read()
        else: t = ''
        f = file( dest + '/' + path + '/__init__.py', 'w');  f.write(t+'from %s import *\n' % all_at_once_base);  f.close()

        if xml_recompile or (not Options.update):

            if execute('Generating XML representation...', 'gccxml  %s -fxml=%s %s -I. -I../src/platform/linux  -I../external/include -I../external/boost_1_46_1  -I../external/dbio ' % (all_at_once_source_cpp, all_at_once_xml, cpp_defines), not Options.continue_ ):
                return new_headers

            namespaces_to_wrap = ['::'+path.replace('/', '::')+'::']
            # Temporary injecting Mover in to protocols level
            #if path == 'protocols': namespaces_to_wrap.append('::protocols::moves::')

            code = tools.CppParser.parseAndWrapModule(all_at_once_base, namespaces_to_wrap,  all_at_once_xml, all_at_once_relative_files, max_funcion_size=1024*128)

            objs_list = []
            for i in range( len(code) ):
                all_at_once_N_cpp = all_at_once_cpp+'%s.cpp' % i
                all_at_once_N_obj = all_at_once_obj+'%s.o' % i


                for fl in headers:
                    code[i] = '#include <%s>\n' % fl + code[i]

                f = file(all_at_once_N_cpp, 'w');  f.write(code[i]);  f.close()

                exclude.finalize(all_at_once_N_cpp, dest, path, None, module_name=all_at_once_base, add_by_hand = (i == len(code)-1), files=headers, add_includes=True)
                #finalize_init(all_at_once_N_cpp)

                # -fPIC
                comiler_cmd = "%(compiler)s %(fname)s -o %(obj_name)s -c %(add_option)s -I../external/include  -I../external/dbio %(include_paths)s "
                comiler_dict = dict(add_option=add_option, fname=all_at_once_N_cpp, obj_name=all_at_once_N_obj, include_paths=include_paths, compiler=Options.compiler)

                failed = False
                if execute("Compiling...", comiler_cmd % comiler_dict, True):
                    if Options.compiler != 'clang': failed = True
                    elif execute("Compiling...", comiler_cmd % dict(comiler_dict, compiler='gcc'), True): failed = True

                if not Options.continue_  and failed: return new_headers

                objs_list.append(all_at_once_N_obj)

            execute("Linking...", # -fPIC -ffloat-store -ffor-scope
                "cd %(dest)s/../ && %(compiler)s %(obj)s %(add_option)s  \
                -lmini -lstdc++ -lz\
                 -l%(python_lib)s \
                 -l%(boost_lib)s \
                 %(libpaths)s %(runtime_libpaths)s -o %(dst)s" % dict(add_option=add_loption, obj=' '.join(objs_list),
                    dst=all_at_once_lib, libpaths=libpaths, runtime_libpaths=runtime_libpaths, dest=dest, boost_lib=Options.boost_lib,
                    python_lib=Options.python_lib, compiler=Options.compiler
                    ),
                 not Options.continue_)




    print 'Done!', new_headers
    return new_headers





def buildModule_One(path, dest, include_paths, libpaths, runtime_libpaths, gccxml_path):
    ''' Non recursive build buinding for given dir name, and store them in dest.
        path - relative path to namespace
        dest - path to root file destination, actual dest will be dest + path

        return dict of a newly found headers.
    '''

    #if ParallelBuild :  # When building in parallel we have to set path for each thread as new...
    #    os.chdir( './../../' )


    print 'Processing', path

    # Creating list of headers
    headers = [os.path.join(path, d)
                for d in os.listdir(path)
                    if os.path.isfile( os.path.join(path, d) )
                        and d.endswith('.hh')
                        and not d.endswith('.fwd.hh')
                        ]
    #headers.sort()
    # now, because of abstract class issue we have to sort header list first...
    headers.sort(key = lambda x: IncludeDict.get(x, (None, 999, None) )[1])

    # tmp  for generating original exclude list
    #for i in headers: IncludeDict[i] = True

    new_headers = {}
    for h in headers[:]:
        if h in IncludeDict:
            #print 'IncludeDict[%s] --> %s' % (h, IncludeDict[h])
            if not IncludeDict[h][0]:
                print "Excluding header:", h
                headers.remove(h)
            #else:
            #    print 'Including %s' % h
        else:
            print "Excluding new header:", h
            headers.remove(h)
            new_headers[h] = (False, 999, [])


    print headers

    fname_base = dest + '/' + path

    if not os.path.isdir(fname_base): os.makedirs(fname_base)

    print 'Creating __init__.py file...'
    f = file( dest + '/' + path + '/__init__.py', 'w');  f.close()
    if not headers:  return new_headers  # if source files is empty then __init__.py should be empty too
    def finalize_init(current_fname):
            #print 'Finalizing Creating __init__.py file...'
            f = file( dest + '/' + path + '/__init__.py', 'a');
            f.write('from %s import *\n' % os.path.basename(current_fname)[:-3]);
            f.close()


    include_paths = ' -I'.join( [''] + include_paths + ['../src/platform/linux' , '../src'] )

    nsplit = 1  # 1 files per iteration

    libpaths = ' -L'.join( ['', dest] + libpaths )
    runtime_libpaths = ' -Xlinker -rpath '.join( [''] + runtime_libpaths + ['rosetta'] )
    #if Platform == 'cygwin': runtime_libpaths = ' '

    global Options

    cc_files = []
    add_option  = getCompilerOptions()
    add_loption = getLinkerOptions()

    #last_header_list = []
    #last_header_list_ = []
    for fl in headers:
        #last_header_list.extend(last_header_list_)
        #last_header_list_ = [fl]

        #fcount += 1
        #files = headers[:nsplit]
        #headers = headers[nsplit:]
        #print 'Binding:', files
        hbase = fl.split('/')[-1][:-3]
        hbase = hbase.replace('.', '_')
        #print 'hbase = ', hbase
        #if hbase == 'init': hbase='tint'  # for some reason Boost don't like 'init' name ? by hand?

        fname =     fname_base + '/' + '_' + hbase + '.cc'
        inc_name =  fname_base + '/' + '_' + hbase + '.hh'
        obj_name =  fname_base + '/' + '_' + hbase + '.o'
        dst_name =  fname_base + '/' + '_' + hbase + '.so'
        if Platform == 'cygwin' : dst_name =  fname_base + '/' + '_' + hbase + '.dll'
        decl_name = fname_base + '/' + '_' + hbase + '.exposed_decl.pypp.txt'

        cc_files.append(fname)

        if Options.update:
            try:
                if Options.one_lib_file:
                    if os.path.getmtime(fl) < os.path.getmtime(fname):
                        print 'File: %s is up to date - skipping' % fl
                        finalize_init(fname)
                        continue

                else:
                    if os.path.getmtime(fl) < os.path.getmtime(dst_name):
                        print 'File: %s is up to date - skipping' % fl
                        finalize_init(fname)
                        continue
            except os.error: pass


        mb = module_builder.module_builder_t(files= [fl]
                                         , include_paths = ['../src/platform/linux', '../external/include', '../external/boost_1_46_1'],
                                         #, ignore_gccxml_output = True
                                         #, indexing_suite_version = 1
                                         gccxml_path=gccxml_path,
                                         )

        print commands.getoutput('rm %s/exposed_decl.pypp.txt' % fname_base)

        depend_on = IncludeDict.get(fl, (None, None, []) )[2][:]
        #depend_on.extend( last_header_list )
        #if None in depend_on: depend_on = []
        print 'depend_on = %s' % depend_on
        if depend_on:
            depend_on = depend_on[-1:]
            decls_file = file(fname_base + '/exposed_decl.pypp.txt', 'w')
            for f in depend_on:
                sp_name = f.split('/')
                name = 'python/bindings/rosetta/' + '/'.join(sp_name[:-1]) + '/_' + sp_name[-1].replace('.', '_')[:-3]
                decls_file.write( file(name + '.exposed_decl.pypp.txt').read() )

            decls_file.close()
            mb.register_module_dependency(fname_base)


        #print 'Excluding stuff... ---------------------------------------------'
        exclude.exclude(path, mb, hfile=fl)
        #mb.build_code_creator( module_name = '_' + dname )
        #def extr(something): return '"ABCDEF"'

        #mb.build_code_creator( module_name = '_'+hbase, doc_extractor=extr)#, doc_extractor=doxygen.doxygen_doc_extractor() )
        mb.build_code_creator( module_name = '_'+hbase, doc_extractor=tools.DoxygenExtractorPyPP.doxygen_doc_extractor() )

        mb.code_creator.user_defined_directories.append( os.path.abspath('.') )  # make include relative
        mb.write_module( os.path.join( os.path.abspath('.'), fname ) )

        exclude.finalize(fname, dest, path, mb, module_name='_'+hbase, add_by_hand = (fl==headers[0]), files=[fl])
        finalize_init(fname)

        del mb  # to free extra memory before compiling

        # saving generated 'exposed_decl.pypp.txt' for possible later use...
        commands.getoutput('cp %s/exposed_decl.pypp.txt %s' % (fname_base, decl_name))

        #exclude.finalize_old(fname, path, mb)  # remove init for some reasons.
        #print 'Module name="%s"' % dname

        # Mac OS compiling options: -pipe -ffor-scope -W -Wall -pedantic -Wno-long-long
        #        -Wno-long-double -O3 -ffast-math -funroll-loops -finline-functions
        #        -finline-limit=20000 -s -Wno-unused-variable -march=prescott -fPIC

        if not Options.one_lib_file:
            if execute("Compiling...", # -fPIC
                "gcc %(fname)s -o %(obj_name)s -c \
                 %(add_option)s -I../external/include \
                 %(include_paths)s " % dict(add_option=add_option, fname=fname, obj_name=obj_name, include_paths=include_paths),
                 True):

                print 'Compiliation failed... Creating empty C++ file to test includes...'
                fname =    fname_base + '/' + '_0_.cc'
                obj_name = fname_base + '/' + '_0_.o'
                f = file(fname, 'w');  f.write('#include <%s>\n' % fl);  f.close()
                execute("Compiling empty C++...",
                    "gcc %(fname)s -o %(obj_name)s -c \
                     %(add_option)s -I../external/include \
                     %(include_paths)s " % dict(add_option=add_option, fname=fname, obj_name=obj_name, include_paths=include_paths))

                sys.exit(1)



#                 -lObjexxFCL -lutility -lstdc++ \
            #all_libs = '-lObjexxFCL -lutility -lnumeric -lcore -lprotocols'
            #if Platform == 'cygwin': all_libs = '-lmini'

#-lObjexxFCL -lutility -lnumeric -lcore -lprotocols -lstdc++ \
            #if Platform == 'cygwin': runtime_libpaths = ''
            execute("Linking...", # -fPIC -ffloat-store -ffor-scope
                "cd %(dest)s/../ && gcc %(obj)s %(add_option)s  \
                -lmini -lstdc++ \
                 -l%(python_lib)s \
                 -l%(boost_lib)s \
                 %(libpaths)s %(runtime_libpaths)s -o %(dst)s" % dict(add_option=add_loption, obj=obj_name,
                    dst=dst_name, libpaths=libpaths, runtime_libpaths=runtime_libpaths, dest=dest, boost_lib=Options.boost_lib,
                    python_lib=Options.python_lib,
                    )
                 )



    # Generate just one lib file -------------------------------------------------------------------
    if Options.one_lib_file:
        all_files = cc_files
        max_files = Options.one_lib_max_files or len(all_files)

        f = file( dest + '/' + path + '/__init__.py', 'w'); f.close()  # truncating the __init__ file

        for f_range in range(0, len(all_files), max_files):
            cc_files = all_files[ f_range : f_range+max_files ]
            #print 'cc_files:', cc_files


            dir_base = 'rosetta_%s_%03d' % (path.replace('/', '_'), f_range/max_files)
            cc_all   = fname_base + '/' + '_%s.cc'  % dir_base
            obj_name  = fname_base + '/' + '_%s.o'  % dir_base
            dst_name = fname_base + '/' + '_%s.so' % dir_base
            if Platform == 'cygwin' : dst_name = fname_base + '/' + '_%s.dll' % dir_base

            begining, end = '', ''
            for f in cc_files:
                lines = file(f).read().split('\n')
                b, e, = 9999999, 9999999

                for i in range( len(lines) ):
                    if lines[i].startswith('BOOST_PYTHON_MODULE('): b = i
                    if lines[i].startswith('}'): e = i

                begining += '\n'.join( lines[:b] )
                end += '\n'.join( lines[b+1:e] )

            text = begining + 'BOOST_PYTHON_MODULE(_'+ dir_base + ') {\n' + end + '\n}\n'

            f = file(cc_all, 'w');  f.write(text);  f.close()

            cc_recompile = True

            if Options.update:
                cc_recompile = False
                try:
                    for f in cc_files:
                        print 'checking file %s...' % f
                        if os.path.getmtime(f) >= os.path.getmtime(dst_name):
                            cc_recompile = True
                            break
                except os.error: cc_recompile = True

            if cc_recompile:
                execute("Compiling one lib...",
                    "gcc %(fname)s -o %(obj_name)s -c \
                     %(add_option)s -I../external/include \
                     %(include_paths)s " % dict(add_option=add_option, fname=cc_all, obj_name=obj_name, include_paths=include_paths) )

                #all_libs = '-lObjexxFCL -lutility -lnumeric -lcore -lprotocols'
                #if Platform == 'cygwin': all_libs = '-lmini'

                execute("Linking one lib...",
                    "cd %(dest)s/../ && gcc %(obj_name)s %(add_option)s  \
                     -lmini -lstdc++ \
                     -l%(python_lib)s \
                     -l%(boost_lib)s \
                     %(libpaths)s %(runtime_libpaths)s -o %(dst)s" % dict(add_option=add_loption, obj_name=obj_name,
                        dst=dst_name, libpaths=libpaths, runtime_libpaths=runtime_libpaths, dest=dest, boost_lib=Options.boost_lib,
                        python_lib=Options.python_lib,
                        )
                     )
            #print 'Finalizing Creating __init__.py file...'
            f = file( dest + '/' + path + '/__init__.py', 'a');
            f.write('from _%s import *\n' % dir_base);  f.close()


    print 'Done!'
    return new_headers


def buildModule_All(path, dest, include_paths, libpaths, runtime_libpaths, gccxml_path):
    ''' Non recursive build buinding for given dir name, and store them in dest.
        path - relative path to namespace
        dest - path to root file destination, actual dest will be dest + path
    '''
    print 'Processing', path
    dname = os.path.basename(path)

    # Creating list of headers
    headers = [os.path.join(path, d)
                for d in os.listdir(path)
                    if os.path.isfile( os.path.join(path, d) )
                        and d.endswith('.hh')
                        and not d.endswith('.fwd.hh')
                        ]

    for i in exclude.exclude_header_list:
        if i in headers:
            print "Excluding header:", i
            headers.remove(i)

    print headers

    fname_base = dest + '/' + path

    if not os.path.isdir(fname_base): os.makedirs(fname_base)

    dst_name = fname_base + '/' + '_' + dname + '.so'

    if not headers:  # if source files is empty then __init__.py should be empty too
        print 'Creating __init__.py file...'
        f = file( dest + '/' + path + '/__init__.py', 'w');
        f.close()
        return

    include_paths = ' -I'.join( [''] + include_paths + ['../src/platform/linux' , '../src'] )

    nsplit = 99999  # 4 files per iteration
    fcount = 0
    namespace_wraper = ' '.join( [ 'namespace '+ n + ' { ' for n in path.split('/')] ) + ' %s ' + ' '.join( [ '}' for n in path.split('/')] ) + '\n'

    while headers:
        files = headers[:nsplit]
        headers = headers[nsplit:]
        #print 'Binding:', files

        fname = fname_base +    '/' + '_' + dname + '.cc'
        inc_name = fname_base +    '/' + '_' + dname + '.hh'
        obj_name = fname_base + '/' + '_' + dname + '.o'

        # Read all the include files and write them in to a single one. (Walk around for Py++ bug)
        #incList = ['#include <'+x+'>\n' for x in files]

        lines = []
        for f in files:
            lines.extend( open(f).read().split('\n') )

        begining, middle, rest = '', '', ''

        for l in lines:
            if l.startswith('#ifndef'): l = '// Commented by BuildBindings.py ' + l
            if l.startswith('#endif'): l = '// Commented by BuildBindings.py ' + l
            if l.startswith('#define'):
                begining += l + '\n'
                l = '// Moved to top of the file ' + l

            if l.startswith('#include'):
                middle += l + '\n'
                l = '// Moved to middle of the file ' + l

            rest += l + '\n'

        S = re.findall(r'struct (.*){', rest)
        for s in S:
            begining += namespace_wraper % ('struct ' + s + ';\n' )

        S = re.findall(r'class (\w*)', rest)
        for s in S:
            begining += namespace_wraper % ('class ' + s + ';\n' )


        h = open(inc_name, 'w')
        h.write( begining + '\n' + middle + '\n'+ rest + '\n')
        h.close()

        mb = module_builder.module_builder_t(files=files #[inc_name]  #files=files
                                         , include_paths = ['../src/platform/linux', '../external/include', '../external/boost_1_46_1'],
                                         #, ignore_gccxml_output = True
                                         #, indexing_suite_version = 1
                                         gccxml_path=gccxml_path,
                                         )
        print 'Excluding stuff... ---------------------------------------------'
        exclude.exclude(path, mb)

        mb.build_code_creator( module_name = '_' + dname )

        mb.code_creator.user_defined_directories.append( os.path.abspath('.') )  # make include relative
        mb.write_module( os.path.join( os.path.abspath('.'), fname ) )

        exclude.finalize(fname, dest, path, mb)

        del mb  # to free extra memory before compiling

        #exclude.finalize_old(fname, path, mb)  # remove init for some reasons.
        print 'Module name="%s"' % dname

        if Platform == 'linux' and PlatformBits == '32': add_option = '-malign-double'
        else: add_option = ''

        execute("Compiling...", # -fPIC
            "gcc %(fname)s -o %(obj_name)s -c \
             %(add_option)s -ffloat-store -ffor-scope -I../external/include \
             %(include_paths)s " % dict(add_option=add_option, fname=fname, obj_name=obj_name, include_paths=include_paths) )

        fcount += 1

    obj_name = fname_base + '/' + '_' + dname + '.o '
    #for i in range(fcount):
    #    obj_name += fname_base + '/' + '_' + dname + '.' + str(i) + '.o '

    libpaths = ' -L'.join( ['', dest] + libpaths )
    runtime_libpaths = ' -Xlinker -rpath '.join( [''] + runtime_libpaths + ['rosetta'] )

    if Platform == 'linux': add_option = '-shared'
    else: add_option = '-dynamiclib'

    global Options
    execute("Linking...",
            "cd %(dest)s/../ && gcc %(add_option)s -fPIC \
             -ffloat-store -ffor-scope \
             -lObjexxFCL -lutility -lnumeric -lcore -lprotocols -lstdc++ \
             -l%(python_lib)s \
             -l%(boost_lib)s \
             %(libpaths)s %(runtime_libpaths)s %(obj)s -o %(dst)s" % dict(add_option=add_option, obj=obj_name,
                dst=dst_name, libpaths=libpaths, runtime_libpaths=runtime_libpaths, dest=dest, boost_lib=Options.boost_lib),
                python_lib=Options.python_lib,
             )

# boost_python-xgcc40-mt-1_37

#              %(libpaths)s %(runtime_libpaths)s %(obj)s -o %(dst)s.dylib" % dict(obj=obj_name,

#     -lboost_python-gcc40-mt-1_36


    print 'Done!'
#             -Xlinker -rpath %(runtime_libpaths)s -Xlinker -rpath /home/sergey/y/lib \
# -lboost_python-gcc34-mt-1_34_1




'''

print 'Checking for ParallelPython...',
try:
    import pp
    ParallelPython = True
    print ' found!'
    JobServer = pp.Server()
    Jobs = []
except ImportError:
    ParallelPython = False
    print ' not found!'


def build():
    mb = module_builder.module_builder_t(
         files=['src/utility/exit.hh'])

        #, gccxml_path=gccxml.executable ) #path to gccxml executable

    #mb.print_declarations()



    mb.build_code_creator( module_name='rosetta', doc_extractor=doxygen.doxygen_doc_extractor() )
    mb.code_creator.user_defined_directories.append( os.path.abspath('.') )  # make include relative

    mb.write_module( os.path.join( os.path.abspath('.'), 'generated.cpp' ) )


'''

if __name__ == "__main__": main(sys.argv)

# class revision 26929
# ? Score Function, Conformation?

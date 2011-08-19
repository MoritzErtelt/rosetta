#!/usr/bin/env python
# -*- coding: utf-8 -*-
# :noTabs=true:

# (c) Copyright Rosetta Commons Member Institutions.
# (c) This file is part of the Rosetta software suite and is made available under license.
# (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
# (c) For more information, see http://www.rosettacommons.org. Questions about this can be
# (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.
# This code made available under dual license: RosettaCommons license and GPLv3

## @file   CppParser.py
## @brief  Parse C++ code and wrap it in Python
## @author Sergey Lyskov


import os, sys, gc

import xml.dom.minidom

import doxygen

class CD:
    def __init__(self, **entries): self.__dict__.update(entries)
    def __repr__(self):
        r = '|'
        for i in dir(self):
            if not i.startswith('__'): r += '%s --> %s, ' % (i, getattr(self, i))
        return r[:-2]+'|'



class ReferenceSection:
    def __init__(self, ReferenceSection):
        # do we even need this??? self.ReferenceSection = ReferenceSection
        self.TypeNodes,    self.Types    = {}, {}
        self.ContextNodes, self.Contexts = {}, {}
        self.Files = {}

        for i in ReferenceSection.childNodes:
            if i.nodeName == 'Types':
                for j in i.childNodes:
                    if j.nodeName != '#text':  self.TypeNodes[j.getAttribute('id')] = j

            if i.nodeName == 'Contexts':
                for j in i.childNodes:
                    if j.nodeName != '#text':  self.ContextNodes[j.getAttribute('id')] = j

            if i.nodeName == 'Files':
                for j in i.childNodes:
                    if j.nodeName != '#text':  self.Files[j.getAttribute('id')] = j.getAttribute('name')


    def getContext(self, context_id):
        if self.Contexts.get(context_id) is None:
            node = self.ContextNodes[context_id]
            if node.nodeName == 'TranslationUnit': self.Contexts[context_id] = '::'
            else: self.Contexts[context_id] = self.getContext(node.getAttribute('context') ) + self.ContextNodes[context_id].getAttribute('name') +'::'

        #print 'getContext', context_id, '-->', self.Contexts[context_id]
        return self.Contexts[context_id]


    def getType(self, type_id):
        if self.Types.get(type_id) is None:
            node = self.TypeNodes[type_id]

            if node.nodeName == 'FundamentalType':  self.Types[type_id] = node.getAttribute('kind')
            else: self.Types[type_id] = 'Unknow type: ' + node.nodeName


        return self.Types[type_id]


    def getFile(self, file_id):
        pass

#
# T Y P E S
#
class CppType:
    def getPureTypeName(self): return self.T()  # return name of a type without const or *, & modifiers...
    def getFile(self, simplify=False): return self.file_
#    def __repr__(self):  return  self.type_

class CppType_Fundamental(CppType):  # 'FundamentalType'
    def __init__(self, type_, kind): self.type_ = type_;  self.kind = kind;  self.file_ = None
    def getContext(self): return ''
    def getKind(self): return 'Fundamental'
    def T(self, simplify=False): return self.type_

class CppType_Simple(CppType):  # 'PointerType', 'ReferenceType', 'CvQualifiedType'
    def __init__(self, type_, postfix, kind): self.type_ = type_;  self.postfix = postfix;  self.kind = kind
    def getContext(self): return self.type_.getContext()
    def getKind(self): return self.type_.getKind()
    def getPureTypeName(self): return self.type_.getPureTypeName()
    def T(self, simplify=False): return self.type_.T(simplify=simplify) + ' ' + self.postfix
    def getFile(self): return self.type_.getFile()
    #def __repr__(self):  return  'CppType_Simple_'+self.postfix

class CppType_Composite(CppType):  #, 'FunctionType', 'ArrayType' - have some additionla information and non trivial composition
    def __init__(self, type_, kind): self.type_ = type_;  self.kind = kind
    def getContext(self): return ''
    def getKind(self): return 'Composite'
    def T(self, simplify=False): return '___XQWERTY___' + self.type_


class CppType_Complex(CppType):  # 'Class', 'Struct', 'Union', 'Typedef', 'Enumeration' - these one have context and definition location
    #def __init__(self, type_, context): self.type_ = type_ ;  self.context = context
    def __init__(self, name, context, kind):        self.name = name;  self.context = context;  self.kind = kind
    def getContext(self): return self.context
    def getKind(self): return self.kind
    def T(self, simplify=False):
        if self.context == '::' : return '::' + self.name
        return '::'+self.context + '::' + self.name


class CppType_Typedef(CppType_Complex):
    def __init__(self, name, context, type_): self.name = name;  self.context = context;  self.type_ = type_; self.kind = 'Typedef'
    def T(self, simplify=False):
        if simplify: return self.type_.T(simplify=simplify)
        else: return CppType_Complex.T(self)



class CppNamespace:
    ''' data holder for C++ namespace. name represent namespace name and context is full context in which it recides
    '''
    def __init__(self, name, context):
        self.name, self.context = name, context

    def getChildrenContext(self):
        if self.context:  return self.context + self.name + '::'
        else:  return self.name

    def wrap(self): pass


class CppEnum:
    def __init__(self, node, context, refSection, name=None):
        self.name = name or node.getAttribute('name')
        self.context = context
        self.public  = (not node.hasAttribute('access')) or  (node.getAttribute('access') == 'public')
        self.values = []

        self.file_ =  refSection.Files[node.getAttribute('file')]
        self.line  =  node.getAttribute('line')

        for c in node.childNodes:
            if c.nodeName == 'EnumValue':  # GCC XML
                self.values.append( CD(name=c.getAttribute('name'), init=c.getAttribute('init') ) )


    def wrap_prefix_code(self, indent=''): return ''


    def wrap(self, indent='', wrappingScope='boost::python::'):
        print 'WrappingEnum %s...' % self.name

        if not self.public: return ''
        r = '  %senum_< %s%s >("%s", "%s")\n' % (wrappingScope, self.context, self.name, self.name, doxygen.getDoxygenComment(self.file_, self.line))
        for v in self.values:
            r += '    .value("%s", %s%s)\n' % (v.name, self.context, v.name)
        r += '    .export_values();\n'
        return  ('\n' + indent).join( r.split('\n') ) + '\n'


    def getFileList(self):  # return list of files where types was created (use it for includes!)
        return [self.file_]



class CppVariable:
    def __init__(self, node, context, refSection):
        self.name = node.getAttribute('name')
        self.context = context
        self.public  = (not node.hasAttribute('access')) or  (node.getAttribute('access') == 'public')
        #self.type_ = refSection.getType( node.getAttribute('type') ) ) )

        self.file_ =  refSection.Files[node.getAttribute('file')]
        self.line  =  node.getAttribute('line')


    def wrap(self, indent='', wrappingScope='boost::python::'):
        if not self.public: return ''
        r = '  %sdef_readwrite("%s", &%s%s);\n' % (wrappingScope, self.name, self.context, self.name)
        return  ('\n' + indent).join( r.split('\n') ) + '\n'



class CppFunction:
    OperatorToCppNameMap = {'=':'assign', '()':'__call__', '[]':'__getitem__'}
    def __init__(self, node, context, returnType, refSection, memberFunction=False, const=False, constructor=False, const_overloaded=False):
        self.name = node.getAttribute('name')
        self.context = context
        self.demangled = node.getAttribute('demangled')
        self.returnType = returnType
        self.memberFunction = memberFunction
        self.const = const
        self.const_overloaded = const_overloaded
        self.constructor = constructor
        self.reference = refSection

        self.operator      =  node.nodeName == 'OperatorMethod'  or  node.nodeName == 'OperatorFunction'
        self.static        =  node.hasAttribute('static')        # GCC XML
        self.virtual       =  node.hasAttribute('virtual')       # GCC XML
        self.pure_virtual  =  node.hasAttribute('pure_virtual')  # GCC XML
        self.public        =  (not node.hasAttribute('access')) or  (node.getAttribute('access') == 'public')    # to do - create 'access' datamember and
        self.protected     =  (not node.hasAttribute('access')) or  (node.getAttribute('access') == 'protected') #    turn this in to a propertys
        self.private       =  (not node.hasAttribute('access')) or  (node.getAttribute('access') == 'private')
        self.artificial    =  node.hasAttribute('artificial')
        self.file_         =  refSection.Files[node.getAttribute('file')]
        self.line          =  node.getAttribute('line')

        self.argsTypes = []
        for c in node.childNodes:
            if c.nodeName == 'ParmVar':  # Clang XML
                self.argsTypes.append( CD(name=c.getAttribute('name'), type_=refSection.getType( c.getAttribute('type') ) ) )

            if c.nodeName == 'Argument':  # GCC XML
                self.argsTypes.append( CD(name=c.getAttribute('name'), type_=refSection.getType( c.getAttribute('type') ) , default=c.getAttribute('default') ) )


    def __str__(self):
        ''' Generate string representing function name and types (but no context information for function itself), like this: int foo(char a, bool b)
        '''
        return ( '%s %s(%s)' % (self.returnType.T(simplify=True), self.name , self.getSimpleArgsType(simplify=True) ) )
        '''    .replace('::platform::Size', '::core::Size')
                        .replace('::core::scoring::rna::Vector', '::numeric::xyzVector<double>') \
                        .replace('::core::scoring::methods::EnergyMethodOP', '::utility::pointer::owning_ptr<core::scoring::methods::EnergyMethod>') \
                        .replace('::core::Vector', '::numeric::xyzVector<double>').replace('::core::kinematics::DomainMap', '::ObjexxFCL::FArray1D<int>') \
                        .replace('::std::basic_string<char,std::char_traits<char>,std::allocator<char> >', '::std::string') \
                        .replace('double', '::core::Real').replace('long unsigned int', '::core::Size')
                        '''

    def isDefalutArgWrappable(self, x):
        ''' check if default arg can be wrapped and return string for it, return epmty string castable to False otherwise
        '''
        _r = ''
        if x.default:
            # GCC gives non-working default arg here, so what we do is just bind without default... - probably could be fixed later
            # in future we can just locate enums in all parsed code but for now lets not guess...
            known_bad_defaults = ['(((utility::tag::Tag*)operator new(', 'utility::vector0<int, std::allocator<int>', 'MATCH_YR', 'std::cout', 'typename ', 'std::make_pair [with ',
                'core::chemical::ChemicalManager::residue_type_set(const std::string&)(((const std::string&)(& core::chemical::FA_STANDARD',
                'core::scoring::hbonds::DUMMY_DERIVS',
            ]
            for bd in known_bad_defaults:
                if x.default.startswith(bd): return _r

            if len( x.default ) > 64: return _r  # Golden rule: if default arg description more then 64 - gcc got it wrong!

            if x.type_.getKind() == 'Enumeration':
                if x.default.startswith(x.type_.getContext()):
                    _r += '=%s' % x.default
                else:
                    _r += '=%s::%s' % (x.type_.getContext(), x.default)  # x.type_.getContext(),
            else:
                _r += '=(%s)' % (x.default)
        # Now, we need to find if all args to the right is also wrappable...
        for a in self.argsTypes[ self.argsTypes.index(x)+1 : ]:
            if not self.isDefalutArgWrappable(a): return ''
        return _r

    def writeFunctionDeclarationTypes(self, withArgs=True, withDefaultArgs=False):
        #def write_def(x):
        #    if self.isDefalutArgWrappable(x): return '=%s' % self.isDefalutArgWrappable(x)
        #    #if x.default: return '=%s' % x.default
        #    else: return ''
        return ', '.join( map(lambda (i, x): x.type_.T() + ' __a%s%s' % (i, self.isDefalutArgWrappable(x)), enumerate(self.argsTypes)) )


    def getSimpleArgsType(self, constructor=False, withArgs=True, simplify=False):
        if not constructor: return ', '.join( map(lambda (i, x): x.type_.T(simplify=simplify) + ' __a%s' % i, enumerate(self.argsTypes)) )
        else:
            def foo(x):
                if x.default and self.isDefalutArgWrappable(x):
                    return x.type_.T()
                else: return x.type_.T()
            r, default_state = '', False
            for x in self.argsTypes:
                if (not default_state) and  self.isDefalutArgWrappable(x):
                    default_state = True
                    r += 'boost::python::optional< %s ' % x.type_.T()
                else: r += x.type_.T()
                r += ', '

            if default_state: r = r[:-2] + ' >, '
            return r[:-2]


    def getArgTypes(self):
        if self.argsTypes:
            def wrap_argument(x):
                _r = 'boost::python::arg("%s")' % x.name
                _r += self.isDefalutArgWrappable(x)

                return _r

            #return '    , ( %s )\n' % ', '.join( map(wrap_argument, self.argsTypes) )
            return ', '.join( map(wrap_argument, self.argsTypes) )
        else: return ''


    def isWrapable(self):
        ''' Check if we can really wrap this object now, not theoretical but practically...
        '''
        tp = [self.returnType] + map(lambda x: x.type_, self.argsTypes)
        for a in tp:  # check if function contain types that we don't know how to deal with yet...
            #print a
            if a.T().find('___XQWERTY___') >= 0: return False
            if a.T().find('::std::pair<boost::unordered_detail::hash_iterator_equivalent_keys<std::allocator<std::pair<') >= 0: return False

        #if self.pure_virtual or (not self.public): return False
        if (not self.public): return False


        #  check if return result is sane. Some types (like int *) mean that we actully have an iterator functions and this one should be taked care at class abstraction level
        for t in ['char *', 'int *', 'int const *', 'double *', 'double const *', 'Real *', 'Real const *', 'kiss_fft_cpx *',
                  'kiss_fft_cfg', 'kiss_fftnd_cfg', 'kiss_fftr_cfg', ]:
            if (self.returnType.T() or '').endswith(t): return False

        if self.returnType.T().startswith('::boost::unordered_detail::hash_const_iterator_unique_keys<std::allocator') : return False

        #if self.operator  and  self.name in ['<', '>', '+', '-', '->', '*', '&', '++', '--', '+=', '-=', '*=', '==', '!=']: return False  # will deal with this later...

        if self.operator  and  self.name=='='  and   self.artificial:  return False
            #print self.context, self.name, self.returnType.T(), self.argsTypes[0].type_.T()
            #if self.returnType.T() != self.context[:-2] + ' &'  or self.argsTypes[0].type_.T() != self.context[:-2] + ' const &': return False
            #print self.name, '= C=%s, N=%s, T=%s' % (self.context, self.name, self.argsTypes[0].type_.T())

        if self.operator  and  (self.name not in self.OperatorToCppNameMap): return False

        return True


    def getReturnValuePolicyString(self):
        primitive_types = ['bool', 'char', 'int', 'float', 'double', 'size_t', 'string', 'Real', 'Size', 'Length', 'core::PackerEnergy', 'core::Energy']

        def endsWith(str_, suffix, list_=primitive_types):  # return true if str_ end with any element on list_
            for x in [ e+suffix for e in list_]:
                if str_.endswith(x+' &') or str_.endswith(x+' *'): return True
            return False

        if self.returnType.getKind() == 'Enumeration':
            if endsWith(self.returnType.T(), ' const', ['']):  # enum_type const &, enum_type const *,
                return '    , boost::python::return_value_policy< boost::python::copy_const_reference >()\n'

            elif self.returnType.T().endswith(' *') or self.returnType.T().endswith(' &'): # enum_type &, enum_type *,
                return '    , boost::python::return_value_policy< boost::python::copy_non_const_reference>()\n'

            return ''

        elif endsWith(self.returnType.T(), ' const'):  # int const &, int const *
            return '    , boost::python::return_value_policy< boost::python::copy_const_reference >()\n'

        elif endsWith(self.returnType.T(), ''):  # int &, int *
            return '    , boost::python::return_value_policy< boost::python::copy_non_const_reference>()\n'

        #elif endsWith(self.returnType.T(), ' const', ['']):  # const &, const *, ... &, ... *
        #    return '    , boost::python::return_value_policy< boost::python::reference_existing_object >()\n'

        elif self.returnType.T().endswith(' *') or self.returnType.T().endswith(' &'):  # ... &, ... *
            return '    , boost::python::return_value_policy< boost::python::reference_existing_object >()\n'

        return ''


    def isCallbackStructNeeded(self):
        #if self.name == '=': return False  # virtual assign operator? No way we can wrap it...
        if self.virtual and self.public: return True  # and (not f.const)
        return False

    def writeCallbackCode(self, class_, callback, indent=''):
        if not self.isWrapable(): return ''
        r = ''
        #if (self.virtual and self.public and not self.const_overloaded) or  (self.constructor and self.public):  # for now we allow to overload only non-const functions in Python...
        if self.isCallbackStructNeeded() or  (self.constructor and self.public):  # for now we allow to overload only non-const functions in Python...
            def foo( (i, a) ):
                if (not self.constructor) and a.type_.T()[-1] == '&': return 'utility::PyAP( __a%s )' % i
                else: return '__a%s' % i

            args = ', '.join( map(lambda (i, x): '__a%s' % i, enumerate(self.argsTypes)) )
            args_AP = ', '.join( map(foo, enumerate(self.argsTypes)) )
            if self.constructor: r += '\n%s(%s) : %s(%s) {' % (callback, self.writeFunctionDeclarationTypes(withDefaultArgs=True), class_, args)  # Writing constructors...  # and not self.artificial
            else:
                if self.operator:
                    py_name =  self.OperatorToCppNameMap [self.name]
                    cpp_name = 'operator' + self.name
                else:
                    cpp_name = self.name
                    py_name = self.name

                r += '\n%s %s(%s) %s{\n' % (self.returnType.T(), cpp_name, self.writeFunctionDeclarationTypes(withDefaultArgs=True), ['', ' const '][self.const])
                return_ = ['return', ''][self.returnType.T() == 'void']
                if self.pure_virtual:
                    r += '  %s this->get_override("%s")(%s);\n' % (return_, py_name, args_AP)
                else:
                    r += '  if( boost::python::override f = this->get_override("%s") ) %s f(%s);\n' % (py_name, return_, args_AP)
                    r += '  else %s %s(%s);\n' % (return_, self.context+cpp_name, args)
                    r += '}\n'
                    r += '\n%s __default_%s(%s) %s{\n  %s this->%s(%s);\n' % (self.returnType.T(), py_name, self.getSimpleArgsType(), ['', ' const '][self.const], return_, self.context+cpp_name, args)

            r += '}\n'
            r = ('\n' + indent).join( r.split('\n') ) + '\n'
        return r


    def wrap_prefix_code(self, indent=''): return ''

    def wrap(self, indent='', wrappingScope='boost::python::', useCallbackStruct=''):#overloaded=False):
        if not self.isWrapable(): return ''
        args = self.getSimpleArgsType()

        if self.operator:
            py_name = self.OperatorToCppNameMap [self.name]
            cpp_name = 'operator' + self.name
        else:
            cpp_name = self.name
            py_name = self.name

        if useCallbackStruct:  useCallbackStruct += '::'

        D = dict(py_name=py_name, cpp_name=cpp_name, context=self.context, returnType=self.returnType.T(), args=args, CallbackStruct=useCallbackStruct,
                 wrappingScope=wrappingScope, const=' const' if self.const else '', doc=doxygen.getDoxygenComment(self.file_, self.line))
        r = '\n{ // %(context)s%(cpp_name)s%(const)s\n' % D

        if self.memberFunction and not self.static:
            r += '  typedef %(returnType)s ( %(context)s * %(py_name)s_function_type)(%(args)s) %(const)s;\n\n' % D
            if useCallbackStruct: r += '  typedef %(returnType)s ( %(CallbackStruct)s * %(py_name)s_callback_function_type)(%(args)s) %(const)s;\n\n' % D
        else:
            r += '  typedef %(returnType)s ( * %(py_name)s_function_type)(%(args)s);\n\n' % D

        r += '  %(wrappingScope)sdef("%(py_name)s"\n' % D

        if self.pure_virtual:  r += '    , ::boost::python::pure_virtual( %(py_name)s_function_type( &%(context)s%(cpp_name)s ) )\n' % D
        else:                  r += '    , %(py_name)s_function_type( &%(context)s%(cpp_name)s )\n' % D  # r += '    , %s_function_type( &%s%s )\n' % (self.name, self.context, self.name )

        #if useCallbackStruct  and  self.virtual  and  not self.pure_virtual  and not self.const_overloaded:
        if useCallbackStruct and self.isCallbackStructNeeded() and not self.pure_virtual  and not self.const_overloaded:
            r += '    , %(py_name)s_callback_function_type( &%(CallbackStruct)s__default_%(py_name)s )\n' % D
            #r += '    , &%(CallbackStruct)s::__default_%(py_name)s\n' % D

        if self.argsTypes:  r+= '    , ( %s )\n' % self.getArgTypes()

        #print self.returnType.T(), self.name, '-->', self.getReturnValuePolicyString()
        r += self.getReturnValuePolicyString()

        # Bug in boost: it can't hanlde more then 5 Template args here, so we omit doc string if this is the case...
        if (useCallbackStruct  and  self.virtual  and  not self.pure_virtual  and not self.const_overloaded) and \
           self.argsTypes and self.getReturnValuePolicyString(): r += '    );\n' % D
        else: r += '    , "%(doc)s" );\n' % D

        #if self.static: r += '  %(wrappingScope)sstaticmethod("%(py_name)s");\n' % D
        r += '}\n'

        return  ('\n' + indent).join( r.split('\n') ) + '\n'

    def getFileList(self): # , exclude=[] # return list of files where types was created (use it for includes!)
        '''if self in exclude:
            return []

        exclude.append(self)
        '''
        r = [self.file_]
        for a in [self.returnType] + map(lambda x: x.type_, self.argsTypes):
            r.append(a.getFile())
            '''
            base_type = a.T().split(' ')[0]
            if base_type in self.reference.Objects:
                #print '~~~~~ ', base_type
                self.reference.Objects[ base_type ].getFileList(exclude)
                '''

        return r

    def __repr__(self):
        return 'CppFunction: %s Context=%s isWrapable=%s, %s %s' % (self.name, self.context, self.isWrapable(), self.returnType, self.argsTypes)



class CppClass:
    def __init__(self, node, context, refSection):
        self.reference = refSection
        self.name = node.getAttribute('name')
        self.context = context
        self.bases = []
        self.constructors = []
        self.destructor = None
        self.functions = []
        self.enums = []
        self.dataMembers = []
        self.requiredTypes = []  # types that need to be declared for class to be parsable (ie includes needed)

        self.abstract   = node.hasAttribute('abstract')
        self.incomplete = node.hasAttribute('incomplete')
        self.file_      = refSection.Files[node.getAttribute('file')]
        self.line       = node.getAttribute('line')

        for ch in node.childNodes:
            #if ch.nodeName == '#text': continue  # why on earth this node type is everywhere???
            if ch.nodeName == 'Base':
                self.bases.append( CD( type_=refSection.getType(ch.getAttribute('type')), access=ch.getAttribute('access'), virtual=bool(int(ch.getAttribute('virtual')))  ) )

    def getVirtualBases(self):  # return list of virtual bases classes
        r = []
        for b in self.bases:
            if b.type_.T() in self.reference.Objects:
                o = self.reference.Objects[ b.type_.T() ]
                if b.virtual: r.append( o.context + o.name )
                r += o.getVirtualBases()
        return r


    def isBase(self, base):
        for b in self.bases:
            if b.type_.T() == base.context+base.name: return True
            if b.type_.T() in self.reference.Objects and  (self.reference.Objects[ b.type_.T() ].isBase(base)): return True
        return False


    def isDepend(self, other):
        for b in self.bases:
            if b.type_.T().find( (other.context+other.name)[2:] ) >= 0 : return True
            if b.type_.T() in self.reference.Objects and  (self.reference.Objects[ b.type_.T() ].isDepend(other)): return True
        return False



    def getChildrenContext(self):  return self.context + self.name + '::'

    def getAllVirtualFunctions(self, D, checkAll=True):
        ''' fill dict with all virtual functions that need to be implemented before class could be created. {full cpp name : CppFunction}
        - return non-empty dict if all function that needed (pure_virtual) are wrapable, and false ({}) otherwise
        '''
        for f in self.functions:
            if f.public and f.virtual  and (checkAll or f.pure_virtual):
                name =  str(f)

                if name not in D  and f.virtual:
                    D[name] = f
                    if f.pure_virtual and (not f.isWrapable()): return {}


            elif f.pure_virtual: return {}

        for b in self.bases:
            if b.type_.T() in self.reference.Objects:
                o = self.reference.Objects[ b.type_.T() ]
                #if o.isWrapable():
                if b.access == 'public' and  (not o.getAllVirtualFunctions(D)): return {}
                elif not o.getAllVirtualFunctions(D, checkAll=False): return {}

        return D



    def isCreatable(self, asBase=False, with_callback_struct=False):
        if with_callback_struct:
            return self.getAllVirtualFunctions({})
            '''
            for f in self.functions:
                if f.pure_virtual  and  (not f.isWrapable()): return False

            for b in self.bases:
                if b.type_.T() in self.reference.Objects:
                    o = self.reference.Objects[ b.type_.T() ]
                    if o.isWrapable() and not o.isCreatable():
                        #print o, ' --> False'
                        return False

            return True
            '''
        else:

            if self.abstract: return False
            for f in self.functions:
                if f.pure_virtual: return False

            if self.constructors:
                for c in self.constructors:
                    if len(c.argsTypes) == 1  and  c.argsTypes[0].type_.T().find(self.context+self.name+' ') >= 0: continue  # skip copy constructor
                    if c.public: return True
                    if asBase and c.protected: return True
                #print self.name, 'Is NOT creatable!'
                return False

            return True


    def isCopyable(self, asBase=False):
        ''' check if there is private constructor and assign operator
        '''
        if self.abstract: return False
        if (self.destructor and (not self.destructor.public)) or (not self.isCreatable(asBase)): return False
        for c in self.constructors:  # check if copy construtor is present and private...
            if (c.private)  and  len( c.argsTypes ) == 1  and  c.argsTypes[0].type_.T() == self.getChildrenContext()[:-2] + ' const &' :  return False

        for b in self.bases:  # lests check if all bases are copyable too...
            if b.type_.T() in self.reference.Objects and  (not self.reference.Objects[ b.type_.T() ].isCopyable(asBase=True)): return False

        #for f in self.functions:  # check if assign opperator is private...
        #    if (not f.public)  and  f.name == '=': return False  # not exactly exact mucth, but good enought for now
        return True




    def isCallbackStructNeeded(self):
        ''' Return True if class have public virtual functions, so it make sense to allow callback from Python...
        '''
        if self.getVirtualBases(): return False  # we cant auto create virtual base classes


        for f in self.functions:
            if f.pure_virtual and not f.public: return False

        if self.constructors: # check if there is a way to construct subclass... (ie constructors that public or no constructors)
            for c in self.constructors:
                if len(c.argsTypes) == 1  and  c.argsTypes[0].type_.T().find(self.context+self.name+' ') >= 0: continue  # skip copy constructor
                if c.public: break
            else: return False

        if not self.isCreatable(with_callback_struct=True): return False  # there is no point to wrap it if we can't create it

        for f in self.functions:
            #if f.virtual and f.public: return True  # and (not f.const)
            if f.isCallbackStructNeeded() and f.isWrapable(): return True  # and (not f.const)

        return False




    def isWrapable(self):
        ''' Check if we can really wrap this object now, not theoretical but practically...
        '''
        if self.context+self.name == '::utility::pointer::ReferenceCount': return False
        if self.incomplete: return False
        for b in self.bases:
            if b.type_.T() == '::boost::noncopyable_::noncopyable': return False
        return True


    def isHeldTypeOP(self):
        for b in self.bases:
            if b.type_.T() == '::utility::pointer::ReferenceCount': return True
            if b.type_.T() in self.reference.Objects and  (self.reference.Objects[ b.type_.T() ].isHeldTypeOP()): return True
        return False


    def getHeldType(self, heldTypeBase=None):
        if not heldTypeBase: heldTypeBase = self.context+self.name

        r =  heldTypeBase  #self.context+self.name

        bases = ''
        for b in self.bases:
            if b.type_.T() == '::utility::pointer::ReferenceCount': continue
            if b.type_.T().startswith('::utility::vector'): continue
            if b.type_.T().startswith('::std::iterator'): continue
            if b.type_.T().startswith('::std::'): continue
            bases += b.type_.T() + ', '

        if bases: r += ', boost::python::bases< %s >' % bases[:-2]

        if self.isHeldTypeOP(): r+=  ', ::utility::pointer::owning_ptr< %s >' % heldTypeBase #(self.context, self.name)
        return r


    def markConstOverloaded(self):
        ''' function.const_overloaded → True/False for all coonst member functions if the same non-const function present
        '''
        for f in self.functions:
            if f.const:
                for h in self.functions:
                    if f.demangled[:-6] == h.demangled:  # we don't need to compare return types, just type signature with removed 'const'
                        print 'MakringConstOverloaded %s' % f.demangled, '→ True'
                        f.const_overloaded = True; break
                else: f.const_overloaded = False

    def write_implicitly_convertible_code(self, use_callback_struct, D):
        r = ''
        if self.isHeldTypeOP():
            convert_to = ['%(context)s%(name)s const' % D]
            for b in self.bases:
                if b.type_.T() != '::utility::pointer::ReferenceCount' and \
                   b.type_.T() in  self.reference.Objects  and  self.reference.Objects[ b.type_.T() ].isHeldTypeOP():
                     convert_to.append( b.type_.T() )
            r += '\n'

            # Adding COP → OP conversion
            r += '  boost::python::to_python_converter< utility::pointer::owning_ptr< %(context)s%(name)s const >, utility::COP_to_Python_converter< %(context)s%(name)s >, false >();\n\n' % D

            for i in convert_to:
                r += '  boost::python::implicitly_convertible< utility::pointer::owning_ptr< %(context)s%(name)s >\n' % D
                r += '                                       , utility::pointer::owning_ptr< %s > >();\n\n' % i

            if use_callback_struct:
                r += '  boost::python::implicitly_convertible< utility::pointer::owning_ptr< %(callback)s >\n' % D
                r += '                                       , utility::pointer::owning_ptr< %(context)s%(name)s > >();\n\n' % D

        return r


    #def getCallbackClassName(self): return 'PY_'+ self.name
    def getCallbackClassName(self): return self.name

    def getExposerName(self, name): return name.replace(' ', '_').replace('<', '_T_').replace('>', '_T').replace('::', '_').replace(',', '_') + '_exposer'

    def wrap_prefix_code(self, indent=''):
        r = ''
        # Callback Struct →
        if self.isCallbackStructNeeded():
            callback= self.getExposerName(self.getCallbackClassName()) + '_callback'

            D = dict(name=self.name, context=self.context, callback=callback)

            r += 'struct %(callback)s : public %(context)s%(name)s, boost::python::wrapper< %(context)s%(name)s >\n{\n' % D

            #for f in self.constructors+self.functions:
            #    r += f.writeCallbackCode(class_=self.context+self.name, callback=callback, indent='  ')

            for f in self.constructors + self.getAllVirtualFunctions({}).values():
                r += f.writeCallbackCode(class_=self.context+self.name, callback=callback, indent='  ')

            '''
            # now lets iterate thrugh all public bases and see if there is pure virtual functions that we need to implement...
            for b in self.bases:
                if b.type_.T() in self.reference.Objects:
                    o = self.reference.Objects[ b.type_.T() ]
                    for f in o.functions:
                        if f.pure_virtual and  ('%s(%s)' % (f.name , f.getSimpleArgsType()) ) not in written:
                            print written
                            r += f.writeCallbackCode(class_=self.context+self.name, callback=callback, indent='  ')
                            written.append( '%s(%s)' % (f.name , f.getSimpleArgsType()))
            '''

            r += '};\n'
        # ← Callback Struct

        return ('\n' + indent).join( r.split('\n') ) + '\n'

    def wrap(self, indent=''):
        #r = self.wrap_(indent, use_callback_struct=False)
        #if self.isCallbackStructNeeded(): r += self.wrap_(indent, use_callback_struct=True)
        r = self.wrap_(indent, use_callback_struct=self.isCallbackStructNeeded())
        return r

    def wrap_(self, indent='', use_callback_struct=False):
        if not self.isWrapable(): return ''

        print 'WrappingClass %s %s... [' % (self.context, self.name),
        for b in self.bases: print b.type_.T(),
        print ']'

        self.markConstOverloaded()
        if use_callback_struct: class_name = self.getCallbackClassName()
        else: class_name = self.name

        default_constructors = filter(lambda x: x.default, self.constructors)
        default_constructor = default_constructors and default_constructors[0]  # must be always... at least one... - not really...
        exposer = self.getExposerName(class_name) #self.name.replace(' ', '_').replace('<', '_T_').replace('>', '_T').replace('::', '_').replace(',', '_') + '_exposer'
        callback=exposer+'_callback'

        if use_callback_struct: heldTypeBase = callback
        else: heldTypeBase = self.context+self.name

        D = dict(name=self.name, class_name=class_name, context=self.context, exposer=exposer, callback=callback, heldType=self.getHeldType(heldTypeBase), doc=doxygen.getDoxygenComment(self.file_, self.line))

        r = '\n{ // %(context)s%(class_name)s \n' % D

        r += self.write_implicitly_convertible_code(use_callback_struct, D)

        r += '  utility::wrap_access_pointer< %(context)s%(name)s >("%(class_name)s");\n' % D

        if use_callback_struct: r += '  boost::python::class_< %s, boost::noncopyable >("__CPP_%s__", "", boost::python::no_init);\n\n' % (self.getHeldType(), self.name)

        if self.isCopyable():  # or self.isHeldTypeOP():
            r += '  typedef boost::python::class_< %(heldType)s > %(exposer)s_type;\n' % D
        else:
            r += '  typedef boost::python::class_< %(heldType)s, boost::noncopyable > %(exposer)s_type;\n' % D

        if default_constructor:
            if default_constructor.argsTypes:
                default_constructor_args = '< %s >( (%s) , "%s" )' % (default_constructor.getSimpleArgsType(constructor=True), default_constructor.getArgTypes(), doxygen.getDoxygenComment(default_constructor.file_, default_constructor.line))
            else:
                default_constructor_args = '< %s >()' % default_constructor.getSimpleArgsType(constructor=True)

        #print ' default_constructor and self.isCreatable()...........', default_constructor, self.isCreatable()

        if (default_constructor and self.isCreatable())  or (default_constructor and self.isCreatable(with_callback_struct=use_callback_struct)) :
            r += '  %(exposer)s_type %(exposer)s("%(class_name)s", "%(doc)s", boost::python::init %(default_constructor_args)s );\n' %  dict(D.items(), default_constructor_args=default_constructor_args )
        elif self.isCreatable() or self.isCreatable(with_callback_struct=use_callback_struct):
            r += '  %(exposer)s_type %(exposer)s("%(class_name)s", "%(doc)s" );\n' %  D
        else:
            r += '  %(exposer)s_type %(exposer)s("%(class_name)s", "%(doc)s", boost::python::no_init );\n' %  D

        if default_constructor and self.isCreatable():
            for c in self.constructors:
                if (c is not default_constructor) and  c.public:  #  and (not c.artificial):
                    if c.argsTypes:
                        r += '  %(exposer)s.def( boost::python::init< %(args)s > ( (%(e_args)s) , "%(constr_doc)s" ) );\n' % dict(D.items(), args=c.getSimpleArgsType(constructor=True), e_args=c.getArgTypes(), constr_doc=doxygen.getDoxygenComment(c.file_, c.line))
                    else:
                        r += '  %(exposer)s.def( boost::python::init< %(args)s > () );\n' % dict(D.items(), args=c.getSimpleArgsType() )



        if self.enums:
            r+= '    boost::python::scope %(name)s_scope( %(exposer)s );\n' % D
            for e in self.enums:
                r+= e.wrap(indent='  ')

        wrapped = []
        statics = {}
        for f in self.functions:
            r+= f.wrap(indent='  ', wrappingScope=exposer + '.', useCallbackStruct=['', callback][use_callback_struct])  #, overloaded=self.isOverloaded(f) )
            if f.static and f.isWrapable():  statics[f.name] = '  %(wrappingScope)sstaticmethod("%(py_name)s");\n' % dict(wrappingScope=exposer + '.', py_name=f.name)
            wrapped.append( str(f) )

        if use_callback_struct:
            F = self.getAllVirtualFunctions({})
            for f in F:
                if f not in wrapped:
                    r+= F[f].wrap(indent='  ', wrappingScope=exposer + '.', useCallbackStruct=['', callback][use_callback_struct])  #, overloaded=self.isOverloaded(f) )
                    wrapped.append( f )


        #for v in self.dataMembers:  r += v.wrap(indent='  ', wrappingScope=exposer + '.')

        for i in statics.values():
            r += '\n' + i + '\n'

        #r += self.wrapSpecialMethods()
        # str, looking for 'ostream& operator<<(ostream &, T)
        str_op_name = self.context[2:] + 'operator<<(std::ostream&, %s const&)' % (self.context[2:]+self.name)  # core::conformation::operator<<(std::ostream&, core::conformation::Residue const&)
        #print str_op_name
        if str_op_name in self.reference.Objects:
            if self.reference.Objects[str_op_name].returnType.T()== '::std::ostream &':
                #_o = self.reference.Objects[str_op_name]
                #print '.........', _o.artificial, _o.demangled, _o.file_, _o.line
                r += '  %(exposer)s.def( boost::python::self_ns::str( boost::python::self ) );\n' % D

        #for o in self.reference.Objects.values():
        #    if o.name.startswith('<<'): print '~~~~ ', o

        r += '}\n'
        return ('\n' + indent).join( r.split('\n') ) + '\n'

    def getFileList(self):  # return list of files where types was created (use it for includes!)
        r = [self.file_]
        for f in self.constructors+self.functions: r += f.getFileList()
        for t in self.requiredTypes: r.append( t.getFile() )

        for b in self.bases:
            if b.type_.T() in self.reference.Objects: r += self.reference.Objects[ b.type_.T() ].getFileList()

        return r

    def __repr__(self):
        return 'CppClass: %s %s\n' % (self.name, self.context) + \
               '  constructors: %s\n' % self.constructors + \
               '  function: %s\n' % self.functions


    '''  To do:
             .staticmethod( "get_instance" );              core/pack/task/operation/_ResFilterFactory.cc
             vars: core/id/_TorsionID.cc
        .def( bp::self != bp::self )
        .def( bp::self < bp::self )
        .def( bp::self_ns::str( bp::self ) )
        .def( bp::self == bp::self )
        .def( bp::self_ns::str( bp::self ) );

    bp::scope().attr("BOGUS_TORSION_ID") = core::id::BOGUS_TORSION_ID;


'''

#
#
#
class GccXML:
    def __init__(self, dom):
        self.dom = dom  # Do we even need this???

        self.Objects = {}     # storage of all objects indexed by C++ names
        self.Contexts = {}    # storage of all objects indexed by C++ namespaces
        self.Namespaces = {}  # storage of CppNamespace's by XML id's
        self.Files = {}       # storage of file names (strings) indexed by XML id's
        self.TypeNodes, self.Types = {}, {}  # storage of types nodes and types (strings) indexed by XML id's
        self.Nodes = {}       # storage for everything else...

    def getType(self, type_id):
        if type_id not in self.Types:
            if type_id not in self.TypeNodes: self.Types[type_id] = CppType_Fundamental( 'UnknowType_%s' % type_id, 'unknow')
            else:
                node = self.TypeNodes[type_id]
                if   node.nodeName == 'FundamentalType':   self.Types[type_id] = CppType_Fundamental( node.getAttribute('name'), node.nodeName )

                elif node.nodeName == 'PointerType':       self.Types[type_id] = CppType_Simple( self.getType( node.getAttribute('type')), '*', node.nodeName )
                elif node.nodeName == 'ReferenceType':     self.Types[type_id] = CppType_Simple( self.getType( node.getAttribute('type')), '&', node.nodeName )
                elif node.nodeName == 'CvQualifiedType':   self.Types[type_id] = CppType_Simple( self.getType( node.getAttribute('type')), 'const', node.nodeName )

                elif node.nodeName in ['ArrayType', 'FunctionType']:
                    #print 'Creating composite', type_id, node.nodeName
                    self.Types[type_id] = CppType_Composite( node.nodeName, node.nodeName )
                    #print 'Creating composite', type_id, node.nodeName, self.Types[type_id].T()

                elif node.nodeName == 'Typedef':  # maybe not preatty and certainly verbose - but it fix everything...
                    #return self.getType( node.getAttribute('type') )
                    self.Types[type_id] = CppType_Typedef( node.getAttribute('name'), self.Nodes[node.getAttribute('context')].getAttribute('demangled'), self.getType( node.getAttribute('type') ) )

                elif node.nodeName in ['Class', 'Struct', 'Union', 'Typedef', 'Enumeration']:
                    #self.Types[type_id] = CppType_Complex( '::' + node.getAttribute('demangled'), '::' + self.Nodes[node.getAttribute('context')].getAttribute('demangled') )
                    self.Types[type_id] = CppType_Complex( node.getAttribute('name'), self.Nodes[node.getAttribute('context')].getAttribute('demangled'), node.nodeName )
                    #print 'Creating composite', type_id, node.nodeName, self.Types[type_id].name, self.Types[type_id].context

                else: self.Types[type_id] = CppType_Fundamental( 'UnknowType_%s_%s' % (node.nodeName, type_id), node.nodeName) #  + self.getType( node.getAttribute('type') )


                '''
                elif node.nodeName == 'ArrayType':         self.Types[type_id] = '___XQWERTY___Array_%s'  % self.getType( node.getAttribute('type') )
                elif node.nodeName == 'FunctionType':
                    args = []
                    for ch in node.childNodes:
                        if ch.nodeName == 'Argument': args.append( self.getType(ch.getAttribute('type')) )

                    self.Types[type_id] = '___XQWERTY___FunctionType_%s(*)(%s)' % (self.getType( node.getAttribute('returns') ), ', '.join(args) )
                '''
                file_ = node.hasAttribute('file') and self.Files[node.getAttribute('file')]
                line  = node.getAttribute('line')
                self.Types[type_id].file_ = file_
                self.Types[type_id].line  = line

            #print self.Types[type_id].T(), self.Types[type_id].getFile()

        #print type_id, self.Types[type_id].T(), self.Types[type_id].getContext()
        return self.Types[type_id]


    def parse(self, relevantFilesList=None):
        ''' Parse GCC_XML, we do this in two passes, first: namespaces, files and creating Dict of all elements.
            second - parsing actual elements (functions, classes)
        '''
        # Pass №1 Indexing Namespace's and File's
        print 'GccXML parsing, Pass №1...'
        for c in self.dom.documentElement.childNodes:  # dom.documentElement suppose to be dom.getElementsByTagName(GCC_XML')
            if c.nodeName == '#text': continue
            c_id = c.getAttribute('id')
            if c.nodeName in ['FundamentalType', 'PointerType', 'ReferenceType', 'CvQualifiedType', 'Class', 'Struct', 'Union', 'Typedef', 'FunctionType', 'Enumeration', 'ArrayType', 'OffsetType', 'MethodType']: self.TypeNodes[c_id] = c

            elif c.nodeName == 'File':
                self.Files[c_id] = c.getAttribute('name')
                if self.Files[c_id].startswith('./'): self.Files[c_id] = self.Files[c_id][2:]

            #else: self.Nodes[c_id] = c
            self.Nodes[c_id] = c  # we might want to have full reference for simplicity sake...

        print 'GccXML parsing, Pass №2...'
        for c in self.dom.documentElement.childNodes:  # dom.documentElement suppose to be dom.getElementsByTagName(GCC_XML')
            if c.nodeName == '#text': continue
            c_id = c.getAttribute('id')
            if c.nodeName in ['Namespace', 'Class', 'Struct']:
                #if c.hasAttribute('context') : context = self.Namespaces[c.getAttribute('context')].getChildrenContext()
                if c.getAttribute('context') in self.Namespaces : context = self.Namespaces[c.getAttribute('context')].getChildrenContext()
                else: context = ''
                self.Namespaces[c_id] = CppNamespace(c.getAttribute('name'), context)


        relevantFilesList = relevantFilesList or self.Files.values()

        # Pass №3 Actually doing the job...
        print 'GccXML parsing, Pass №3...'
        for node in self.dom.documentElement.childNodes:
            if node.nodeName == '#text': continue
            #if (not node.hasAttribute('file'))  or  (self.Files[node.getAttribute('file')] not in relevantFilesList) : continue

            object_ = None

            if node.nodeName == 'Enumeration':
                context =  '::' + self.Nodes[node.getAttribute('context')].getAttribute('demangled') + '::'
                object_ =  CppEnum(node, context, self)

            if node.nodeName in ['Function', 'OperatorFunction']:
                context =  self.Namespaces[node.getAttribute('context')].getChildrenContext()
                type_ = self.getType( node.getAttribute('returns') )
                object_ =  CppFunction(node, context, type_, self)

            if node.nodeName in ['Class', 'Struct']:
                #if node.nodeName == 'Struct' and node.getAttribute('artificial') == '1': continue

                if node.getAttribute('context') not in self.Namespaces: continue  # Clearly something not right - we probably hitting default/inner GCC classes here...
                context =  self.Namespaces[node.getAttribute('context')].getChildrenContext()
                object_ = CppClass(node, context, self)
                members = node.getAttribute('members')[:-1].split(' ')
                ch_context = object_.getChildrenContext()
                #print node.getAttribute('name'), members
                for m_id in [ x for x in members if x]:
                    m = self.Nodes[m_id]
                    #if m.getAttribute('access') != "public": continue
                    if m.nodeName == 'Constructor':  # and (not m.hasAttribute('artificial') ):
                        object_.constructors.append( CppFunction(m, ch_context, CppType_Fundamental('', ''), self, constructor=True) )
                        object_.constructors[-1].default = m.getAttribute('access') == "public"  and (not m.hasAttribute('artificial'))  # m.hasAttribute('explicit')  # and  ch.getAttribute('is_default_ctor') == '1'

                    if  m.nodeName == 'Destructor': object_.destructor = CppFunction(m, ch_context, None, self)

                    if m.nodeName in ['Method', 'OperatorMethod']:
                        #if m.getAttribute('access') != "public": continue
                        ch_type_ = self.getType( m.getAttribute('returns') )
                        object_.functions.append( CppFunction(m, ch_context, ch_type_, self, memberFunction=True, const=m.hasAttribute('const') ) )

                    if m.nodeName == 'Enumeration' and (not m.getAttribute('name').startswith('$_') ):
                        object_.enums.append( CppEnum(m, ch_context, self) )
                    '''
                    if m.nodeName == 'Field':  # we actually looking for enum here... this it to handle old style: enum {a, b} something:
                        en_node = self.Nodes[ m.getAttribute('type') ]
                        if en_node.nodeName == 'Enumeration':
                            object_.enums.append( CppEnum(en_node, ch_context, self, name=m.getAttribute('name') ) )
                    '''
                    if m.nodeName == 'Field':  # Ok, this could be inner type (typedef, enum etc) or datamemeber
                        object_.requiredTypes.append( self.getType( m.getAttribute('type') ) )

                        # TODO: add some code here to distinguish between datamembers and typedefs
                        object_.dataMembers.append( CppVariable(m, ch_context, self) )




            if object_:
                if not self.Contexts.get(context): self.Contexts[context] = [ object_ ]
                else: self.Contexts[ context ].append( object_ )
                #print '~~~~~', object_.context + object_.name
                self.Objects[ object_.context + object_.name ] = object_
                self.Objects[ node.getAttribute('demangled') ] = object_  # functions could be overloaded...


        print 'GccXML parsing... Done!'


def generateIncludes(incl_list):
    incl_list = list( set( filter(bool, incl_list) ) )
    #print incl_list
    for i, f in enumerate(incl_list):
        if f.endswith('.fwd.hh'):
            if os.path.isfile(f[:-7]+'.hh'): incl_list[i] = f[:-7]+'.hh'

    if 'core/types.hh' in incl_list:  # ＃　→ばか！！！
        incl_list.append('numeric/xyzVector.hh')
        incl_list = list( set( incl_list ) )

    r = ''
    for f in incl_list:
        if f.split('/')[0] in ['utility', 'numeric', 'basic', 'ObjexxFCL', 'core', 'protocols']:
            r += '#include <%s>\n' % f

    return r


def sortObjects(l):
    ''' sort list of C++ objects for wrapping
    '''
    def swap(i, j):
        a = l[i];  l[i]=l[j];  l[j]=a

    f = True
    while f:
        f = False
        for i in range( len(l) ):
            for j in range( i, len(l) ):
                if isinstance(l[i], CppClass) and isinstance(l[j], CppClass):
                    if l[i].isDepend(l[j]) and not l[j].isDepend(l[i]):  swap(i, j); f = True; break
                if isinstance(l[i], CppClass) and isinstance(l[j], CppEnum):
                    swap(i, j); f = True; break


def wrapModule(name, name_spaces, context, relevant_files_list, max_funcion_size):
    ''' Template for creating one module, and wrapping each elelemnts... (each elements must have .wrap)
    '''
    objects = []
    for n in name_spaces:
        if n in context:
            objects.extend( context[n] )

    sortObjects( objects )

    code = []

    module_addon = '#include <utility/PyHelper.hh>\n'

    r  = module_addon+'\n'

    s = ''
    includes = []
    prefix_code = ''
    for i in objects:
        if i.file_ in relevant_files_list:
            s += i.wrap('  ')
            prefix_code += i.wrap_prefix_code()

            includes += i.getFileList()

        if len(s)+len(prefix_code)+len(includes) >= max_funcion_size:
            r += 'void %s_partial_%s(void);\n' % (name, len(code))
            code.append( module_addon+'%s\n\n%s\nvoid %s_partial_%s(void)\n{\n%s\n}\n\n' % (generateIncludes(includes), prefix_code, name, len(code), s)  )
            s, prefix_code, includes = '', '', []

    r += '%s\n\n%s\nBOOST_PYTHON_MODULE( %s ){\n' % (generateIncludes(includes), prefix_code, name)
    for i in range( len(code) ): r += '\n  %s_partial_%s();\n' % (name, i)
    r += '\n' + s + '}\n'
    return code+[r]


def parseAndWrapModule(module_name, namespaces_to_wrap, xml_source, relevant_files_list, ParserType=GccXML, max_funcion_size=1024*1024*1024):
    print 'Wrapping %s... with file list as: %s...' % (namespaces_to_wrap, relevant_files_list)
    for f in relevant_files_list[:] :  relevant_files_list.append('./'+f)

    print 'Parse XML using mini dom...'
    dom = xml.dom.minidom.parse(xml_source)
    #dom =  xml.dom.pulldom.parseString(xml_source)

    #import xml.dom.ext.reader
    #reader = xml.dom.ext.reader.Sax2.reader()
    #dom = reader.fromStream(xml_source)

    print 'Parse XML using mini dom... Done!'
    cxml = ParserType(dom)
    cxml.parse(relevantFilesList=relevant_files_list)
    #return wrapModule(module_name, cxml.Contexts[namespace_to_wrap], cxml.Contexts)
    res = wrapModule(module_name, namespaces_to_wrap, cxml.Contexts, relevant_files_list, max_funcion_size)

    del cxml  # trying to save memory...
    del dom
    gc.collect()

    return res


def parseTranslationUnit(TU):
    for i in TU.childNodes:
        print 'nodeName', i.nodeName, i.namespaceURI


def CLANG_XML(clang_xml):
    for n in clang_xml.childNodes:
        print n, dir(n)
        print 'nodeName', n.nodeName
        print '\n\n'

        if n.nodeName == 'TranslationUnit': parseTranslationUnit(n)
    '''
    tr_unit = clang_xml.getElementsByTagName('TranslationUnit')

    for i in tr_unit:
        print i
'''


def main(args):
    print'Starting...'
    #dom = xml.dom.minidom.parse('TestingXML.xml')
    dom = xml.dom.minidom.parse('Example2.xml')

    cl = dom.getElementsByTagName('CLANG_XML')
    print cl, dom.documentElement
    for i in cl:
        CLANG_XML(i)

    cxml = ClangXML(dom)

    cxml.parse(relevantFilesList=['Example2.cpp'])
    print cxml.Contexts

    print wrapModule('test', cxml.Contexts['::NM::'], cxml.Contexts)


def GCC_XML_main(args):
    print'Starting...'
    dom = xml.dom.minidom.parse('Example1.gccxml.xml')

    gxml = GccXML(dom)

    gxml.parse(relevantFilesList=['Example1.cpp'])

    for k in gxml.TypeNodes: gxml.getType(k)
    print 'Types:', gxml.Types
    print 'Files:', gxml.Files

    print wrapModule('test', '::NM::', gxml.Contexts)





class ClangXML:
    def __init__(self, dom):
        self.dom = dom  # Do we even need this???

        self.Contexts = {}

        for i in dom.documentElement.childNodes:  # dom.documentElement suppose to be dom.getElementsByTagName('CLANG_XML')
            if i.nodeName == 'TranslationUnit':  self.TranslationUnit = i
            if i.nodeName == 'ReferenceSection':  self.ReferenceSection = ReferenceSection(i)



    def parse(self, node=None, relevantFilesList=None):
        relevantFilesList = relevantFilesList or self.ReferenceSection.Files.values()
        node = node or self.TranslationUnit
        #print 'relevantFilesList:', relevantFilesList

        if node.nodeName == '#text': return
        if node.hasAttribute('file')  and  self.ReferenceSection.Files[node.getAttribute('file')] not in relevantFilesList: return

        #print '~~~', node

        context =  node.hasAttribute('context') and self.ReferenceSection.getContext( node.getAttribute('context') )
        type_ = node.hasAttribute('type') and self.ReferenceSection.getType( node.getAttribute('type') )

        object_ = None

        if node.nodeName == 'Namespace':
            object_ = CppNamespace(node.getAttribute('name'), context)

        elif node.nodeName == 'Function': object_ =  CppFunction(node, context, type_, self.ReferenceSection)

        elif node.nodeName == 'CXXRecord': object_ = CppClass(node, context, self.ReferenceSection)


        if object_:
            if not self.Contexts.get(context): self.Contexts[context] = [ object_ ]
            else: self.Contexts[ context ].append( object_ )

        #if node.nodeName == 'ReferenceSection':

        if node.nodeName in ['Namespace', 'TranslationUnit'] :  # we only need to automatically parse sub-nodes for namespaces...
            for i in node.childNodes:
                self.parse(i, relevantFilesList=relevantFilesList)


'''
    class method
    def parse_Clang_XML(self, node, context, refSection):
        # Now lets parse all child nodes...
        for ch in node.childNodes:
            if ch.nodeName == '#text': continue  # why on earth this node type is everywhere???
            #print 'Processing: ', ch
            ch_context =  ch.hasAttribute('context') and refSection.getContext( ch.getAttribute('context') )
            ch_type_ = ch.hasAttribute('type') and refSection.getType( ch.getAttribute('type') )

            if ch.nodeName == 'CXXConstructor':
                self.constructors.append( CppFunction(ch, ch_context, ch_type_, refSection) )
                self.constructors[-1].default = ch.hasAttribute('is_default_ctor') and  ch.getAttribute('is_default_ctor') == '1'

            elif ch.nodeName == 'CXXMethod':
                self.functions.append( CppFunction(ch, ch_context, ch_type_, refSection) )

'''

"""
    def __cmp__(self, other):
        #print 'ii? ', other.name, isinstance(other, CppClass)
        if isinstance(other, CppClass):
            print '%s.isBase(%s) --> %s' % (self.name, other.context + other.name, self.isBase(other.context + other.name) )
            print '%s.isBase(%s) --> %s' % (other.name, self.context + self.name, other.isBase(self.context + self.name) )
            if self.isBase(other.context + other.name):
                return 1
            if other.isBase(self.context + self.name): return -1
            '''
                           context+self.name in ['::core::chemical::PatchOperation', '::core::chemical::AddChi']:
                print '__cmp__', self.context+self.name, other.name

            for b in other.bases:
                if self.context + self.name == b.type_.T(): return -1
            for b in self.bases:
                if other.context + other.name == b.type_.T(): return 1
                '''
            return 0
        else:
            return -1
    """


#if __name__ == "__main__": main(sys.argv)
if __name__ == "__main__": GCC_XML_main(sys.argv)

'''
ToDo:
 - investigate why we can't use ::core::scoring::ScoreFunctionFactory::create_score_function ?
 - bp::implicitly_convertible< std::string const &, protocols::moves::MinMover >();
 - docs for enums

'''


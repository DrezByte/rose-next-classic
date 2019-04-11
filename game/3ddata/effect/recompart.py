#! /usr/bin/env python
# location: $(SolutionDir)/tool/recompart.py
# ��ƼŬ ���������ϴ� ���̽� ��ũ��Ʈ
# by zho 2003.06.12
# 
# update log:
#

#---------------------------------------
# ��뼳��:
#
# src_path ���� in_ext Ȯ������ ���ϰ�
# dest_path �� out_ext ���� Ȯ������ ������
# exe_path �� �������Ͽ� ���ڷ� ���� �����Ѵ�.
in_ext = ".txt"
out_ext = ".ptl"
exe_path = "ScriptComplier.exe"
src_path = "Particles\\Txt\\"
dest_path = "Particles\\"
#---------------------------------------

import os # for os.path
import re # regular expression
import string # for lower

def get_ext (fullname):
    re_ext = re.search('(\.[a-zA-Z]+)$', string.lower(fullname))
    if (re_ext):
        return re_ext.group(1)

def replace_ext (fullname):
    return fullname[:-len(in_ext)]+out_ext

def doit (srcfile, destfile):
    try:
        print srcfile + "->" + destfile
        os.spawnl(os.P_WAIT, exe_path, exe_path, srcfile, destfile)
    except os.error:
        print 'error: '+exe_path+" "+srcfile+" "+destfile
        pass
        
def visit (arg, dirname, names):
    global src_path, dest_path
    for name in names:
        fullname = dirname + name
        print fullname
        if os.path.isfile(fullname):
            if (in_ext == get_ext(fullname)):
                destfile = dest_path + fullname[len(src_path):]
                destfile = replace_ext(destfile)
                doit(fullname, destfile)

def main():
    os.path.walk(src_path, visit, 0)
    
main()

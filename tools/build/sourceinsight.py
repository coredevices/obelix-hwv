import os
import sys

from utils import *
from utils import _make_path_relative
import glob
import rtconfig

def add_subfolder(path):
    global hpaths
    files=os.listdir(path)
    for file in files:
        file_path=os.path.join(path,file)
        if os.path.isdir(file_path):
            if not (file_path in hpaths):
                hpaths += [file_path]
            add_subfolder(file_path)
            
        

def TargetSI(env):
    global hpaths
    project = ProjectInfo(env)

    filelist=open("si_filelist.txt","w")
    BSP_ROOT = os.path.abspath(env['BSP_ROOT'])
    RTT_ROOT = os.path.abspath(env['RTT_ROOT'])

    match_bsp = False
    if BSP_ROOT.startswith(RTT_ROOT): 
        match_bsp = True

    Files   = project['FILES']
    Headers = project['HEADERS']

    hpaths = [os.path.normpath(i) for i in project['CPPPATH']]
    paths2 = []
    for path in hpaths:
        fn = os.path.normpath(path)
        if fn[-1]==',':
            fn=fn[1:-2]   
        if os.path.exists(fn):
            paths2 += [fn]
    hpaths=paths2    
    for i in paths2:        
        add_subfolder(i)

    for path in hpaths:
        hlist=glob.glob(path+"\\*.h")       
        #print(hlist);
        for hfile in hlist:
            filelist.write(hfile+"\n");
            
        if match_bsp:
            if path.startswith(BSP_ROOT):
                path = '$(BSP_ROOT)' + path.replace(BSP_ROOT, '')
            elif path.startswith(RTT_ROOT):
                path = '$(RTT_ROOT)' + path.replace(RTT_ROOT, '')
        else:
            if path.startswith(RTT_ROOT):
                path = '$(RTT_ROOT)' + path.replace(RTT_ROOT, '')
            elif path.startswith(BSP_ROOT):
                path = '$(BSP_ROOT)' + path.replace(BSP_ROOT, '')
    
    for file in Files:
        fn = os.path.normpath(file)
        filelist.write(fn+"\n");
        if match_bsp:
            if fn.startswith(BSP_ROOT):
                fn = '$(BSP_ROOT)' + fn.replace(BSP_ROOT, '')
            elif fn.startswith(RTT_ROOT):
                fn = '$(RTT_ROOT)' + fn.replace(RTT_ROOT, '')
        else:
            if fn.startswith(RTT_ROOT):
                fn = '$(RTT_ROOT)' + fn.replace(RTT_ROOT, '')
            elif fn.startswith(BSP_ROOT):
                fn = '$(BSP_ROOT)' + fn.replace(BSP_ROOT, '')
    
    filelist.close()
    return

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import argparse
import os.path
import sys
import textwrap

def check_license(source):
    fp=open(source, 'r')
    found=0
    pos=-1
    for line in fp:
        if (line.find('@attention')>=0) :
            found=1
        if ((line.find('****************')>=0) and found==1) :
            found=2
        if (pos<0):
            pos=line.find('@brief')
            if (pos>=0) :
                brief_line=line[pos:]
                print(brief_line, end='')
        if (line.find('#')==0):
            print(line, end='')            
            break
        if (found==1):
            print(line, end='')
    return found
    
def replace_license(source,lic):
    if (os.path.isdir(source)) :
        for entry in os.listdir(source):
            replace_license(os.path.join(source,entry), lic)
        return
    extension = os.path.splitext(source)[1][1:].strip()
    if (extension=='c' or extension=='h' or extension=='C' or extension=='H'): 
        print(extension+' '+source)
    else :
        return
    fp=open(source, 'r')
    src_file=open(lic, 'r').read()
    src_file +="\n"
    
    started=0
    end_added=0
    brief_line=None
    pos=-1
    for line in fp:
        if (line.find('#')==0):
            started=1
        if (pos>=0):
            if (line.find('***************') < 0):
                brief_line += line
                pos=-2
        if (pos==-1):
            pos=line.find('@brief')
            if (pos>=0) :
                brief_line=line[pos:]
        if (started==1):
            if (line.find('**END OF FILE**')>=0):
                src_file += "/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/"
                end_added=1;
            else:
                src_file += line
    if (end_added==0):
        src_file += "/************************ (C) COPYRIGHT Sifli Technology *******END OF FILE****/"
    fname=os.path.basename(source)
    fp.close()
    fp=open(source, 'w')
    print("/**\n  ******************************************************************************", file=fp)
    print("  * @file   %s" %(fname), file=fp)
    print("  * @author Sifli software development team", file=fp)
    if (brief_line!=None):
        print("  * %s" %(brief_line),end='', file=fp) 
    print("  ******************************************************************************\n*/", file=fp)    
    print(src_file, file=fp)
    fp.close()
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="General Usage ", 
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog=textwrap.dedent('''\
         Check license of source code
         '''))

    parser.add_argument('action', choices=['check', 'replace'], default='check')
    parser.add_argument(
        '--source',
        type=str,
        help='Source file')
    parser.add_argument(
        '--lic',
        type=str,
        default='',
        help='New license file.')
            
    FLAGS, unparsed = parser.parse_known_args()
        
    if (FLAGS.action == 'check'):
        check_license(FLAGS.source)
    if (FLAGS.action == 'replace'):
        replace_license(FLAGS.source, FLAGS.lic)
       

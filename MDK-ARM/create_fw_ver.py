import sys, os, shutil, getopt
import subprocess
import re


def usage():
    print("""Usage: %s [-h] [-p] [-g]
    -h          This help, Create from last hg commit msg fw_ver[number_last_commit]
    -p          Added parameter, some like d for Debug (Optional)
    -g          Get last commit from Git
    """ % sys.argv[0])
    
def version_parse(data):
    # template #.#.#.#, # -- number
    exp = r'\d+[.]+\d+[.]+\d+[.]+\d+'
    version = re.findall(exp, data)

    if (len(version) == 0):
        # template #.#.#, # -- number
        exp = r'\d+[.]+\d+[.]+\d+'
        version = re.findall(exp, data)
    if (len(version) == 0):
        print('Error: Version not found')
        raise SystemExit(2)
        
    return version[0]
    
def create_fw_ver_h(last_msg):
    # Create fw_ver.h
    out_path = os.getcwd() + '\\Src\\init\\'
    if ('MDK-ARM' in out_path):
        out_path = os.getcwd()[:-len('MDK-ARM')] + '\\Src\\init\\'
    fw_ver_file_name = 'fw_ver.h'
    print('Info: Create: ' + fw_ver_file_name + ' in --> ' + out_path)
    try:
        f = open(out_path + fw_ver_file_name, 'w')
    except:
        print('Error: Out path: ' + out_path + ' [NOT FIND]')
        print('Info: Try find in \\init_main')
        out_path = os.getcwd() + '\\Src\\init_main\\'
        if ('MDK-ARM' in out_path):
            out_path = os.getcwd()[:-len('MDK-ARM')] + '\\Src\\init_main\\'
        
    print('Info: Create: ' + fw_ver_file_name + ' in --> ' + out_path)
    try:
        f = open(out_path + fw_ver_file_name, 'w')
    except:
        print('Error: Out path: ' + out_path + ' [NOT FIND]')
        raise SystemExit(0)        
    f.write('#ifndef FW_VER_H_\n')
    f.write('#define FW_VER_H_\n\n')

    f.write('#define FW_VER "fw_v' + last_msg + '"\n\n')

    f.write('#endif /* FW_VER_H_ */\n') 
    f.close()
    print('Info: ' + fw_ver_file_name + ' [Created successfully]')
    print('Info: fw_v' + last_msg)
    
def set_fw_ver_mercurial(param = ''):
    process = subprocess.Popen(["hg", "log", "--limit", " 1"], stdout=subprocess.PIPE)
    output = str(process.communicate()[0])

    # output = '0.0.1021 -- added create_fw_ver.py\n'
    # print(output)

    last_msg = version_parse(output)
    if (last_msg == ''):
        print('Error: Version not found')
        raise SystemExit(2)
    
    print('Info: Commit number: ' + last_msg)
    create_fw_ver_h(last_msg + param)
    
def set_fw_ver_git(param = ''):
    process = subprocess.Popen(["git", "log", "-1"], stdout=subprocess.PIPE)
    output = str(process.communicate()[0])

    # output = '0.0.1021 -- added create_fw_ver.py\n'
    # print(output)

    last_msg = version_parse(output)
    
    print('Info: Commit number: ' + last_msg)
    create_fw_ver_h(last_msg + param)    
    
def main():
    param = ''
    type = 'mercurial'
    # Get param
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hp:g')
    except getopt.GetoptError as err:
        usage() # print help information and exit:
        print(err)  # will print something like "option -a not recognized"
        raise SystemExit(2)
    for o, a in opts:
        if o in ('-h'):
            usage()
            return
        elif o in ("-p"):
            param = a
            print('Info: Additional param: ' + param)
        elif o in ("-g"):
            type = 'git'
        else:
            return print("Undefined param" + o)
    print('Info: Type: ' + type)
    if type == 'mercurial':
        set_fw_ver_mercurial(param)
    elif type == 'git':
        set_fw_ver_git(param)

if __name__ == '__main__':
    main()
    
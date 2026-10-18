"""Build, audit, package and publish an explicitly requested stable version.
Credentials come only from Git Credential Manager in memory; they are never logged or saved.
Usage: python tools/release.py 0.4.0 --public CHECKOUT --notes MARKDOWN
"""
from pathlib import Path
import argparse,ctypes,ctypes.wintypes as wt,hashlib,io,json,os,re,shutil,subprocess,time,urllib.request,urllib.error,urllib.parse,zipfile
parser=argparse.ArgumentParser();parser.add_argument('version');parser.add_argument('--public',required=True);parser.add_argument('--notes',required=True);args=parser.parse_args()
assert re.fullmatch(r'0\.[4-9]\.0',args.version),'This roadmap stops at 0.9.0'
root=Path(__file__).resolve().parents[1];public=Path(args.public).resolve();os.chdir(root)
version=args.version;tag='v'+version;out=root/'build/releases'/tag;out.mkdir(parents=True,exist_ok=True)
msys=Path('C:/msys64/ucrt64');env=os.environ.copy();env['PATH']=str(msys/'bin')+';'+env['PATH']
def run(command,cwd=root,capture=False,**kw):
    result=subprocess.run([str(x) for x in command],cwd=cwd,env=env,check=True,stdout=subprocess.PIPE if capture else None,**kw)
    return result.stdout if capture else None
def git(where,*command,capture=False):return run(['git','-c','safe.directory='+where.as_posix(),'-C',where,*command],capture=capture)
for folder,name in [(root,'dont-get-bored-develop'),(public,'dont-get-bored')]:
    assert git(folder,'remote','get-url','origin',capture=True).decode().strip()=='https://github.com/lengm70/'+name+'.git'
    git(folder,'fetch','origin')
    assert git(folder,'rev-list','--left-right','--count','HEAD...origin/main',capture=True).decode().strip()=='0\t0','Remote advanced; inspect before release'
    assert not git(folder,'ls-remote','--tags','origin','refs/tags/'+tag,capture=True).strip(),'Version already exists'
assert not git(public,'status','--porcelain',capture=True).strip(),'Public checkout has user changes'
run([msys/'bin/cmake.exe','--build','build','-j','4']);run([msys/'bin/ctest.exe','--test-dir','build','--output-on-failure'])
run([msys/'bin/cmake.exe','-S','.', '-B','build/release','-G','Ninja','-DCMAKE_BUILD_TYPE=Release','-DBUILD_TESTING=OFF','-DCMAKE_C_COMPILER='+str(msys/'bin/gcc.exe'),'-DCMAKE_CXX_COMPILER='+str(msys/'bin/g++.exe'),'-DCMAKE_MAKE_PROGRAM='+str(msys/'bin/ninja.exe'),'-DCMAKE_PREFIX_PATH='+str(msys)])
run([msys/'bin/cmake.exe','--build','build/release','-j','4'])
files=set(filter(None,git(root,'ls-files','-z',capture=True).decode().split('\0')))
files.update(filter(None,git(root,'ls-files','--others','--exclude-standard','-z',capture=True).decode().split('\0')))
files={n for n in files if (root/n).is_file()}
allowed={'src','tests','assets','config','resources','external','docs','tools','.vscode'}
secret=[rb'gh[pousr]_[A-Za-z0-9]{30,}',rb'github_pat_[A-Za-z0-9_]{30,}',rb'sk-[A-Za-z0-9_-]{24,}',rb'-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----']
def audit(name,data):
    parts=Path(name).parts
    assert not any(p.lower() in {'.git','.agents','.codex','.claude','.aws','.cursor','__pycache__'} for p in parts),name
    assert not name.endswith(('config/settings.ini','config/highscores.ini','config/breakout.ini','config/progress.ini')),name
    assert not any(re.search(pattern,data) for pattern in secret),'Credential signature in '+name
for name in files:
    assert Path(name).parts[0] in allowed or name in {'.gitignore','CMakeLists.txt','README.md'},'Unexpected source file '+name
    audit(name,(root/name).read_bytes())
notes=Path(args.notes).read_text(encoding='utf-8')
# Package only default assets, not custom imports copied by the development build.
package=out/'package/dont-get-bored';assert not package.exists(),'Output already prepared; inspect/resume explicitly'
package.mkdir(parents=True);exe=root/'build/release/dont_get_bored.exe';shutil.copy2(exe,package/exe.name);shutil.copy2(exe,out/exe.name)
for name in files:
    if name.startswith('assets/'):
        target=package/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(root/name,target)
seen=set();pending=[package/exe.name]
while pending:
    imports=run([msys/'bin/objdump.exe','-p',pending.pop()],capture=True).decode(errors='replace')
    assert 'pei-x86-64' in imports
    for name in re.findall(r'DLL Name:\s*(\S+)',imports):
        key=name.lower()
        if key in seen:continue
        seen.add(key)
        if key.startswith(('api-ms-win-','ext-ms-win-')) or (Path(os.environ['SystemRoot'])/'System32'/name).exists():continue
        dll=msys/'bin'/name;assert dll.is_file(),'Missing DLL '+name;shutil.copy2(dll,package/name);pending.append(package/name)
licenses=package/'licenses';licenses.mkdir()
for library in ['raylib','glfw','gcc-libs','winpthreads']:
    for file in (msys/'share/licenses'/library).rglob('*'):
        if file.is_file():shutil.copy2(file,licenses/(library+'-'+file.name))
shutil.copy2(root/'external/raygui-LICENSE',licenses/'raygui-LICENSE')
(package/'VERSION.txt').write_text(version+'\n',encoding='utf-8')
(package/'README_RUN.txt').write_text("don't get bored "+tag+" — Windows x64\n\n完整解压后运行 dont_get_bored.exe，保留DLL和assets。无需安装编译器。\nExtract the entire ZIP and run the EXE; keep DLLs and assets alongside it.\n个人设置与纪录在config中自动创建，升级时保留自己的config。\nSpace starts; P pauses; R restarts; Esc returns. Read game-specific controls on screen.\nSource: https://github.com/lengm70/dont-get-bored\n",encoding='utf-8')
portable=out/('dont-get-bored-'+tag+'-windows-x64.zip')
with zipfile.ZipFile(portable,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
    for file in sorted(package.rglob('*')):
        if file.is_file():audit(str(file.relative_to(package)),file.read_bytes());z.write(file,file.relative_to(package.parent))
# Run an isolated extracted package with no MSYS2 PATH and close only our own test window.
smoke=out/'smoke'
with zipfile.ZipFile(portable) as z:z.extractall(smoke)
startup=subprocess.STARTUPINFO();startup.dwFlags|=subprocess.STARTF_USESHOWWINDOW;startup.wShowWindow=0
clean=env.copy();clean['PATH']=str(Path(os.environ['SystemRoot'])/'System32')+';'+os.environ['SystemRoot']
user=ctypes.WinDLL('user32');callback=ctypes.WINFUNCTYPE(wt.BOOL,wt.HWND,wt.LPARAM);windows=[]
user.GetWindowThreadProcessId.argtypes=[wt.HWND,ctypes.POINTER(wt.DWORD)];user.ShowWindow.argtypes=[wt.HWND,ctypes.c_int];user.PostMessageW.argtypes=[wt.HWND,wt.UINT,wt.WPARAM,wt.LPARAM]
with (out/'smoke.log').open('wb') as log:
    process=subprocess.Popen([str(smoke/'dont-get-bored'/exe.name)],cwd=smoke/'dont-get-bored',env=clean,startupinfo=startup,stdout=log,stderr=subprocess.STDOUT)
    @callback
    def find(hwnd,_):
        pid=wt.DWORD();user.GetWindowThreadProcessId(hwnd,ctypes.byref(pid))
        if pid.value==process.pid:windows.append(hwnd);user.ShowWindow(hwnd,0)
        return True
    for attempt in range(100):
        if process.poll() is not None:break
        windows.clear();user.EnumWindows(find,0)
        if windows and (smoke/'dont-get-bored/config/breakout.ini').exists():break
        time.sleep(.1)
    assert process.poll() is None and windows,'Portable package startup failed'
    for hwnd in windows:user.PostMessageW(hwnd,0x10,0,0)
    assert process.wait(timeout=15)==0,'Portable package did not close normally'
print('AUDIT, TESTS, RELEASE BUILD AND PORTABLE SMOKE PASS',flush=True)
# Commit the approved files in private; export only its tree into public history.
git(root,'add','--',*sorted(files));git(root,'diff','--cached','--check');git(root,'commit','-m','Release '+tag+' — '+notes.splitlines()[0].lstrip('# '))
private=git(root,'rev-parse','HEAD',capture=True).decode().strip()
with zipfile.ZipFile(io.BytesIO(git(root,'archive','--format=zip','HEAD',capture=True))) as z:
    actual={i.filename for i in z.infolist() if not i.is_dir()};assert actual==files
    for name in actual:
        target=public/name;assert public in target.resolve().parents;target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(z.read(name))
previous=set(filter(None,git(public,'ls-files','-z',capture=True).decode().split('\0')))
for name in previous-files:
    target=public/name;assert public in target.resolve().parents;target.unlink()
git(public,'add','--',*sorted(files|previous));git(public,'diff','--cached','--check');git(public,'commit','-m','Release '+tag)
public_sha=git(public,'rev-parse','HEAD',capture=True).decode().strip()
assert git(root,'rev-parse','HEAD^{tree}',capture=True)==git(public,'rev-parse','HEAD^{tree}',capture=True)
source=out/('dont-get-bored-'+tag+'-source.zip');source.write_bytes(git(public,'archive','--format=zip','--prefix=dont-get-bored-'+tag+'/','HEAD',capture=True))
with zipfile.ZipFile(source) as z:
    for i in z.infolist():
        if not i.is_dir():audit(i.filename,z.read(i))
assets=[out/exe.name,portable,source];checks=out/'SHA256SUMS.txt';checks.write_text(''.join(hashlib.sha256(p.read_bytes()).hexdigest()+'  '+p.name+'\n' for p in assets),encoding='ascii');assets.append(checks)
for folder in [root,public]:git(folder,'tag','-a',tag,'-m','Release '+tag);git(folder,'push','--atomic','origin','main','refs/tags/'+tag)
credential=subprocess.run(['git','credential','fill'],input='protocol=https\nhost=github.com\n\n',text=True,capture_output=True,check=True)
auth=dict(line.split('=',1) for line in credential.stdout.splitlines() if '=' in line)
headers={'Authorization':'Bearer '+auth['password'],'Accept':'application/vnd.github+json','User-Agent':'dont-get-bored-release'}
def api(path,method='GET',payload=None,data=None,kind=None):
    h=headers.copy()
    if payload is not None:data=json.dumps(payload,ensure_ascii=False).encode();h['Content-Type']='application/json'
    if kind:h['Content-Type']=kind
    req=urllib.request.Request(path if path.startswith('https://') else 'https://api.github.com'+path,data=data,headers=h,method=method)
    with urllib.request.urlopen(req,timeout=120) as r:return json.load(r)
result={'version':tag,'private':private,'public':public_sha,'releases':{},'files':{}}
for folder,name,sha in [(root,'dont-get-bored-develop',private),(public,'dont-get-bored',public_sha)]:
    base='/repos/lengm70/'+name
    release=api(base+'/releases','POST',{'tag_name':tag,'target_commitish':sha,'name':tag+' 正式版','body':notes+'\n\n下载完整 windows-x64.zip 并解压运行。另附EXE（需配合ZIP内素材与DLL）、源码ZIP和SHA-256校验。\n\n自动测试、Release构建、无MSYS2环境路径启动验证与敏感文件检查均通过。','draft':True,'prerelease':False})
    for asset in assets:
        data=asset.read_bytes();a=api(release['upload_url'].split('{')[0]+'?name='+urllib.parse.quote(asset.name),'POST',data=data,kind='application/octet-stream')
        assert a['size']==len(data) and a['digest']=='sha256:'+hashlib.sha256(data).hexdigest()
        print(name+': verified '+asset.name,flush=True)
    release=api(base+'/releases/'+str(release['id']),'PATCH',{'draft':False,'prerelease':False,'make_latest':'true'})
    assert not release['draft'] and not release['prerelease'] and len(release['assets'])==4
    result['releases'][name]=release['html_url']
    result['files'][name]=git(folder,'diff-tree','--no-commit-id','--name-status','-r','HEAD',capture=True).decode().splitlines()
(out/'result.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print('COMPLETE '+tag+' '+str(result['releases']),flush=True)
